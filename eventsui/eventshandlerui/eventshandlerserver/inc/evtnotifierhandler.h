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
* Description:  Handler for notifier on Event
*
*/



#ifndef EVTNOTIFIERHANDLER_H
#define EVTNOTIFIERHANDLER_H

//  INCLUDES

#include <e32base.h> 

#include <apgcli.h>
#include <apacmdln.h>
#include <bacline.h>
#include <AknNotifierWrapper.h>
#include <s32mem.h>

#include "evtinfonoteparams.h"
#include "evthandlerserverconsts.h"
#include "evteventmanager.h"
#include "evtevent.h"
#include "evteventinfo.h"


// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION  PROTOTYPES

// FORWARD DECLARATIONS
class MEvtNotifierObserver;
class CEvtGlobalQuery;

// CLASS DECLARATION

/**
*  Handler for query on Event. This handler show query notifier
*  and return result to connect manager.
*
*  @since Series 60 9.1
*/
class CEvtNotifierHandler: public CActive
    {
    public:
    //Handler id
    enum THandlerType
        {
        EHandlerNotifier,   ///Device selection handler id
        EHandlerGlobalNote
        };
    
    public: 
        /**
        * Two-phase construction.
        */
        static CEvtNotifierHandler * NewL(
        	MEvtNotifierObserver& aNotifierObserver,
        	TEvtEventInfo aEventInfo );

        /**
        * Destructor
        */
        virtual ~CEvtNotifierHandler();
		
        /**
        * Check if Event is Launchable - Is it present and Active
        */
		TBool IsLaunchable();
        
        /**
        * Starting the Notifier
        */
        void StartEvtNotifierL();

    private:
        /**
        * From CActive
        */
        void RunL();

        /**
        * From CActive
        */
        void DoCancel();

        /**
        * From CActive
        */
        TInt RunError( TInt aError );

    private:

        /**
        * Second phase of the construction
        */
        void ConstructL( );

        /**
        * Private constructor
        */
        CEvtNotifierHandler(
        	MEvtNotifierObserver& aNotifierObserver,
        	TEvtEventInfo aEventInfo );
        
    private:
		
	    /**
		 * Notifier Observer.
		 */
        MEvtNotifierObserver& iNotifierObserver;
		
	    /**
		 * RNotifer Instance.
		 */
        RNotifier iNotifier;
		
	    /**
		 * Event Global Query.
		 * Owns:
		 */
        CEvtGlobalQuery* iEvtGlobalQuery;
		
	    /**
		 * Event's Info note object to internalize and externalize
		 * the information required by Info Note.
		 * Owns:
		 */
        CEvtInfoNoteInputParam* iInputParam;
		
	    /**
		 * Buffer which contains the Input parameters to notifier
		 * Owns:
		 */
        CBufFlat* iInputBuffer;
		
	    /**
		 * Response buffer to pass to the notifier
		 */
	    TBuf8<KEvtNotifierBufferLength>  iResponseBuffer;
		
	    /**
		 * Current Event's dynamic Info
		 */
	    TEvtEventInfo iEventInfo;
		
	    /**
		 * Handler type which can be either RNotifier or Global query
		 */
	    THandlerType iHandlerType;
		
	    /**
		 * Engine Class to get the Event object
		 * Owns:
		 */
	    CEvtEventManager* iEventManager;
		
	    /**
		 * Event object for which the notifier has to be launched
		 * Owns:
		 */
	    CEvtEvent* iEvent;
    };

#endif //EVTNOTIFIERHANDLER_H

// End of File
