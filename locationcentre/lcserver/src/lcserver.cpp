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
* Description:  Location Centre Server object.
*
*/


// SYSTEM INCLUDES

// USER INCLUDES
#include "lcserver.h"
#include "lcserverinterface.h"
#include "lcserversession.h"
#include "lcservershutdowntimer.h"
#include "lcserverengine.h"

// CONSTANT DEFINTIONS

// ----- Member funtions for CLcServer ---------------------------------

// ---------------------------------------------------------------------------
// CLcServer::CLcServer
// ---------------------------------------------------------------------------
//
CLcServer::CLcServer()
    :CServer2( CActive::EPriorityStandard )
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.
    }
         
// ---------------------------------------------------------------------------
// CLcServer::~CLcServer
// ---------------------------------------------------------------------------
//
CLcServer::~CLcServer()
    {
    // C++ Destructor. Free all resources associated with this class.
    
    // Delete the Server Engine.
    delete iEngine;
    
    // Delete the shutdown timer.
    delete iShutDownTimer;
    
    }
        
// ---------------------------------------------------------------------------
// CLcServer* CLcServer::NewL
// ---------------------------------------------------------------------------
//
CLcServer* CLcServer::NewL()
    {
    CLcServer* self = NewLC();
    CleanupStack::Pop( self );
    return self;         
    }

// ---------------------------------------------------------------------------
// CLcServer* CLcServer::NewLC
// ---------------------------------------------------------------------------
//
CLcServer* CLcServer::NewLC()
    {
    // Symbian Two phased constructor. Leaves the object on the Clean-up
    // stack.
    CLcServer* self = new ( ELeave )CLcServer();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;         
    }

// ---------------------------------------------------------------------------
// void CLcService::ConstructL
// ---------------------------------------------------------------------------
//
void CLcServer::ConstructL()
    {
    // Start the Location Centre Server
    StartL( KLocationCentreServerName ); 
    
    // Create the shutdown timer
    iShutDownTimer = CLcServerShutDownTimer::NewL();
    
    // Create the Location Centre Server Engine.
    iEngine = CLcServerEngine::NewL( *this ); 
    }  

// ---------------------------------------------------------------------------
// CSession2* CLcServer::NewSessionL
// ---------------------------------------------------------------------------
//
CSession2* CLcServer::NewSessionL( const TVersion&     aVersion,
                                   const RMessage2&    /* aMessage */ ) const
    {
    // Check the version number. We dont support any other version numbers
    // greater than ( 1, 0, 0 ). Hence, if the version number passed to this
    // function is greater than the supported number leave with KErrNotSupported.
    TVersion version( KLcServerMajorVersionNumber,
                      KLcServerMinorVersionNumber,
                      KLcServerBuildVersionNumber );
    if ( !User::QueryVersionSupported( version, aVersion ))
        {
        User::Leave( KErrNotSupported );
        }

    // Make new session
    CSession2* newSession = 
        CLcServerSession::NewL( *( const_cast< CLcServer* >( this )),
                                *iEngine );

    return newSession;    
    }

// ---------------------------------------------------------------------------
// void CLcServer::IncrementSessions
// ---------------------------------------------------------------------------
//
void CLcServer::IncrementSessions()
    {
    // Increment the number of sessions
    iNumberofSessions++;
    
    // Stop the Shutdown timer. This function does nothing if the timer 
    // is not already running.
    iShutDownTimer->StopTimer();
    }

// ---------------------------------------------------------------------------
// void CLcServer::DecrementSessions
// ---------------------------------------------------------------------------
//
void CLcServer::DecrementSessions()
    {
    
    // If the number of sessions is equal to zero then start the shutdown
    // timer.
    if ( --iNumberofSessions == 0 )
        {
        iShutDownTimer->StartTimer();
        }     
    }

// ---------------------------------------------------------------------------
// void CLcServer::LcRegistryUpdated
// ---------------------------------------------------------------------------
//
void CLcServer::LcRegistryUpdated()
    {
    // Inform all the Sessions contained here that there is an update to the
    // registry so that they can inform any clients which have registered
    // for notification
    iSessionIter.SetToFirst();
    
    while ( iSessionIter )
        {
        // Obtain the element at the current position and notify the corresponding
        // session object.
        CLcServerSession& session = static_cast <CLcServerSession&>( *iSessionIter );
        session.RegistryUpdated();
        
        // Increment the Session object to point to the next session instance
        iSessionIter++;
        }
    }
// End of File
