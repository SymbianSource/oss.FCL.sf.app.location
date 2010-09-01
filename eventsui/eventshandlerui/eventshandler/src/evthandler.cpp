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
* Description:  Client Provider for Events Handler Server
*
*/


//INCLUDE FILES

//SYSTEM INCLUDES
#include <e32base.h>
#include <bacline.h> 		// CCommandLineArguments

//USER INCLUDES
#include "evthandler.h"
#include "evthandlerclientserver.h"
#include "evthandlerclient.h"
#include "evthandlerserverconsts.h"
#include "evtdebug.h"
#include "evtmgmtuiengine.h"
#include "evtevent.h"
#include "evteventmanager.h"

//CONSTANTS
_LIT( KEventId,"%d");
_LIT(KEvtHandler,"Handler Ui Panic");
static const TInt KSchedulerCreate = 1;

// ----------------------------------------------------------------------------
// void EvtHandlerStartup::HandleEventL()
// ----------------------------------------------------------------------------
//
void EvtHandlerStartup::HandleEventL()
	{
	EVTUIDEBUG("+ EvtHandlerStartup::HandleEventL()");
	TBuf<KEvtCommandMessageLength> cmdArg;
	TBuf<KEventIdLength> eventId;
    TEvtConsoleAction evtAction = EEvtActionNone;
	TEvtEventId evtId = 0;
	TInt action=0;
	TBool isValidEventId = EFalse;
	
	// create and install the active scheduler we need
	CActiveScheduler* scheduler=new(ELeave) CActiveScheduler;
	__ASSERT_ALWAYS(scheduler!=NULL,User::Panic(KEvtHandler, KSchedulerCreate));
	CleanupStack::PushL(scheduler);
	CActiveScheduler::Install(scheduler);  

	while( ETrue )
		{
		//Create CommandLine Arguments and read it.
		CCommandLineArguments* args = CCommandLineArguments::NewLC();
		// check for valid argument count
		TInt cnt = args->Count();
        TPtrC cmdPrt1( args->Arg( 0 ) );
		if( cnt == 2 ) // 2 - Process name, Console Action:Event Id
			{
			TPtrC cmdPrt( args->Arg( 1 ) ); // read first argument - Console Action:Event Id
			cmdArg.Copy( cmdPrt );
			}
		CleanupStack::PopAndDestroy( args );
		
		// Check for valid arguments 
		if( cmdArg.Length() <= 0 )
			{
			break;
			}
		
		// Extract the Action for Event
		TLex lex( cmdArg );
		TInt errorCode = lex.Val( action );
		if( errorCode != KErrNone)
			{
			break;
			}
		
		// Get the meaningful Action.
		evtAction = static_cast<TEvtConsoleAction>(action);
        
        // Increment the Delimiter
        lex.Inc();

        // Extract the Id of Event
		errorCode = lex.Val( evtId, EDecimal );
        if( errorCode != KErrNone)
            {
            break;
            }
        
        // Format the Event Id buffer
		eventId.Format( KEventId, evtId );
		
		// Set the Valid Event Id flag as True 
		isValidEventId = ETrue;
		break;
		}

	if ( isValidEventId )
		{
		EVTUIDEBUG1("EvtHandlerStartup::HandleEventL - Event Id %S",&eventId);
		
		// Create an engine instance to handle the requested actions.
		CEvtMgmtUiEngine* engine = CEvtMgmtUiEngine::NewLC();
		
		switch( evtAction )
		    {
		    case EEvtActionFired:
				{
		        // Calculate the Accuracy of the Fired Event
		        TEvtFireAccuracy accuracy = engine->CalculateFiredAccuracyL( evtId );
		        
		        // Launch the Server to handle the fired event
		        REvtHandlerSession popupClient;
		        CleanupClosePushL( popupClient );
		        User::LeaveIfError( popupClient.Connect() );
		        popupClient.SendEventId( eventId, accuracy );
		        CleanupStack::PopAndDestroy( );   // popupClient
		        break;
				}
		    case EEvtActionChangeState:
		        {
				// Handle the Change State
		        engine->HandleFiredTriggerStateL( evtId );
		        break;
				}
		    default:
		        break;
		    }
		
		CleanupStack::PopAndDestroy( );	// engine
		}
	
	// Delete active scheduler
   	CleanupStack::PopAndDestroy(scheduler);
   	
	EVTUIDEBUG("- EvtHandlerStartup::HandleEventL()");
	}

// ---------------------------------------------------------------------------
// TInt EvtHandlerServerStartup::StartEvtHandlerServer()
// ---------------------------------------------------------------------------
//
TInt EvtHandlerStartup::StartEvtHandler()
    {
	EVTUIDEBUG("+ EvtHandlerStartup::StartEvtHandler()");
    __UHEAP_MARK;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    TInt ret = KErrNoMemory;
    if ( cleanupStack )
        {
        TRAP(ret, HandleEventL());
        delete cleanupStack;
        }
    __UHEAP_MARKEND;
	EVTUIDEBUG("- EvtHandlerStartup::StartEvtHandler()");
    return ret;    
    }

// -----------------------------------------------------------------------------
// TInt E32Main
// Server process entry-point
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
	EVTUIDEBUG("+ E32Main()");
    return EvtHandlerStartup::StartEvtHandler();    
    }

// End of File
