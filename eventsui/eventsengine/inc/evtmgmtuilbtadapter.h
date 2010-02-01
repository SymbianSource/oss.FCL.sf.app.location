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
* Description:  It is adapter class to handle lbt operations
*
*/


#ifndef EVTMGMTUILBTADAPTER_H_
#define EVTMGMTUILBTADAPTER_H_

#include <e32base.h>        // CBase

#include <lbs.h>
#include <lbtcommon.h>
#include <lbtserver.h> 
#include <lbt.h>
#include <lbttriggerchangeeventobserver.h> 
#include <lbttriggerdynamicinfo.h>

#include "evtmgmtuilbtobserver.h"
#include "evtdefs.h"

class CLbtStartupTrigger;
class CLbtGeoCircle;
class CLbtTriggerConditionArea;
class CLbtTriggerChangeEventNotifier;
class CLbtTriggerFilterByAttribute;

/**
 *  @class CEvtMgmtUiLbtAdapter
 *  Class that represents the adapter for lbt operation
 *
 *  This class acts as an adapter for lbt engine. 
 *  It  is used to create, update, delete, retrive triggers.
 *  It is also used to change state of the trigger
 *
 *  @since S60 v9.1
 */

class CEvtMgmtUiLbtAdapter : public CActive,
								public MLbtTriggerChangeEventObserver
    {
public:
    /*
     *  state machine for lbt operation
     */
    enum TLbtAdtapterState
        {        
        /*
         * state for creating trigger
         */
        ELbtAdapterCreate,
        
        /*
         *  state for updating trigger
         */
        ELbtAdapterUpdate,
        
        /*
        *  state for updating command line argument of trigger
        */
        ELbtAdapterUpdateCmdLineArg,
        
        /*
         *  state for deleting trigger
         */
        ELbtAdapterDelete,
        
        /*
         *  state for deleting triggers
         */
        ELbtAdapterDeleteTriggers,

        /*
         *  state for changing state of the trigger
         */
        ELbtAdapterSetTriggerState,
        
        /*
         *  state for changing state of the triggers
         */
       ELbtAdapterSetTriggersState,
        
        /**
         * Default option
         */
        ENone
        };
    
    public:  // Constructor and destructor  
      /**
       * Static Two phase constructor
       *         
       */
      static CEvtMgmtUiLbtAdapter* NewL(MEvtMgmtUiLbtObserver&  aObserver);

      /**
       * Virtual destructor
       */
      virtual ~CEvtMgmtUiLbtAdapter(); 
         
    public: // new functions
        /*
         * It is the wrapper function to trap leaving function CreateTriggerL()
         * 
         * @since S60 v9.1
         * 
         * @param [in] aCenter - location of the trigger.
         * @param [in] aRadiusInKiloMeters - radius of the trigger
         * @param [in] aTriggerName - name of the trigger
         * @param [in] aState - state of the trigger
         * @param [in] aTriggerId - trigger id of the trigger -
         *              if it is zero, trigger will be created
         *              othewise it will be updated
         * @param [in] aEventId - event id of the triggering event
         *              It will be passed as command line argument.
         */
        void CreateTrigger(const TCoordinate& aCenter,
                TReal aRadiusInKiloMeters, const TPtrC& aTriggerName,
                CLbtTriggerEntry::TLbtTriggerState aState );

        
        /*
        * CEvtMgmtUiLbtAdapter ::UpdateCmdLineArgL()
        * Update the command line argument of the give trigger
        * @param[in] aTriggerId - id of the trigger
        * @param[in] aEventId - event id of the triggering event
        *              It will be passed as command line argument.
        */
        void UpdateCmdLineArgL( TLbtTriggerId aTriggerId, TEvtEventId aEventId );
        
        /*
        * CEvtMgmtUiLbtAdapter ::UpdateLocationL()
        * Update the location of the give trigger
        * @param[in] aTriggerId - id of the trigger
        * @param [in] aCenter - location of the trigger.
        * @param [in] aRadiusInMeters - radius of the trigger
        * @param [in] aState - state of the trigger      
        */
        void UpdateTriggerL( TLbtTriggerId aTriggerId,
                const TCoordinate& aCenter, TReal aRadiusInKiloMeters,
                CLbtTriggerEntry::TLbtTriggerState aState );
        /*
         * It is used to delete trigger from lbt database
         * 
         * @since S60 v9.1
         * 
         * @param [in] aTriggerId - trigger id of the trigger
         * TODO: @param [in] aDeleteTriggerOnly remove
         * 
         */ 
         void DeleteTriggerL( TLbtTriggerId  aTriggerId );
           
         /*
          * It is used to delete triggers from lbt database
          * 
          * @since S60 v9.1
          * 
          * @param [in] aTriggerIdList - id-list of triggers to be deleted.
          * 
          */
         void DeleteTriggersL( const RArray<TLbtTriggerId>& aTriggerIdList );  
         
         /*
          * It is used to change the state of the trigger.
          * 
          * @since S60 v9.1
          * 
          * @param [in] aTriggerId - trigger id of the trigger
          * @param [in] aState - value to which given trigger's state should be changed
          *  
          */    
        void SetTriggerStateL( TLbtTriggerId  aTriggerId,
									CLbtTriggerEntry::TLbtTriggerState aState );
        
        /*
         * It is used to change the state of the triggers.
         * 
         * @since S60 v9.1
         * 
         * @param [in] aTriggerIdList - trigger id list of the triggers
         * @param [in] aState - value to which given trigger's state should be changed
         *  
         */ 
        void SetTriggersStateL( const RArray<TLbtTriggerId>& aTriggerIdList,
                                            CLbtTriggerEntry::TLbtTriggerState aState ) ; 

        /*
          * It is used to retrieve location information of given trigger
          * from lbt database
          * 
          * @since S60 v9.1
          * 
          * @param [in] aTriggerId - trigger id of the trigger
          * @param [out] aCenter -  location details of the given trigger
          *  
          */
        void RetrieveLocationL( TLbtTriggerId aTriggerId, TCoordinate& aCenter );
         
        /*
          * It is used to retrieve list of triggers created by EventsUi
          * 
          * @since S60 v9.1
          * 
          * @param [in] triggerList - Reference to trigger array
          * @param [out] aState -  Active/Inactive State of Trigger
          *  
          */
		void GetTriggerListL( RArray<TLbtTriggerId>& aTriggerList,
								CLbtTriggerEntry::TLbtTriggerState aState ); 

        /*
          * It is used to retrieve list of triggers created by EventsUi
          * 
          * @since S60 v9.1
          * 
          * @param [in] triggerList - Reference to trigger array
          * @param [out] aState -  Valid/Invalid State of Trigger
          *  
          */
		void GetTriggerListL( RArray<TLbtTriggerId>& aTriggerList,
                TLbtTriggerDynamicInfo::TLbtTriggerValidity aValid );
        /*
          * It is used to returns the Position Accuracy of Fired Trigger.
          * 
          * @since S60 v9.1
          * 
          * @param [in] aPosition - Contains the Position Accuracy for Fired Trigger
          * @param [in] aTriggerId - Trigger Id
          * @param [out] TBool - Boolean value for Trigger Fired or Not.
          *  
          */
		TBool GetFiredPositionL( TPosition& aPosition,
											const TLbtTriggerId aTriggerId );

        /*
          * It is used to get the distance accuracy for Fired trigger.
          * 
          * @since S60 v9.1
          * 
          * @param [in] aTriggerId - Trigger Id
          * @param [out] aDistance - Accuracy in metres at which the trigger fired.
          *  
          */
		void GetFiredTriggerAccuracyL( TLbtTriggerId aTriggerId, TReal32& aDistance );

        /*
          * It is used to retrieve State of given trigger
          * from lbt database
          * 
          * @since S60 v9.1
          * 
          * @param [in] aTriggerId - trigger id of the trigger
          * @param [out] TLbtTriggerState -  State of the given trigger
          *  
          */
		CLbtTriggerEntry::TLbtTriggerState GetTriggerStateL( 
											TLbtTriggerId  aTriggerId );
		
		void CancelRequest();
 
    public: //derived
        
        /*
         * Derived from CActive
         */
        void RunL();
        
        /*
         * Derived from CActive
         */
        void DoCancel();
        
        /*
         * Derived from CActive
         */
        TInt RunError( TInt aError );
        
        /*
         * Derived from MLbtTriggerChangeEventObserver
         */
    	void TriggerChangedL(const TLbtTriggerChangeEvent &aEvent);
      
    private:  // Constructor
        /**
         * C++ Default constructor
         */       
        CEvtMgmtUiLbtAdapter(MEvtMgmtUiLbtObserver&  aObserver);
  
        /**
         * Second phase of the two phase constructor
         */ 
         void ConstructL();
    
    private: // New functions
        /*
         * It is used to create of update trigger.
         * If trigger id is zero, it will create trigger.
         * If trigger id is non-zero, it will update the trigger.
         * 
         * @since S60 v9.1
         * 
         * @param [in] aCenter - location of the trigger.
         * @param [in] aRadiusInKiloMeters - radius of the trigger
         * @param [in] aTriggerName - name of the trigger
         * @param [in] aState - state of the trigger
         * @param [in] aTriggerId - trigger id of the trigger -
         *              if it is zero, trigger will be created
         *              othewise it will be updated
         * @param [in] aEventId - event id of the triggering event
         *              It will be passed as command line argument.
         */
        void CreateTriggerL(const TCoordinate& aCenter,
                           TReal aRadiusInKiloMeters, const TPtrC& aTriggerName,
                           CLbtTriggerEntry::TLbtTriggerState aState );
        
        /*
         * It is used to create the Codition Area for a trigger depending on the trigger type.
         * 
         * @since S60 v9.1
         * 
         * @param [in] aCenter - location of the trigger.
         * @param [out] CLbtTriggerConditionArea - Condition Area for Trigger.
         */
        CLbtTriggerConditionArea* CEvtMgmtUiLbtAdapter::GetConditionAreaL( 
                        const TCoordinate& aCenter,
                        TReal aRadiusInKiloMeters );
    private:
        /*
         *  Handler to lbt session
         */
        RLbt                    iLbt;
        
        /*
         *  Handler to lbt session
         */
        RLbt                    iLbtObserver;
        
        /*
         *  Handler to lbt server
         */
        RLbtServer              iLbtServer;
       	 
        /**
         * Reference Trigger Change Notifier
         *
         */
       	CLbtTriggerChangeEventNotifier * iEventChangeObserver;
        
        /*
         * Id of a trigger
         */
        TLbtTriggerId           iTriggerId;
       
        /*
         *  State machine's current state
         */
        TLbtAdtapterState       iState;       
        
        /*
         *  start up trigger object
         *  owns
         */
        CLbtStartupTrigger*     iTrigger;
        
        /*
         * Observer to notify lbt changes or errors
         */
        MEvtMgmtUiLbtObserver&  iObserver;    
       
        /*
         * Filter to do any lbt operation(Update State).
         * Does not own
         */
        CLbtTriggerFilterByAttribute* iAttrFilter;    
    };

#endif //EVTMGMTUILBTADAPTER_H_
