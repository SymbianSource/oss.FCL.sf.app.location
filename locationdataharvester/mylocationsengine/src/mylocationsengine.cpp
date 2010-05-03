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
#include <CNTFLDST.H>
#include <EPos_CPosLmDatabaseManager.h>
#include <lbsposition.h>
#include <bautils.h>
#include <f32file.h>
#include "mylocationsengine.h"
#include "mylocationsdefines.h"
#include "geocodeupdate.h" //header for CGeocodeUpdate class
//handle for CMyLocationsHistoryDbObserver class
#include "mylocationlogger.h"
#if ( defined __WINSCW__ ) || ( defined __WINS__ )
_LIT ( KImageStorageDrive, "C:\\Maptile\\");
#endif
_LIT(KFolderName,":\\MapTile\\");
const TInt KImagePathSize=36;
const TInt KBufSize=256;
const TInt KDefaultFilePathSize = 20;

// separator
_LIT( KSeparator, ",");
_LIT(KContactPrefered, "Contact Prefered");
_LIT(KContactHome, "Contact Home");
_LIT(KContactWork, "Contact Work");
_LIT(KPNGType, ".png");
_LIT(KSingleSpace, " ");

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

    MYLOCLOGSTRING(" start contact db observation ");
    StartContactsChangeNotifierL();
    MYLOCLOGSTRING(" start landmark db observation ");
    StartLandmarksChangeNotifier();

    //set the folder path to store maptile
    imageFilePath.Zero();
    SetFolderPathL();

    TInt status;
    iCalSession = CCalSession::NewL();
    NotifyChangeL(status);

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
    CActive(EPriorityStandard), iCalSession(NULL), iCalView(NULL), iContactsDb(
            NULL), iContactChangeNotifier(NULL), iLandmarkDb(NULL),
            iMapTileInterface(NULL), iMyLocationsDatabaseManager(NULL),
            iMaptileDatabase(NULL), iAddressCompare(NULL),
            iMaptileGeocoderPluginAvailable(EFalse),iCalenderNotification(NULL)
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
   
    GeocodeUpdate::CreateContactdb();
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

            TLookupItem lookupItem;
            lookupItem.iSource = ESourceCalendar;
            lookupItem.iUid = calChangeEntry.iEntryId;
            //TODO: comapare address and then delete 
            TRAP_IGNORE( iMaptileDatabase->DeleteEntryL(lookupItem) );
            TRAP_IGNORE( CalenderEntryAddedL(calChangeEntry) );
            break;
        }
        case EChangeDelete:
        {
            TLookupItem lookupItem;
            lookupItem.iSource = ESourceCalendar;
            lookupItem.iUid = calChangeEntry.iEntryId;
           TRAP_IGNORE( iMaptileDatabase->DeleteEntryL(lookupItem));
            break;
        }
        };

    }
}

void CMyLocationsEngine::CalenderEntryAddedL(TCalChangeEntry aCalChangeEntry)
{
    __TRACE_CALLSTACK;
    CCalEntry* calEntry = NULL;
    calEntry = iCalView->FetchL(aCalChangeEntry.iEntryId);
    TPtrC address(calEntry->LocationL());
    if(address.Length()>0)
    {        
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
        //Get the contact item
        CContactItem* contactItem =
                iContactsDb->ReadContactL(aEvent.iContactId);

        if (contactItem)
        {
            CleanupStack::PushL(contactItem);

            // Get the default/prefered address details
            CPosLandmark *preferedAddressLm = GetContactLocationDetailsLC(
                    contactItem, EAddressPref);

            // Get the work address details
            CPosLandmark *workAddressLm = GetContactLocationDetailsLC(
                    contactItem, EAddressWork);

            // Get the home address details
            CPosLandmark *homeAddressLm = GetContactLocationDetailsLC(
                    contactItem, EAddressHome);

            // if the contact item has no validated address (preferef, home or work)
            if (!preferedAddressLm && !workAddressLm && !homeAddressLm)
            {
                if (aEvent.iType == EContactDbObserverEventContactChanged)
                {
                    // If the contact is a modified one and it might already exist in
                    // mylocations db, so delete it
                    iMyLocationsDatabaseManager->UpdateDatabaseL(NULL,
                            aEvent.iContactId, ESourceContactsPref,
                            EEntryDeleted);
                    iMyLocationsDatabaseManager->UpdateDatabaseL(NULL,
                            aEvent.iContactId, ESourceContactsWork,
                            EEntryDeleted);
                    iMyLocationsDatabaseManager->UpdateDatabaseL(NULL,
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
                    iMyLocationsDatabaseManager->UpdateDatabaseL(homeAddressLm,
                            aEvent.iContactId, ESourceContactsHome, changeType);
                    CleanupStack::PopAndDestroy(homeAddressLm);
                }

                // if work address available, update Mylocations.  
                if (workAddressLm)
                {
                    iMyLocationsDatabaseManager->UpdateDatabaseL(workAddressLm,
                            aEvent.iContactId, ESourceContactsWork, changeType);
                    CleanupStack::PopAndDestroy(workAddressLm);
                }

                // if prefered address available, update Mylocations.  
                if (preferedAddressLm)
                {
                    iMyLocationsDatabaseManager->UpdateDatabaseL(
                            preferedAddressLm, aEvent.iContactId,
                            ESourceContactsPref, changeType);
                    CleanupStack::PopAndDestroy(preferedAddressLm);
                }

            }

            // Pop and destroy the contactItem
            CleanupStack::PopAndDestroy(contactItem);
        }
    }
    else if (aEvent.iType == EContactDbObserverEventContactDeleted)
    {
        // the contact is deleted, so delete the corresponding entries from database.

        // delete prefered address in database
        iMyLocationsDatabaseManager->UpdateDatabaseL(NULL, aEvent.iContactId,
                ESourceContactsPref, EEntryDeleted);

        // delete work address in database
        iMyLocationsDatabaseManager->UpdateDatabaseL(NULL, aEvent.iContactId,
                ESourceContactsWork, EEntryDeleted);

        // delete home address in database
        iMyLocationsDatabaseManager->UpdateDatabaseL(NULL, aEvent.iContactId,
                ESourceContactsHome, EEntryDeleted);
    }
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::TriggerMaptileRequestL()
// Callback that provides information about the contact database change event.
// -----------------------------------------------------------------------------
//

void CMyLocationsEngine::TriggerMaptileRequestL(TContactDbObserverEvent& aEvent)
{
    __TRACE_CALLSTACK;
    TLookupItem lookupItem;
    lookupItem.iUid = aEvent.iContactId;
    // If contact is deleted delete from mylocations db
    /*    if (aEvent.iType == EContactDbObserverEventContactDeleted)
     {
     MYLOCLOGSTRING("EContactDbObserverEventContactDeleted" );
     // delete prefered address in database
     lookupItem.iSource = ESourceContactsPref;
     FindEntryAndDeleteL(lookupItem);

     lookupItem.iSource = ESourceContactsWork;
     FindEntryAndDeleteL(lookupItem);

     lookupItem.iSource = ESourceContactsHome;
     FindEntryAndDeleteL(lookupItem);
     }*/

    //Get the contact item
    iEventType = aEvent.iType;
    CContactItem* contactItem = iContactsDb->ReadContactL(aEvent.iContactId);
    CleanupStack::PushL(contactItem);

    CPosLandmark *preferedAddressLm = NULL;
    CPosLandmark *workAddressLm = NULL;
    CPosLandmark *homeAddressLm = NULL;

    // If contact is modified or added, update the mylocations db
    /* if (contactItem)
     {
     CleanupStack::PushL(contactItem);
     */
    // Get the home address details
    homeAddressLm = GetContactAddressDetailsLC(contactItem, EAddressHome);

    // Get the work address details
    workAddressLm = GetContactAddressDetailsLC(contactItem, EAddressWork);

    // Get the default/prefered address details
    preferedAddressLm = GetContactAddressDetailsLC(contactItem, EAddressPref);

    // iContactUid = aEvent.iContactId;
    switch (aEvent.iType)
    {
    case EContactDbObserverEventContactChanged:
    {
        MYLOCLOGSTRING("EContactDbObserverEventContactChanged" );MYLOCLOGSTRING1("iMapTileRequestQueue.Count()-%d",iMapTileRequestQueue.Count() );

        if (iMapTileRequestQueue.Count() > 0)
        {
            if (iMapTileRequestQueue[0]->iUId == aEvent.iContactId)
            {
                if (preferedAddressLm)
                {
                    CleanupStack::PopAndDestroy(preferedAddressLm);
                }
                if (workAddressLm)
                {
                    CleanupStack::PopAndDestroy(workAddressLm);
                }
                if (homeAddressLm)
                {
                    CleanupStack::PopAndDestroy(homeAddressLm);
                }

                CleanupStack::PopAndDestroy(contactItem);
                MYLOCLOGSTRING("retrun from geolocation callback" );
                return;
            }
        }
        /*
         // if the contact item has no validated address (preferef, home or work)
         if (!preferedAddressLm && !workAddressLm && !homeAddressLm)
         {
         MYLOCLOGSTRING("Contact changed no  address" );
         //Delete the entries from maptile lookup table
         lookupItem.iSource = ESourceContactsPref;
         iMaptileDatabase->DeleteEntryL(lookupItem);

         lookupItem.iSource = ESourceContactsWork;
         iMaptileDatabase->DeleteEntryL(lookupItem);

         lookupItem.iSource = ESourceContactsHome;
         iMaptileDatabase->DeleteEntryL(lookupItem);

         }*/
        /* else
         {*/
        MYLOCLOGSTRING("Contact address changed" );

        // if default address available, update Mylocations. 
        lookupItem.iSource = ESourceContactsPref;
        if (preferedAddressLm)
        {
            MYLOCLOGSTRING("preferedAddressLm address changed" );

            if (iAddressCompare->IsAddressChangedL(*preferedAddressLm,
                    aEvent.iContactId, ESourceContactsPref))

            {
                //remove entry from database
                iMaptileDatabase->DeleteEntryL(lookupItem);
                RequestMapTileImageL(*preferedAddressLm, ESourceContactsPref,
                        aEvent.iContactId);
            }

            CleanupStack::PopAndDestroy(preferedAddressLm);

        }
        else
        {
            iMaptileDatabase->DeleteEntryL(lookupItem);
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
                iMaptileDatabase->DeleteEntryL(lookupItem);
                RequestMapTileImageL(*workAddressLm, ESourceContactsWork,
                        aEvent.iContactId);
            }

            CleanupStack::PopAndDestroy(workAddressLm);

        }
        else
        {
            iMaptileDatabase->DeleteEntryL(lookupItem);
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
                iMaptileDatabase->DeleteEntryL(lookupItem);
                RequestMapTileImageL(*homeAddressLm, ESourceContactsHome,
                        aEvent.iContactId);
            }
            CleanupStack::PopAndDestroy(homeAddressLm);
        }
        else
        {
            iMaptileDatabase->DeleteEntryL(lookupItem);
        }
        // }
        break;
    }
    case EContactDbObserverEventContactDeleted:
    {

        lookupItem.iSource = ESourceContactsPref;
        iMaptileDatabase->DeleteEntryL(lookupItem);

        lookupItem.iSource = ESourceContactsWork;
        iMaptileDatabase->DeleteEntryL(lookupItem);

        lookupItem.iSource = ESourceContactsHome;
        iMaptileDatabase->DeleteEntryL(lookupItem);

        MYLOCLOGSTRING("EContactDbObserverEventContactDeleted ");
        /*             // the contact is deleted, so delete the corresponding entries from database.
         TLookupItem lookupItem;
         lookupItem.iUid = aEvent.iContactId;
         iMaptileDatabase->DeleteEntryL(lookupItem);*/
        break;
    }
    case EContactDbObserverEventContactAdded:
    {
        MYLOCLOGSTRING("EContactDbObserverEventContactAdded" );
        if (preferedAddressLm)
        {
            RequestMapTileImageL(*preferedAddressLm, ESourceContactsPref,
                    aEvent.iContactId);
            CleanupStack::PopAndDestroy(preferedAddressLm);
        }
        if (workAddressLm)
        {
            RequestMapTileImageL(*workAddressLm, ESourceContactsWork,
                    aEvent.iContactId);
            CleanupStack::PopAndDestroy(workAddressLm);
        }
        if (homeAddressLm)
        {
            RequestMapTileImageL(*homeAddressLm, ESourceContactsHome,
                    aEvent.iContactId);
            CleanupStack::PopAndDestroy(homeAddressLm);
        }
        break;
    }

    };
    CleanupStack::PopAndDestroy(contactItem);
    //}

}

void CMyLocationsEngine::RequestMapTileImageL(const TDesC& aAddressDetails,
        const TUidSourceType aAddressType, const TInt32 aUId)
{
    __TRACE_CALLSTACK;
    SetFolderPathL();
    TBuf<KImagePathSize> mImagePath;

    //mImagePath.Append(KImageStorageDrive);
    mImagePath.Copy(imageFilePath);
    mImagePath.AppendNum(aUId);
    mImagePath.AppendNum(aAddressType);
    mImagePath.Append(KPNGType);

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
    TBuf<KImagePathSize> mImagePath;

    //mImagePath.Append(KImageStorageDrive);
    mImagePath.Copy(imageFilePath);
    mImagePath.AppendNum(aUId);
    mImagePath.AppendNum(aAddressType);
    mImagePath.Append(KPNGType);

    CMapTileRequest* mapTileRequest = new (ELeave) CMapTileRequest;

    mapTileRequest->iLandmarkInfo = CPosLandmark::NewL(aLandmark);
    mapTileRequest->iUId = aUId;
    mapTileRequest->iAddressType = aAddressType;
    mapTileRequest->iEventType = iEventType;
    mapTileRequest->iImagePath.Zero();
    mapTileRequest->iImagePath.Copy(mImagePath);
    MYLOCLOGSTRING1("RequestMapTileImageL() Queue count -%d",iMapTileRequestQueue.Count());

    if (iMapTileRequestQueue.Count() <= 0)
       {
          // iMapTileRequestQueue.Append(mapTileRequest);
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
TInt CMyLocationsEngine::RequestExecute( CMapTileRequest* aMapTileRequest)
{
    __TRACE_CALLSTACK;
    TInt errorCode;
    switch (aMapTileRequest->iAddressType)
    {
        case ESourceCalendar:
        {
            TRAP(errorCode,iMapTileInterface->GetMapTileImageL(aMapTileRequest->iAddressDetails->Des(),
                            aMapTileRequest->iImagePath, this ));        
            break;
        }
        case ESourceContactsPref:
        case ESourceContactsWork:
        case ESourceContactsHome:
        {
            TRAP(errorCode, iMapTileInterface->GetMapTileImageL(aMapTileRequest->iLandmarkInfo,
                            aMapTileRequest->iImagePath, this));            
            break;
        }
    };  

    return errorCode;
}
 
// -----------------------------------------------------------------------------
// CMyLocationsEngine::GetLocationDetailsLC()
// get locatin details
// -----------------------------------------------------------------------------
//
CPosLandmark* CMyLocationsEngine::GetContactAddressDetailsLC(
        const CContactItem *aContactItem, TContactAddressType aAddressType)
{
    __TRACE_CALLSTACK;
    CPosLandmark *landmark = NULL;

    // Set the street
    TInt adrId = FindContactsField(aContactItem, aAddressType,
            KUidContactFieldVCardMapADR);

    if (adrId != KErrNotFound)
    {
        TPtrC tempText =
                aContactItem->CardFields()[adrId].TextStorage()->Text();
        if (tempText.Length() > 0)
        {
            if (!landmark)
            {
                landmark = CPosLandmark::NewL();
                CleanupStack::PushL(landmark);
            }

            landmark->SetPositionFieldL(EPositionFieldStreet, tempText);
        }
    }

    // Set the City
    adrId = FindContactsField(aContactItem, aAddressType,
            KUidContactFieldVCardMapLOCALITY);
    if (adrId != KErrNotFound)
    {
        TPtrC tempText =
                aContactItem->CardFields()[adrId].TextStorage()->Text();
        if (tempText.Length() > 0)
        {
            if (!landmark)
            {
                landmark = CPosLandmark::NewL();
                CleanupStack::PushL(landmark);
            }
            landmark->SetPositionFieldL(EPositionFieldCity, tempText);
        }
    }

    // Set the state/region
    adrId = FindContactsField(aContactItem, aAddressType,
            KUidContactFieldVCardMapREGION);
    if (adrId != KErrNotFound)
    {
        TPtrC tempText =
                aContactItem->CardFields()[adrId].TextStorage()->Text();
        if (tempText.Length() > 0)
        {
            if (!landmark)
            {
                landmark = CPosLandmark::NewL();
                CleanupStack::PushL(landmark);
            }
            landmark->SetPositionFieldL(EPositionFieldState, tempText);
        }
    }

    // Set the Postal code
    adrId = FindContactsField(aContactItem, aAddressType,
            KUidContactFieldVCardMapPOSTCODE);
    if (adrId != KErrNotFound)
    {
        TPtrC tempText =
                aContactItem->CardFields()[adrId].TextStorage()->Text();
        if (tempText.Length() > 0)
        {
            if (!landmark)
            {
                landmark = CPosLandmark::NewL();
                CleanupStack::PushL(landmark);
            }
            landmark->SetPositionFieldL(EPositionFieldPostalCode, tempText);
        }
    }

    // Set the country
    adrId = FindContactsField(aContactItem, aAddressType,
            KUidContactFieldVCardMapCOUNTRY);
    if (adrId != KErrNotFound)
    {
        TPtrC tempText =
                aContactItem->CardFields()[adrId].TextStorage()->Text();
        if (tempText.Length() > 0)
        {
            if (!landmark)
            {
                landmark = CPosLandmark::NewL();
                CleanupStack::PushL(landmark);
            }
            landmark->SetPositionFieldL(EPositionFieldCountry, tempText);
        }
    }
       
    return landmark;

}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::GetContactLocationDetailsLC()
// Finds the contact's location details
// -----------------------------------------------------------------------------
//
CPosLandmark* CMyLocationsEngine::GetContactLocationDetailsLC(
        const CContactItem *aContactItem, TContactAddressType aAddressType)
{
    __TRACE_CALLSTACK;//return value
    CPosLandmark *landmark = NULL;
    // Get the geo field
    TInt addrInd = FindContactsField(aContactItem, aAddressType,
            KUidContactFieldVCardMapGEO);
    if (addrInd != KErrNotFound)
    {
        // Geo field present. 

        TPtrC addrText =
                aContactItem->CardFields()[addrInd].TextStorage()->Text();
        if (addrText.Length() > 0)
        {
            //Parse the addresstext to get the latitude and longitude
            TInt separator = addrText.Find(KSeparator);
            if (separator != KErrNotFound)
            {
                TReal64 latitude = 0;
                TReal64 longitude = 0;
                TLex lexLatitude(addrText.Left(addrText.Length() - separator));
                TLex lexLongitude(addrText.Right(addrText.Length() - separator
                        - 1));
                if (lexLatitude.Val(latitude) == KErrNone && lexLongitude.Val(
                        longitude) == KErrNone)
                {
                    TLocality loc(TCoordinate(latitude, longitude), 0);

                    landmark = CPosLandmark::NewL();
                    CleanupStack::PushL(landmark);

                    // Fill the location details into the landmark object
                    landmark->SetPositionL(loc);

                    // Set the landmark name as contact name
                    TBuf<KBufSize> sName1;
                    TBool nameEmpty = ETrue;
                    //get the second name and
                    TInt sNameInd = aContactItem->CardFields().Find(
                            KUidContactFieldGivenName);
                    if (sNameInd != KErrNotFound)
                    {
                        TPtrC      sName =
                                        aContactItem->CardFields()[sNameInd].TextStorage()->Text();
                        sName1.Copy(sName);
                        nameEmpty = EFalse;
                    }

                    sNameInd = aContactItem->CardFields().Find(
                            KUidContactFieldFamilyName);
                    if (sNameInd != KErrNotFound)
                    {
                        if (!nameEmpty)
                        {
                            sName1.Append(KSingleSpace);
                        }
                        TPtrC
                                sName =
                                        aContactItem->CardFields()[sNameInd].TextStorage()->Text();
                        sName1.Append(sName);
                    }

                    TRAP_IGNORE( landmark->SetLandmarkNameL( sName1 ) );

                    if (aAddressType == EAddressPref) // default/prefered address
                    {
                        TRAP_IGNORE( landmark->SetLandmarkDescriptionL(KContactPrefered) );
                    }
                    else if (aAddressType == EAddressWork) // work address
                    {
                        TRAP_IGNORE( landmark->SetLandmarkDescriptionL(KContactWork) );
                    }
                    else // home address
                    {
                        TRAP_IGNORE( landmark->SetLandmarkDescriptionL(KContactHome) );
                    }

                    // Set the street
                    TInt adrId = FindContactsField(aContactItem, aAddressType,
                            KUidContactFieldVCardMapADR);
                    if (adrId != KErrNotFound)
                    {
                        TPtrC
                                tempText =
                                        aContactItem->CardFields()[adrId].TextStorage()->Text();
                        if (tempText.Length() > 0)
                        {
                            landmark->SetPositionFieldL(EPositionFieldStreet,
                                    tempText);
                        }
                    }

                    // Set the City
                    adrId = FindContactsField(aContactItem, aAddressType,
                            KUidContactFieldVCardMapLOCALITY);
                    if (adrId != KErrNotFound)
                    {
                        TPtrC
                                tempText =
                                        aContactItem->CardFields()[adrId].TextStorage()->Text();
                        if (tempText.Length() > 0)
                        {
                            landmark->SetPositionFieldL(EPositionFieldCity,
                                    tempText);
                        }
                    }

                    // Set the state/region
                    adrId = FindContactsField(aContactItem, aAddressType,
                            KUidContactFieldVCardMapREGION);
                    if (adrId != KErrNotFound)
                    {
                        TPtrC
                                tempText =
                                        aContactItem->CardFields()[adrId].TextStorage()->Text();
                        if (tempText.Length() > 0)
                        {
                            landmark->SetPositionFieldL(EPositionFieldState,
                                    tempText);
                        }
                    }

                    // Set the Postal code
                    adrId = FindContactsField(aContactItem, aAddressType,
                            KUidContactFieldVCardMapPOSTCODE);
                    if (adrId != KErrNotFound)
                    {
                        TPtrC
                                tempText =
                                        aContactItem->CardFields()[adrId].TextStorage()->Text();
                        if (tempText.Length() > 0)
                        {
                            landmark->SetPositionFieldL(
                                    EPositionFieldPostalCode, tempText);
                        }
                    }

                    // Set the country
                    adrId = FindContactsField(aContactItem, aAddressType,
                            KUidContactFieldVCardMapCOUNTRY);
                    if (adrId != KErrNotFound)
                    {
                        TPtrC
                                tempText =
                                        aContactItem->CardFields()[adrId].TextStorage()->Text();
                        if (tempText.Length() > 0)
                        {
                            landmark->SetPositionFieldL(EPositionFieldCountry,
                                    tempText);
                        }
                    }
                }
            }
        }
    }

    return landmark;
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::FindContactsField()
// Finds the contact's field type id
// -----------------------------------------------------------------------------
//

TInt CMyLocationsEngine::FindContactsField(const CContactItem *aContactItem,
        TContactAddressType aAddressType, TUid aField)
{
    __TRACE_CALLSTACK;
    if (aAddressType == EAddressPref) // default/prefered address
    {
        return aContactItem->CardFields().Find(aField);
    }
    else if (aAddressType == EAddressWork) // work address
    {
        return aContactItem->CardFields().Find(KUidContactFieldVCardMapWORK,
                aField);
    }
    else // home address
    {
        return aContactItem->CardFields().Find(KUidContactFieldVCardMapHOME,
                aField);
    }
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
// CMyLocationsEngine::MyLocationsDbManager()
// Gets handle to mylocations database manager.
// -----------------------------------------------------------------------------
//
CMyLocationsDatabaseManager& CMyLocationsEngine::MyLocationsDbManager()
{
    __TRACE_CALLSTACK;
    return *iMyLocationsDatabaseManager;
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
        TBuf<KMaxAddressLength> lmAddress;
        CPosLandmark* readLandmark = iLandmarkDb->ReadLandmarkLC(
                iLmEvent.iLandmarkItemId);

        if (readLandmark)
        {
            iMyLocationsDatabaseManager->GetLandmarkFullAddress(lmAddress,
                    readLandmark);

            CPosLandmark* landmark = CPosLandmark::NewL();
            CleanupStack::PushL(landmark);

            TRAP_IGNORE( landmark->SetLandmarkNameL( lmAddress ));

            TPtrC iconFileName;
            TInt iconIndex;
            TInt iconMaskIndex;
            TInt err = readLandmark->GetIcon(iconFileName, iconIndex,
                    iconMaskIndex);

            if (err == KErrNone)
            {
                TRAP_IGNORE( landmark->SetIconL(
                                iconFileName, iconIndex, iconMaskIndex ) );
            }

            // update the entry in mylocations database.
            iMyLocationsDatabaseManager->UpdateDatabaseL(landmark,
                    iLmEvent.iLandmarkItemId, ESourceLandmarks, MapChangeType(
                            ESourceLandmarks, iLmEvent.iEventType));

            CleanupStack::PopAndDestroy(landmark);
            CleanupStack::PopAndDestroy(readLandmark);
        }
    }
        break;
    case EPosLmEventLandmarkDeleted:
    {
        // delete the corresponding entries in mylocations database.
        iMyLocationsDatabaseManager->UpdateDatabaseL(NULL,
                iLmEvent.iLandmarkItemId, ESourceLandmarks, EEntryDeleted);
    }
        break;

    case EPosLmEventCategoryCreated:
    {
        // delete the corresponding entries in mylocations database.
        iMyLocationsDatabaseManager->UpdateDatabaseL(NULL,
                iLmEvent.iLandmarkItemId, ESourceLandmarksCategory,
                MapChangeType(ESourceLandmarksCategory, iLmEvent.iEventType));
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
// CMyLocationsEngine::RunError()
// Implements cancellation of an outstanding request.
// -----------------------------------------------------------------------------
//
TInt CMyLocationsEngine::RunError(TInt /*aError*/)
{
    __TRACE_CALLSTACK;
    return KErrNone;
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

    if (iMapTileRequestQueue.Count() > 0)
    {

        MYLOCLOGSTRING1("No.of RequestQueue - %d",iMapTileRequestQueue.Count());

        if (aErrCode == KErrNone )
        {           
            TLookupItem lookupItem;
            lookupItem.iSource = iMapTileRequestQueue[0]->iAddressType;
            lookupItem.iUid = iMapTileRequestQueue[0]->iUId;
            lookupItem.iFilePath.Copy(aMapTilePath);
            TRAP_IGNORE( HandleMaptileDatabaseL(iMapTileRequestQueue[0]->iEventType,lookupItem ) );
        }

            delete iMapTileRequestQueue[0];
            iMapTileRequestQueue.Remove(0);
            iMapTileRequestQueue.Compress();
    }
    
    
    //Process the next request
    if (iMapTileRequestQueue.Count() > 0)
    {
        MYLOCLOGSTRING1("MapTile fetch completed request-%d",iMapTileRequestQueue.Count());
        for (TInt cnt = 0; cnt < iMapTileRequestQueue.Count(); cnt++)
        {          
            if ( KErrNone == RequestExecute(iMapTileRequestQueue[0]) )
            {
                break;
            }
            else
            {
                delete iMapTileRequestQueue[0];
                iMapTileRequestQueue.Remove(0);
                iMapTileRequestQueue.Compress();
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
void CMyLocationsEngine::HandleMaptileDatabaseL(
        TInt aEventType, TLookupItem& aLookupItem)
{
    __TRACE_CALLSTACK;
    if (aEventType == EContactDbObserverEventContactChanged || aEventType
            == EChangeModify)
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
    else if (aEventType == EContactDbObserverEventContactAdded || aEventType
            == EChangeAdd)
    {
        iMaptileDatabase->CreateEntryL(aLookupItem);

    }
}

// -----------------------------------------------------------------------------
// CMyLocationsEngine::RestGeoCodeCompleted()
// observed when rest geo codeing completed.
// started lat and lon field updation into contact db.
// -----------------------------------------------------------------------------
//
void CMyLocationsEngine::RestGeoCodeCompleted(TReal aLatitude, TReal aLongitude)
{
    __TRACE_CALLSTACK;

    MYLOCLOGSTRING1("No. of iMapTileRequestQueue - %d",iMapTileRequestQueue.Count());
    if (iMapTileRequestQueue.Count() > 0)
    {
        switch (iMapTileRequestQueue[0]->iAddressType)
        {
            /*//TODO:
            case ESourceCalendar:
            {
                break;
            }*/
            case ESourceContactsPref:
            case ESourceContactsWork:
            case ESourceContactsHome:
            {
                GeocodeUpdate::UpDate(iMapTileRequestQueue[0]->iUId,
                        iMapTileRequestQueue[0]->iAddressType, aLatitude,
                        aLongitude);
                MYLOCLOGSTRING("Geo code updated into contact db");
                break;
            }
        };

        
    }

}

//End of file
