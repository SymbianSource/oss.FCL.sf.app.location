/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Location Centre Server start up.
*
*/


// SYSTEM INCLUDES

// USER INCLUDES
#include "lcserverstartup.h"
#include "lcserverinterface.h"
#include "lcserver.h"

// ----- Member funtions for LcServerStartup ---------------------------------

// ---------------------------------------------------------------------------
// TInt LcServerStartup::StartLocationCentreServer
// ---------------------------------------------------------------------------
//
TInt LcServerStartup::StartLocationCentreServer()
    {
    __UHEAP_MARK;

    CTrapCleanup* cleanupStack = CTrapCleanup::New();

    TInt ret = KErrNoMemory;
    if ( cleanupStack )
        {
        TRAP(ret, StartLocationCentreServerL());
        delete cleanupStack;
        }

    __UHEAP_MARKEND;

    return ret;
    }

// ---------------------------------------------------------------------------
// void LcServerStartup::StartLocationCentreServer
// ---------------------------------------------------------------------------
//
void LcServerStartup::StartLocationCentreServerL()
    {
    // Rename thread to aid debugging
    User::LeaveIfError( User::RenameThread( KLocationCentreServerName ));

    // Start Active scheduler and Server
    CActiveScheduler* scheduler = new( ELeave ) CActiveScheduler;
    CleanupStack::PushL( scheduler );
    CActiveScheduler::Install( scheduler );

    CLcServer* server = CLcServer::NewL();
    CleanupStack::PushL( server );

    CleanupStack::Pop(2, scheduler); // server

    RProcess::Rendezvous( KErrNone );

    CActiveScheduler::Start();

    delete server;
    delete scheduler;    
    }
    
// -----------------------------------------------------------------------------
// TInt E32Main
// Server process entry-point
// -----------------------------------------------------------------------------
//
TInt E32Main()
    {
    return LcServerStartup::StartLocationCentreServer();
    }    
                                                               
// End of File

