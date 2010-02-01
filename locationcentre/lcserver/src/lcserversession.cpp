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
* Description:  Location Centre Server side session object.
*
*/


// SYSTEM INCLUDES
#include <s32mem.h>

// USER INCLUDES
#include "lcserversession.h"
#include "lcserver.h"
#include "lcipcparams.h"
#include "lcserverinterface.h"
#include "lcserverengine.h"
#include "lcdebug.h"

// ----- Member funtions for CLcServerSession ---------------------------------

// ---------------------------------------------------------------------------
// CLcServerSession::CLcServerSession
// ---------------------------------------------------------------------------
//
CLcServerSession::CLcServerSession( CLcServer&        aLcServer,
                                    CLcServerEngine&  aLcServerEngine )
    :iLcServer( aLcServer ),
    iEngine( aLcServerEngine )
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.
    }
         
// ---------------------------------------------------------------------------
// CLcServerSession::~CLcServerSession
// ---------------------------------------------------------------------------
//
CLcServerSession::~CLcServerSession()
    {
    // C++ Destructor. Free all resources associated with this class.
    
    // Complete the Observer method with KErrServerTerminated if it exists
    if ( iNotificationOutStanding )
        {
        iObserverMessage.Complete( KErrServerTerminated );
        }
        
    // Decrement the count only if it has been incremented in the Construction
    if( iDecrementSession )
        {
        iLcServer.DecrementSessions();
        }
    }
        
// ---------------------------------------------------------------------------
// CLcServerSession* CLcServerSession::NewL
// ---------------------------------------------------------------------------
//
CLcServerSession* CLcServerSession::NewL( CLcServer&        aLcServer,
                                          CLcServerEngine&  aLcServerEngine )
    {
    CLcServerSession* self = NewLC( aLcServer, aLcServerEngine );
    CleanupStack::Pop( self );
    return self;         
    }

// ---------------------------------------------------------------------------
// CLcServerSession* CLcServerSession::NewLC
// ---------------------------------------------------------------------------
//
CLcServerSession* CLcServerSession::NewLC( CLcServer&        aLcServer,
                                           CLcServerEngine&  aLcServerEngine )
    {
    // Symbian Two phased constructor. Leaves the object on the Clean-up
    // stack.
    CLcServerSession* self = 
                        new ( ELeave )CLcServerSession( aLcServer,
                                                        aLcServerEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;         
    }

// ---------------------------------------------------------------------------
// void CLcService::ConstructL
// ---------------------------------------------------------------------------
//
void CLcServerSession::ConstructL()
    {
    
    // Increment the server's increment session routine.
    iLcServer.IncrementSessions();
  
    // Set the flag to mark that the number of sessions have been incremented.
    // This flag is set at the end of the contruction process to ensure that
    // we dont decrement the count in the destructor unless we have actually
    // incremented it. This is necessary because there is a possibility that
    // the ConstructL leaves before incrementing the count.
    iDecrementSession = ETrue;    
    }  

// ---------------------------------------------------------------------------
// void CLcServerSession::RegistryUpdated
// ---------------------------------------------------------------------------
//
void CLcServerSession::RegistryUpdated()
    {
    DEBUG( "+ CLcServerSession::RegistryUpdated")
    
    // If the Observer has been set, then complete the Observer
    // request with KErrNone.
    if ( iNotificationOutStanding )
        {
        DEBUG3("Notify Client", 0, 0, 0 );
        
        iObserverMessage.Complete( KErrNone );
        }
    else
        {
        DEBUG3("Notify Lost", 0, 0, 0 );
        iNotifyPending = ETrue;
        }        
    iNotificationOutStanding = EFalse;
    
    DEBUG( "- CLcServerSession::RegistryUpdated")
    }
    
// ---------------------------------------------------------------------------
// void CLcServerSession::ServiceL
// ---------------------------------------------------------------------------
//
void CLcServerSession::ServiceL( const RMessage2& aMessage )
    {
    DEBUG( "+ CLcServerSession::ServiceL")
    
    switch( aMessage.Function())
        {
        case ERegisterObserver:
            {
            DEBUG( "Message : ERegisterObserver")
            
            // If an Observer message exists then this is a Duplicate request
            // which is not accepted.
            if ( iNotificationOutStanding )
                {
                DEBUG3("Notify Already there", 0, 0, 0 );
                aMessage.Complete( KErrAlreadyExists );
                }
            else if ( iNotifyPending )
                {
                DEBUG3("Notify Immediately", 0, 0, 0 );
                aMessage.Complete( KErrNone );
                iNotifyPending = EFalse;
                }
            else
                {
                DEBUG3("Notify Later", 0, 0, 0 );
                // The Client application has requested for an Observer to
                // notify about any changes to the Location Centre Registry.
                // Store the message object.
                iObserverMessage = aMessage;
                iNotificationOutStanding = ETrue;                
                }            
            break;
            }
        case ERemoveObserver:
            {
            DEBUG( "Message : ERemoveObserver")
                                	
            // If the Observer has been set, then complete the Observer
            // request with KErrCancel.
            if ( iNotificationOutStanding )
                {
                iObserverMessage.Complete( KErrCancel );
                }
            iNotificationOutStanding = EFalse;

            // Complete the current request with KErrNone
            aMessage.Complete( KErrNone );
            
            break;
            }
        case ELcFilteredAppsBufferLength:
        case ELcFilteredApps:
        case ELcSpecifiedAppsBufferLength:
        case ELcSpecifiedApps:
        case ELcAppInfoLength:
        case ELcAppInfo:
            {
            TInt desLength = aMessage.GetDesLength( 0 );
            if ( desLength < KErrNone )
                {
                User::Leave( desLength );
                }
             
            if ( desLength >= KMaxTInt/2 )
                {
                User::Leave( KErrArgument );
                }
                
            // These messages are not handled here but only in the Engine
            // class. Hence, pass it onto the Engine class for handling.
            iEngine.DoServiceL( aMessage );   
            break;
            }
        default:
            {
            aMessage.Complete( KErrNotSupported );
            }
        }
        
    DEBUG( "- CLcServerSession::ServiceL")
    }

// ---------------------------------------------------------------------------
// void CLcServerSession::ServiceError
// ---------------------------------------------------------------------------
//
void CLcServerSession::ServiceError( const RMessage2& aMessage, 
                                           TInt       aError )
    {
    DEBUG( "+ CLcServerSession::ServiceError")
    
    aMessage.Complete( aError );
    
    DEBUG( "- CLcServerSession::ServiceError")
    }
                                                                                             
// End of File
