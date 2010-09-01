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
* Description:  server implementation for Events Handler Server
*
*/


#include "evthandlerclientserver.h"
#include "evthandlerserversession.h"
#include "evthandlerserverconsts.h"
#include "evtnotifierhandler.h"
#include "evtmgmtuiengine.h"
#include "evtevent.h"
#include "evtdebug.h"

#include "evthandlerserver.h"

#include <AknGlobalNote.h>  // CAknGlobalNote
#include <w32std.h>
#include <apgtask.h>
#include <apgcli.h>
#include <apacmdln.h>
#include <bacline.h>

static const TInt KHandlerAppUid = 0x2001E668;
_LIT(KHandlerEXE, "evthandler.exe");
_LIT( KDelimiter, ":");

// ---------------------------------------------------------------------------
// CEvtHandlerServer::CEvtHandlerServer()
// ---------------------------------------------------------------------------
//
inline CEvtHandlerServer::CEvtHandlerServer()
		:CServer2( 0, ESharableSessions ), 
		iEventsQueue( KEvtItemArrayGranularity )
	{}

// ---------------------------------------------------------------------------
// CEvtHandlerServer::~CEvtHandlerServer()
// ---------------------------------------------------------------------------
//
CEvtHandlerServer::~CEvtHandlerServer()
	{
    EVTUIDEBUG( "+ CEvtHandlerServer::~CEvtHandlerServer()" );	
	if( iEvtSnoozeHandler )
		{
		delete iEvtSnoozeHandler;		
		}
		
	if( iEvtNotifierHandler )
		{
		delete iEvtNotifierHandler;		
		}    
    
    iEventsQueue.Close();
    
    EVTUIDEBUG( "- CEvtHandlerServer::~CEvtHandlerServer()" );
	}

// ---------------------------------------------------------------------------
// CEvtHandlerServer::NewLC()
// ---------------------------------------------------------------------------
//
CServer2* CEvtHandlerServer::NewLC()
	{
    EVTUIDEBUG( "+ CEvtHandlerServer::NewLC()" );
	CEvtHandlerServer* self = new ( ELeave ) CEvtHandlerServer;
	CleanupStack::PushL( self );
	self->ConstructL();
	EVTUIDEBUG( "- CEvtHandlerServer::NewLC()" );
	return self;
	}

// ---------------------------------------------------------------------------
// CEvtHandlerServer::ConstructL()
// ---------------------------------------------------------------------------
//
void CEvtHandlerServer::ConstructL()
/*
 * 2nd phase construction - ensure the timer and server objects are running
 */
	{
    EVTUIDEBUG( "+ CEvtHandlerServer::ConstructL()" );
	StartL( KEvtHandlerServerName );
	iShutdownTimer.ConstructL();
	iEvtSnoozeHandler = CEvtSnoozeHandler::NewL( *this );	
    EVTUIDEBUG( "- CEvtHandlerServer::ConstructL()" );
	}

// ---------------------------------------------------------------------------
// CEvtHandlerServer::NewSessionL()
// ---------------------------------------------------------------------------
//
CSession2* CEvtHandlerServer::NewSessionL( const TVersion&, const RMessage2& ) const
/*
 * Cretae a new client session. This should really check the version number.
 */
	{
    EVTUIDEBUG( "+ CEvtHandlerServer::NewSessionL()" );
	return new ( ELeave ) CEvtHandlerServerSession();
	}

// ---------------------------------------------------------------------------
// CEvtHandlerServer::HandleNewEventL()
// ---------------------------------------------------------------------------
//
void CEvtHandlerServer::HandleNewEventL( const TEvtEventId aEventId, const TInt aAccuracy )
    {
	EVTUIDEBUG2("+ CEvtHandlerServer::HandleNewEventL() - EventId = %d, Accuracy = %d", aEventId, aAccuracy);
    iShutdownTimer.Cancel();
       
    // Stop any running request
	delete iEvtNotifierHandler;		
	iEvtNotifierHandler = NULL;    
    
    // Remove if it was present in Snooze Que
	TTime time;
	time.HomeTime();
	TEvtSnoozeItem snoozeItem( aEventId, static_cast<TEvtFireAccuracy>(aAccuracy), time );
    iEvtSnoozeHandler->RemoveIfPresent( snoozeItem );	
     
    // Create an Event Info structure
    TEvtEventInfo evtInfo( aEventId, static_cast<TEvtFireAccuracy>(aAccuracy), ETrue, ETrue );
     
    TIdentityRelation<TEvtEventInfo> matcher(TEvtEventInfo::Match);
    TInt index = iEventsQueue.Find(evtInfo,matcher);

    // Remove if it was present in Notifier Que
    if( index != KErrNotFound )
        {
        iEventsQueue.Remove( index );
        }
        
    // Append this element to the front of queue
    iEventsQueue.Insert( evtInfo, 0 );
    
    // Restart the request handling process
    HandleNextEventL();
	EVTUIDEBUG("- CEvtHandlerServer::HandleNewEventL()");
    }

// ---------------------------------------------------------------------------
// CEvtHandlerServer::HandleNextEventL()
// ---------------------------------------------------------------------------
//
void CEvtHandlerServer::HandleNextEventL()
	{
	EVTUIDEBUG( "+ CEvtHandlerServer::HandleNextEventL()")
	TInt evtCount  = iEventsQueue.Count();
	if( evtCount )
		{		
		EVTUIDEBUG1( "EventId = %d", iEventsQueue[ 0 ].iEventId );
		iEvtNotifierHandler = CEvtNotifierHandler::NewL( *this, iEventsQueue[ 0 ] );
		
		// Check if the Event is Launchable - Is it present and Active	
		if( iEvtNotifierHandler->IsLaunchable() )
			{
			iEvtNotifierHandler->StartEvtNotifierL();
		
	        // Set the tone play to False So that it does not play tone next time.
	        if( iEventsQueue[ 0 ].iPlayTone )
	        	iEventsQueue[ 0 ].iPlayTone = EFalse;
			}
		else
			{
			delete iEvtNotifierHandler;		
			iEvtNotifierHandler = NULL;    
			
			iEventsQueue.Remove( 0 );
			evtCount  = iEventsQueue.Count();
			}
		}
    
	if( !evtCount )
        {
        if( iEvtSnoozeHandler->IsSnoozeQueEmpty() )
            {
            EVTUIDEBUG( "Handler Server shutdown")
            iShutdownTimer.Start();    
            }
        }
    EVTUIDEBUG( "- CEvtHandlerServer::HandleNextEventL()" );
	}
	
// ---------------------------------------------------------------------------
// CEvtHandlerServer::SnoozeCompleteL()
// 
// Derived from MEvtSnoozeObserver 
// ---------------------------------------------------------------------------
//
void CEvtHandlerServer::SnoozeCompleteL( 
    	TEvtSnoozeItem& aSnoozeItem, 
    	TBool aSnoozeAgain )
    {
	EVTUIDEBUG2("+ CEvtHandlerServer::SnoozeCompleteL() - EventId = %d, Snooze = %d", 
	                    aSnoozeItem.iEventId, aSnoozeAgain);
       
    // Stop any running request
	delete iEvtNotifierHandler;		
	iEvtNotifierHandler = NULL;    
     
    // Create an Event Info structure
    TEvtEventInfo evtInfo( aSnoozeItem.iEventId, aSnoozeItem.iAccuracy, ETrue, aSnoozeAgain );
     
    // Append this element to the front of queue
    iEventsQueue.Insert( evtInfo, 0 );
    
    // Restart the request handling process
    HandleNextEventL();
	EVTUIDEBUG("- CEvtHandlerServer::SnoozeCompleteL()");
    }
	
// ---------------------------------------------------------------------------
// CEvtHandlerServer::HandlerCompleteL()
// 
// Derived from MEvtNotifierObserver 
// ---------------------------------------------------------------------------
//
void CEvtHandlerServer::HandlerCompleteL( 
    	TEvtInfoNoteResult aResult,
        CEvtEvent* aEvent )
	{	
    EVTUIDEBUG2( "+ CEvtHandlerServer::HandlerCompleteL() - EventId = %d, Notifier Action = %d", iEventsQueue[ 0 ].iEventId, aResult );
	
	TEvtEventId evtId = aEvent->EventId();
	TBool repeat = aEvent->Repeat();
	
	// Delete the previous running instance of the server
	delete iEvtNotifierHandler;
	iEvtNotifierHandler = NULL;
	
	switch( aResult )
		{
		case EOpenTrigger:
		case EShowDetails:
			{
			EVTUIDEBUG( "Open Event" );
			
	        TBuf8< KEvtCommandMessageLength > buffer;
	        buffer.AppendNum( evtId );
	        buffer.Append( KDelimiter );
	        buffer.AppendNum( aResult );
	        buffer.Append( KDelimiter );
	        
	        // Create a session to Windows server to check the running of 
	        // handler ui
		    RWsSession  ws;
		    User::LeaveIfError( ws.Connect() );
		    CleanupClosePushL( ws ); // pushing to cleanupstack
		    
		    // Check if Handler Application is already running. Incase its
		    // send the command message to the application
		    TApaTaskList taskList( ws );
		    TApaTask task = taskList.FindApp( TUid::Uid( KHandlerAppUid ) );
		    if ( task.Exists() )
		        {
		        // Bring the task to the foreground and send a message to it.		        	       
		        task.BringToForeground();
		        User::LeaveIfError( task.SendMessage( TUid::Uid( KUidApaMessageSwitchOpenFileValue ), buffer ));		        
		        }
		    else
		    	{

		    	// Start and instance of the new application
		    	LaunchAppL( TUid::Uid( KHandlerAppUid), buffer );			    
	            }
	        CleanupStack::PopAndDestroy();	// ws	
	        
	        // Remove if it was present in Snooze Que
			TTime time;
			time.HomeTime();
			TEvtSnoozeItem snoozeItem( iEventsQueue[0].iEventId, iEventsQueue[0].iAccuracy, time );
            iEvtSnoozeHandler->RemoveIfPresent( snoozeItem );		    		
	        break;
			}		
		case ESnoozeTrigger:
			{
			EVTUIDEBUG( "Snooze Event" );		
			TTime time;
			time.HomeTime();
			TEvtSnoozeItem snoozeItem( iEventsQueue[0].iEventId, iEventsQueue[0].iAccuracy, time );
			iEvtSnoozeHandler->Snooze( snoozeItem );
            break;			
			}
		case ECloseInfoNote:
			{
			EVTUIDEBUG( "Closing Note" );	
            
            // Update State if needed.
            if( !repeat )
                ChangeStateL( evtId );
            
	        // Remove if it was present in Snooze Que
			TTime time;
			time.HomeTime();
			TEvtSnoozeItem snoozeItem( iEventsQueue[0].iEventId, iEventsQueue[0].iAccuracy, time );
            iEvtSnoozeHandler->RemoveIfPresent( snoozeItem );		
            break;			
			}
		default:
		    {
	        // Remove if it was present in Snooze Que
			TTime time;
			time.HomeTime();
			TEvtSnoozeItem snoozeItem( iEventsQueue[0].iEventId, iEventsQueue[0].iAccuracy, time );
            iEvtSnoozeHandler->RemoveIfPresent( snoozeItem );	
		    break;    
		    }					
		}
			
    // The note has been handled. Remove the note from the queue		
	iEventsQueue.Remove( 0 );
	
	HandleNextEventL();	
    EVTUIDEBUG( "- CEvtHandlerServer::HandlerCompleteL()" );
	}

// ---------------------------------------------------------------------------
// CEvtHandlerServer::ChangeStateL() 
// ---------------------------------------------------------------------------
//
void CEvtHandlerServer::ChangeStateL( TEvtEventId aEvtId )
    {  
    EVTUIDEBUG1( "Change State of %d", aEvtId );
    TBuf16< KEvtCommandMessageLength > buffer;
    buffer.AppendNum( EEvtActionChangeState );
    buffer.Append( KDelimiter );
    buffer.AppendNum( aEvtId );

    RProcess proc;
    TInt retVal = proc.Create( KHandlerEXE(), buffer);
    if(retVal == KErrNone)
        {
        proc.Resume();
        }
    proc.Close();
    }

// ---------------------------------------------------------------------------
// CEvtHandlerServer::LaunchAppL() 
// ---------------------------------------------------------------------------
//
void CEvtHandlerServer::LaunchAppL(
                                TUid                    aApplicationUid,
                          const TDesC8&                  aCmdLineArg )
    {  
    DEBUG("+ CEvtHandlerServer::LaunchAppL");
    
    // Create the Session handle and connect to the Application architecture
    // server. This is required to obtain the details of the application from
    // the Application UID.
    RApaLsSession lsSession;
    User::LeaveIfError( lsSession.Connect() );
    CleanupClosePushL( lsSession );
		            
    DEBUG("+ CEvtHandlerServer::LaunchAppL - Get the Application Information");
    // Get the Application Information.
    TApaAppInfo appInfo;    
    TInt error = lsSession.GetAppInfo( appInfo, aApplicationUid );
    if ( error )
    	{
    	EVTUIDEBUG1("Get the Application Information Error : %d", error );
    	User::Leave( error );
    	}
    
    DEBUG("+ CEvtHandlerServer::LaunchAppL - Fill the command line argument structure");
    // Fill the command line argument structure
    CApaCommandLine* cmdLine = CApaCommandLine::NewLC();
    
    DEBUG("+ CEvtHandlerServer::LaunchAppL - Set the Application name");
    // Set the Application name.    
    TFileName appName = appInfo.iFullName;
    cmdLine->SetExecutableNameL( appName );
    
    DEBUG("+ CEvtHandlerServer::LaunchAppL - Set the Command");
    // Set the Command
    cmdLine->SetCommandL( EApaCommandRun );

    DEBUG("+ CEvtHandlerServer::LaunchAppL - Set the Command line arguments");
    // Set the Command line arguments.
    if ( aCmdLineArg.Length())
        {
        // These arguments would be sent as Tail end arguments to the launched
        // Application. Its left to the application to handle those
        HBufC8* tailEnd = HBufC8::NewLC( aCmdLineArg.Length());
        tailEnd->Des().Copy( aCmdLineArg );
        
        // The Tail end buffer is copied to the Command line structure. Hence,
        // can delete this variable after the set operation succeeds.
        cmdLine->SetTailEndL( tailEnd->Des());
        
        CleanupStack::PopAndDestroy( tailEnd );
        }

    DEBUG("+ CEvtHandlerServer::LaunchAppL - Start the Application");
	// Start the Application
	User::LeaveIfError( lsSession.StartApp( *cmdLine ));
    
    // Clean up all the allocated heap variables.
    CleanupStack::PopAndDestroy( cmdLine );
    CleanupStack::PopAndDestroy(); //lsSession 		         
    
    DEBUG("- CEvtHandlerServer::LaunchAppL");
    }
// End of File
