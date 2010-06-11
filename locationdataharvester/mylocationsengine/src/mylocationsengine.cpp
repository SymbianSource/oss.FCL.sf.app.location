/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Mylocation engine source implementation for location picker
*              and maptile service.
*
*/

// INCLUDE FILES
#include <f32file.h>
#include <calchangecallback.h> 
#include <cntitem.h>
#include <cntfldst.h>
#include <EPos_CPosLmDatabaseManager.h>
#include <lbsposition.h>
#include <bautils.h>
#include <f32file.h>
#include <locationservicedefines.h>
#include "mylocationsengine.h"
#include "mylocationsdefines.h"
#include "geocodeupdate.h" //header for GeocodeUpdate class
//handle for CMyLocationsHistoryDbObserver class
#include "mylocationlogger.h"
#if ( defined __WINSCW__ ) || ( defined __WINS__ )
_LIT ( KImageStorageDrive, "C:\\MapTile\\");
#endif
_LIT(KFolderName,":\\MapTile\\");
const TInt KImagePathSize=36;
const TInt KDefaultFilePathSize = 20;

// ============================ MEMBER FUNCTIONS ===============================

CMyLocationsEngine* CMyLocationsEngine::NewL()
{
    CMyLocationsEngine* self = new (ELeave) CMyLocationsEngine();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::ConstructL()
{
    __TRACE_CALLSTACK;
    CActiveScheduler::Add(this);

    //Connection to Landmark DB
    iLandmarkDb = CPosLandmarkDatabase::OpenL();
    ExecuteAndDeleteLD(iLandmarkDb->InitializeL());

    MYLOCLOGSTRING("Connection to Landmark DB.");
    // create CMyLocationsDatabaseManager instance
    iMyLocationsDatabaseManager = new (ELeave) CMyLocationsDatabaseManager;
    iMyLocationsDatabaseManager->ConstructL();
    
    MYLOCLOGSTRING("create CMyLocationsDatabaseManager instance");

    TRAPD( error , iMapTileInterface = CMapTileInterface::NewL());
    if( error == KErrNone )
    {
        iMaptileGeocoderPluginAvailable = ETrue;	
    }
    
    MYLOCLOGSTRING("iMapTileInterface = CMapTileInterface::NewL()");
    //Maptile Db instance
    iMaptileDatabase = CLookupMapTileDatabase::NewL(KMapTileLookupDatabaseName);

    MYLOCLOGSTRING("Maptile Db instance created ");
    
    iAddressCompare = CAddressComparision::NewL();
	
    iMyLocationThreeAMTimer = CLocationGeoTagTimerAO::NewL(*this);
  
    iMyLocationThreeAMTimer->StartTimer();
    
    MYLOCLOGSTRING(" iMyLocationThreeAMTimer = CLocationGeoTagTimerAO::NewL(this)");
  
    //Create instance of contact manager 
    iContactManager = new QContactManager();

    MYLOCLOGSTRING(" start contact db observation ");
    StartContactsChangeNotifierL();

    //set the folder path to store maptile
    imageFilePath.Zero();
    SetFolderPathL();

    TInt status = KErrNone;
    iCalSession = CCalSession::NewL();
    NotifyChangeL(status);

    // Start listening to landmarks db changes
    StartLandmarksChangeNotifier();   

}
// -----------------------------------------------------------------------------
// CMyLocationsEngine::SetFolderPath()
// set folder path structure
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::SetFolderPathL()
{
    __TRACE_CALLSTACK;
    RFs session;
    User::LeaveIfError(session.Connect());
   
    if ( imageFilePath.Length() > 0 && BaflUtils::FolderExists(session, imageFilePath))
    {
        session.Close();
        MYLOCLOGSTRING("Image folder path exists");
        return;
    }
    const TUint EMediaMemoryCard = 0x000014;
    imageFilePath.Zero();
#if ( defined __WINSCW__ ) || ( defined __WINS__ )
    imageFilePath.Copy(KImageStorageDrive);
    MYLOCLOGSTRING("using in  debug mode");
#else        
    TVolumeInfo vol;
    TInt driveNumber = EDriveE;
    TChar driveLetter;
    TDriveInfo driveInfo;
    TInt error( KErrNone );
    error=session.Volume(vol, driveNumber);
    MYLOCLOGSTRING1("E drive volume info status -%d",error);
    if (error == KErrNone)
    {
        error = session.Drive(driveInfo, driveNumber);
        MYLOCLOGSTRING1("E driveinfo status -%d",error);
        switch(driveInfo.iType)
        {
            case EMediaFlash:
            case EMediaHardDisk:
            case EMediaNANDFlash:
            case EMediaMemoryCard:
            case EMediaCdRom:
            {
                MYLOCLOGSTRING(" E drive match case");
                session.DriveToChar( driveNumber , driveLetter );
                imageFilePath.Append(driveLetter);
                imageFilePath.Append(KFolderName);
                break;
            }
            default:
            break;

        };
    }

    if (!imageFilePath.Length() > 0)
    {
        driveNumber = EDriveF;
        error = session.Volume(vol, driveNumber);
        MYLOCLOGSTRING1("F drive volume info status -%d",error);

        if (error == KErrNone)
        {
            error = session.Drive(driveInfo, driveNumber);
            MYLOCLOGSTRING1("F drive info status-%d", error);

            switch (driveInfo.iMediaAtt)
            {
                /** Solid-state media. */
                case EMediaFlash:
                case EMediaHardDisk:
                case EMediaNANDFlash:
                case EMediaMemoryCard:
                {
                    MYLOCLOGSTRING("F drive exist as this type");
                    session.DriveToChar(driveNumber, driveLetter);
                    imageFilePath.Append(driveLetter);
                    imageFilePath.Append(KFolderName);
                    break;
                }

                default:
                break;

            };
        }
    }
#endif
    if (imageFilePath.Length()>0 && !BaflUtils::FolderExists(session, imageFilePath))
    {
        session.MkDirAll(imageFilePath);
        MYLOCLOGSTRING("folder path does not exist , created ");
    }
    session.Close();
}
// -----------------------------------------------------------------------------
// CMyLocationsEngine::CMyLocationsEngine()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CMyLocationsEngine::CMyLocationsEngine() :
            CActive(EPriorityStandard), 
            iCalSession(NULL), iCalView(NULL), 
	        iContactsDb(NULL), iContactChangeNotifier(NULL), 
			iLandmarkDb(NULL),
            iMapTileInterface(NULL), iMyLocationsDatabaseManager(NULL),
            iMaptileDatabase(NULL), iAddressCompare(NULL),
            iMyLocationThreeAMTimer(NULL),
            iMaptileGeocoderPluginAvailable(EFalse),iCalenderNotification(NULL),
            iContactManager(NULL)
{

}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::~CMyLocationsEngine()
// Destructor.
// -----------------------------------------------------------------------------
//
CMyLocationsEngine::~CMyLocationsEngine()
{
    __TRACE_CALLSTACK;// delete the member variables.
   
    delete iContactChangeNotifier;

    delete iMyLocationsDatabaseManager;

    delete iLandmarkDb;

    delete iContactsDb;
   
    delete iCalView;
	
    delete iCalSession;
	 
	delete iMapTileInterface;

    delete iMaptileDatabase;

    delete iAddressCompare;
    
    delete iCalenderNotification;
    
	delete iMyLocationThreeAMTimer;
		
    delete iContactManager;
    
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::NotifyChangeL()
// Starts calender db create notification to get callbacks. 
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::NotifyChangeL(TInt &aStatus)
{
    __TRACE_CALLSTACK;
    
    TBufC<KDefaultFilePathSize> defaultFile = iCalSession->DefaultFileNameL();
    TChar drive = defaultFile[0];    
    TRAP(aStatus, iCalSession->OpenL( defaultFile ));
    MYLOCLOGSTRING1("iCalSession->OpenL() status-%d",aStatus);
    if ( KErrNone == aStatus )
    {
        // create a calendar entry view with the opened session
        iCalView = CCalEntryView::NewL(*iCalSession);
        StartCalenderChangeNotifierL();
    }
    else
    {
        if( !iCalenderNotification )
        {
            iCalenderNotification = CCalenderNotification::NewL( this );
        }
        
        
        iCalenderNotification->CheckCalenderDbFileStructure( drive );
    }
}
// -----------------------------------------------------------------------------
// CMyLocationsEngine::StartCalenderChangeNotifierL()
// Starts calender change notification method to get callbacks when entries are 
// added/modified/deleted in contacts
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::StartCalenderChangeNotifierL()
{
    __TRACE_CALLSTACK;// Get the current time and set Calendar filter to consider entries whose start time
    // fall under the timeframe one year past and one year ahead of the current time.
    if (iCalSession)
    {
        MYLOCLOGSTRING("iCalSession is not null");
        TTime currentTime;
        currentTime.HomeTime();

        TTime startTime = currentTime - TTimeIntervalYears(1);
        TTime endTime = currentTime + TTimeIntervalYears(1);

        TCalTime calStartTime;
        calStartTime.SetTimeLocalL(startTime);
        
        MYLOCLOGSTRING("SetTimeLocalL(startTime)");

        TCalTime calEndTime;
        calEndTime.SetTimeLocalL(endTime);
        
        MYLOCLOGSTRING("SetTimeLocalL(endTime)");

        // set the filter for modification tracking
        CCalChangeNotificationFilter *filter =
                CCalChangeNotificationFilter::NewL(
                        MCalChangeCallBack2::EChangeEntryAll, ETrue,
                        CalCommon::TCalTimeRange(calStartTime, calEndTime));
        MYLOCLOGSTRING(" CCalChangeNotificationFilter::NewL()");
        // 'this' object implements MCalChangeCallBack
        iCalSession->StartChangeNotification(*this, *filter);
    }
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::StartContactsChangeNotifierL()
// Starts contacts change notification method to get callbacks when entries are 
// added/modified/deleted in contacts
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::StartContactsChangeNotifierL()
{
    __TRACE_CALLSTACK;
   
    GeocodeUpdate::createContactdb();
    iContactsDb = CContactDatabase::OpenL();
    // Create CContactChangeNotifier object with 'this' object. 
    iContactChangeNotifier = CContactChangeNotifier::NewL(*iContactsDb,this);
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::StartLandmarksChangeNotifier()
// Starts landmarks change notification method to get callbacks when entries are added/modified/deleted in landmarks
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::StartLandmarksChangeNotifier()
{
    __TRACE_CALLSTACK;// Resume event listening
    iLandmarkDb->NotifyDatabaseEvent(iLmEvent, iStatus);
    SetActive();
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::CalChangeNotification()
// Callback when there is a change in the calendar database.
// -----------------------------------------------------------------------------
//

void CMyLocationsEngine::CalChangeNotification(
        RArray<TCalChangeEntry>& aChangeItems)
{
    __TRACE_CALLSTACK;
       
    if(iCalenderNotification)
    {
        delete iCalenderNotification;
        iCalenderNotification = NULL;
    }
    
    // get entries associated with this UID
    for (int i = 0; i < aChangeItems.Count(); i++)
    {

        TCalChangeEntry calChangeEntry = aChangeItems[i];
        iEventType = calChangeEntry.iChangeType;
        switch (calChangeEntry.iChangeType)
        {
        case EChangeAdd:
        {
            
            TRAP_IGNORE(CalenderEntryAddedL(calChangeEntry));
            break;
        }

        case EChangeModify:
        {
            if (iMapTileRequestQueue.Count() > 0)
            {
                if (calChangeEntry.iEntryId == iMapTileRequestQueue[0]->iUId)
                {
                    return;
                }
            }
            TLookupItem lookupItem;
            lookupItem.iSource = ESourceCalendar;
            lookupItem.iUid = calChangeEntry.iEntryId;
            TRAP_IGNORE(ManipulateMapTileDataBaseL(lookupItem));
            //TODO: comapare address and then delete 
            TRAP_IGNORE( CalenderEntryAddedL(calChangeEntry) );
            break;
        }
        case EChangeDelete:
        {
            TLookupItem lookupItem;
            lookupItem.iSource = ESourceCalendar;
            lookupItem.iUid = calChangeEntry.iEntryId;
            TRAP_IGNORE(ManipulateMapTileDataBaseL(lookupItem));           
            TRAP_IGNORE( UpdateDatabaseL( NULL, 
                              calChangeEntry.iEntryId, ESourceCalendar, EEntryDeleted ) );
            break;
        }
        };

    }
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::CalenderEntryAddedL()
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::CalenderEntryAddedL(TCalChangeEntry aCalChangeEntry)
{
    __TRACE_CALLSTACK;
    CCalEntry* calEntry = NULL;
    calEntry = iCalView->FetchL(aCalChangeEntry.iEntryId);
    TPtrC address(calEntry->LocationL());
    if(address.Length()>0)
    {        
    //create entry in the data base and maintain a fetching state.
        TLookupItem lookupItem;
        lookupItem.iUid = aCalChangeEntry.iEntryId;
        lookupItem.iSource = ESourceCalendar;
        lookupItem.iFilePath.Zero();
        lookupItem.iFetchingStatus = EMapTileFetchingInProgress;
        iMaptileDatabase->CreateEntryL(lookupItem);  
        
        RequestMapTileImageL(address,ESourceCalendar, aCalChangeEntry.iEntryId);
    }
    delete calEntry;
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::HandleDatabaseEventL()
// Callback that provides information about the contact database change event.
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
{
    __TRACE_CALLSTACK;
    
    HandlelandmarkDatabaseL(aEvent);
    
    //Forward the event for maptile fetching only if maptile plugin available
    if( iMaptileGeocoderPluginAvailable )
    {
        TriggerMaptileRequestL(aEvent);
    }
    
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::HandlelandmarkDatabaseL()
// Process the contact database event and updates the landmark database
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::HandlelandmarkDatabaseL(
        TContactDbObserverEvent& aEvent)
{
    __TRACE_CALLSTACK;// If contact is modified or added, update the mylocations db
    if (aEvent.iType == EContactDbObserverEventContactChanged || aEvent.iType
            == EContactDbObserverEventContactAdded)
    {
       
        CPosLandmark *preferedAddressLm = NULL;
        CPosLandmark *workAddressLm = NULL;
        CPosLandmark *homeAddressLm = NULL;
        
        QContact contactInfo = iContactManager->contact( aEvent.iContactId );
         
        //Get the contact name details
        QContactName name = contactInfo.detail( QContactName::DefinitionName );
        QString firstName = name.firstName();
        QString lastName = name.lastName();
        TPtrC16 tempPtr1(reinterpret_cast<const TUint16*>(firstName.utf16()));
        TPtrC16 tempPtr2( reinterpret_cast <const TUint16*>(lastName.utf16()));
        
        TBuf<KBufSize> landmarkName;
        landmarkName.Append( tempPtr1 );
        landmarkName.Append( tempPtr2 );
        
        TInt itemCount = 0;
        
  
        double longitude, latitude;
        
        foreach ( QContactAddress address, contactInfo.details<QContactAddress>() )
        {
          QStringList context = address.contexts();
          if ( context.isEmpty() ) // no context
          {
              //check whether geocordinate available
              TBool geoField = IsGeoCoordinateAvailable( 
                       contactInfo, NULL , latitude, longitude );
              if( geoField )
              {
                  // Get the default/prefered address details
                  preferedAddressLm = GetContactAddressDetailsLC( address );
                  TLocality loc( TCoordinate( latitude, longitude ), 0 );
                  // Fill the location details into the landmark object
                  preferedAddressLm->SetPositionL(loc);
                  preferedAddressLm->SetLandmarkNameL( landmarkName );
                  itemCount++;

              }
          }
          else if (context.first() == QContactAddress::ContextHome  )
          {
              //check whether geocordinate available
              TBool geoField = IsGeoCoordinateAvailable( 
                               contactInfo, QContactAddress::ContextHome, latitude, longitude );
              if( geoField )
              {
                  // Get the home address details
                  homeAddressLm = GetContactAddressDetailsLC( address );
                  TLocality loc(TCoordinate( latitude,longitude ), 0);
                  homeAddressLm->SetPositionL(loc);
                  homeAddressLm->SetLandmarkNameL( landmarkName );
                  itemCount++;
              }
          }
          else if (context.first() == QContactAddress::ContextWork )
          {
              //check whether geocordinate available
              TBool geoField = IsGeoCoordinateAvailable( 
                        contactInfo, QContactAddress::ContextWork, latitude, longitude );
              if( geoField )
              {
                  // Get the work address details
                  workAddressLm = GetContactAddressDetailsLC( address );
                  TLocality loc(TCoordinate( latitude, longitude ), 0);
                  // Fill the location details into the landmark object
                  workAddressLm->SetPositionL(loc);
                  workAddressLm->SetLandmarkNameL( landmarkName );
                  itemCount++;
              }
        
          }
        }

        // if the contact item has no validated address (preferef, home or work)
        if (!preferedAddressLm && !workAddressLm && !homeAddressLm)
        {
            if (aEvent.iType == EContactDbObserverEventContactChanged)
            {
                // If the contact is a modified one and it might already exist in
                // mylocations db, so delete it
                UpdateDatabaseL(NULL,
                        aEvent.iContactId, ESourceContactsPref,
                        EEntryDeleted);
                UpdateDatabaseL(NULL,
                        aEvent.iContactId, ESourceContactsWork,
                        EEntryDeleted);
                UpdateDatabaseL(NULL,
                        aEvent.iContactId, ESourceContactsHome,
                        EEntryDeleted);
            }
        }
        else
        {
            // There is atleast one valid address present.       

            // Get the TEntryChangeType for contacts.
            TEntryChangeType changeType = MapChangeType(
                    ESourceContactsPref, aEvent.iType);

            // if home address available, update Mylocations.  
            if (homeAddressLm)
            {
                UpdateDatabaseL(homeAddressLm,
                        aEvent.iContactId, ESourceContactsHome, changeType);
           }

            // if work address available, update Mylocations.  
            if (workAddressLm)
            {
                UpdateDatabaseL(workAddressLm,
                        aEvent.iContactId, ESourceContactsWork, changeType);
            }

            // if prefered address available, update Mylocations.  
            if (preferedAddressLm)
            {
                UpdateDatabaseL(
                        preferedAddressLm, aEvent.iContactId,
                        ESourceContactsPref, changeType);
           }
            CleanupStack::PopAndDestroy( itemCount );
        }  

    }
    else if (aEvent.iType == EContactDbObserverEventContactDeleted)
    {
        // the contact is deleted, so delete the corresponding entries from database.

        // delete prefered address in database
        UpdateDatabaseL(NULL, aEvent.iContactId,
                ESourceContactsPref, EEntryDeleted);

        // delete work address in database
        UpdateDatabaseL(NULL, aEvent.iContactId,
                ESourceContactsWork, EEntryDeleted);

        // delete home address in database
        UpdateDatabaseL(NULL, aEvent.iContactId,
                ESourceContactsHome, EEntryDeleted);
    }
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::TriggerMaptileRequestL()
// Process the contact address information for fetching maptile.
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::TriggerMaptileRequestL(TContactDbObserverEvent& aEvent)
{
    __TRACE_CALLSTACK;
    TLookupItem lookupItem;      
    lookupItem.iUid = aEvent.iContactId;
    // If contact is deleted delete from mylocations db
    if (aEvent.iType == EContactDbObserverEventContactDeleted)
    {        
        lookupItem.iSource = ESourceContactsPref;
        TRAP_IGNORE( ManipulateMapTileDataBaseL(lookupItem));

        lookupItem.iSource = ESourceContactsWork;
        TRAP_IGNORE( ManipulateMapTileDataBaseL(lookupItem));
        
        lookupItem.iSource = ESourceContactsHome;
        TRAP_IGNORE( ManipulateMapTileDataBaseL(lookupItem));
              
        MYLOCLOGSTRING("EContactDbObserverEventContactDeleted ");
        return;
    }

    //Get the contact item
    iEventType = aEvent.iType;
    QContact contactInfo = iContactManager->contact( aEvent.iContactId );
    
  
    CPosLandmark *preferedAddressLm = NULL;
    CPosLandmark *workAddressLm = NULL;
    CPosLandmark *homeAddressLm = NULL;
    
    TInt itemCount = 0;
    
    foreach ( QContactAddress address, contactInfo.details<QContactAddress>() )
    {
        QStringList context = address.contexts();
        if ( context.isEmpty() ) // no context
        {
            preferedAddressLm = GetContactAddressDetailsLC( address );
            itemCount++;
        }
        else if ( context.first() == QContactAddress::ContextHome  )
        {
            homeAddressLm = GetContactAddressDetailsLC( address );
            itemCount++;
        }
        else if ( context.first() == QContactAddress::ContextWork )
        {
            workAddressLm = GetContactAddressDetailsLC( address );
            itemCount++;
        }
    }
   
    switch (aEvent.iType)
    {
        case EContactDbObserverEventContactChanged:
        {
            MYLOCLOGSTRING("EContactDbObserverEventContactChanged" );
            MYLOCLOGSTRING1("iMapTileRequestQueue.Count()-%d",iMapTileRequestQueue.Count() );

            if (iMapTileRequestQueue.Count() > 0)
            {
                if (iMapTileRequestQueue[0]->iUId == aEvent.iContactId)
                {
                    CleanupStack::PopAndDestroy( itemCount );
                    MYLOCLOGSTRING("retrun from geolocation callback" );
                    return;
                }
            }
       
            // if default address available, update Mylocations. 
            lookupItem.iSource = ESourceContactsPref;
            if (preferedAddressLm)
            {
                MYLOCLOGSTRING("preferedAddressLm address changed" );

                if (iAddressCompare->IsAddressChangedL(*preferedAddressLm,
                    aEvent.iContactId, ESourceContactsPref))

                {
                    //remove entry from database
                    TRAP_IGNORE( ManipulateMapTileDataBaseL(lookupItem));
                    RequestMapTileImageL(*preferedAddressLm, ESourceContactsPref,
                            aEvent.iContactId);
                }
            }
            else
            {
                TRAP_IGNORE( ManipulateMapTileDataBaseL(lookupItem));
                
            }  

            // if work address available, update Mylocations.
            lookupItem.iSource = ESourceContactsWork;
            if (workAddressLm)
            {
                MYLOCLOGSTRING("workAddressLm address changed" );
                if (iAddressCompare->IsAddressChangedL(*workAddressLm,
                        aEvent.iContactId, ESourceContactsWork))
                //remove entry from database
                {
                    
                    TRAP_IGNORE( ManipulateMapTileDataBaseL(lookupItem));
                    RequestMapTileImageL(*workAddressLm, ESourceContactsWork,
                            aEvent.iContactId);
                }
            }
            else
            {
                TRAP_IGNORE( ManipulateMapTileDataBaseL(lookupItem));
               
            }
    
            // if home address available, update Mylocations.
            lookupItem.iSource = ESourceContactsHome;
            if (homeAddressLm)
            {
                MYLOCLOGSTRING("homeAddressLm address changed" );
    
                if (iAddressCompare->IsAddressChangedL(*homeAddressLm,
                        aEvent.iContactId, ESourceContactsHome))
    
                {
                    //remove entry from databse                    
                    TRAP_IGNORE( ManipulateMapTileDataBaseL(lookupItem));
                    RequestMapTileImageL(*homeAddressLm, ESourceContactsHome,
                            aEvent.iContactId);
                }
 
            }
            else
            {
                TRAP_IGNORE( ManipulateMapTileDataBaseL(lookupItem));
                
            }
            // }
            break;
        }    
        case EContactDbObserverEventContactAdded:
        {
            MYLOCLOGSTRING("EContactDbObserverEventContactAdded" );
            if (preferedAddressLm)
            {
                RequestMapTileImageL(*preferedAddressLm, ESourceContactsPref,
                        aEvent.iContactId);
            }
            if (workAddressLm)
            {
                RequestMapTileImageL(*workAddressLm, ESourceContactsWork,
                        aEvent.iContactId);
            }
            if (homeAddressLm)
            {
                RequestMapTileImageL(*homeAddressLm, ESourceContactsHome,
                        aEvent.iContactId);
            }
            break;
        }

    }
 
    CleanupStack::PopAndDestroy( itemCount );

}


// -----------------------------------------------------------------------------
// CMyLocationsEngine::RequestMapTileImageL()
// Request to get maptiel
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::RequestMapTileImageL(const TDesC& aAddressDetails,
        const TUidSourceType aAddressType, const TInt32 aUId)
{
    __TRACE_CALLSTACK;
    SetFolderPathL();
    TBuf<KImagePathSize> mImagePath;
    mImagePath.Copy(imageFilePath);
 

    CMapTileRequest* mapTileRequest = new (ELeave) CMapTileRequest;

    mapTileRequest->iAddressDetails = aAddressDetails.AllocL();
    mapTileRequest->iUId = aUId;
    mapTileRequest->iAddressType = aAddressType;
    mapTileRequest->iEventType = iEventType;
    mapTileRequest->iImagePath.Zero();
    mapTileRequest->iImagePath.Copy(mImagePath);
    if (iMapTileRequestQueue.Count() <= 0)
    {
        if ( KErrNone == RequestExecute(mapTileRequest) )
        {
            iMapTileRequestQueue.Append(mapTileRequest);
        }
        else
        {
            delete mapTileRequest;
        }       
    }
    else
    {
        MYLOCLOGSTRING("Added one more request to request queue" );
        iMapTileRequestQueue.Append(mapTileRequest);
    }
}
// -----------------------------------------------------------------------------
// CMyLocationsEngine::RequestMapTileImageL()
// Request to get maptiel
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::RequestMapTileImageL(CPosLandmark& aLandmark,
        const TUidSourceType aAddressType, const TInt32 aUId)
{
    __TRACE_CALLSTACK;

    MYLOCLOGSTRING("check folder path existance!");
    SetFolderPathL();
 

    CMapTileRequest* mapTileRequest = new (ELeave) CMapTileRequest;

    mapTileRequest->iLandmarkInfo = CPosLandmark::NewL(aLandmark);
    mapTileRequest->iUId = aUId;
    mapTileRequest->iAddressType = aAddressType;
    mapTileRequest->iEventType = iEventType;
    mapTileRequest->iImagePath.Zero();
    mapTileRequest->iImagePath.Copy(imageFilePath);
    MYLOCLOGSTRING1("RequestMapTileImageL() Queue count -%d",iMapTileRequestQueue.Count());

    if (iMapTileRequestQueue.Count() <= 0)
       {
          
           if( KErrNone == RequestExecute(mapTileRequest) )
           {
               iMapTileRequestQueue.Append(mapTileRequest);
           }
           else
           {
               delete mapTileRequest;
           }          
       }
       else
       {
           MYLOCLOGSTRING("Added one more request to request queue" );
           iMapTileRequestQueue.Append(mapTileRequest);
       }
    
   
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::RequestExecute()
// Executes the maptile image processing request
// -----------------------------------------------------------------------------
//
TInt CMyLocationsEngine::RequestExecute( CMapTileRequest* aMapTileRequest)
{
    __TRACE_CALLSTACK;
    TInt errorCode = KErrNone;
    MYLOCLOGSTRING1("Request address type - %d ",aMapTileRequest->iAddressType);
    switch (aMapTileRequest->iAddressType)
    {
        case ESourceCalendar:
        {
            TRAP(errorCode,iMapTileInterface->GetGeoCodeFromAddressL(*(aMapTileRequest->iAddressDetails),
                            aMapTileRequest->iImagePath, this ));        
            break;
        }
        case ESourceContactsPref:
        case ESourceContactsWork:
        case ESourceContactsHome:
        {
            TRAP(errorCode, iMapTileInterface->GetGeoCodeFromAddressL(aMapTileRequest->iLandmarkInfo,
                            aMapTileRequest->iImagePath, this));            
            break;
        }
        default:
            break;
    };  

    return errorCode;
}
 
// -----------------------------------------------------------------------------
// CMyLocationsEngine::GetLocationDetailsLC()
// get locatin details
// -----------------------------------------------------------------------------
//
CPosLandmark* CMyLocationsEngine::GetContactAddressDetailsLC( 
                                   QContactAddress& aContactAddress )
{
    __TRACE_CALLSTACK;
    CPosLandmark *landmark = NULL;
    
    QString country = aContactAddress.country();
    QString locality = aContactAddress.locality();
    QString street = aContactAddress.street();
    QString region = aContactAddress.region();
    QString postalcode = aContactAddress.postcode();
    
    landmark = CPosLandmark::NewL();
    CleanupStack::PushL(landmark);
    
    if ( !country.isEmpty() )
    {
        TPtrC16 tempText(reinterpret_cast<const TUint16*>(country.utf16()));
        landmark->SetPositionFieldL( EPositionFieldCountry, tempText );
    }
    if ( !locality.isEmpty() )
    {
        TPtrC16 tempText(reinterpret_cast<const TUint16*>(locality.utf16())); 
        landmark->SetPositionFieldL(EPositionFieldCity, tempText);
    }
    if ( !street.isEmpty() )
    { 
        TPtrC16 tempText(reinterpret_cast<const TUint16*>(street.utf16()));
        landmark->SetPositionFieldL(EPositionFieldStreet, tempText);
    }
    if ( !postalcode.isEmpty() )
    {
        TPtrC16 tempText(reinterpret_cast<const TUint16*>(postalcode.utf16()));
        landmark->SetPositionFieldL(EPositionFieldPostalCode, tempText);
    }

     return landmark;
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::MapsChangeType()
// Maps the source's change type to Mylocations entry change type
// -----------------------------------------------------------------------------
//
TEntryChangeType CMyLocationsEngine::MapChangeType(TUidSourceType aSrcType,
        TUint32 aChangeType)
{
    __TRACE_CALLSTACK;// return value
    TEntryChangeType retVal = EEntryUnknown;

    switch (aSrcType)
    {
    // if source type is calendar
    case ESourceCalendar:
    {
        switch( aChangeType )
            {
            case EChangeAdd:
                retVal = EEntryAdded;
                break;
            case EChangeDelete:
                retVal =  EEntryDeleted;
                break;
            case EChangeModify:
                retVal =  EEntryModified;
                break;
            }
        break;
    }
    // if source type is contacts
    case ESourceContactsPref:
    case ESourceContactsWork:
    case ESourceContactsHome:
    {
        switch (aChangeType)
        {
        case EContactDbObserverEventContactAdded:
            retVal = EEntryAdded;
            break;
        case EContactDbObserverEventContactDeleted:
            retVal = EEntryDeleted;
            break;
        case EContactDbObserverEventContactChanged:
            retVal = EEntryModified;
            break;
        }
        break;
    }

        // if source type is landmarks or maps history
    case ESourceLandmarks:
    {
        switch (aChangeType)
        {
        case EPosLmEventLandmarkCreated:
            retVal = EEntryAdded;
            break;
        case EPosLmEventLandmarkDeleted:
            retVal = EEntryDeleted;
            break;
        case EPosLmEventLandmarkUpdated:
            retVal = EEntryModified;
            break;
        }
        break;
    }
    case ESourceLandmarksCategory:
    {
        switch (aChangeType)
        {
        case EPosLmEventCategoryCreated:
            retVal = EEntryAdded;
            break;
        }
    }
        break;
    }
    return retVal;
}


// -----------------------------------------------------------------------------
// CMyLocationsEngine::IsGeoCoordinateAvailable()
// Checks whether geocoordinate available in the contact detail.
// -----------------------------------------------------------------------------
//
TBool CMyLocationsEngine::IsGeoCoordinateAvailable( QContact& aContact, 
        QString aAddressType, double& aLatitude , double& aLongitude )

{
      TBool geoFieldAvailable = EFalse;
      QContactGeoLocation geoLocation;
      
      foreach( geoLocation, aContact.details<QContactGeoLocation>() )
      {
          if( !geoLocation.isEmpty())
          {
              QStringList context = geoLocation.contexts();
              if ( context.isEmpty() )
              {
                  if ( aAddressType.isEmpty() )
                  {
                      geoFieldAvailable = ETrue;
                      break;
                  }
              }
              else if( context.first() == aAddressType )
              {
                  geoFieldAvailable = ETrue;
                  break;
              }
              
          }
          
      }
      if( geoFieldAvailable )
      {
          aLatitude = geoLocation.latitude();
          aLongitude = geoLocation.longitude();
      }
      return geoFieldAvailable;
}



// -----------------------------------------------------------------------------
// CMyLocationsEngine::RunL()
// Handles active object's request completion event.
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::RunL()
{
    __TRACE_CALLSTACK;
    switch (iLmEvent.iEventType)
    {
        case EPosLmEventLandmarkCreated:
        case EPosLmEventLandmarkUpdated:
        {
            CPosLandmark* readLandmark = iLandmarkDb->ReadLandmarkLC(
                    iLmEvent.iLandmarkItemId);
    
            if (readLandmark)
            {    
                // update the entry in database.
                UpdateDatabaseL( readLandmark,
                        iLmEvent.iLandmarkItemId, ESourceLandmarks, MapChangeType(
                                ESourceLandmarks, iLmEvent.iEventType ) );
    
                CleanupStack::PopAndDestroy(readLandmark);
            }
        }
        break;
        case EPosLmEventLandmarkDeleted:
        {
            // delete the corresponding entries in mylocations database.
            UpdateDatabaseL(NULL,
                    iLmEvent.iLandmarkItemId, ESourceLandmarks, EEntryDeleted);
        }
        break;

    }

    // start the change notifier again;
    StartLandmarksChangeNotifier();

}
// -----------------------------------------------------------------------------
// CMyLocationsEngine::DoCancel()
// Implements cancellation of an outstanding request.
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::DoCancel()
{
    __TRACE_CALLSTACK;
    iLandmarkDb->CancelNotifyDatabaseEvent();
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::GeoCodefetchingCompleted()
// Handles the maptile fetching completion event and updates the maptile lookup db.
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::GeoCodefetchingCompleted( TInt aErrCode, const TReal& aLatitude,
            const TReal& aLongitude, const TDesC& aMapTilePath )
{
    __TRACE_CALLSTACK;
    MYLOCLOGSTRING1("MapTilefetchingCompleted aErrCode - %d ",aErrCode);
    MYLOCLOGSTRING1("iMapTileRequestQueue.Count - %d",iMapTileRequestQueue.Count());

    if (iMapTileRequestQueue.Count() > 0)
    {
        MYLOCLOGSTRING1("No.of RequestQueue - %d",iMapTileRequestQueue.Count());
       
	    TLookupItem lookupItem;
        lookupItem.iSource = iMapTileRequestQueue[0]->iAddressType;
        lookupItem.iUid = iMapTileRequestQueue[0]->iUId;

        if (aErrCode == KErrNone)
        {
            UpdateGeoCodeToAppDataBase( aLatitude, aLongitude );
            
            TBool flag = EFalse;
            TRAP_IGNORE( flag = iMaptileDatabase->FindEntryByFilePathL(aMapTilePath) );
            if ( flag )
            {  
                MYLOCLOGSTRING1("%S - found in the DB",&aMapTilePath);
            
             				
                lookupItem.iFilePath.Copy(aMapTilePath);
                lookupItem.iFetchingStatus = EMapTileFectchingCompleted;
                TRAP_IGNORE( UpdateMaptileDatabaseL(iMapTileRequestQueue[0]->iEventType, lookupItem ) );              

                MYLOCLOGSTRING("UpdateMaptileDatabaseL handled");

                //Process the pending maptile requests
                ProcessNextMaptileRequest();
                
            }
            else
            {
                TRAPD( error, iMapTileInterface->GetMapTileL( aLatitude, aLongitude ) );
                if ( error != KErrNone )
                {
                    //Log error message
                     MYLOCLOGSTRING1("iMapTileInterface->GetMapTileL() status-%d",error);
                     MapTilefetchingCompleted(error, KNullDesC);
                }            
            }            
        }
        else
        {
		   if ( aErrCode == KErrCouldNotConnect )
		   {
		       lookupItem.iFetchingStatus = EMapTileFetchingNetworkError;
               iMyLocationThreeAMTimer->StartTimer();
              
		   }
		   else
		   {
		       lookupItem.iFetchingStatus = EMapTileFetchingUnknownError;
		   }
           TRAP_IGNORE( UpdateMaptileDatabaseL(iMapTileRequestQueue[0]->iEventType,lookupItem ) );
           
		   ProcessNextMaptileRequest();
        }
    }    
}

void CMyLocationsEngine::MyLocationThreeAMTimerExpiredL()
{
    //Forward the event for maptile fetching only if maptile plugin available
    if( iMaptileGeocoderPluginAvailable )
    {
        RArray<TLookupItem> iLookupItems;
        iMaptileDatabase->FindEntriesByMapTileFetchingStateL((TUint32)EMapTileFetchingNetworkError,
                                            iLookupItems);
        for( TUint32 i = 0; i < iLookupItems.Count(); i++ )
        {
            TLookupItem iItem = iLookupItems[i];
            switch( iItem.iSource )
            {
                // Get the home address details
                case ESourceContactsHome:
                case ESourceContactsWork:
                case ESourceContactsPref:
                     { 
                        QContact contactInfo = iContactManager->contact( iItem.iUid );
                        CPosLandmark *addressLm = NULL;
    
                        foreach ( QContactAddress address, contactInfo.details<QContactAddress>() )
                        {
                            QStringList context = address.contexts();
                            if ( ( context.isEmpty() && iItem.iSource == ESourceContactsPref ) ||
                                   ( context.first() == QContactAddress::ContextHome  && iItem.iSource == ESourceContactsHome ) ||
                                   ( context.first() == QContactAddress::ContextWork  && iItem.iSource == ESourceContactsWork ) ) 
                            {
                                // Get the default/prefered address details
                                addressLm = GetContactAddressDetailsLC( address );
                                if( addressLm ) 
                                {
                                    RequestMapTileImageL( *addressLm,
                                           ( TUidSourceType )iItem.iSource, iItem.iUid );
                                    CleanupStack::PopAndDestroy( addressLm );
                                    break;
                                }
                            }
                        }                    
                     }
                     break;
                
                case ESourceCalendar:
                     {
                        CCalEntry* calEntry = NULL;
                        calEntry = iCalView->FetchL(iItem.iUid);
                        if( calEntry )
                        {
                            CleanupStack::PushL(calEntry);
                            TPtrC address(calEntry->LocationL());
                            if(address.Length()>0)
                            {        
                                RequestMapTileImageL( address, ESourceCalendar, iItem.iUid);
                            }
                            CleanupStack::PopAndDestroy(calEntry);
                        }
                        else
                        {
                            iMaptileDatabase->DeleteEntryL( iItem );
                        }
                     }
                     break;
                 
                 default:
                     break;
              }
         }// end for
     }
}


// -----------------------------------------------------------------------------
// CMyLocationsEngine::MapTilefetchingCompleted()
// Handles the maptile fetching completion event and updates the maptile lookup db.
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::MapTilefetchingCompleted(TInt aErrCode,
        const TDesC& aMapTilePath)
{
    __TRACE_CALLSTACK;
    MYLOCLOGSTRING1("MapTilefetchingCompleted aErrCode - %d ",aErrCode);
    MYLOCLOGSTRING1("iMapTileRequestQueue.Count - %d",iMapTileRequestQueue.Count());

    if ( iMapTileRequestQueue.Count() > 0 )
    {

        MYLOCLOGSTRING1("No.of RequestQueue - %d",iMapTileRequestQueue.Count());

        TLookupItem lookupItem;
        lookupItem.iSource = iMapTileRequestQueue[0]->iAddressType;
        lookupItem.iUid = iMapTileRequestQueue[0]->iUId;

        if ( aErrCode == KErrNone )
        {           
            lookupItem.iFilePath.Copy(aMapTilePath);
   		    lookupItem.iFetchingStatus = EMapTileFectchingCompleted;
 
        }
        else if ( aErrCode == KErrCouldNotConnect )
		{
		    lookupItem.iFetchingStatus = EMapTileFetchingNetworkError;
            iMyLocationThreeAMTimer->StartTimer();
              
		}
		else
		{
		    lookupItem.iFetchingStatus = EMapTileFetchingUnknownError;
		}
		
        TRAP_IGNORE( UpdateMaptileDatabaseL(iMapTileRequestQueue[0]->iEventType,lookupItem ) );
    }
    
    ProcessNextMaptileRequest();
}


// -----------------------------------------------------------------------------
// CMyLocationsEngine::ProcessNextMaptileRequest()
// Process the next maptile request if any pending.
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::ProcessNextMaptileRequest()
{
    //Process the next request if it is pending
    if ( iMapTileRequestQueue.Count() > 0)
    {
        //Remove the current top request 
        delete iMapTileRequestQueue[0];
        iMapTileRequestQueue.Remove(0);
        iMapTileRequestQueue.Compress();
    
        //Process the next request in queue
        MYLOCLOGSTRING1("MapTile fetch completed request-%d",iMapTileRequestQueue.Count());
        for (TInt cnt = 0; cnt < iMapTileRequestQueue.Count(); cnt++)
        {          
            if ( KErrNone == RequestExecute( iMapTileRequestQueue[0]) )
            {
                   break;
            }
            else
            {
               //Process the next request in queue
               ProcessNextMaptileRequest();

            }
        }
    }
    else
    {
       MYLOCLOGSTRING("MapTile fetch completed no request in queue");
           iMapTileRequestQueue.Reset();
    }    
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::HandleMaptileDatabaseL()
// Handle maptile database(find/create/update/delete).
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::UpdateMaptileDatabaseL(
        TInt aEventType, TLookupItem& aLookupItem)
{
    __TRACE_CALLSTACK;
    if (aEventType == EContactDbObserverEventContactChanged || aEventType
            == EChangeModify || aEventType == EContactDbObserverEventContactAdded ||
            aEventType == EChangeAdd )
    {
        if (iMaptileDatabase->FindEntryL(aLookupItem))
        {
            iMaptileDatabase->UpdateEntryL(aLookupItem);
        }
        else
        {
            iMaptileDatabase->CreateEntryL(aLookupItem);
        }
    }
    iMyLocationsDatabaseManager->UpdateMapTilePath( aLookupItem.iUid, aLookupItem.iSource, 
                                            aLookupItem.iFilePath );    
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::RestGeoCodeCompleted()
// observed when rest geo codeing completed.
// started lat and lon field updation into contact db.
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::UpdateGeoCodeToAppDataBase( TReal aLatitude, TReal aLongitude )
{
    __TRACE_CALLSTACK;

    MYLOCLOGSTRING1("No. of iMapTileRequestQueue - %d",iMapTileRequestQueue.Count());
    if (iMapTileRequestQueue.Count() > 0)
    {
        switch (iMapTileRequestQueue[0]->iAddressType)
        {
        //TODO:
        case ESourceCalendar:
        {
           
            CPosLandmark *landmark=NULL;
            landmark=iMapTileInterface->GetLandMarkDetails();
            TRAP_IGNORE( UpdateDatabaseL( landmark, iMapTileRequestQueue[0]->iUId,
                ESourceCalendar,
                MapChangeType( ESourceCalendar, iMapTileRequestQueue[0]->iEventType ) ) );
            MYLOCLOGSTRING("Geo-codinate updated to calender db");                     
            break;
        }
        case ESourceContactsPref:
        case ESourceContactsWork:
        case ESourceContactsHome:
        {
            GeocodeUpdate::updateGeocodeToContactDB(
                    iMapTileRequestQueue[0]->iUId,
                    iMapTileRequestQueue[0]->iAddressType, aLatitude,
                    aLongitude);
            MYLOCLOGSTRING("Geo-codinate updated to contact db");
            break;
        }
        };

        
    }

}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::UpdateDatabaseL()
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::UpdateDatabaseL( CPosLandmark* aLandmark, const TUint32 aUid, 
        const TUint32 aSourceType, const TEntryChangeType aChangeType )
{
    __TRACE_CALLSTACK;
    Cancel();
    iMyLocationsDatabaseManager->UpdateDatabaseL( aLandmark, aUid, 
        aSourceType, aChangeType );
    if( aSourceType != ESourceLandmarks )
    {
        StartLandmarksChangeNotifier();
    }
    
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::ManipulateMapTileDataBaseL()
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::ManipulateMapTileDataBaseL(TLookupItem aLookupItem)
{
    __TRACE_CALLSTACK;
    TBool entryAvailable=EFalse;
    entryAvailable = iMaptileDatabase->FindEntryL(aLookupItem);
    iMaptileDatabase->DeleteEntryL(aLookupItem);
    if (entryAvailable)
    {
        iMaptileDatabase->DeleteMapTileL(aLookupItem);
    }
            
}
//End of file

