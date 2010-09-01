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
* Description:  Events Dialog Launcher.
*
*/


// System Includes
#include <e32base.h>
#include <eikenv.h>
#include <eikappui.h>
#include <NpdApi.h>					// Notepad
#include <avkon.hrh>	

// User Includes
#include "evtdlglauncher.h"
#include "evteventsdlg.h"
#include "evtrequest.h"

// ================ Member funtions for CEvtDlgLauncher class ===============

// ---------------------------------------------------------------------------
// CEvtDlgLauncher::CEvtDlgLauncher
// ---------------------------------------------------------------------------
//
CEvtDlgLauncher::CEvtDlgLauncher( CEvtMgmtUiEngine& aEngine, MEvtDlgObserver& aObserver ):
							CActive( EPriorityStandard ),iObserver(aObserver),iEngine(aEngine)
	{
	}

// ---------------------------------------------------------------------------
// CEvtDlgLauncher::~CEvtDlgLauncher
// ---------------------------------------------------------------------------
//
CEvtDlgLauncher::~CEvtDlgLauncher()
	{
    CActive::Cancel();
    iEngine.RemoveObserver(this);
	}
	
// ---------------------------------------------------------------------------
// CEvtDlgLauncher::NewL
// ---------------------------------------------------------------------------
//
CEvtDlgLauncher* CEvtDlgLauncher::NewL( CEvtMgmtUiEngine& aEngine, MEvtDlgObserver& aObserver )
    {
	CEvtDlgLauncher* self = NewLC( aEngine, aObserver );
	CleanupStack::Pop( self );
	return self;
    }

// ---------------------------------------------------------------------------
// CEvtDlgLauncher::NewLC
// ---------------------------------------------------------------------------
//
CEvtDlgLauncher* CEvtDlgLauncher::NewLC( CEvtMgmtUiEngine& aEngine, MEvtDlgObserver& aObserver )
    {
	CEvtDlgLauncher* self = new ( ELeave )CEvtDlgLauncher( aEngine, aObserver );
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
    } 

// ---------------------------------------------------------------------------
// CEvtDlgLauncher::ConstructL
// ---------------------------------------------------------------------------
//
void CEvtDlgLauncher::ConstructL()
    {
    // Add the object to the Active scheduler
    CActiveScheduler::Add( this );
    iEngine.SetObserver(this);
    }
    
// ---------------------------------------------------------------------------
// void CEvtDlgLauncher::RunL()
// ---------------------------------------------------------------------------
//                             
void CEvtDlgLauncher::RunL()
    {
    switch( iRequest->RequestType() )
    	{
    	case ERequestEditor:
    		{
		    iDialog = CEvtEventsDlg::NewL( 
		    	EFalse, 
		    	iRequest->Event(),  
		    	iEngine, EFalse );            
		     
		    iDialog->ExecuteLD( );
            iObserver.NotifyDlgDismissedL();
            
    		break;
    		}
    	case ERequestNotepad:        
	    	{
	    	TInt ret = CNotepadApi::ExecReadOnlyTextViewerL( iRequest->Event().Description(), iRequest->Event().Subject(), KNullDesC );
		    if( !ret )
	            {
	            iObserver.NotifyDlgDismissedL();
	            }
    		break;
    		}
    	default:
    		break;
    	}
    }
    
// ---------------------------------------------------------------------------
// void CEvtDlgLauncher::DoCancel()
// ---------------------------------------------------------------------------
// 
void CEvtDlgLauncher::DoCancel()
    {
    }

// ---------------------------------------------------------------------------
// TInt CEvtDlgLauncher::RunError()
// ---------------------------------------------------------------------------
//    
TInt CEvtDlgLauncher::RunError( TInt aError )
	{
	return CActive::RunError( aError );	
	}
	
// ---------------------------------------------------------------------------
// CEvtDlgLauncher::LaunchEventDlg
// ---------------------------------------------------------------------------
//
void CEvtDlgLauncher::LaunchDlgL( CEvtRequest& aRequest )
	{
    if ( IsActive( ) )
        {
        User::Leave ( KErrInUse );
        }
    
    iRequest = &aRequest;
    
    TRequestStatus* status = &iStatus;
    SetActive();    
    User::RequestComplete( status, KErrNone );

	}

// ---------------------------------------------------------------------------
// CEvtDlgLauncher::CancelDlgL
// ---------------------------------------------------------------------------
//
void CEvtDlgLauncher::CancelDlgL( TBool aIsForcedCancel )
	{
	// We have to dismiss any Dialog which is launched on top of Editor
	// So are looking for dismiss.
	if( iDialog )
		{
	    delete iDialog;
	    iDialog = NULL;	
		}
	if( aIsForcedCancel )
		{
	    TKeyEvent key;
	    key.iCode = EKeyEscape;
	    key.iModifiers = 0;
	    CEikonEnv::Static()->SimulateKeyEventL( key, EEventKey );
		}
    CActive::Cancel();
	}
		
// ---------------------------------------------------------------------------
// CEvtDlgLauncher::NotifyEventChangeL()
// Derived from MEvtMgmtUiEngineObserver
// ---------------------------------------------------------------------------
//
void CEvtDlgLauncher::NotifyEventChangeL(TInt /*aErrorCode*/, TInt /*aState */)
    {
    // Not needed     
    }

// ---------------------------------------------------------------------------
// CEvtDlgLauncher::NotifyModelChangeL()
// Derived from MEvtMgmtUiEngineObserver
// ---------------------------------------------------------------------------
//
void CEvtDlgLauncher::NotifyModelChangeL()
    {
    // Not needed 
    }  
    
