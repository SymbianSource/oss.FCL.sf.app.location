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
* Description:  Client side session to the Location Centre server.
*
*/


// SYSTEM INCLUDES

// USER INCLUDES
#include "lcclientsession.h"

// CONSTANT DEFINTIONS
const TInt KNumofConnectAttempts = 200;

// ----- Member funtions for RLcClientSession ---------------------------------

// ---------------------------------------------------------------------------
// RLcClientSession::RLcClientSession
// ---------------------------------------------------------------------------
//
RLcClientSession::RLcClientSession()
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.
    }
         
// ---------------------------------------------------------------------------
// RLcClientSession::~RLcClientSession
// ---------------------------------------------------------------------------
//
RLcClientSession::~RLcClientSession()
    {
    // C++ Destructor. Free all resources associated with this class.
    }

// ---------------------------------------------------------------------------
// RLcClientSession::~RLcClientSession
// ---------------------------------------------------------------------------
//
TInt RLcClientSession::CreateSession()
    {
    TInt error = KErrNone;
    
    // Attempt KNumofConnectAttempts to make a connection to the server
    for ( TInt i = 0; i <= KNumofConnectAttempts; i++ )
    	{
    	error = RSessionBase::CreateSession( 
    	                            KLocationCentreServerName, 
    	                            TVersion( KLcServerMajorVersionNumber,
                                              KLcServerMinorVersionNumber,
                                              KLcServerBuildVersionNumber ),
                                    8);
    	if ( error != KErrNotFound && error != KErrServerTerminated )
    		{
    		// If the Create session returns an error code other than
    		// KErrNotFound or KErrServerTerminated, then the request
    		// is terminated immediately with the error code.
    		// If it is any of these error codes then the server is
    		// not running. In this case, try to start the server
    		return error;
    		}

    	error = StartServer();
    	if ( error != KErrAlreadyExists && error != KErrNone)
    		{
            // Server returned error code other than KErrAlreadyExists
            // or KErrNone. The request is terminated with this
            // error code.
            // If the error code is one of these then the server is
            // up and running. Now try to establish the connection.
    		return error;
    		}
    	}
    return error; 
    }

// ---------------------------------------------------------------------------
// RLcClientSession::~RLcClientSession
// ---------------------------------------------------------------------------
//    
void RLcClientSession::Close()
    {
    RHandleBase::Close();
    }

// ---------------------------------------------------------------------------
// RLcClientSession::SendReceive
// ---------------------------------------------------------------------------
//    
void RLcClientSession::SendReceive(       TLcServerMsgType    aMsgType, 
                                    const TIpcArgs&           aArgs,
                                          TRequestStatus&     aStatus ) const
    {
    return RSessionBase::SendReceive( aMsgType,
                                      aArgs,
                                      aStatus );    
    }
    
// ---------------------------------------------------------------------------
// RLcClientSession::SendReceive
// ---------------------------------------------------------------------------
//    
TInt RLcClientSession::SendReceive(       TLcServerMsgType    aMsgType, 
                                    const TIpcArgs&           aArgs ) const
    {
    return RSessionBase::SendReceive( aMsgType,
                                      aArgs );
    }

// ---------------------------------------------------------------------------
// RLcClientSession::SendReceive
// ---------------------------------------------------------------------------
//    
void RLcClientSession::SendReceive( TLcServerMsgType    aMsgType,
                                    TRequestStatus&     aStatus ) const
    {
    return RSessionBase::SendReceive( aMsgType, aStatus );    
    }
    
// ---------------------------------------------------------------------------
// RLcClientSession::SendReceive
// ---------------------------------------------------------------------------
//    
TInt RLcClientSession::SendReceive( TLcServerMsgType    aMsgType ) const
    {
    return RSessionBase::SendReceive( aMsgType );
    }
    
// ---------------------------------------------------------------------------
// RLcClientSession::StartServer
// ---------------------------------------------------------------------------
//    
TInt RLcClientSession::StartServer()
    {
    // Create a new server process.
    // If the server is already running then the function fails with
    // KErrAlreadyExists
    RProcess server;
    TInt error = server.Create( KLocationCentreServerExe,
                                KNullDesC );
    if ( !error )
        {
        TRequestStatus died;
        server.Rendezvous( died );
        if ( died != KRequestPending )
            {
            // Logon failed - Server is not yet running, so cannot have terminated
            User::WaitForRequest( died );           // Eat signal
            server.Kill( 0 );                       // Abort startup
            }
        else
            {
            server.Resume();
            User::WaitForRequest( died );           // Wait for start or death
            }

        // We can't use the 'exit reason' if the server panicked as this
        // is the panic 'reason' and may be '0' which cannot be distinguished
        // from KErrNone
        error = ( server.ExitType() == EExitPanic ) ? KErrGeneral : died.Int();
        server.Close();
        }
    return error;      
    }                                                                                                              
// End of File
