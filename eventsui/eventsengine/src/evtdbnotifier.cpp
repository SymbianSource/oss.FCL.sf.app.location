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
* Description:  Notifier class for any change in Storage API dtabase.
*
*/


// System Includes
#include <centralrepository.h>

// User Includes
#include "evtdbnotifier.h"
#include "evtdebug.h"

// ================ Member funtions for CEvtDatabase class ==================

// ---------------------------------------------------------------------------
// CEvtDbNotifier::CEvtDbNotifier
// ---------------------------------------------------------------------------
//
CEvtDbNotifier::CEvtDbNotifier( MEvtDbObserver&    aObserver )
						:CActive( EPriorityStandard ),
						iDbObserver( aObserver )
    {
    }
    
// ---------------------------------------------------------------------------
// CEvtDbNotifier::~CEvtDbNotifier
// ---------------------------------------------------------------------------
//
CEvtDbNotifier::~CEvtDbNotifier()
    {
	Cancel();
    delete iRepository; 
    } 
    
// ---------------------------------------------------------------------------
// CEvtDbNotifier::NewL
// ---------------------------------------------------------------------------
//
CEvtDbNotifier* CEvtDbNotifier::NewL( MEvtDbObserver&    aObserver )
    {
	CEvtDbNotifier* self = NewLC( aObserver );
	CleanupStack::Pop( self );
	return self;
    }

// ---------------------------------------------------------------------------
// CEvtDbNotifier::NewLC
// ---------------------------------------------------------------------------
//
CEvtDbNotifier* CEvtDbNotifier::NewLC( MEvtDbObserver&    aObserver )
    {
	CEvtDbNotifier* self = new ( ELeave )CEvtDbNotifier( aObserver );
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
    } 

// ---------------------------------------------------------------------------
// CEvtDbNotifier::ConstructL
// ---------------------------------------------------------------------------
//
void CEvtDbNotifier::ConstructL()
    {
	EVTUIDEBUG( "+ CEvtDbNotifier::ConstructL()" );
    CActiveScheduler::Add(this);
    
	EVTUIDEBUG( "Calling CRepository::NewL" );
    // Create the Central repository object notifying the Db change
    TRAPD(err, iRepository = CRepository::NewL( TUid::Uid( KCRUidEvtStorageDb ) ) );
	EVTUIDEBUG1( "Calling CRepository::NewL Cenrep Error Code- %d", err );
	User::LeaveIfError( err );
    // Request for notification
    StartNotification();
    
	EVTUIDEBUG( "- CEvtDbNotifier::ConstructL()" );
    } 

// ---------------------------------------------------------------------------
// void CEvtDbNotifier::RunL()
// ---------------------------------------------------------------------------
//                             
void CEvtDbNotifier::RunL()
    {
	EVTUIDEBUG( "+ CEvtDbNotifier::RunL()" );
    // resubscribe before processing new value to prevent missing updates
	StartNotification();
	
	// processing new value
	HandleDbChangedL();
	
	EVTUIDEBUG( "- CEvtDbNotifier::RunL()" );
    }
    
// ---------------------------------------------------------------------------
// void CEvtDbNotifier::DoCancel()
// ---------------------------------------------------------------------------
// 
void CEvtDbNotifier::DoCancel()
    {
	EVTUIDEBUG( "+ CEvtDbNotifier::DoCancel()" );
    // Cancel the outstanding CR notification request
    if( iRepository )
        {
        iRepository->NotifyCancel( KEvtDbChangeType );        
        }
	EVTUIDEBUG( "- CEvtDbNotifier::DoCancel()" );
    }

// ---------------------------------------------------------------------------
// TInt CEvtDbNotifier::RunError()
// ---------------------------------------------------------------------------
//    
TInt CEvtDbNotifier::RunError( TInt aError )
	{    	
	return CActive::RunError( aError );	
	} 

// ---------------------------------------------------------------------------
// void CEvtDbNotifier::StartNotification()
// ---------------------------------------------------------------------------
//  
void CEvtDbNotifier::StartNotification()
    {
	EVTUIDEBUG( "+ CEvtDbNotifier::StartNotification()" );
    if( !IsActive() )
        {
        iStatus = KRequestPending;
        // Request for notification
    	iRepository->NotifyRequest( KEvtDbChangeType, iStatus );
        SetActive();          
        }  
	EVTUIDEBUG( "- CEvtDbNotifier::StartNotification()" );
    }

// ---------------------------------------------------------------------------
// CEvtDbNotifier::HandleDbChangedL
// ---------------------------------------------------------------------------
//
void CEvtDbNotifier::HandleDbChangedL( )
    {    
	EVTUIDEBUG( "+ CEvtDbNotifier::HandleDbChangedL()" );
    // Notify the Observer for the database change type
    iDbObserver.HandleDbChangedL( );
	EVTUIDEBUG( "- CEvtDbNotifier::HandleDbChangedL()" );
    }
    
// ---------------------------------------------------------------------------
// CEvtDbNotifier::DbChangedL
// ---------------------------------------------------------------------------
//
void CEvtDbNotifier::DbChangedL( )
    {
	EVTUIDEBUG( "+ CEvtDbNotifier::DbChangedL()" );
    // Get the type of database change from Cenrep key
    TInt flag = 0;
    User::LeaveIfError( iRepository->Get( KEvtDbChangeType, flag ) );  
    
    EVTUIDEBUG1( "In DbChangedL Get %d", flag );
    flag = ( flag == 0 ) ? 1 : 0;
    EVTUIDEBUG1( "In DbChangedL Set %d", flag );
    
    // Set the KEvtDbChangeType key of the Cenrep, So that other database
    // connection can get the notifications
    User::LeaveIfError( iRepository->Set( KEvtDbChangeType, flag ) );
	EVTUIDEBUG( "- CEvtDbNotifier::DbChangedL()" );
    }
    
// End of File
