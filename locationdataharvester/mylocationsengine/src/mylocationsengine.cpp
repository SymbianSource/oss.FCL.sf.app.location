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
#include <QFile>
#include <f32file.h>
#include <calchangecallback.h> 
#include <cntitem.h>
#include <cntfldst.h>
#include <EPos_CPosLmDatabaseManager.h>
#include <lbsposition.h>
#include <bautils.h>
#include <f32file.h>
#include <locationservicedefines.h>
#include <e32property.h>
#include "contactsubscriber.h"
#include "calendarsubscriber.h"
#include "mylocationsengine.h"
#include "geocodeupdate.h" //header for GeocodeUpdate class
//handle for CMyLocationsHistoryDbObserver class
#include "mylocationlogger.h"
#if ( defined __WINSCW__ ) || ( defined __WINS__ )
_LIT ( KImageStorageDrive, "C:\\MapTile\\");
#endif
_LIT(KFolderName,":\\MapTile\\");
const TInt KImagePathSize=36;
const TInt KDefaultFilePathSize = 20;
const TUid KMaptileStatusPublish={0x2002680A};
enum TMaptileStatusKeys {EMaptileStatusInteger=0x1};

//Protocol : [appid-addresstype-maptilestatus]
_LIT8( KMaptileStatusFormat, "%d-%d-%d" );
const TInt KProtocolBufferSize = 16;

const QString KSpace(" ");

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
	
    iGeocodeUpdate = new GeocodeUpdate();
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
    iContactSubscriber = CContactSubscriber::NewL(this);
    iCalendarSubscriber = CCalendarSubscriber::NewL(this);
    TInt ret = RProperty::Define( KMaptileStatusPublish, EMaptileStatusInteger, RProperty::EByteArray  );

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
            iCalSession(NULL),
            iCalView(NULL), 
	        iContactsDb(NULL), 
	        iContactChangeNotifier(NULL), 
			iLandmarkDb(NULL),
            iMapTileInterface(NULL),
            iMyLocationsDatabaseManager(NULL),
            iMaptileDatabase(NULL),
            iMyLocationThreeAMTimer(NULL),
            iMaptileGeocoderPluginAvailable(EFalse),
            iCalenderNotification(NULL),
            iContactManager(NULL),
            iContactSubscriber(NULL),
            iCalendarSubscriber(NULL),
            iGeocodeUpdate(NULL),
            iLastContactId( -1 ) ,
            iLastCalendarId( 0 )
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
   
    Cancel();
    
    delete iContactChangeNotifier;

    delete iMyLocationsDatabaseManager;

    delete iLandmarkDb;

    delete iContactsDb;
   
    delete iCalView;
	
    delete iCalSession;
	 
	delete iMapTileInterface;

    delete iMaptileDatabase;
       
    delete iCalenderNotification;
    
	delete iMyLocationThreeAMTimer;
		
    delete iContactManager;
    
    delete iContactSubscriber;

    delete iCalendarSubscriber;
    
    delete iGeocodeUpdate;
    
    for( TInt index = 0; index < iAddressInfo.Count(); index++ )
    {
        delete iAddressInfo[index];
        iAddressInfo.Remove(index);
        iAddressInfo.Compress();
    }
    
    iAddressInfo.Reset();
    
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
// CMyLocationsEngine::GetChangeNoficationL()
// To get callbacks through publisher from contact context
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::GetChangeNotificationL( TInt &aId, TInt& addressType, TInt& addressCount  )
{
    __TRACE_CALLSTACK;
   
    iLastContactId = aId;
    
    //If the requested id is already in queue, just return
    for( TInt index = 0 ; index < iMapTileRequestQueue.Count(); index++ )
    {
        if( iLastContactId == iMapTileRequestQueue[index]->iUId )
        {           
            MYLOCLOGSTRING("contact id is in queue");
            return;
        }
    }
    
    TAppAddressInfo* addressInfo = new (ELeave) TAppAddressInfo;
    addressInfo->iUid = aId;
    addressInfo->iAddressType =  addressType;
    //Memory will be freed when the queue is deleted
    if( iAddressInfo.Append(addressInfo) != KErrNone )
    {
        delete addressInfo;
    }
    
    //Get all 3 adress
    if( addressCount > 1 && iAddressInfo.Count() < addressCount )
        return;  
    
    
    for( TInt index = 0; index < iAddressInfo.Count(); index++ )
    {
        TUidSourceType type = static_cast<TUidSourceType>(iAddressInfo[index]->iAddressType );
    
        //if type is contact
        if( type == ESourceContactsPref || type == ESourceContactsWork || 
                      type == ESourceContactsHome )
        {
            QContact contactInfo = iContactManager->contact( iAddressInfo[index]->iUid );
            
            CPosLandmark *contactAddressLm = NULL;
          
            TInt itemCount = 0;
            
            foreach ( QContactAddress address, contactInfo.details<QContactAddress>() )
            {
                QStringList context = address.contexts();
                if ( context.isEmpty()  && type ==  ESourceContactsPref ) // no context
                {
                    contactAddressLm = GetContactAddressDetailsLC( address );
                    itemCount++;
                    break;
                }
                else if ( !context.isEmpty() && context.first() == QContactAddress::ContextHome &&
                                    type == ESourceContactsHome )
                {
                    contactAddressLm = GetContactAddressDetailsLC( address );
                    itemCount++;
                    break;
                }
                else if ( !context.isEmpty() && context.first() == QContactAddress::ContextWork  && 
                                type == ESourceContactsWork )
                {
                    contactAddressLm = GetContactAddressDetailsLC( address );
                    itemCount++;
                    break;
                }
            }

            //Update the entry with inprogress status
            TLookupItem lookupItem;
            lookupItem.iUid = iAddressInfo[index]->iUid;
            lookupItem.iSource = type;
            lookupItem.iFilePath.Zero();
            lookupItem.iFetchingStatus = EMapTileFetchingInProgress;
            iMaptileDatabase->UpdateEntryL( lookupItem );
            
            //Request for maptile fetching
            if( contactAddressLm )
            {
                RequestMapTileImageL( *contactAddressLm, type, 
                         iAddressInfo[index]->iUid, EContactDbObserverEventContactChanged );
            }
            
            CleanupStack::PopAndDestroy( itemCount );
        }
    }
    for( TInt index = 0; index < iAddressInfo.Count(); index++ )
    {
	    //free the allocated memory
        delete iAddressInfo[index];
		iAddressInfo.Remove(index);
        iAddressInfo.Compress();
	}
    
    iAddressInfo.Reset();
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::SubscribeFromCalendarL()
// To get callbacks through publisher from calendar context
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::SubscribeFromCalendarL(TInt aId)
{
    __TRACE_CALLSTACK;
    iLastCalendarId = aId;
    for ( int index =0;iMapTileRequestQueue.Count()>index ;index++)
    {
        if( iLastCalendarId == iMapTileRequestQueue[index]->iUId )
        {            
            return;
        }
    }
    
    CCalEntry* calEntry = NULL;
    calEntry = iCalView->FetchL(aId);
    CleanupStack::PushL(calEntry);
    TPtrC address(calEntry->LocationL());
    if( address.Length()>0 )
    {
        TLookupItem lookupItem;
        lookupItem.iUid = aId;
        lookupItem.iSource = ESourceCalendar;
        lookupItem.iFilePath.Zero();
        lookupItem.iFetchingStatus = EMapTileFetchingInProgress;
        iMaptileDatabase->UpdateEntryL( lookupItem );
        RequestMapTileImageL( address, ESourceCalendar, aId , EChangeModify );
    }
    CleanupStack::PopAndDestroy(calEntry);
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
   
    iGeocodeUpdate->createContactdb();
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

        // Check if this is some undefined change in calendar db. 
        if( aChangeItems[0].iChangeType == EChangeUndefined && aChangeItems[0].iEntryType == EChangeEntryAll )
        {
            // Refresh the calendar related entries in the location and maptiledb.
            RefreshCalendarEntryListL();
            break;
        }
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
            TRAP_IGNORE( CalenderEntryModifyL(calChangeEntry) ) ;
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
// CMyLocationsEngine::RefreshCalendarEntryListL()
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::RefreshCalendarEntryListL()
{
    //Get all the calendar ids and check its validity. 
    //Delete if they are no more valid.
    RArray<TUint32> ids;
    iMaptileDatabase->GetAllCalendarIdsL( ids );
    for( TInt i = 0; i < ids.Count(); i++ )
    {
        if( !IsCalendarEntryValidL( ids[i] ) )
        {
            TLookupItem lookupItem;
            lookupItem.iSource = ESourceCalendar;
            lookupItem.iUid = ids[i];
            TRAP_IGNORE( ManipulateMapTileDataBaseL( lookupItem ) );           
            TRAP_IGNORE( UpdateDatabaseL( NULL, 
                    ids[i], ESourceCalendar, EEntryDeleted ) );
        
        }        
    }
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::IsCalendarEntryValidL()
// -----------------------------------------------------------------------------
//
TBool CMyLocationsEngine::IsCalendarEntryValidL( TUint32 aId )
{
    return ( ( iCalView->FetchL( aId ) == NULL ) ? EFalse:ETrue );
}
// -----------------------------------------------------------------------------
// CMyLocationsEngine::CalenderEntryAddedL()
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::CalenderEntryAddedL(TCalChangeEntry aCalChangeEntry)
{
    __TRACE_CALLSTACK;
    
    TUint32 entryId=0;
    entryId=aCalChangeEntry.iEntryId;
    //create entry in the data base and maintain a fetching state.
    TLookupItem lookupItem;
    lookupItem.iUid = entryId ;
    lookupItem.iSource = ESourceCalendar;
    lookupItem.iFilePath.Zero();
    lookupItem.iFetchingStatus = EMapTileFetchingInProgress;     
    TRAP_IGNORE( iMaptileDatabase->CreateEntryL(lookupItem) );
    CCalEntry* calEntry = NULL;
    calEntry = iCalView->FetchL( entryId );
    CleanupStack::PushL(calEntry);
    TPtrC address(calEntry->LocationL());     
    if (address.Length() > 0)
    {
        RequestMapTileImageL( address, ESourceCalendar, entryId , EChangeAdd );
    }  
    CleanupStack::PopAndDestroy(calEntry);
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::CalenderEntryModifyL()
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::CalenderEntryModifyL(TCalChangeEntry aCalChangeEntry)
{
    __TRACE_CALLSTACK;
    TUint32 entryId = 0;
    entryId = aCalChangeEntry.iEntryId;
    TLookupItem lookupItem;
    lookupItem.iSource = ESourceCalendar;
    lookupItem.iUid = entryId;
    CCalEntry* calEntry = NULL;
    calEntry = iCalView->FetchL(entryId);
    CleanupStack::PushL(calEntry);
    if(iGeocodeUpdate->isGeocodeNotAvailable(entryId))
    {        
        TPtrC address(calEntry->LocationL());
        if (iMyLocationsDatabaseManager->CheckIfAddressChanged(address, entryId,
                ESourceCalendar))
        {
            lookupItem.iFilePath.Zero();
            lookupItem.iFetchingStatus = EMapTileFetchingInProgress;
            TRAP_IGNORE( iMaptileDatabase->ReSetEntryL(lookupItem) );
            if (address.Length() > 0)
            {
                RequestMapTileImageL(address, ESourceCalendar, entryId , EChangeModify);
            }
            else
            {
                UpdateDatabaseL(NULL, entryId, ESourceCalendar, EEntryDeleted);
            }
            if ( lookupItem.iFilePath.Length() > 0 )
            {
                iMaptileDatabase->DeleteMapTileL(lookupItem);
            }
    
        }        
    }   
    CleanupStack::PopAndDestroy(calEntry);
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::HandleDatabaseEventL()
// Callback that provides information about the contact database change event.
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
{
    __TRACE_CALLSTACK;
    
    //Forward the event for maptile fetching only if maptile plugin available
    if( iMaptileGeocoderPluginAvailable )
    {
        TriggerMaptileRequestL(aEvent);
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
    if ( aEvent.iType == EContactDbObserverEventContactDeleted || aEvent.iType == EContactDbObserverEventOwnCardDeleted )
    {        
        //Delete the entries from maptile database
        lookupItem.iSource = ESourceContactsPref;
        TRAP_IGNORE( ManipulateMapTileDataBaseL(lookupItem));

        lookupItem.iSource = ESourceContactsWork;
        TRAP_IGNORE( ManipulateMapTileDataBaseL(lookupItem));
        
        lookupItem.iSource = ESourceContactsHome;
        TRAP_IGNORE( ManipulateMapTileDataBaseL(lookupItem));
              
        // Delete entries from  mylocation database
        TRAP_IGNORE( UpdateDatabaseL( NULL, aEvent.iContactId,
                     ESourceContactsPref, EEntryDeleted ));
       
        TRAP_IGNORE( UpdateDatabaseL( NULL, aEvent.iContactId,
               ESourceContactsHome, EEntryDeleted ));

        TRAP_IGNORE( UpdateDatabaseL( NULL, aEvent.iContactId,
                               ESourceContactsWork, EEntryDeleted ) );  
        
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
        case EContactDbObserverEventOwnCardChanged:
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

            TBuf<KBufSize> landmarkName;
            GetContactName(aEvent.iContactId,landmarkName);

            // if default address available, update Mylocations. 
            lookupItem.iSource = ESourceContactsPref;
            if (preferedAddressLm)
            {
                iMyLocationsDatabaseManager->UpdateEntryName( aEvent.iContactId, ESourceContactsPref, 
                                     landmarkName );
                MYLOCLOGSTRING("preferedAddressLm address changed" );

                if ( iMyLocationsDatabaseManager->CheckIfAddressChanged(*preferedAddressLm,
                    aEvent.iContactId, ESourceContactsPref) )

                {
                    lookupItem.iFilePath.Zero();
                    lookupItem.iFetchingStatus = EMapTileFetchingInProgress;
                    TRAP_IGNORE( iMaptileDatabase->ReSetEntryL(lookupItem) );
                    
                    RequestMapTileImageL(*preferedAddressLm, ESourceContactsPref,
                            aEvent.iContactId, iEventType );

                    if ( lookupItem.iFilePath.Length() > 0 )
                    {
                        iMaptileDatabase->DeleteMapTileL(lookupItem);
                    }
                    
                }
            }
            else
            {
            	TRAP_IGNORE( UpdateDatabaseL( NULL, aEvent.iContactId,
                     ESourceContactsPref, EEntryDeleted ));
                TRAP_IGNORE( ManipulateMapTileDataBaseL(lookupItem));
                
            }
            
            // if home address available, update Mylocations.
            lookupItem.iSource = ESourceContactsHome;
            if (homeAddressLm)
            {
                iMyLocationsDatabaseManager->UpdateEntryName( aEvent.iContactId, ESourceContactsHome, 
                                     landmarkName );
                MYLOCLOGSTRING("homeAddressLm address changed" );
                if ( iMyLocationsDatabaseManager->CheckIfAddressChanged(*homeAddressLm,
                        aEvent.iContactId, ESourceContactsHome) )
                {
                    lookupItem.iFilePath.Zero();
                    lookupItem.iFetchingStatus = EMapTileFetchingInProgress;
                    TRAP_IGNORE( iMaptileDatabase->ReSetEntryL(lookupItem) )
                    //remove entry from databse                    
                    RequestMapTileImageL(*homeAddressLm, ESourceContactsHome, aEvent.iContactId,
                        iEventType);
                    if (lookupItem.iFilePath.Length() > 0) 
                    {
                        iMaptileDatabase->DeleteMapTileL(lookupItem);
                    }                 
                }
            }
            else
            {
                TRAP_IGNORE( UpdateDatabaseL( NULL, aEvent.iContactId,
                     ESourceContactsHome, EEntryDeleted ));            
                TRAP_IGNORE( ManipulateMapTileDataBaseL(lookupItem));
                
            }
 

            // if work address available, update Mylocations.
            lookupItem.iSource = ESourceContactsWork;
            if (workAddressLm)
            {
            	  iMyLocationsDatabaseManager->UpdateEntryName( aEvent.iContactId, ESourceContactsWork, 
                                     landmarkName );

                MYLOCLOGSTRING("workAddressLm address changed" );
                if ( iMyLocationsDatabaseManager->CheckIfAddressChanged(*workAddressLm,
                        aEvent.iContactId, ESourceContactsWork) )
                {
                    lookupItem.iFilePath.Zero();
                    lookupItem.iFetchingStatus = EMapTileFetchingInProgress;
                    TRAP_IGNORE( iMaptileDatabase->ReSetEntryL(lookupItem) )

                    RequestMapTileImageL(*workAddressLm, ESourceContactsWork,
                            aEvent.iContactId, iEventType);
                    if (lookupItem.iFilePath.Length() > 0) 
                    {
                        iMaptileDatabase->DeleteMapTileL(lookupItem);
                    }
                } 
            }
            else
            {
                TRAP_IGNORE( UpdateDatabaseL( NULL, aEvent.iContactId,
                     ESourceContactsWork, EEntryDeleted ));            
                TRAP_IGNORE( ManipulateMapTileDataBaseL(lookupItem));
               
            }
            break;
        }    
        case EContactDbObserverEventContactAdded:
        {
            TLookupItem lookupItem;
            lookupItem.iUid = aEvent.iContactId;
            lookupItem.iFilePath.Zero();
            lookupItem.iFetchingStatus = EMapTileFetchingInProgress;
            
            MYLOCLOGSTRING("EContactDbObserverEventContactAdded" );
            if (preferedAddressLm)
            {
                //create entry in the data base and maintain a fetching state.
                lookupItem.iSource = ESourceContactsPref;
                iMaptileDatabase->CreateEntryL(lookupItem);
                RequestMapTileImageL(*preferedAddressLm, ESourceContactsPref,
                        aEvent.iContactId, iEventType);
            }
            if (homeAddressLm)
            {
                lookupItem.iSource = ESourceContactsHome;
                iMaptileDatabase->CreateEntryL(lookupItem);
                RequestMapTileImageL(*homeAddressLm, ESourceContactsHome,
                        aEvent.iContactId, iEventType);
            }
            if (workAddressLm)
            {
                lookupItem.iSource = ESourceContactsWork;
                iMaptileDatabase->CreateEntryL(lookupItem);
                RequestMapTileImageL(*workAddressLm, ESourceContactsWork,
                        aEvent.iContactId, iEventType);
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
        const TUidSourceType aAddressType, const TInt32 aUId ,const TInt aEventType)
{
    __TRACE_CALLSTACK;
    SetFolderPathL();
    TBuf<KImagePathSize> mImagePath;
    mImagePath.Copy(imageFilePath);
 

    CMapTileRequest* mapTileRequest = new (ELeave) CMapTileRequest;

    mapTileRequest->iAddressDetails = aAddressDetails.AllocL();
    mapTileRequest->iUId = aUId;
    mapTileRequest->iAddressType = aAddressType;
    mapTileRequest->iEventType = aEventType;
    mapTileRequest->iImagePath.Zero();
    mapTileRequest->iImagePath.Copy(mImagePath);
    
    TInt error = KErrNone;
    
    if (iMapTileRequestQueue.Count() <= 0)
    {
        error = iMapTileRequestQueue.Append(mapTileRequest);
        if ( KErrNone == error )
        {
            error = RequestExecute(mapTileRequest);    
        }
    }
    else
    {
        MYLOCLOGSTRING("Added one more request to request queue" );
        error = iMapTileRequestQueue.Append(mapTileRequest);
    }
    
    //If any error , free the allocated memory
    if( error != KErrNone )
    {
        delete mapTileRequest;
    }
    
}
// -----------------------------------------------------------------------------
// CMyLocationsEngine::RequestMapTileImageL()
// Request to get maptiel
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::RequestMapTileImageL(CPosLandmark& aLandmark,
                    const TUidSourceType aAddressType, const TInt32 aUId,
                    const TInt aEventType )
{
    __TRACE_CALLSTACK;

    MYLOCLOGSTRING("check folder path existance!");
    SetFolderPathL();
 

    CMapTileRequest* mapTileRequest = new (ELeave) CMapTileRequest;

    mapTileRequest->iLandmarkInfo = CPosLandmark::NewL(aLandmark);
    mapTileRequest->iUId = aUId;
    mapTileRequest->iAddressType = aAddressType;
    mapTileRequest->iEventType = aEventType;
    mapTileRequest->iImagePath.Zero();
    mapTileRequest->iImagePath.Copy(imageFilePath);
    MYLOCLOGSTRING1("RequestMapTileImageL() Queue count -%d",iMapTileRequestQueue.Count());

    TInt error = KErrNone;
    
    if (iMapTileRequestQueue.Count() <= 0)
       {
           error = iMapTileRequestQueue.Append(mapTileRequest);
           //error = RequestExecute(mapTileRequest);
           if( KErrNone == error  )
           {
               //error = iMapTileRequestQueue.Append(mapTileRequest);
               error = RequestExecute(mapTileRequest);
           }
  
       }
       else
       {
           MYLOCLOGSTRING("Added one more request to request queue" );
           error = iMapTileRequestQueue.Append(mapTileRequest);
       }
    
       //If any error, free the memory allocated
       if( error != KErrNone )
       {
           delete mapTileRequest;
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
// CMyLocationsEngine::GetContactAddressDetailsLC()
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
    
                // update the entry in database.
                UpdateDatabaseL( readLandmark,
                        iLmEvent.iLandmarkItemId, ESourceLandmarks, MapChangeType(
                                ESourceLandmarks, iLmEvent.iEventType ) );
    
                CleanupStack::PopAndDestroy(readLandmark);
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
    MYLOCLOGSTRING1("GeoCodefetchingCompleted aErrCode - %d ",aErrCode);
    MYLOCLOGSTRING1("iMapTileRequestQueue.Count - %d",iMapTileRequestQueue.Count());

    TBuf8<KProtocolBufferSize> buffer;
    
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
                //Publish the maptile status
                if( iLastContactId == iMapTileRequestQueue[0]->iUId )
                {
                    buffer.Zero();
                    buffer.Format( KMaptileStatusFormat, iLastContactId, lookupItem.iSource, lookupItem.iFetchingStatus );
                    RProperty::Set( KMaptileStatusPublish, EMaptileStatusInteger, buffer );
                }
                //Publish the maptile status ,if it was from calendar
                if( iLastCalendarId == iMapTileRequestQueue[0]->iUId )
                {
                    buffer.Zero();
                    buffer.Format( KMaptileStatusFormat, iLastCalendarId, lookupItem.iSource, lookupItem.iFetchingStatus );
                    RProperty::Set( KMaptileStatusPublish, EMaptileStatusInteger, buffer );
                }
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
           
           //Publish the maptile status
		   if( iLastContactId == iMapTileRequestQueue[0]->iUId )
		   {
		      buffer.Zero();
		      buffer.Format( KMaptileStatusFormat, iLastContactId, lookupItem.iSource, lookupItem.iFetchingStatus );
		      RProperty::Set( KMaptileStatusPublish, EMaptileStatusInteger, buffer );
		   }
            //Publish the maptile status ,if it was from calendar
		   if( iLastCalendarId == iMapTileRequestQueue[0]->iUId )
            {
		      buffer.Zero();
              buffer.Format( KMaptileStatusFormat, iLastCalendarId, lookupItem.iSource, lookupItem.iFetchingStatus );
              RProperty::Set( KMaptileStatusPublish, EMaptileStatusInteger, buffer );
            }
		   
		   ProcessNextMaptileRequest();
        }
    }    
}




// -----------------------------------------------------------------------------
// CMyLocationsEngine::MyLocationThreeAMTimerExpiredL()
// Triggers the maptile fetching at 3.00 for the network failure cases.
// -----------------------------------------------------------------------------
//
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
                            if ( ( context.isEmpty() && iItem.iSource == ESourceContactsPref  ) 
                                 ||
                                 ( !context.isEmpty() && 
                                    ( ( context.first() == QContactAddress::ContextHome  && iItem.iSource == ESourceContactsHome ) ||
                                      ( context.first() == QContactAddress::ContextWork  && iItem.iSource == ESourceContactsWork ) ) ) )
                            {
                                // Get the default/prefered address details
                                addressLm = GetContactAddressDetailsLC( address );
                                if( addressLm ) 
                                {
                                    RequestMapTileImageL( *addressLm,
                                           ( TUidSourceType )iItem.iSource, iItem.iUid, EContactDbObserverEventContactChanged );
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
                                RequestMapTileImageL( address, ESourceCalendar, iItem.iUid , EChangeModify);
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
            CreateMultipleMaptiles( aMapTilePath );
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
		
        TRAP_IGNORE( UpdateMaptileDatabaseL( iMapTileRequestQueue[0]->iEventType,lookupItem ) );

        // if the source type is contacts, update the contact name into the locationdatalookupdb. 
        // This has to be done, because there is a possibility  that the user might change the 
        // contact name between geocodefetchingcomplete and maptilefetchingcomplete.
        if( iMapTileRequestQueue[0]->iAddressType == ESourceContactsPref ||
            iMapTileRequestQueue[0]->iAddressType == ESourceContactsHome ||
            iMapTileRequestQueue[0]->iAddressType == ESourceContactsWork )
        {
            TBuf<KBufSize> landmarkName;
            GetContactName( iMapTileRequestQueue[0]->iUId, landmarkName );
            
            iMyLocationsDatabaseManager->UpdateEntryName( 
                    iMapTileRequestQueue[0]->iUId, ESourceContactsPref, landmarkName );
            iMyLocationsDatabaseManager->UpdateEntryName( 
                    iMapTileRequestQueue[0]->iUId, ESourceContactsHome, landmarkName );
            iMyLocationsDatabaseManager->UpdateEntryName( 
                    iMapTileRequestQueue[0]->iUId, ESourceContactsWork, landmarkName );
        }

        //Publish the maptile status , if it was from contact
        if( iLastContactId == iMapTileRequestQueue[0]->iUId )
        {
             TBuf8<KProtocolBufferSize> buffer;
             buffer.Format( KMaptileStatusFormat, iLastContactId, lookupItem.iSource, lookupItem.iFetchingStatus );
             RProperty::Set( KMaptileStatusPublish, EMaptileStatusInteger, buffer );
        }
        //Publish the maptile status ,if it was from calendar
        if( iLastCalendarId == iMapTileRequestQueue[0]->iUId )
        {
            TBuf8<KProtocolBufferSize> buffer;
            buffer.Format( KMaptileStatusFormat, iLastCalendarId, lookupItem.iSource, lookupItem.iFetchingStatus );
            RProperty::Set( KMaptileStatusPublish, EMaptileStatusInteger, buffer );
        }
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
    if (aLookupItem.iFetchingStatus == EMapTileFetchingUnknownError
            || aLookupItem.iFetchingStatus == EMapTileFetchingNetworkError)
    {
        TRAP_IGNORE( UpdateDatabaseL( NULL,
                        aLookupItem.iUid, aLookupItem.iSource, EEntryDeleted ) );

    }
    else
    {      
        TPtrC8 ptr(reinterpret_cast<const TUint8*>(MAPTILE_IMAGE_HURRIGANES));   
        HBufC* buffer = NULL;
        buffer=HBufC::NewLC(ptr.Length());
        buffer->Des().Copy(ptr);
        if(buffer)
        {
            aLookupItem.iFilePath.Append(*buffer);
        }
        CleanupStack::PopAndDestroy(buffer);
        iMyLocationsDatabaseManager->UpdateMapTilePath(aLookupItem.iUid,
                aLookupItem.iSource, aLookupItem.iFilePath);
    }
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
            iGeocodeUpdate->updateGeocodeToCalenderDB(iMapTileRequestQueue[0]->iUId,
                    aLatitude,aLongitude);                    
            CPosLandmark *landmark = NULL;
            landmark = iMapTileInterface->GetLandMarkDetails();
            if (NULL != landmark)
            {
                TRAP_IGNORE( landmark->SetPositionFieldL(EPositionFieldComment,
                        iMapTileRequestQueue[0]->iAddressDetails->Des() ) );
                TRAP_IGNORE( UpdateDatabaseL( landmark, iMapTileRequestQueue[0]->iUId,
                                ESourceCalendar,
                                MapChangeType( ESourceCalendar, iMapTileRequestQueue[0]->iEventType ) ) );
            }
            MYLOCLOGSTRING("Geo-codinate updated to calender db");
            break;
        }
        case ESourceContactsPref:
        case ESourceContactsWork:
        case ESourceContactsHome:
        {
            iGeocodeUpdate->updateGeocodeToContactDB(
                    iMapTileRequestQueue[0]->iUId,
                    iMapTileRequestQueue[0]->iAddressType, aLatitude,
                    aLongitude);
            TBuf<KBufSize> landmarkName;
            GetContactName(iMapTileRequestQueue[0]->iUId,landmarkName);
            //Update mylocation database 
            iMapTileRequestQueue[0]->iLandmarkInfo->SetLandmarkNameL( landmarkName );
            TRAP_IGNORE( UpdateDatabaseL( 
                    iMapTileRequestQueue[0]->iLandmarkInfo, 
                    iMapTileRequestQueue[0]->iUId,
                    iMapTileRequestQueue[0]->iAddressType,
                    MapChangeType( 
                        static_cast<TUidSourceType>( iMapTileRequestQueue[0]->iAddressType ), 
                                                  iMapTileRequestQueue[0]->iEventType )));
            
            MYLOCLOGSTRING("Geo-codinate updated to contact db");
            break;
        }
        };
    }

}


// -----------------------------------------------------------------------------
// CMyLocationsEngine::GetContactName()
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::GetContactName( TInt32 aUId,TDes& aName)
{
	QContact contactInfo = iContactManager->contact( aUId );
	QContactName name = contactInfo.detail( QContactName::DefinitionName );
	QString firstName = name.firstName();
	QString lastName = name.lastName();
	QString fullName("");
	
	if( lastName.isEmpty() )
    {
	    fullName = firstName;
    }
	else
    {
        if( !firstName.isEmpty() )
        {
            fullName = firstName + KSpace;
        }
        fullName = fullName + lastName; 
    }
	
	aName.Copy( reinterpret_cast<const TUint16*>(fullName.utf16()) );
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::UpdateDatabaseL()
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::UpdateDatabaseL( CPosLandmark* aLandmark, const TUint32 aUid, 
        const TUint32 aSourceType, const TEntryChangeType aChangeType )
{
    __TRACE_CALLSTACK;
    if(IsActive())
    {
        Cancel();
    }
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
void CMyLocationsEngine::ManipulateMapTileDataBaseL(TLookupItem& aLookupItem)
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


// -----------------------------------------------------------------------------
// CMyLocationsEngine::CreateMultipleMaptiles()
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::CreateMultipleMaptiles( const TDesC& aMaptilePath )
{
    __TRACE_CALLSTACK;
    QString filePath =  QString::fromUtf16( aMaptilePath.Ptr(), aMaptilePath.Length() );
    
    //Portrait image , common for contacts/calendar/context menu
    CropAndSaveImage( filePath, 
                      MaptilePortraitWidth, 
                      MaptilePortraitHeight, 
                      QString(),
                      QString( MAPTILE_IMAGE_PORTRAIT ) );
    
    //Landscape image for contacts
    CropAndSaveImage( filePath, 
                      MaptileContactLandscapeWidth, 
                      MaptileContactLandscapeHeight,
                      QString( MAPTILE_IMAGE_CONTACT ),
                      QString( MAPTILE_IMAGE_LANDSCAPE ) );
    
    //Ladscape image for calendar
    CropAndSaveImage( filePath, 
                      MaptileCalendarLandscapeWidth, 
                      MaptileCalendarLandscapeHeight,
                      QString( MAPTILE_IMAGE_CALENDAR ),
                      QString( MAPTILE_IMAGE_LANDSCAPE ) );
    
    //Image for hurriganes
    CropAndSaveImage( filePath, 
                      MaptileHurriganesWidth, 
                      MaptileHurriganesHeight,
                      QString( MAPTILE_IMAGE_HURRIGANES ),
                      QString() );
    QFile file(filePath);
    file.remove();   
}


// -----------------------------------------------------------------------------
// CMyLocationsEngine::CropAndSaveImage()
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::CropAndSaveImage( QString filePath, int width, 
                                   int height, QString appType, QString orientationType )
{    
     __TRACE_CALLSTACK;
     QImage SourcePNG( filePath );
     QImage sourcePixmap( SourcePNG.convertToFormat(QImage::Format_Indexed8));     
     QImage targetPixmap( sourcePixmap.copy( 
                               ( MapTileWidth - width )/2, 
                               ( MapTileHeight - height )/2 , 
                               width, 
                               height ) );
         
     QString targetImage;
     targetImage.append( filePath );
     targetImage.append( appType );
     targetImage.append( orientationType );
     targetPixmap.save( targetImage, MAPTILE_IMAGE_TYPE  );
             
}

//End of file

