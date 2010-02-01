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
* Description:  Location Centre Notification Active Object.
*
*/


// SYSTEM INCLUDES

// USER INCLUDES
#include "lcnotificationao.h"
#include "lcnotification.h"
#include "lcclientsession.h"

// CONSTANT DEFINTIONS

// ----- Member funtions for CLcNotificationAO ---------------------------------

// ---------------------------------------------------------------------------
// CLcNotificationAO::CLcNotificationAO
// ---------------------------------------------------------------------------
//
CLcNotificationAO::CLcNotificationAO( MLcNotification&    aObserver,
                                      RLcClientSession&   aClientSession )
    :CActive( EPriorityStandard ),
    iObserver( aObserver ),
    iClientSession( aClientSession )
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.
    CActiveScheduler::Add( this );
    }
         
// ---------------------------------------------------------------------------
// CLcNotificationAO::~CLcNotificationAO
// ---------------------------------------------------------------------------
//
CLcNotificationAO::~CLcNotificationAO()
    {
    // C++ Destructor. Free all resources associated with this class.
    Cancel();
    }
        
// ---------------------------------------------------------------------------
// CLcNotificationAO* CLcNotificationAO::NewL
// ---------------------------------------------------------------------------
//
CLcNotificationAO* CLcNotificationAO::NewL( MLcNotification&    aObserver,
                                            RLcClientSession&   aClientSession )
    {
    CLcNotificationAO* self = new ( ELeave )CLcNotificationAO( aObserver,
                                                               aClientSession );
    return self;           
    }

// ---------------------------------------------------------------------------
// void CLcNotificationAO::IssueRequest
// ---------------------------------------------------------------------------
//
void CLcNotificationAO::IssueRequest()
    {
    // The request is issued only if there is no outstanding request. If there
    // any request outstanding then don't do anything. Server the previous
    // request.
    if ( !IsActive())
        {
        // Location Centre Server is the ASP. Send a notification request to
        // the same.
        iClientSession.SendReceive( ERegisterObserver, iStatus );
        SetActive();
        }
    }

// ---------------------------------------------------------------------------
// void CLcNotificationAO::CancelRequest
// ---------------------------------------------------------------------------
//
void CLcNotificationAO::CancelRequest()
    {
    Cancel();
    }
        
// ---------------------------------------------------------------------------
// void CLcNotificationAO::RunL
// ---------------------------------------------------------------------------
//
void CLcNotificationAO::RunL()
    {
    // Communicate the Success/Failure of the Notification request to the
    // Location Centre Client.
    iObserver.LcStatusChangedL( iStatus.Int());

    // Issue the Observer request unless the server has been terminated.
    if ( iStatus.Int() != KErrServerTerminated )
        {
        IssueRequest();
        }
    }

// ---------------------------------------------------------------------------
// void CLcNotificationAO::DeCancel
// ---------------------------------------------------------------------------
//
void CLcNotificationAO::DoCancel()
    {
    iClientSession.SendReceive( ERemoveObserver );
    }                     
            
// End of File
