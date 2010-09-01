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
* Description:  Server class for Events Handler Server
*
*/


#ifndef C_EVTHANDLERSERVER_H
#define C_EVTHANDLERSERVER_H

#include <e32base.h>

#include "evthandlershutdowntimer.h"
#include "evtnotifierobserver.h"
#include "evtdefs.h"
#include "evteventinfo.h"
#include "evtsnoozehandler.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION  PROTOTYPES

// FORWARD DECLARATIONS
class CEvtMgmtUiEngine;
class CEvtNotifierHandler;

// CLASS DECLARATION

/**
 * Events Handler Server class. This class extends the CServer2
 * class for Events Handler Server.
 *
 * @lib evthandlerserver.exe
 * @since S60 v9.1
 */
class CEvtHandlerServer : public CServer2, 
		                  public MEvtNotifierObserver, 
		                  public MEvtSnoozeObserver
	{
public:
	static CServer2* NewLC();
	virtual ~CEvtHandlerServer();
	
	/**
	 * Handles the Event
	 */
	void HandleNewEventL( const TEvtEventId aEventId, const TInt aAccuracy );
	
private:
	CEvtHandlerServer();
	void ConstructL();
	CSession2* NewSessionL( const TVersion& aVersion, const RMessage2& aMessage ) const;
	void HandleEventIdL( const TDesC& aMessage );
	void HandleNextEventL();
	void UpdateEventsQueueL( CEvtEvent* aEvent );
	
    void LaunchAppL(       TUid       aApplicationUid,
                     const TDesC8&     aCmdLineArg );
    
    void ChangeStateL( TEvtEventId aEvtId );
	
private: // derived methods	
	// derived from MEvtNotifierObserver
    void HandlerCompleteL( 
    	TEvtInfoNoteResult aResult,
        CEvtEvent* aEvent );
	
	// derived from MEvtSnoozeObserver
    void SnoozeCompleteL( 
    	TEvtSnoozeItem& aSnoozeItem, 
    	TBool aSnoozeAgain );
	
private:
	TInt iSessionCount;
    /**
    * event handler shutdown timer
    * Owns
    */
	CEvtHandlerShutdownTimer iShutdownTimer;

    /**
    * event notifier handler 
    * Owns
    */    
    CEvtNotifierHandler*	iEvtNotifierHandler;	    

    /**
    * event snooze handler 
    * Owns
    */    
    CEvtSnoozeHandler*	iEvtSnoozeHandler;	   

    /**
    * event managment engine
    * Owns
    */    
    CEvtMgmtUiEngine*		iEvtMgmtUiEngine;

    /**
    * events id queue for notifier handler
    * Owns
    */    
	RArray<TEvtEventInfo> iEventsQueue;
	};
	
#endif  // C_EVTHANDLERSERVER_H

// End of File
