/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface to store and access the Location Events.
*
*/


#ifndef C_EVTEVENTMANAGER_H
#define C_EVTEVENTMANAGER_H

// System Includes
#include <e32base.h>

// User Includes
#include "evtbasiceventinfo.h"
#include "evtevent.h"

// Forward Declarations
class CEvtDatabase;
class CEvtBasicEventInfo;
class CEvtEvent;
class MEvtStorageDbObserver;

// extern data types

// global Functions

// constants go here

/**
 *  Interface class to store and access the Location Events.
 *  This class allows for Events to be stored in persistent database. 
 *  Events can be added, deleted and modified from database.
 *  This class also allows for monitoring any changes to the database. This
 *  is achieved by setting observer to the database.
 *
 *  @lib evtstorage.lib
 *  @since S60 v9.1
 */
class CEvtEventManager : public CBase
    {        
public:  // Exported Functions
    /**
     * Constructs a new instance of Event Storage Manager interface.
     *
     * @return The new instance of Event Storage Manager object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CEvtEventManager* NewL();
    
    /**
     * Constructs a new instance of Event Storage Manager interface.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of Event Storage Manager object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CEvtEventManager* NewLC();  

    /**
    * Destructor.
    */
    virtual ~CEvtEventManager();

    /**
     * Gets a sorted list of Events based on Event Status Criteria. The sort
     * is based on Event's Subject Field.
     *
     * The application can decide upon the list of Events returned.
     * This can be achieved by passing a Event Status as the parameter
     * to this function. The status of the Events is defined by 
     * @ref TEvtEventStatus      
     *  
     * The ownership of the list of Events is transferred to the caller.
     * 
     * @since S60 v9.1
     * @param[in] TEvtEventStatus Event Status that decides the 
     *                    list of Events to be returned.   
     * @param[out] aEventArray Reference to an array of pointers to Event.
     *                           This Array would be updated with the
     *                           @ref CEvtBasicEventInfo objects.
     * @return One of the following error values.
     *         - KErrNone, If the Events were successfully retrieved.
     *         - System wide error code if the database operation
     *                fails for any other reason.     
     */
    IMPORT_C void GetEventsL( 
    						TEvtEventStatus 	aEvtStatus,
                     		RPointerArray<CEvtBasicEventInfo>& aEvtEventArray );

    /**
     * Gets a sorted list of ALL Events. The sort is based on Event's 
     * Subject Field.
     *
     * The application gets all the Events entry from the database.     
     *  
     * The ownership of the list of Events is transferred to the caller.
     * 
     * @since S60 v9.1  
     * @param[out] aEventArray Reference to an array of pointers to Event.
     *                           This Array would be updated with the
     *                           @ref CEvtBasicEventInfo objects.
     * @return One of the following error values.
     *         - KErrNone, If the Events were successfully retrieved.
     *         - System wide error code if the database operation
     *                fails for any other reason.    
     */
    IMPORT_C void GetEventsL( 
                     		RPointerArray<CEvtBasicEventInfo>& aEvtEventArray );

    /**
     * Gets an Event based on Event Id passed.
     *
     * The application can get the Event info based on the EventId passed.  
     *  
     * The ownership of the returned Event is transferred to the caller.
     * 
     * @since S60 v9.1
     * @param[in] aEvtId EventId that decides the Event to be returned.   
     * @return CEvtEvent - Pointer to @ref CEvtEvent, if the Event was found
                         in the database.
                         - NULL, if the event was not present in the database.
     */
    IMPORT_C CEvtEvent* GetEventL( TEvtEventId            aEvtId );

    /**
     * Deletes the List of Events from Database.
     *
     * The application delete the list of Events from the database.     
     * 
     * @since S60 v9.1
     * @param[in] aEventArray Reference to the Events Id array that are to 
     *                          be removed form the database.  
     * @return One of the following error values.
     *         - KErrNone, If the Event was successfully deleted.
     *         - System wide error code if the database operation
     *                fails for any other reason. 
     */
    IMPORT_C void RemoveEventsL( 
                            const RArray<TEvtEventId>&     aEvtIdArray );
    
    /**
     * Update the status of events in Database.
     *
     * The application will pass the list of Events whose status are updated in the 
     * database.  
     * 
     * @since S60 v9.1
     * @param[in] aEventArray Reference to the Events Id array that are to 
     *                          be updated in database.  
     * @param[in] TEvtEventStatus Event Status that decides the 
     *                    list of Events to be returned.      
     * @return One of the following error values.
     *         - KErrNone, If the Event was successfully deleted.
     *         - System wide error code if the database operation
     *                fails for any other reason. 
     */
    IMPORT_C void UpdateEventsStatusL( 
                                const RArray<TEvtEventId>&     aEvtIdArray,
                                TEvtEventStatus    aEvtStatus );

    /**
     * Updates the given Event to Database.
     *
     * The application will pass the Event that has to be updated in the 
     * database.    
     * 
     * @since S60 v9.1
     * @param[in] aEvtEvent Reference to the Event that is to be updated in
     *                          the database.  
     * @return One of the following error values.
     *         - KErrNone, If the Event was successfully updated.
     *         - System wide error code if the database operation
     *                fails for any other reason. 
     */
    IMPORT_C void UpdateEventL( 
                            CEvtEvent&          aEvtEvent );

    /**
     * Inserts a new Event to Database.
     *
     * The application will pass the Event that has to be inserted in the 
     * database.
     *  
     * @since S60 v9.1
     * @param[in] CEvtEvent Reference to the Event that has to 
     *                          be inserted into the database. 
     * @return One of the following error values.
     *         - KErrNone, If the Event was successfully inserted.
     *         - System wide error code if the database operation
     *                fails for any other reason.
     */
    IMPORT_C void AddEventL( CEvtEvent&     aEvtEvent );

    /**
     * Update the status of an Event in Database.
     *
     * The application will pass the Event whose status is updated in the 
     * database.
     *  
     * @since S60 v9.1
     * @param[in] TEvtEventStatus Event Status that decides the 
     *                    list of Events to be returned.   
     * @param[in] CEvtEvent Reference to the Event that has to 
     *                          be updated into the database. 
     * @return One of the following error values.
     *         - KErrNone, If the Event was successfully updated.
     *         - System wide error code if the database update
     *                fails for any other reason.  
     */
    IMPORT_C void UpdateEventStatusL(
                            TEvtEventId      aEvtId,
                            TEvtEventStatus    aEvtStatus);
    
    /**
     * Delete an Event from Database.
     *
     * The application will pass the Event that has to be deleted from the
     * database.
     *  
     * @since S60 v9.1
     * @param[in] aEventId Id of the Event that is to be removed form
     *                          the database. 
     * @return One of the following error values.
     *         - KErrNone, If the Event was successfully deleted.
     *         - System wide error code if the database operation
     *                fails for any other reason.  
     */
    IMPORT_C void RemoveEventL( TEvtEventId      aEvtId );
    
    /**
     * Set the Observer for the Storage Database.
     *
     * The application can listen to the change in the Storage database 
     * by setting the Observer.
     *  
     * @since S60 v9.1
     * @param[in] aDbObserver An Observer to listen to Storage database
     * 						changes. 
     */
    IMPORT_C void SetObserver( MEvtStorageDbObserver*      aDbObserver );

private:
    /**
     * Default C++ Constructor.
     */
    CEvtEventManager();

    /**
     * Second phase of the two phase constructor.
     */
    void ConstructL();

private: // data
    /**
     *  CEvtDatabaseRequest Instance
     */
    CEvtDatabase* iEvtDb;

    };

#endif // C_EVTEVENTMANAGER_H
