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
* Description:  Server Main for Events Handler Server
*
*/

// SYSTEM INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <coemain.h>
#include <eikenv.h>
#include <eikproc.h>

// USER INCLUDES
#include "evthandlerserver.h"
#include "evthandlerclientserver.h"
#include "evthandlerserverstartup.h"
#include "evtserverappui.h"
#include "evtserverapp.h"
#include "evtserverappdocument.h"
#include "evtdebug.h"

// ---------------------------------------------------------------------------
// void EvtHandlerServerStartup::StartEvtHandlerServerL()
// ---------------------------------------------------------------------------
//
void EvtHandlerServerStartup::StartEvtHandlerServerL()
/*
 * Perform all server initialisation, in particular creation of the
 * scheduler and server and then run the scheduler
 */
	{
    EVTUIDEBUG( "+ EvtHandlerServerStartup::StartEvtHandlerServerL()" );

	// naming the server thread after the server helps to debug panics
	User::LeaveIfError( RThread::RenameMe( KEvtHandlerServerName ) );
	//
	// create the server (leave it on the cleanup stack)
	CEvtHandlerServer::NewLC();
	//
	// Initialisation complete, now signal the client
	RProcess::Rendezvous( KErrNone );
	//
	// Ready to run - Start the Scheduler
	CEikonEnv::Static()->Execute();
	//
	// Cleanup the server
	CleanupStack::PopAndDestroy( );

    EVTUIDEBUG( "- EvtHandlerServerStartup::StartEvtHandlerServerL()" );
	}

// ---------------------------------------------------------------------------
// TInt EvtHandlerServerStartup::StartEvtHandlerServer()
// ---------------------------------------------------------------------------
//
TInt EvtHandlerServerStartup::StartEvtHandlerServer()
    {
    EVTUIDEBUG( "+ EvtHandlerServerStartup::StartEvtHandlerServer()" );
    __UHEAP_MARK;
    TInt ret = KErrNoMemory;
	// We need the Control Environment for handling Key lock while launching the Global note
	// We need AppUi to Hide the Server from Fast Swap Window(FSW).
    CEikonEnv* env = new CEikonEnv();
    if ( env )
        {
		TRAP(ret, env->ConstructL());
	    if( !ret && env->AppUi() == NULL )
	    {
        CApaApplication* app = new CEvtServerApp;
        if ( app )
            {
            env->WsSession().Flush();
            CEikDocument* doc = NULL;
            TRAP_IGNORE( doc = static_cast<CEikDocument*>(app->CreateDocumentL( env->Process() )) );
            if( doc )
                {
                CEikAppUi* appUi = NULL;
                TRAP_IGNORE( appUi = static_cast<CEikAppUi*>( doc->CreateAppUiL() ) );
                if(appUi)
                    {
                    appUi->SetDocument( doc );
                    TRAPD( err, appUi->ConstructL());
                    if(!err)
                        {
                        env->SetAppUi( appUi );
                        TRAP( ret, StartEvtHandlerServerL() );
                        }
                    appUi->SetDocument( NULL );
                    }
                delete doc;
                }
            delete app;
            }
	    }
        env->PrepareToExit();
        env->DestroyEnvironment();
        }
    __UHEAP_MARKEND;
    EVTUIDEBUG1( "- EvtHandlerServerStartup::StartEvtHandlerServer()", ret );
    return ret;    
    }

// -----------------------------------------------------------------------------
// TInt E32Main
// Server process entry-point
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    EVTUIDEBUG( "+ E32Main() start" );    
    return EvtHandlerServerStartup::StartEvtHandlerServer();        
    }    
                                                               
// End of File
