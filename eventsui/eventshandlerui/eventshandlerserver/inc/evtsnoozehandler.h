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
* Description:  Handler for Snoozing of Event
*
*/



#ifndef EVTSNOOZEHANDLER_H
#define EVTSNOOZEHANDLER_H

//  INCLUDES

#include <e32base.h> 
#include <e32std.h>
#include <cenrepnotifyhandler.h>

#include "evtdefs.h"

// CONSTANTS

// MACROS

// DATA TYPES
	
// FUNCTION  PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Event Strucutre for Snooze.
*
*  @since Series 60 9.2
*/
class TEvtSnoozeItem
    { 
    public :  
		/**
        * Copy constructor
        */
        TEvtSnoozeItem(TEvtEventId aId, TEvtFireAccuracy aAcc, TTime aTime, TUint8 aCount = 1)
            	    {
        	        iEventId = aId;
        	        iAccuracy = aAcc;
        	        iTime = aTime;
        	        iCount = aCount;
            	    };
			
		/**
        * inline function used for comparision
        */
        static inline TInt Match(const TEvtSnoozeItem& aFirst, const TEvtSnoozeItem& aSecond)
            {
            if (aFirst.iEventId == aSecond.iEventId)
                return 1;
            return 0;
            };
			
		/**
        * inline function used for comparision
        */
        static inline TInt Compare(const TEvtSnoozeItem& aFirst, const TEvtSnoozeItem& aSecond)
            {
            if (aFirst.iTime < aSecond.iTime)
                return -1;
            if (aFirst.iTime > aSecond.iTime)
                return 1;
            return 0;
            };
    public :
		// Event's Information like Event Id, Accuracy, Time and Count
    	TEvtEventId iEventId;
    	TEvtFireAccuracy iAccuracy;
    	TTime iTime;
    	TUint8 iCount;
 };

/**
*  Observer class for snooze handlers. Observer will get the call back when
*  Snooze for a particular event is complete.
*
*  @since Series 60 9.1
*/
class MEvtSnoozeObserver
    {
    public: 
        /**
        * Call back function when the Snooze is complete.
		* @param aSnoozeItem Snooze Item
		* @param aSnoozeAgain Snooze again flag which indicates if the event
		* has to be snnozed again.
        */
        virtual void SnoozeCompleteL( 
        	TEvtSnoozeItem& aSnoozeItem, TBool aSnoozeAgain ) = 0;
    };
    
    
/**
*  Handler for Snoozing the Event.
*
*  @since Series 60 9.1
*/
class CEvtSnoozeHandler: public CTimer, public MCenRepNotifyHandlerCallback
    {
    public: 
        /**
        * Two-phase construction.
        */
        static CEvtSnoozeHandler * NewL(
        	MEvtSnoozeObserver& aSnoozeObserver );

        /**
        * Destructor
        */
        virtual ~CEvtSnoozeHandler();
        
        /**
        * Remove the Event from the Snooze Que
		* @param[in] aSnoozeItem Snooze Item
        */
        void RemoveIfPresent(const TEvtSnoozeItem& aSnoozeItem);
        
        /**
        * Add the Event to Snooze
		* @param[in] aSnoozeItem Snooze Item
        */
        void Snooze(TEvtSnoozeItem aSnoozeItem);
        
        /**
        * Check if the Snooze Que is empty.
		* @ret Return ETrue if the Snooze Que is empty.
        */
        TBool IsSnoozeQueEmpty();

    private: 	// Methods derived from MCenRepNotifyHandlerCallback
        void HandleNotifyInt(TUint32 aId, TInt aNewValue);

    private:
        /**
        * From CActive
        */
        void RunL();

    private:

        /**
        * Second phase of the construction
        */
        void ConstructL( );

        /**
        * Private constructor
        */
        CEvtSnoozeHandler(
        	MEvtSnoozeObserver& aSnoozeObserver );
        
    private:
        /**
        * Snoozer Observer
        */   
        MEvtSnoozeObserver& iSnoozeObserver;

        /**
        * events id queue for snooze
        * Owns
        */    
    	RArray<TEvtSnoozeItem> iSnoozeQueue;

        /**
        * events id queue for snooze
        * Owns
        */    
    	RArray<TEvtSnoozeItem> iSnoozeCompleteQueue;

    	/**
    	 * Cenrep Session to Snooze Settings.
    	 * Own
    	 */
         CRepository* iSettingSession;
    	 
    	/**
    	 * Handler to Cenrep Interval key Notifier
    	 * Own
    	 */
    	CCenRepNotifyHandler* iIntervalNotifyHandler;
    	 
    	/**
    	 * Handler to Cenrep Period key Notifier
    	 * Own
    	 */
    	CCenRepNotifyHandler* iPeriodNotifyHandler;

        /**
        * Snooze Interval in minutes
        */    
    	TInt iSnoozeInterval;

        /**
        * Snooze Count
        */    
    	TInt iSnoozeCount;
    };

#endif //EVTSNOOZEHANDLER_H

// End of File
