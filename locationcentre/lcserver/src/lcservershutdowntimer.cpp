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
* Description:  Shutdown timer for Location Centre server.
*
*/


// SYSTEM INCLUDES
#include <centralrepository.h>

// USER INCLUDES
#include "lcservershutdowntimer.h"
#include "lcprivatecrkeys.h"

// CONSTANT DEFINTIONS
TInt KMilliSectoMicroConversion = 1000;

// ----- Member funtions for CLcServerShutDownTimerShutDownTimer -------------

// ---------------------------------------------------------------------------
// CLcServerShutDownTimer::CLcServerShutDownTimer
// ---------------------------------------------------------------------------
//
CLcServerShutDownTimer::CLcServerShutDownTimer()
    :CTimer( CActive::EPriorityLow )
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.
    
    // The timer needs to be added to the Active scheduler explicitly. So 
    // adding it here
    CActiveScheduler::Add( this );
    }
         
// ---------------------------------------------------------------------------
// CLcServerShutDownTimer::~CLcServerShutDownTimer
// ---------------------------------------------------------------------------
//
CLcServerShutDownTimer::~CLcServerShutDownTimer()
    {
    // C++ Destructor. Free all resources associated with this class.
    Cancel();
    }
        
// ---------------------------------------------------------------------------
// CLcServerShutDownTimer* CLcServerShutDownTimer::NewL
// ---------------------------------------------------------------------------
//
CLcServerShutDownTimer* CLcServerShutDownTimer::NewL()
    {
    CLcServerShutDownTimer* self = NewLC();
    CleanupStack::Pop( self );
    return self;         
    }

// ---------------------------------------------------------------------------
// CLcServerShutDownTimer* CLcServerShutDownTimer::NewLC
// ---------------------------------------------------------------------------
//
CLcServerShutDownTimer* CLcServerShutDownTimer::NewLC()
    {
    // Symbian Two phased constructor. Leaves the object on the Clean-up
    // stack.
    CLcServerShutDownTimer* self = new ( ELeave )CLcServerShutDownTimer();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;         
    }

// ---------------------------------------------------------------------------
// void CLcServerShutDownTimer::ConstructL
// ---------------------------------------------------------------------------
//
void CLcServerShutDownTimer::ConstructL()
    {
    // Read the shutdown timer value from the Location Centre Central
    // repository
    CRepository* repository = CRepository::NewLC( KCRUidLocationCentre );    
    repository->Get( KLcShutdownTimerValue, iShutDownTimerValue );    
    CleanupStack::PopAndDestroy( repository );
    
    // Convert the Shutdown timer value into microseconds
    iShutDownTimerValue *= KMilliSectoMicroConversion;
    
    // Call the base class ConstructL
    CTimer::ConstructL();     
    }  

// ---------------------------------------------------------------------------
// void CLcServerShutDownTimer::ConstructL
// ---------------------------------------------------------------------------
//
void CLcServerShutDownTimer::StartTimer()
    {
    // Check if there is a request outstanding. Incase, a request is
    // outstanding then do nothing. If not start a new timer request
    
    if ( !IsActive())
        {
        CTimer::After( iShutDownTimerValue );
        }
    }

// ---------------------------------------------------------------------------
// void CLcServerShutDownTimer::ConstructL
// ---------------------------------------------------------------------------
//
void CLcServerShutDownTimer::StopTimer()
    {
    Cancel();
    }
    
// ---------------------------------------------------------------------------
// CLcServerShutDownTimer::RunL
// ---------------------------------------------------------------------------
//
void CLcServerShutDownTimer::RunL()
    {
    // Timer is not cancelled by the server. Hence, we can assume tha there
    // are no new connection requests to the server. Its safe to shutdown
    // the server
    CActiveScheduler::Stop();
    }

// ---------------------------------------------------------------------------
// CLcServerShutDownTimer::RunError
// ---------------------------------------------------------------------------
//        
TInt CLcServerShutDownTimer::RunError( TInt aError )
    {
    return aError;
    }
// End of File

