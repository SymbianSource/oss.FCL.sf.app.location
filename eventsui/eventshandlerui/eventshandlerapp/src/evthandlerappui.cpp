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
* Description:  Application class for Events Handler Application UI.
*
*/


// SYSTEM FILES
#include <avkon.hrh>
#include <AknsConstants.h>
#include <aknnotewrappers.h>
#include <textresolver.h>

// USER INCLUDES
#include "evthandlerappui.h"
#include "evtrequest.h"
#include "evtmgmtuiengine.h"
#include "evtdebug.h"

// ----------------- Member funtions for CEvtHandlerAppUi --------------------

// ---------------------------------------------------------------------------
// void CEvtHandlerAppUi::ConstructL
// ---------------------------------------------------------------------------
//
void CEvtHandlerAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin | EAknEnableMSK ); 
    iEngine = CEvtMgmtUiEngine::NewL(); 
    iDlg = CEvtDlgLauncher::NewL( *iEngine, *this ); 
    }

// ---------------------------------------------------------------------------
// CEvtHandlerAppUi::~CEvtHandlerAppUi
// ---------------------------------------------------------------------------
//
CEvtHandlerAppUi::~CEvtHandlerAppUi()
    {	
    delete iDlg;
    iEventQue.ResetAndDestroy();
	iEventQue.Close();
   	delete iEngine;
    }

// ---------------------------------------------------------------------------
// TKeyResponse CEvtHandlerAppUi::HandleKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CEvtHandlerAppUi::HandleKeyEventL( const TKeyEvent& /*aKeyEvent*/,
                                              TEventCode /*aType*/ )
    {
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------------------------
// void CEvtHandlerAppUi::HandleCommandL
// ---------------------------------------------------------------------------
//
void CEvtHandlerAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyExit: 
        case EEikCmdExit:
            {
            Exit();
            break;
            }
       default: 
       	    { 	          	           	    
       	    break; 
       	    }
        }
    }

// ---------------------------------------------------------------------------
// void CEvtHandlerAppUi::HandleWsEventL
// ---------------------------------------------------------------------------
// 	
void CEvtHandlerAppUi::HandleWsEventL(const TWsEvent &aEvent, CCoeControl* aDestination)
	{
	TInt eventType = aEvent.Type();
	switch ( eventType )
		{
		case EEventKey:
			{
			// Handle Key Escape key.
			if( aEvent.Key()->iCode == EKeyEscape  )
				{
				if( !iFlag )
					iFlag = ETrue;	
				else
					iFlag = EFalse;
	    		iDlg->CancelDlgL( EFalse );
				}
			break;
			}
		case KAknUidValueEndKeyCloseEvent:
			{
			// Handle End key.
			if( iDlg )
				{
				EVTUIDEBUG("= CEvtHandlerAppUi::HandleWsEventL() - End Key");
				// Construct the TApaTask object.
				TApaTask task(iEikonEnv->WsSession( ));
				task.SetWgId(CEikonEnv::Static()->RootWin().Identifier());
				task.BringToForeground();
				
				// We Cancel any outstanding Dialog.
				iFlag = ETrue;
	    		iDlg->CancelDlgL( ETrue );
				return;
				}
			break;
			}
		default:
		break;
		}
		
	return CEikAppUi::HandleWsEventL( aEvent, aDestination );
	}
	
// ---------------------------------------------------------------------------
// void CEvtHandlerAppUi::NotifyDlgDismissedL
// ---------------------------------------------------------------------------
//    
void CEvtHandlerAppUi::NotifyDlgDismissedL( )
	{	
	if( !iFlag )
		{
		EVTUIDEBUG1("+ CEvtHandlerAppUi::NotifyDlgDismissedL() - %d", iEventQue[0]->Event().EventId());
		CEvtRequest* evtReq = iEventQue[0];
		iEventQue.Remove( 0 );
		delete evtReq;
		if( iEventQue.Count() == 0 )
			{
			Exit();	
			}
		
		HandleNextMessageL( );
		EVTUIDEBUG("- CEvtHandlerAppUi::NotifyDlgDismissedL()");
		}
	}
	
// ---------------------------------------------------------------------------
// void CEvtHandlerAppUi::ProcessCommandParametersL
// ---------------------------------------------------------------------------
//
TBool CEvtHandlerAppUi::ProcessCommandParametersL( CApaCommandLine &aCommandLine )
	{
	EVTUIDEBUG("+ CEvtHandlerAppUi::ProcessCommandParametersL()");
	// Process Command Parameters

    TBuf8<128> temp;
	temp.Copy(aCommandLine.TailEnd()); 
        
    TLex8 input( temp );
    
    TEvtEventId triggerId(0);
    TInt type(0);

    User::LeaveIfError( input.Val(triggerId, EDecimal) );
    input.Inc();
    User::LeaveIfError( input.Val(type) );
    
    HandleRecievedMessageL(triggerId, static_cast<TEvtRequestType>(type));

	EVTUIDEBUG("- CEvtHandlerAppUi::ProcessCommandParametersL()");
	return CEikAppUi::ProcessCommandParametersL( aCommandLine );
	}

// ---------------------------------------------------------------------------
// void CEvtHandlerAppUi::ProcessMessageL
// ---------------------------------------------------------------------------
//	
void CEvtHandlerAppUi::ProcessMessageL( TUid /*aUid*/, 
                      const TDesC8& aParams )
    {
	EVTUIDEBUG("+ CEvtHandlerAppUi::ProcessMessageL()");
    TBuf8<128> temp;
    temp.Copy( aParams );   
        
    TLex8 input( temp );
    
    TEvtEventId triggerId(0);
    TInt type(0);

    User::LeaveIfError( input.Val(triggerId, EDecimal) );
    input.Inc();
    User::LeaveIfError( input.Val(type) );
    
    HandleRecievedMessageL(triggerId, static_cast<TEvtRequestType>(type));
    
	EVTUIDEBUG("- CEvtHandlerAppUi::ProcessMessageL()");
    }

// ---------------------------------------------------------------------------
// void CEvtHandlerAppUi::HandleRecievedMessageL
// ---------------------------------------------------------------------------
//	
void CEvtHandlerAppUi::HandleRecievedMessageL( const TLbtTriggerId &  aTriggerId, 
                      TEvtRequestType aReqestType )
    {
	EVTUIDEBUG2("+ CEvtHandlerAppUi::HandleRecievedMessageL() - Id:%d, Type:%d", aTriggerId, aReqestType );
    
	// We first update the Trigger state.
	iEngine->HandleFiredTriggerStateL( aTriggerId );
	
    CEvtRequest* evtReq = CEvtRequest::NewL();
    CEvtEvent* evt = iEngine->EventL( aTriggerId );
    evtReq->SetEventL( evt );
    evtReq->SetRequestType( aReqestType );
    iEventQue.Insert(evtReq, 0);
	
	iFlag = ETrue;
	iDlg->CancelDlgL( ETrue );
	HandleNextMessageL( );
	
	EVTUIDEBUG("- CEvtHandlerAppUi::HandleRecievedMessageL()");
    }

// ---------------------------------------------------------------------------
// void CEvtHandlerAppUi::HandleNextMessageL
// ---------------------------------------------------------------------------
//	
void CEvtHandlerAppUi::HandleNextMessageL( )
    {
	EVTUIDEBUG("+ CEvtHandlerAppUi::HandleNextMessageL()");
	if( iEventQue.Count() > 0 )
		{
		EVTUIDEBUG1("= CEvtHandlerAppUi::HandleNextMessageL() - TriggerId:%d", iEventQue[0]->Event().EventId() );
	    CEvtRequest* evtReq = iEventQue[0];
	    iFlag = EFalse;
	    iDlg->LaunchDlgL( *evtReq );
		}
	EVTUIDEBUG("- CEvtHandlerAppUi::HandleNextMessageL()");
    }
                          	
// End of File  
