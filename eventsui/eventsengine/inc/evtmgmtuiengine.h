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
* Description:  Engine class for Events Management UI.
*
*/


#ifndef EVTMGMTUIENGINE_H
#define EVTMGMTUIENGINE_H

#include <e32base.h>
#include "evtstoragedbobserver.h"
#include "evtmgmtuilbtobserver.h"
#include "evtdefs.h"
#include "evtbasiceventinfo.h"
#include "evtevent.h"

// forward declarations go here:
class CEvtEventManager;
class CEvtEvent;
class CEvtMgmtUiLbtAdapter;
typedef TUint32 TEvtEventAttributeMask;

class MEvtMgmtUiEngineObserver
    {
public:
    /**
     * It will be used to notify observer that event operation
     * has been completed
     * @param aTriggerId trigger id of newly created trigger
     * @param aErrorCode It will contain the status of the operation
     *                KErrNone if successful otherwise error status code          
     */
    virtual void NotifyEventChangeL(TInt aErrorCode, TInt aState ) = 0;
    
    /**
     * It will be used to notify observer that database
     * has been completed
     */
    virtual void NotifyModelChangeL() = 0;
    };


/**
 *  @class CEvtMgmtUiEngine
 *  Class that represents the engine for evtmgmtui application
 *  
 *  It  is used to create, update, delete, retrive events.
 *  It is also used to change state of the event.
 *
 *  @since Symbian v9.1
 */
class CEvtMgmtUiEngine : public CBase,
                         public MEvtMgmtUiLbtObserver,
                         public MEvtStorageDbObserver
    {
    public:
        /**
         * Enumeration to Filter based on status.
         */
        enum TEvtEventStatusFilter
            {
            /**
             * To retrive active events
             */
            EEvtFilterActive                               = 0,
            
            /**
             * To retrive draft events
             */
            EEvtFilterDraft                                = 1,
            
            /**
             * To retrive completed events
             */
            EEvtFilterCompleted                            = 2,
            
            /**
             * To retrieve all events
             */
            EEvtFilterAll                                  = 3
            };

        /**
         * Enumeration to identify the modified attributes for an event
         */
        enum TEvtEventAttribute
            {
            EEvtEventAttributeSubject = 1,
            EEvtEventAttributePlace = 2,
            EEvtEventAttributeRadius = 16,
            EEvtEventAttributeRepeat = 32,
            EEvtEventAttributeDesc = 64,
            EEvtEventAttributeTone = 128,
            EEvtEventAttributeAudioLoop = 256,
            EEvtEventAttributeStatus = 512
            };
    public: // Constructor and destructor  
        /**
         * Static Two phase constructor
         */
        IMPORT_C static CEvtMgmtUiEngine* NewL();
        /**
         * Static Two phase constructor
         */
        IMPORT_C static CEvtMgmtUiEngine* NewLC();
    
        /**
         * Virtual destructor
         */
         virtual ~CEvtMgmtUiEngine();
     
    public://new functions
        
        /**
          * Set observer to notify engine operation is completed.
          *
          * @since S60 v5.0    
          * @paramp[in] aObserver - Observer which wants to get notifications.
          */
        IMPORT_C void SetObserver(MEvtMgmtUiEngineObserver* aObserver);
        
        /**
          * Remove observer to notify engine operation is completed.
          *
          * @since S60 v5.0    
          * @paramp[in] aObserver - Observer which wants to get notifications.
          */    
        IMPORT_C void RemoveObserver(MEvtMgmtUiEngineObserver* aObserver);
        
        /**
         * returns event object
         *
         * @since S60 v5.0    
         * 
         * @param[in] aId - event id of the event to be retrieved. 
         * @ret event object 
         */
        IMPORT_C CEvtEvent* EventL( TEvtEventId aId );
              
        /**
          * Updates model
          *
          * @since S60 v5.0 
          * 
          * TODO: should be removed
          */
        IMPORT_C void UpdateModelL( RPointerArray<CEvtBasicEventInfo>& aEventsArray,
                                        TEvtEventStatusFilter  aEventStatusFilter );
        
         /**
          * It will add an event to database
          * @since S60 v5.0    
          */
         IMPORT_C void AddEventL( CEvtEvent& aEvtEvent );
         
         /**
           * It will modify event to database
           * @since S60 v5.0    
           */
         IMPORT_C void ModifyEventL( CEvtEvent& aEvtEvent, 
                             TEvtEventAttributeMask aEventAttributeMask );
          
         /**
           * It will delete the list of Events from the database.
           * @param[in] aId id of event to be deleted
           * @since S60 v5.0    
           */
         IMPORT_C void DeleteEventL(  TEvtEventId aId = -1 );
         
         /**
          * It will delete event from database
          * @param[in] aEvtIdArray aEventArray Reference to the Events Id array that  
          *                          are to be removed form the database.  
          * @since S60 v5.0    
          */
         IMPORT_C void DeleteEventsL(  const RArray<TEvtEventId>* aEvtIdArray );
         
         /*
          * Retrieve location details
          */
         IMPORT_C void RetrieveLocationL( TEvtEventId aEventId, TCoordinate& aCoordinate );
         
         /**
           * It will set state of the event.   
           * @param[in] aId id of event to be updated
           * @param aEvtStatus status of the event
           * @since S60 v5.0    
           */
         IMPORT_C void SetEventStateL( TEvtEventStatus aEvtStatus, TEvtEventId aId = -1 );
         
         /**
          * It will set state of the events.   
          * @param[in] aEvtIdArray idlist of events to be updated
          * @param aEvtStatus status of the event
          * @since S60 v5.0    
          */
         IMPORT_C void SetEventsStateL( TEvtEventStatus aEvtStatus, 
                     const RArray<TEvtEventId>* aEvtIdArray );
    
         /**
          * Synchronizes the LBT and EvtStorage Db.     
          * @since S60 v5.0    
          */
         IMPORT_C void SyncDbsL();     
         
         /**
          * Calculates the Accuracy of Fired Trigger.     
          * @since S60 v5.0    
          */
         IMPORT_C TEvtFireAccuracy CalculateFiredAccuracyL( 
                                                const TEvtEventId aEventId );
                                                
         /**
          * Handle Fired Trigger state.    
          * Trigger State should be completed if fired Event is not repetitive. 
          * @since S60 v5.0    
          */
         IMPORT_C void HandleFiredTriggerStateL( 
                                                const TEvtEventId aEventId );
         /**
          * Cancels all the outstanding lbt request.    
          * @since S60 v5.0    
          */
         IMPORT_C void CancelRequest();
          
    public: //Derived
        /**
         * Derived from MEvtMgmtUiLbtObserver
         */
        void NotifyTriggerChangeL( const TLbtTriggerId &  /*aTriggerId*/,
                                        TLbtTriggerChangeType aType );
        /**
         * Derived from MEvtMgmtUiLbtObserver
         */
        void NotifyLbtChangeL(TLbtTriggerId &  aTriggerId, TInt aState );
        
        /**
        * Derived from MEvtMgmtUiLbtObserver
        */
        void NotifyLbtError(TInt aErrorCode, TInt aState );
        /**
        * Derived from MEvtStorageDbObserver
        */
        void HandleStorageDbChangedL();
    
    private:
    // constructor and the Symbian second-phase constructor
    
        CEvtMgmtUiEngine();
    
        void ConstructL();
       
    private: // data
         
        /**
        * event manager
        * Owns
        */
        CEvtEventManager*       iEventManager; 
       
        /**
        * event 
        * Owns
        */
        CEvtMgmtUiLbtAdapter*   iLbtAdapter;
        
        /*
         * Observer to notify engine operation is completed.
         * Does not own
         */
        RPointerArray< MEvtMgmtUiEngineObserver > iObserverArray;
        
        /*
         * Temporary Event to hold till the Event is added/Updated to lbt.
         * Does not own
         */
        CEvtEvent* iEvent;
        
        /*
         * Temporary array to hold idlist till the Events are deleted/status changed to lbt.
         * Does not own
         */
        const RArray<TEvtEventId>* iEvtIdArray;
        
        /*
         * Temporary variable to hold state till the Events status changed to lbt.
         * 
         */
        TEvtEventStatus iEvtStatus;

    };

#endif // C_EVTMGMTUIMODEL_H
