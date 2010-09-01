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
* Description:  Key Lock Settings Handler.
*
*/


// System Includes

// User Includes
#include "evtkeylockhandler.h"
#include "evtdebug.h"

// ================ Member funtions for CEvtKeyLockHandler class ===============

// ---------------------------------------------------------------------------
// CEvtKeyLockHandler::CEvtKeyLockHandler
// ---------------------------------------------------------------------------
//
CEvtKeyLockHandler::CEvtKeyLockHandler( ) : iKeyLocked(EFalse)
	{
	}

// ---------------------------------------------------------------------------
// CEvtKeyLockHandler::~CEvtKeyLockHandler
// ---------------------------------------------------------------------------
//
CEvtKeyLockHandler::~CEvtKeyLockHandler()
	{
	iKeyLock.Close();
	}
	
// ---------------------------------------------------------------------------
// CEvtKeyLockHandler::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtKeyLockHandler* CEvtKeyLockHandler::NewL( )
    {
	CEvtKeyLockHandler* self = NewLC( );
	CleanupStack::Pop( self );
	return self;
    }

// ---------------------------------------------------------------------------
// CEvtKeyLockHandler::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtKeyLockHandler* CEvtKeyLockHandler::NewLC( )
    {
	CEvtKeyLockHandler* self = new ( ELeave )CEvtKeyLockHandler( );
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
    } 

// ---------------------------------------------------------------------------
// CEvtKeyLockHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CEvtKeyLockHandler::ConstructL()
    {
    EVTUIDEBUG( "+ CEvtKeyLockHandler::ConstructL()" );
    
	User::LeaveIfError( iKeyLock.Connect() );
	
    EVTUIDEBUG( "- CEvtKeyLockHandler::ConstructL()" );
    }

// ---------------------------------------------------------------------------
// CEvtKeyLockHandler::MaintainState
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtKeyLockHandler::MaintainState( )
	{
	iKeyLocked = iKeyLock.IsKeyLockEnabled();
	
	// Disable the Key Lock without showing the Note.
    if( iKeyLocked )
		{
    	iKeyLock.DisableWithoutNote();
		}
    EVTUIDEBUG1( "= CEvtKeyLockHandler::MaintainState() - %d", iKeyLocked );
    }

// ---------------------------------------------------------------------------
// CEvtKeyLockHandler::UpdateStateWithNote
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtKeyLockHandler::UpdateStateWithNote( )
	{
    EVTUIDEBUG1( "= CEvtKeyLockHandler::UpdateStateWithNote() - %d", iKeyLocked );
	
	// Check the previously Maintained key lock state and the current key lock state.
	// This avoids updation if both the states are same.
    if( iKeyLocked && !iKeyLock.IsKeyLockEnabled() )
		{
		iKeyLock.EnableKeyLock();
		}
    else if( !iKeyLocked && iKeyLock.IsKeyLockEnabled() )
    	{
		iKeyLock.DisableKeyLock();
		}
	iKeyLocked = EFalse;
    EVTUIDEBUG( "- CEvtKeyLockHandler::UpdateStateWithNote() - Key Lock Updated" );
    }

// ---------------------------------------------------------------------------
// CEvtKeyLockHandler::UpdateStateWithoutNote
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtKeyLockHandler::UpdateStateWithoutNote( )
	{
    EVTUIDEBUG1( "= CEvtKeyLockHandler::UpdateStateWithoutNote() - %d", iKeyLocked );
	
	// Check the previously Maintained key lock state and the current key lock state.
	// This avoids updation if both the states are same.
    if( iKeyLocked && !iKeyLock.IsKeyLockEnabled() )
		{
		iKeyLock.EnableWithoutNote();
		}
    else if( !iKeyLocked && iKeyLock.IsKeyLockEnabled() )
    	{
		iKeyLock.DisableWithoutNote();
		}
	iKeyLocked = EFalse;
    EVTUIDEBUG( "- CEvtKeyLockHandler::UpdateStateWithoutNote() - Key Lock Updated" );
    }

// ---------------------------------------------------------------------------
// CEvtKeyLockHandler::EnableKeyLock
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtKeyLockHandler::EnableKeyLock( TBool aFlag )
	{
    iKeyLocked = aFlag;
    EVTUIDEBUG1( "= CEvtKeyLockHandler::EnableKeyLock() - %d", iKeyLocked );
    }
