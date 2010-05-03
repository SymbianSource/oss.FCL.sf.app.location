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
* Description: Mylocation engine header implementation.
*
*/


#ifndef __MYLOCATIONSENGINE_H__
#define __MYLOCATIONSENGINE_H__

// INCLUDES

// calendar related headers
#include <calsession.h>
#include <calentryview.h>

// Header for contact DB
#include <CNTDB.H>

// Header for landmarks db
#include<epos_cposlandmarkdatabase.h>

// mylocations database manager
#include "maptileinterface.h"
#include "mylocationsdatabasemanager.h"
#include "lookupmaptiledb.h"
#include "addresscomparision.h"
#include "calendernotification.h"

/**  
 * This class contains the required data  for maptile image  processing.
 * Each  address added/modified in contact database will be added as queue
 * using this class for maptile processing.
 */
class CMapTileRequest: public CBase
{
public:
    //Consturctor
    CMapTileRequest():iLandmarkInfo(NULL),iAddressDetails(NULL){};
    //Destrucotr
    ~CMapTileRequest(){delete iLandmarkInfo;delete iAddressDetails;};
public:
    //Landmark contains address details
    CPosLandmark* iLandmarkInfo;
    //single row address.
    HBufC* iAddressDetails;
    //Contact id
    TInt32 iUId;
    //Address type 
    TInt32 iAddressType;
    //Image path 
    TFileName iImagePath;
    //Event type    
    TInt iEventType;
};

// CLASS DECLARATION
/**  Class used to listen on various database change events from applications like calendar, 
 * contacts, landmarks
 * 
 * This class listens on database change events from applications like contacts, calendar, landmarks
 * and triggers updating the Mylocations database accordingly. 
 * This process starts on phone bootup and always runs in the background.
 */
class CMyLocationsEngine : public CActive, public MCalChangeCallBack2,
				 public MContactDbObserver, public MMapTileObserver,public MNotifyChange
			
{
    
public:
    
    /** 
     * Contact address types
     */
    enum TContactAddressType
        {
        /** Address Type Pref */
        EAddressPref,
        /** Address type Work */
        EAddressWork,
        /** Address type Home */
        EAddressHome,
        };

public:

    /** 
     * CMyLocationsEngine::NewL()
     */
    static CMyLocationsEngine* CMyLocationsEngine::NewL();
    
    /**
     * ~CMyLocationsEngine.
     * Virtual Destructor.
     */
    virtual ~CMyLocationsEngine();
    
    /** Starts calender change notification method to get callbacks when entries are added/modified/deleted in calender
     *
     */      
    void StartCalenderChangeNotifierL();

    /** Starts contacts change notification method to get callbacks when entries are added/modified/deleted in contacts
     *
     */      
    void StartContactsChangeNotifierL();

    /** Starts landmarks change notification method to get callbacks when entries are added/modified/deleted in landmarks
     *
     */      
    void StartLandmarksChangeNotifier();
    
    /** Maps the source type change type to Mylocations entry change type
    * @param[in] aSrcType Source type of the change
    * @param[in] aChangeType Type of change ( added/modified/deleted ).
    * @return The mylocations entry change type value.
     */      
    TEntryChangeType MapChangeType( TUidSourceType aSrcType, TUint32 aChangeType );    
 
    /** Gets the handle to mylocations database manager
     *
     */      
    CMyLocationsDatabaseManager& MyLocationsDbManager();

private:
    
    /**
     * ConstructL.
     * 2nd phase constructor.
     */
    void ConstructL();
    
    /**
     * CMyLocationsEngine.
     * C++ default constructor. 
     */
    CMyLocationsEngine();
    
    /** Creates a landmark object from contact item's location details and checks whether address available.
    * @param[in] aContactItem A contact item whose location details needs to be used to create landmark object.
    * @param[in] aAddressType Mentions the address type ( prefered/work/home ) of the contact item to be used.
    * @returns A valid landmark object if location in the contact item is validated, NULL otherwise.
     */      
    CPosLandmark* GetContactAddressDetailsLC( const CContactItem *aContactItem, 
            TContactAddressType aAddressType );
            
    /** Creates a landmark object from contact item's location details
    * @param[in] aContactItem A contact item whose location details needs to be used to create landmark object.
    * @param[in] aAddressType Mentions the address type ( prefered/work/home ) of the contact item to be used.
    * @returns A valid landmark object if location in the contact item is validated, NULL otherwise.
    */    
    CPosLandmark* GetContactLocationDetailsLC( const CContactItem *aContactItem, 
           TContactAddressType aAddressType );

    /** Finds the contact's field type id
    * @param[in] aContactItem A contact item whose field details needs to found.
    * @param[in] aAddressType Mentions the address type ( prefered/work/home ) of the contact item to be used.
    * @param[in] aField The field type of interest.
    * @return If found, the index of the field within the field set, or KErrNotFound if not found.
    */      
    TInt FindContactsField( const CContactItem *aContactItem, TContactAddressType aAddressType, 
            TUid aField );

    /**
     *  A call back function for calendar change notification
	 *	@param aChangeItems  List of changes since the last notification.	
	 */      
    virtual void CalChangeNotification( RArray<TCalChangeEntry>& aChangeItems );
    
    /** Tests the contact database observer event type and handles it. The ID of a 
    * contact affected by the change event, if relevant, can be retrieved via TContactDbObserverEvent::iContactId.
    * @param aEvent Provides information about the change event. 
    */
    virtual void HandleDatabaseEventL( TContactDbObserverEvent aEvent );
    
    /** 
     * Triggers the maptile processing for complete address
     * @param aEvent  Provides information about the change event.   
     */      
	void TriggerMaptileRequestL(  TContactDbObserverEvent& aEvent  );
			
    /** 
     * Process the contact database event and updates the landmark database
     * @param aEvent  Provides information about the change event.   
     */      
	void HandlelandmarkDatabaseL( TContactDbObserverEvent& aEvent );
    /** 
    * Process the maptile database event and updates the maptile database
    * @param aEventType  Provides information about the type of request address.
    * @param aLookupItem Provides information about single address of contact.  
    */  
	void HandleMaptileDatabaseL(TInt aEventType ,
	                             TLookupItem& aLookupItem );
    /**
     *  Handles active object's request completion event. 
     */
    void RunL();

    /** 
     * Implements cancellation of an outstanding request. 
     */
    void DoCancel();
 
    /** 
     * Implements RunError of active object. 
     */
    TInt RunError( TInt aError );
    
    /** 
     * Requests for map tile image , structure serch.
     * @param aLandmark  Provides information about the address.
     * @param aAddressType Provides information about address type .  
     * @param aUId, uid of the event.
     */
    void RequestMapTileImageL( CPosLandmark&  aLandmark, const TUidSourceType aAddressType,
                               const TInt32 aUId  );
    
    /** 
     * Requests for map tile image , one box serch.
     * @param aAddressDetails  Provides information about the address.
     * @param aAddressType Provides information about address type .  
     * @param aUId, uid of the event.
     */
    void RequestMapTileImageL(const TDesC& aAddressDetails,
            const TUidSourceType aAddressType, const TInt32 aUId);
    
    /** 
     * Add to maptile request queue.
     * @param mapTileRequest, request queue.
     * @return If successful, retrun KErrNone, or return some systemwide error.
     */
    TInt RequestExecute( CMapTileRequest* aMapTileRequest);
	/**
    * SetFolderPath.
    * Sets the maptile storing path.
    */
    void SetFolderPathL();
    /**
    * Calender entry added.
    */
    void CalenderEntryAddedL(TCalChangeEntry aCalChangeEntry);

public:  //From MMapTileObserver
    
    /**
     *  Informs the maptile fetching completion  
     */ 
    void MapTilefetchingCompleted( TInt aErrCode,const TDesC& aMapTilePath );
    
    /** 
     * Informs the geocode completion
     */
    void RestGeoCodeCompleted( TReal latitude,TReal longitude);
    
public://from MNotifychange
    void NotifyChangeL(TInt &aStatus);
    
private:

    // Data member
    
    //handle to timer class
    //calendar session handle
    CCalSession *iCalSession;
    
    // calendar view
    CCalEntryView *iCalView;   

    // Handle to the contacts database
    CContactDatabase* iContactsDb;
    
    // Receives events reporting changes to a contact database.
    CContactChangeNotifier *iContactChangeNotifier;
    
    // Handle to landmarks db
    CPosLandmarkDatabase* iLandmarkDb;
    
    // landmark change event type.
    TPosLmEvent iLmEvent;
   
    //Handle to maptile interface
    CMapTileInterface* iMapTileInterface;
    
    CMyLocationsDatabaseManager*  iMyLocationsDatabaseManager;
    
    //maptiledb 
    CLookupMapTileDatabase* iMaptileDatabase;

    //Current event type
    TInt iEventType;
    
    //Address comparison pointer
    CAddressComparision *iAddressCompare;
    
    //Maptile image request queue
    RArray<CMapTileRequest*> iMapTileRequestQueue;
    
    //Flag to check maptilegeocoder plugin availability
    TBool iMaptileGeocoderPluginAvailable;
	
	//Maptile image path
    TFileName imageFilePath;
    //Object to listen calender db create notification.    
    CCalenderNotification  *iCalenderNotification;
    
};

#endif // __MYLOCATIONSENGINE_H__
// End of File
