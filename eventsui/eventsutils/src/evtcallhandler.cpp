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
* Description:  Incoming Call Handler.
*
*/


// System Includes
#include <ctsydomainpskeys.h>
#include <e32property.h>

// User Includes
#include "evtcallhandler.h"
#include "evtdebug.h"

// ================ Member funtions for CEvtCallHandler class ===============

// ---------------------------------------------------------------------------
// CEvtCallHandler::CEvtCallHandler
// ---------------------------------------------------------------------------
//
CEvtCallHandler::CEvtCallHandler( MEvtCallObserver& aObserver ): 
									CActive(EPriorityStandard),
									iObserver(aObserver),
    								iCallStatusPkg(iCallStatus)
    {
    }

// ---------------------------------------------------------------------------
// CEvtCallHandler::~CEvtCallHandler
// ---------------------------------------------------------------------------
//
CEvtCallHandler::~CEvtCallHandler()
	{
    Cancel();
 
    if (iTelephony)
        delete iTelephony;
	}
	
// ---------------------------------------------------------------------------
// CEvtCallHandler::NewL
// ---------------------------------------------------------------------------
//
CEvtCallHandler* CEvtCallHandler::NewL( MEvtCallObserver& aObserver )
    {
	CEvtCallHandler* self = NewLC( aObserver );
	CleanupStack::Pop( self );
	return self;
    }

// ---------------------------------------------------------------------------
// CEvtCallHandler::NewLC
// ---------------------------------------------------------------------------
//
CEvtCallHandler* CEvtCallHandler::NewLC( MEvtCallObserver& aObserver )
    {
	CEvtCallHandler* self = new ( ELeave )CEvtCallHandler( aObserver );
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
    } 

// ---------------------------------------------------------------------------
// CEvtCallHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CEvtCallHandler::ConstructL()
    {
    EVTUIDEBUG( "+ CEvtCallHandler::ConstructL()" );
    
    iTelephony = CTelephony::NewL();
 
    CActiveScheduler::Add(this);
	
	StartListening();
	
    EVTUIDEBUG( "- CEvtCallHandler::ConstructL()" );
    }
    
// ---------------------------------------------------------------------------
// CEvtCallHandler::RunL
// ---------------------------------------------------------------------------
//
void CEvtCallHandler::RunL()
    {
    if (iStatus == KErrNone)
        {
        CTelephony::TCallStatus status = iCallStatus.iStatus;
 
        switch (status)
            {
            case CTelephony::EStatusRinging:
                {
                // The phone is ringing.
                iObserver.HandleIncomingCallL();
                break;
                }
            default:
                {
                // Not interested in other events.
                break;
                }
            }
        // Start listening for the next call
        StartListening();
        }
    }
    
// ---------------------------------------------------------------------------
// CEvtCallHandler::RunError
// ---------------------------------------------------------------------------
//
TInt CEvtCallHandler::RunError(TInt anError)
    {
    return anError;
    }
    
// ---------------------------------------------------------------------------
// CEvtCallHandler::DoCancel
// ---------------------------------------------------------------------------
//
void CEvtCallHandler::DoCancel()
    {
    iTelephony->CancelAsync(CTelephony::EVoiceLineStatusChangeCancel);
    }
    
// ---------------------------------------------------------------------------
// CEvtCallHandler::StartListening
// ---------------------------------------------------------------------------
//
void CEvtCallHandler::StartListening()
    {
    iTelephony->NotifyChange(iStatus, CTelephony::EVoiceLineStatusChange,
            iCallStatusPkg);
    SetActive();
    }

// ---------------------------------------------------------------------------
// CEvtCallHandler::IsCallActive
// ---------------------------------------------------------------------------
//
TBool CEvtCallHandler::IsCallActive( )
	{
    TInt callActiveState = EPSCTsyCallTypeNone;

    // Get current call status from P&S
    const TInt errCode( RProperty::Get(
        KPSUidCtsyCallInformation,
        KCTsyCallType,
        callActiveState ) );

    // Check current call type
    TBool result(
        ( EPSCTsyCallTypeCSVoice == callActiveState ) &&
        ( KErrNone == errCode ) );
        
    EVTUIDEBUG1( "= CEvtCallHandler::IsCallActive() - %d", result );

    return result;
    }

// ---------------------------------------------------------------------------
// CEvtCallHandler::IsCallIncoming
// ---------------------------------------------------------------------------
//
TBool CEvtCallHandler::IsCallIncoming( )
	{
    TInt callActiveState = EPSCTsyCallStateNone;

    // Get current call status from P&S
    const TInt errCode( RProperty::Get(
        KPSUidCtsyCallInformation,
        KCTsyCallState,
        callActiveState ) );

    const TBool callIncoming(
        ( EPSCTsyCallStateAlerting == callActiveState ) ||
        ( EPSCTsyCallStateRinging == callActiveState ) );

    // Check current call status
    TBool result( callIncoming && ( KErrNone == errCode ) );
        
    EVTUIDEBUG1( "= CEvtCallHandler::IsCallIncoming() - %d", result );
    
    return result;
    }
    
