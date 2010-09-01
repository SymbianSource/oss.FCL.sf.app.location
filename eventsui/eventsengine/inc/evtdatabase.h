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
* Description:  Class that directly interacts with Sqlite database.
*
*/


#ifndef C_EVTDATABASE_H
#define C_EVTDATABASE_H

// System Includes
#include <sqldb.h>

// User Includes
#include "evtbasiceventinfo.h"
#include "evtevent.h"
#include "evtdbobserver.h"
#include "evtstoragedbobserver.h"

// Forward Declarations
class CEvtDbNotifier;

// extern data types

// global Functions

// constants
const TInt KTempStringLength=32;

/**
 *  Interface class to Location Events Database.
 *  This class allows for Events to be stored in persistent database
 *   - Sqlite. Events can be added, deleted and modified from 
 *  database.
 *
 *  @lib evtstorage.lib
 *  @since S60 v9.1
 */
class CEvtDatabase : public CBase, 
					   public MEvtDbObserver
    {   
public:  // Member Functions
    /**
     * Constructs a new instance of CEvtDatabase.
     *
     * @return The new instance of CEvtDatabase.
     * @leave System wide error code if the object creation fails.         
     */
    static CEvtDatabase* NewL( );
    
    /**
     * Constructs a new instance of Event Storage Manager interface.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of CEvtDatabase.
     * @leave System wide error code if the object creation fails.         
     */
    static CEvtDatabase* NewLC( );  

    /**
    * Destructor.
    */
    virtual ~CEvtDatabase();
                    
    /**
     * Get Events List based on status.
     */                
    void GetEventsL( TEvtEventStatus 	aEvtStatus,
                     RPointerArray<CEvtBasicEventInfo>& aEvtEventArray );
                    
    /**
     * Get Events List based on status.
     */                
    void GetEventsL( RPointerArray<CEvtBasicEventInfo>& aEvtEventArray );
		
    /**
     * Get Event based on unique Id.
     */    
    CEvtEvent* GetEventL( TEvtEventId aEvtId  );
	
    /**
     * Update Event based on unique Id.
     */    
    void UpdateEventL( CEvtEvent& aEvtEvent );
	
    /**
     * Insert Event to database.
     */    
    void AddEventL( CEvtEvent&     aEvtEvent );
	
    /**
     * Remove Events from database.
     */    
    void RemoveEventsL( const RArray<TEvtEventId>& aEvtIdArray );
	
    /**
     * Update Event Status.
     */ 
    void UpdateEventsStatusL( const RArray<TEvtEventId>& aEventArray,  TEvtEventStatus  aEvtStatus );

    /**
     * Update Event Status based on unique Id.
     */    
    void UpdateEventStatusL( 
                            TEvtEventId      aEvtId,
                            TEvtEventStatus    aEvtStatus);
                            
    /**
     * Remove Event from database based on unique Id.
     */    
    void RemoveEventL( TEvtEventId      aEvtId );	
	
    /**
     * Set the Observer fo Storage Database.
     */
    void SetObserver( MEvtStorageDbObserver* aStorageDbObserver );
	
	 // Inherited from MEvtDbObserver
    /**
     * Observer for TDbChangeType changes in Storage Database.
     */
    void HandleDbChangedL( );

   
private:

    /**
     * Open Database Connection.
     */
	void OpenDatabaseL( );

    /**
     * Create and Open Database Connection.
     */
	void CreateOpenDatabaseL( );

    /**
     * Close Database Connection.
     */
	void CloseDatabase();

    /**
     * Create the Schema for Events Storage.
     */
	void CreateSchemaL();

    /**
     * Create All the Tables.
     */
	void CreateTablesL();

    /**
     * Create All the Indexes.
     */
	void CreateIndexL();

    /**
     * Create All the related Triggers.
     */
	void CreateTriggerL();

    /**
     * Get Events based on Query String.
     */
    void GetEventsL( RSqlStatement& 	aStmt,
                     RPointerArray<CEvtBasicEventInfo>& aEvtEventArray );
                     
    /**
     * Update Event Table.
     */
    void UpdateEventTableL( CEvtEvent&          aEvtEvent );

    /**
     * Delete Event.
     */
	void DeleteEventL( TEvtEventId          aEventId );

    /**
     * check if Event is present.
     */
	TBool CheckIfEventPresentL( TEvtEventId          aId );

    /**
     * check if Action is present.
     */
	TBool CheckIfActionPresentL( TInt64          aId );

    /**
     * Get the action Id based on Event Id.
     */
	TInt64 GetActionId( TEvtEventId          aId );

    /**
     * Create Action.
     */
    void CreateActionL( CEvtAction&   aEvtAction );
    
    /**
     * Update Action.
     */
    void UpdateActionL( CEvtAction&   aEvtAction );
    
    /**
     * Delete Action.
     */
	void DeleteActionL( TEvtEventId          aEventId );

    /**
     * Default C++ Constructor.
     */
    CEvtDatabase( );

    /**
     * Second phase of the two phase constructor.
     */
    void ConstructL();

private: // data

    /**
     *  RSqlDatabase Instance
     */
    RSqlDatabase iDb;

    /**
     *  CEvtDbNotifier Instance
     */
    CEvtDbNotifier* iDbNotifier;

    /**
     *  CEvtDbNotifier Instance
     */
    MEvtStorageDbObserver* iStorageDbObserver;
    
    };

#endif // C_EVTDATABASE_H
