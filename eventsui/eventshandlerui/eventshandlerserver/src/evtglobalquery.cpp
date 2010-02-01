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
* Description:  Handler for displaying the Events Global Query
*
*/

// System Includes
#include <coecntrl.h> // CCoeControl
#include <AknGlobalMsgQuery.h> //CAknGlobalMsgQuery
#include <avkon.rsg>
#include <avkon.hrh>
#include <gulicon.h>
#include <AknsUtils.h>
#include <avkon.mbg>
#include <evthandlerserver.mbg>
#include <eikimage.h>
#include <data_caging_path_literals.hrh>
#include <AknsConstants.h> 		// Skin Ids
#include <AknIconUtils.h>
#include <AknsSkinInstance.h>

// User Includes
#include "evtglobalquery.h"
#include "evtinfonoteparams.h"
#include "evtkeylockhandler.h"
#include "evtdebug.h"
#include "evtinfonoteinterface.h"

_LIT( KEvtDefaultIconFileName, "evthandlerserver.mif" );
static const TInt KTimetoPlay = 60;

// ================ Member funtions for CEvtGlobalQuery class ===============

// ---------------------------------------------------------------------------
// CEvtGlobalQuery::CEvtGlobalQuery
// ---------------------------------------------------------------------------
//
CEvtGlobalQuery::CEvtGlobalQuery( CEvtInfoNoteInputParam&      aInputParam ) 
        : CActive (EPriorityStandard),  iInputParam( aInputParam )
	{
    CActiveScheduler::Add( this );
	}

// ---------------------------------------------------------------------------
// CEvtGlobalQuery::~CEvtGlobalQuery
// ---------------------------------------------------------------------------
//
CEvtGlobalQuery::~CEvtGlobalQuery()
	{
	Cancel();
	
    // Delete the Query
	delete iGlobalMsgQuery;
	
    // Delete the tone player
    delete iTonePlayer;
    
    // Delete Key Lock Handler
    if( iKeyLockHandler )
        {
        delete iKeyLockHandler;
        }
	}
	
// ---------------------------------------------------------------------------
// CEvtGlobalQuery::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtGlobalQuery* CEvtGlobalQuery::NewL( CEvtInfoNoteInputParam&      aInputParam )
    {
	CEvtGlobalQuery* self = NewLC(aInputParam);
	CleanupStack::Pop( self );
	return self;
    }

// ---------------------------------------------------------------------------
// CEvtGlobalQuery::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtGlobalQuery* CEvtGlobalQuery::NewLC( CEvtInfoNoteInputParam&      aInputParam )
    {
	CEvtGlobalQuery* self = new ( ELeave )CEvtGlobalQuery(aInputParam);
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
    } 

// ---------------------------------------------------------------------------
// CEvtGlobalQuery::ConstructL
// ---------------------------------------------------------------------------
//
void CEvtGlobalQuery::ConstructL()
    {
    EVTUIDEBUG( "+ CEvtGlobalQuery::ConstructL()" );
	    	
    iGlobalMsgQuery = CAknGlobalMsgQuery::NewL();
        
    iKeyLockHandler = CEvtKeyLockHandler::NewL( );
    
    iTonePlayer = CEvtTonePlayer::NewL( *this );     
	
    EVTUIDEBUG( "- CEvtGlobalQuery::ConstructL()" );
    }

// ---------------------------------------------------------------------------
// void CEvtGlobalQuery::RunL()
// ---------------------------------------------------------------------------
//                             
void CEvtGlobalQuery::RunL()
    {
	EVTUIDEBUG( "+ CEvtGlobalQuery::RunL()" );
    iReturnBuffer->Zero( );
	if(iStatus.Int() == EAknSoftkeyOpen)
		{
		iReturnBuffer->AppendNum( EOpenTrigger );
		
		// Disable the Key Lock irrespective of previously maintained state.
		// Update the Key Lock state.
	    if ( iKeyLockHandler )
	        {
	        iKeyLockHandler->EnableKeyLock( EFalse );
			iKeyLockHandler->UpdateStateWithoutNote();
	        }
		}
	else
		{
		iReturnBuffer->AppendNum( ECloseInfoNote );
	
		// Update the Key Lock state.
	    if ( iKeyLockHandler )
	        {
			iKeyLockHandler->UpdateStateWithNote();
	        }
		}
    User::RequestComplete( iReturnStatus, KErrNone );
	
	EVTUIDEBUG( "- CEvtGlobalQuery::RunL()" );
    }
    
// ---------------------------------------------------------------------------
// void CEvtGlobalQuery::DoCancel()
// ---------------------------------------------------------------------------
// 
void CEvtGlobalQuery::DoCancel()
    {
	EVTUIDEBUG( "+ CEvtGlobalQuery::DoCancel()" );
    // Cancel the outstanding CR notification request
    if( iGlobalMsgQuery )
        {
        iGlobalMsgQuery->CancelMsgQuery();      
        User::RequestComplete( iReturnStatus, KErrCancel );  
        }
	EVTUIDEBUG( "- CEvtGlobalQuery::DoCancel()" );
    }

// ---------------------------------------------------------------------------
// TInt CEvtGlobalQuery::RunError()
// ---------------------------------------------------------------------------
//    
TInt CEvtGlobalQuery::RunError( TInt aError )
	{    	
	return aError;	
	} 

// ---------------------------------------------------------------------------
// void CEvtGlobalQuery::CancelDialog()
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtGlobalQuery::CancelDialog()
	{    	
    EVTUIDEBUG( "+ CEvtGlobalQuery::CancelDialog()" );
	Cancel();
    EVTUIDEBUG( "- CEvtGlobalQuery::CancelDialog()" );
	} 

// ---------------------------------------------------------------------------
// TInt CEvtGlobalQuery::RunError()
// ---------------------------------------------------------------------------
//  
EXPORT_C void CEvtGlobalQuery::StartGlobalQueryL(TRequestStatus& aStatus, TDes8& aRetBuffer)
    {
    EVTUIDEBUG( "+ CEvtGlobalQuery::StartGlobalQueryL()" );
    iReturnStatus = &aStatus;
    iReturnBuffer = &aRetBuffer;
    
    *iReturnStatus = KRequestPending;
    
    if( iGlobalMsgQuery )
        {
        iGlobalMsgQuery->CancelMsgQuery();  
        delete iGlobalMsgQuery;
        iGlobalMsgQuery = NULL;
        }
    
    TFileName fileName;    
    TInt bmpID;
    TInt bmpMaskID;
    
    GetIconDetailsL(fileName, bmpID, bmpMaskID);
    
    // Maintain the Key Lock State and Enable if it is already not enabled
    if ( iKeyLockHandler )
        {
        iKeyLockHandler->MaintainState();
        }
        
    // If tone player is there, then start tone playing
    if ( iInputParam.ToneEnabled() )
        {
        iGlobalMsgQuery = CAknGlobalMsgQuery::NewL();
        iGlobalMsgQuery->ShowMsgQueryL(iStatus, iInputParam.Description(), R_AVKON_SOFTKEYS_OPEN_CLOSE, iInputParam.Subject(), fileName, bmpID, bmpMaskID );
        SetActive();
        iTonePlayer->PlayFileL( iInputParam.Tone(), iInputParam.ToneRepeat(), KTimetoPlay );
        }
    else
        {
        iGlobalMsgQuery = CAknGlobalMsgQuery::NewL();
        iGlobalMsgQuery->ShowMsgQueryL(iStatus, iInputParam.Description(), R_AVKON_SOFTKEYS_OPEN_CLOSE, iInputParam.Subject(), fileName, bmpID, bmpMaskID );
        SetActive();
        }

    EVTUIDEBUG( "- CEvtGlobalQuery::StartGlobalQueryL()" );
    }  

// ---------------------------------------------------------------------------
// void CEvtGlobalQuery::GetIconDetailsL()
// ---------------------------------------------------------------------------
//    
void CEvtGlobalQuery::GetIconDetailsL( TFileName& aFileName, TInt& aBmpID, TInt& aBmpMaskID )
	{    	
    EVTUIDEBUG( "+ CEvtGlobalQuery::GetIconDetailsL()" );
	RProcess currentProcess;
	TParse parser;
	
	parser.Set( currentProcess.FileName(), NULL, NULL );
	TPtrC filename( parser.FullName() );
    //filename.Copy( KTempDrive );
	
	User::LeaveIfError( parser.Set( KEvtDefaultIconFileName, &KDC_APP_BITMAP_DIR, &filename ));
    aFileName = parser.FullName();
    
     //retrieve accuracy level       
    CEvtInfoNoteInputParam::TEvtTriggerAccuracy accuracy = iInputParam.TrigerringAccuracy();
    EVTUIDEBUG1( "Icon creation:Accuracy level = %d", accuracy )
    
    switch( accuracy )
    	{
    	case CEvtInfoNoteInputParam::EEvtAccurateTrigger:
    		aBmpID = EMbmEvthandlerserverQgn_note_locev_alarm_acc;
    		aBmpMaskID = EMbmEvthandlerserverQgn_note_locev_alarm_acc_mask;
    		break;
    	case CEvtInfoNoteInputParam::EEvtInaccurateTrigger:
    		aBmpID = EMbmEvthandlerserverQgn_note_locev_alarm_inacc;
    		aBmpMaskID = EMbmEvthandlerserverQgn_note_locev_alarm_inacc_mask;
    		break;
    	case CEvtInfoNoteInputParam::EEvtHighlyInaccurateTrigger:
    	default:
    		aBmpID = EMbmEvthandlerserverQgn_note_locev_alarm_inacc_heavy;
    		aBmpMaskID = EMbmEvthandlerserverQgn_note_locev_alarm_inacc_heavy_mask;
    		break;
    	}
    EVTUIDEBUG( "- CEvtGlobalQuery::GetIconDetailsL()" );
	} 

// ---------------------------------------------------------------------------
// void CEvtGlobalQuery::TonePlayCompleteL()
// ---------------------------------------------------------------------------
//  
void CEvtGlobalQuery::TonePlayCompleteL( TBool aTimeout )
	{
    EVTUIDEBUG( "+ CEvtGlobalQuery::TonePlayCompleteL()" );
		// Enable KeyLock on Timeout.
    if ( iKeyLockHandler && aTimeout )
        {
        iKeyLockHandler->EnableKeyLock( ETrue );
        iKeyLockHandler->UpdateStateWithoutNote();
        }
        
    // Update the CBA
    iGlobalMsgQuery->UpdateMsgQuery( R_AVKON_SOFTKEYS_OPEN_CLOSE );
    EVTUIDEBUG( "- CEvtGlobalQuery::TonePlayCompleteL()" );
	}
	

