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
* Description:  Asynchronous display of legal and disclaimer note.
*
*/


// System Includes
#include <centralrepository.h>
#include <StringLoader.h>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h> 
#include <evtmgmtui.rsg>
#include <coemain.h>
#include <aknappui.h>

// User Includes
#include "evtmgmtuilegalnote.h"
#include "evtdebug.h"
#include "evtstoragedomaincrkeys.h"

// Constants
const TInt KDisclaimerValueOff = 0;

// ================ Member funtions for CEvtDatabase class ==================

// ---------------------------------------------------------------------------
// CEvtMgmtUiLegalNote::CEvtMgmtUiLegalNote
// ---------------------------------------------------------------------------
//
CEvtMgmtUiLegalNote::CEvtMgmtUiLegalNote( CAknAppUi& aAppUi )
						:CActive( EPriorityStandard ), iAppUi(aAppUi)
    {
    }
    
// ---------------------------------------------------------------------------
// CEvtMgmtUiLegalNote::~CEvtMgmtUiLegalNote
// ---------------------------------------------------------------------------
//
CEvtMgmtUiLegalNote::~CEvtMgmtUiLegalNote()
    {
		Cancel();
    delete iRepository; 
    } 
    
// ---------------------------------------------------------------------------
// CEvtMgmtUiLegalNote::NewL
// ---------------------------------------------------------------------------
//
CEvtMgmtUiLegalNote* CEvtMgmtUiLegalNote::NewL( CAknAppUi& aAppUi )
    {
	CEvtMgmtUiLegalNote* self = NewLC( aAppUi );
	CleanupStack::Pop( self );
	return self;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiLegalNote::NewLC
// ---------------------------------------------------------------------------
//
CEvtMgmtUiLegalNote* CEvtMgmtUiLegalNote::NewLC( CAknAppUi& aAppUi )
    {
	CEvtMgmtUiLegalNote* self = new ( ELeave )CEvtMgmtUiLegalNote( aAppUi );
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
    } 

// ---------------------------------------------------------------------------
// CEvtMgmtUiLegalNote::ConstructL
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiLegalNote::ConstructL()
    {
	EVTUIDEBUG( "+ CEvtMgmtUiLegalNote::ConstructL()" );
    CActiveScheduler::Add(this);
    
	EVTUIDEBUG( "Calling CRepository::NewL" );
  // Create the Central repository object notifying the Disclaimer display
  TRAPD(err, iRepository = CRepository::NewL( TUid::Uid( KCRUidEvtStorageDb ) ) );
	EVTUIDEBUG1( "Calling CRepository::NewL Cenrep Error Code- %d", err );
	User::LeaveIfError( err );
    
	EVTUIDEBUG( "- CEvtMgmtUiLegalNote::ConstructL()" );
    } 

// ---------------------------------------------------------------------------
// void CEvtMgmtUiLegalNote::RunL()
// ---------------------------------------------------------------------------
//                             
void CEvtMgmtUiLegalNote::RunL()
    {
	EVTUIDEBUG( "+ CEvtMgmtUiLegalNote::RunL()" );
    
    HBufC* queryHeader = StringLoader::LoadLC( R_LOCEV_NOTE_DISCLAIMER_HEADING, CCoeEnv::Static() );
    HBufC* queryText = StringLoader::LoadLC( R_LOCEV_NOTE_DISCLAIMER_TEXT, CCoeEnv::Static() );
    CAknMessageQueryDialog* dlg = new ( ELeave ) CAknMessageQueryDialog ( queryText, queryHeader );
    TInt buttonId = dlg->ExecuteLD( R_EVTUI_DISCLAIMER_QUERY );
    CleanupStack::Pop(2);
    if( EEikBidOk == buttonId )
            {
            HBufC* msg = StringLoader::LoadLC( R_LOCEV_CONFIRMATION_NOTE_TEXT );
            CAknQueryDialog* confirmationQuery = CAknQueryDialog::NewL();     
            if( EAknSoftkeyYes == confirmationQuery->ExecuteLD(R_EVTUI_CONFIRMATION_QUERY, *msg) )
                    {
                    User::LeaveIfError( iRepository->Set( KEvtDisclaimerSetting, KDisclaimerValueOff ) );
                    }         
            CleanupStack::PopAndDestroy(msg);
            }
    else		// if( EEikBidCancel == buttonId )
            {
            iAppUi.Exit();
            }
	
	EVTUIDEBUG( "- CEvtMgmtUiLegalNote::RunL()" );
    }
    
// ---------------------------------------------------------------------------
// void CEvtMgmtUiLegalNote::DoCancel()
// ---------------------------------------------------------------------------
// 
void CEvtMgmtUiLegalNote::DoCancel()
    {
		EVTUIDEBUG( "+ CEvtMgmtUiLegalNote::DoCancel()" );
		// Nothing to do.
		EVTUIDEBUG( "- CEvtMgmtUiLegalNote::DoCancel()" );
    }

// ---------------------------------------------------------------------------
// TInt CEvtMgmtUiLegalNote::RunError()
// ---------------------------------------------------------------------------
//    
TInt CEvtMgmtUiLegalNote::RunError( TInt aError )
	{    	
	return CActive::RunError( aError );	
	} 

// ---------------------------------------------------------------------------
// void CEvtMgmtUiLegalNote::StartNotification()
// ---------------------------------------------------------------------------
//  
void CEvtMgmtUiLegalNote::DisplayNoteL()
    {
	EVTUIDEBUG( "+ CEvtMgmtUiLegalNote::DisplayNoteL()" );
    // Get the type of database change from Cenrep key
    TInt flag = 1;
    User::LeaveIfError( iRepository->Get( KEvtDisclaimerSetting, flag ) ); 
    
    if( flag && !IsActive() )
        {
		    TRequestStatus* status = &iStatus;
		    User::RequestComplete( status, KErrNone );
		    SetActive();      
        }  
	EVTUIDEBUG( "- CEvtMgmtUiLegalNote::DisplayNoteL()" );
    }
    
// End of File
