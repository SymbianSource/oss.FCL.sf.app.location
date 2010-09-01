/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Encapuslates the Wait dialog in an Active Object interface 
*
*/


// System Includes
#include <evtmgmteditorui.rsg>
#include <eikenv.h>

// User Includes
#include "evtmgmtuiwaitdlglauncher.h"
#include "evtdebug.h"

// -----------------------------------------------------------------------------
// CLmkWaitDialogLauncher::CLmkWaitDialogLauncher
// C++ Overloaded constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CEvtMgmtUiWaitDialogLauncher::CEvtMgmtUiWaitDialogLauncher()
    :CActive( EPriorityUserInput )
    {
    iIsCancelAvailable = ETrue;
    iObserver = NULL;
    }
    
// -----------------------------------------------------------------------------
// CEvtMgmtUiWaitDialogLauncher::~CEvtMgmtUiWaitDialogLauncher
// C++ destructor
// -----------------------------------------------------------------------------
//    
CEvtMgmtUiWaitDialogLauncher::~CEvtMgmtUiWaitDialogLauncher()
    {
    Cancel();
    
    if ( iSchedWait->IsStarted() )
        {
        iSchedWait->AsyncStop();        
        }
            
    delete iSchedWait; 
    iSchedWait =NULL;
    
    delete iWaitText;
    iWaitText =NULL;
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiWaitDialogLauncher* CEvtMgmtUiWaitDialogLauncher::NewL
// Two phase constructor
// -----------------------------------------------------------------------------
//        
CEvtMgmtUiWaitDialogLauncher* CEvtMgmtUiWaitDialogLauncher::NewL()
    {
    CEvtMgmtUiWaitDialogLauncher* self = 
                new ( ELeave ) CEvtMgmtUiWaitDialogLauncher();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// void CEvtMgmtUiWaitDialogLauncher::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
void CEvtMgmtUiWaitDialogLauncher::ConstructL()
    {
    iSchedWait = new ( ELeave )CActiveSchedulerWait;
    CActiveScheduler::Add( this);
    }

// -----------------------------------------------------------------------------
// void CEvtMgmtUiWaitDialogLauncher::SetObserver
// -----------------------------------------------------------------------------
void CEvtMgmtUiWaitDialogLauncher::SetObserver( MEvtMgmtUiWaitDialogLauncherObserver* aObserver)
    {
    iObserver = aObserver; 
    }

// -----------------------------------------------------------------------------
// void CEvtMgmtUiWaitDialogLauncher::SetTextL
// -----------------------------------------------------------------------------
void CEvtMgmtUiWaitDialogLauncher::SetTextL(const TDesC& aText)
    {
    if(iWaitText)
        {
        delete iWaitText;
        iWaitText = NULL;
        }
    
    iWaitText = aText.AllocL();
    }
    
// -----------------------------------------------------------------------------
// void CEvtMgmtUiWaitDialogLauncher::SetTextL
// -----------------------------------------------------------------------------
void CEvtMgmtUiWaitDialogLauncher::SetTextL( const TInt aResourceId )
    {  
    if(iWaitText)
       {
       delete iWaitText;
       iWaitText = NULL;
       }
    iWaitText = CEikonEnv::Static()->AllocReadResourceAsDes16L( aResourceId );
    }
    
// -----------------------------------------------------------------------------
// void CEvtMgmtUiWaitDialoaLauncher::AllowUserToCancel
// -----------------------------------------------------------------------------
void CEvtMgmtUiWaitDialogLauncher::AllowUserToCancel( TBool aIsCancelAvailable )
    {
    iIsCancelAvailable = aIsCancelAvailable; 
    }

// -----------------------------------------------------------------------------
// void CEvtMgmtUiWaitDialogLauncher::StartWaitDialogL
// Starts the wait dialog
// -----------------------------------------------------------------------------
// 
void CEvtMgmtUiWaitDialogLauncher::StartWaitDialogL()
    {
    EVTUIDEBUG("+ CEvtMgmtUiWaitDialogLauncher::StartWaitDialogL" );
    if( IsActive())
        {
        User::Leave( KErrInUse );
        }
        
    // Issue a request for Wait dialog launching
            
    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    EVTUIDEBUG("- CEvtMgmtUiWaitDialogLauncher::StartWaitDialogL" );
    }

// -----------------------------------------------------------------------------
// void CEvtMgmtUiWaitDialogLauncher::StartWaitDialogL
// Stops the wait dialog
// -----------------------------------------------------------------------------
//    
TInt CEvtMgmtUiWaitDialogLauncher::StopWaitDialogL()
    {
    EVTUIDEBUG("+ CEvtMgmtUiWaitDialogLauncher::StopWaitDialogL" );
    if ( iWaitNote )
        {        	
        TRAPD( err, iWaitNote->ProcessFinishedL() );
        if (err != KErrNone)
    		{
		    delete iWaitNote;
            iWaitNote = NULL;            
            return KErrNone;            
	    	}    	    	
        }

    // Rechecking the Wait note is required since, the value might get modified
    // in DialogDismissedL call
    
    if( iWaitNote )
        {
    		EVTUIDEBUG("= Start Scheduler wait loop" );
    	if ( !iSchedWait->IsStarted() )
    	    iSchedWait->Start();
        }
    EVTUIDEBUG("- CEvtMgmtUiWaitDialogLauncher::StopWaitDialogL" );
                
    // Return the Button ID with which the dialog was dismissed    
    return iButtonId;               
    }

// -----------------------------------------------------------------------------
// void CEvtMgmtUiWaitDialogLauncher::DialogDismissedL
// Inherited from MProgressDialogCallback
// -----------------------------------------------------------------------------
// 
void CEvtMgmtUiWaitDialogLauncher::DialogDismissedL( TInt aButtonId )
    {
    EVTUIDEBUG1("+ CEvtMgmtUiWaitDialogLauncher::DialogDismissedL - %d", aButtonId );
    // Store the Button ID for later retrival
    iButtonId = aButtonId;
    
    if ( iSchedWait->IsStarted() )
        {
        iSchedWait->AsyncStop();        
        }
    iWaitNote = NULL;
    if(iObserver)
    	{
    	iObserver->HandleDialogDismissed( iButtonId );
    	}
    EVTUIDEBUG("- CEvtMgmtUiWaitDialogLauncher::DialogDismissedL" );
    }
            
// -----------------------------------------------------------------------------
// void CEvtMgmtUiWaitDialogLauncher::RunL
// Inherited from CActive
// -----------------------------------------------------------------------------
//
void CEvtMgmtUiWaitDialogLauncher::RunL()
    {
    EVTUIDEBUG("+ CEvtMgmtUiWaitDialogLauncher::RunL" );
    if( iStatus.Int() == KErrNone )
        {
        // Launch the Wait dialog
        iWaitNote = new (ELeave) CAknWaitDialog
                      ((REINTERPRET_CAST(CEikDialog**,&iWaitNote)), ETrue );
        iWaitNote->SetCallback( this ); 
        iWaitNote->SetTextL(*iWaitText);
        
        TInt resourceId ;
        if(iIsCancelAvailable)
            resourceId = R_EVTMGMTUI_WAITDLG_CANCEL;
        else
            resourceId = R_EVTEDITOR_WAITDLG;    
        
    		EVTUIDEBUG("= Before Dialog Launch" );
        if ( !iWaitNote->ExecuteLD( resourceId ) )
            {
    				EVTUIDEBUG("= After Dialog Launch" );
            //making iWaitNote null, since framework destroys waitnote
            //dialog on pressing cancel.
            iWaitNote = NULL;
            }
        }
    EVTUIDEBUG("+ CEvtMgmtUiWaitDialogLauncher::RunL" );
    }

// -----------------------------------------------------------------------------
// void CEvtMgmtUiWaitDialogLauncher::DoCancel
// Inherited from CActive
// -----------------------------------------------------------------------------
//    
void CEvtMgmtUiWaitDialogLauncher::DoCancel()
    {
    TRAP_IGNORE(StopWaitDialogL());     
    }
   
// -----------------------------------------------------------------------------
// void CEvtMgmtUiWaitDialogLauncher::StartWaitDialogSyncL
// -----------------------------------------------------------------------------
//  
void CEvtMgmtUiWaitDialogLauncher::StartWaitDialogSyncL()
    {
    // Check if there is any outstanding operation. If Yes, the one cannot
    // proceed
    if ( iWaitNote || IsActive())
        {
        User::Leave( KErrInUse );
        }
        
    // Launch the Wait dialog
    iWaitNote = new ( ELeave ) CAknWaitDialog
                  (( REINTERPRET_CAST( CEikDialog**,&iWaitNote )), ETrue );
    iWaitNote->SetTextL( *iWaitText );
    
    TInt resourceId ;
    if(iIsCancelAvailable)
        {
        resourceId = R_EVTMGMTUI_WAITDLG_CANCEL;
        }        
    else
        {
        resourceId = R_EVTEDITOR_WAITDLG;
        }
            
    if ( !iWaitNote->ExecuteLD( resourceId ) )
        {
        //making iWaitNote null, since framework destroys waitnote
        //dialog on pressing cancel.
        iWaitNote = NULL;
        }    
    }

// -----------------------------------------------------------------------------
// TInt CEvtMgmtUiWaitDialogLauncher::StopWaitDialogSyncL
// -----------------------------------------------------------------------------
//
TInt CEvtMgmtUiWaitDialogLauncher::StopWaitDialogSyncL()    
    {
    // Dismiss the Wait note
    if ( iWaitNote )
        {        	
        TRAPD( err, iWaitNote->ProcessFinishedL() );
        if (err != KErrNone)
    		{
		    delete iWaitNote;
            iWaitNote = NULL;            
            return KErrNone;            
	    	}    	    	
        }
    
    // Return the stored button ID
    return iButtonId;   
    }
// End of File

