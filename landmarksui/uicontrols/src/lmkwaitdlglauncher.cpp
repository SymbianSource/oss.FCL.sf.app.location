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
* Description:    LandmarksUi Content File -    Encapuslates the Wait dialog in an Active Object interface
*
*/







// System Includes
#include <lmkui.rsg>

// User Includes
#include "lmkwaitdlglauncher.h"

// -----------------------------------------------------------------------------
// CLmkWaitDialogLauncher::CLmkWaitDialogLauncher
// C++ Overloaded constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkWaitDialogLauncher::CLmkWaitDialogLauncher()
    :CActive( EPriorityStandard )
    {
    }

// -----------------------------------------------------------------------------
// CLmkWaitDialogLauncher::~CLmkWaitDialogLauncher
// C++ destructor
// -----------------------------------------------------------------------------
//
CLmkWaitDialogLauncher::~CLmkWaitDialogLauncher()
    {
    Cancel();

    if ( iSchedWait->IsStarted() )
        {
        iSchedWait->AsyncStop();
        }

    delete iSchedWait;
    }

// -----------------------------------------------------------------------------
// CLmkWaitDialogLauncher* CLmkWaitDialogLauncher::NewL
// Two phase constructor
// -----------------------------------------------------------------------------
//
CLmkWaitDialogLauncher* CLmkWaitDialogLauncher::NewL()
    {
    CLmkWaitDialogLauncher* self =
                new ( ELeave ) CLmkWaitDialogLauncher();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// void CLmkWaitDialogLauncher::ConstructL
// Second phase constructor
// -----------------------------------------------------------------------------
//
void CLmkWaitDialogLauncher::ConstructL()
    {
    iSchedWait = new ( ELeave )CActiveSchedulerWait;
    CActiveScheduler::Add( this);
    }

// -----------------------------------------------------------------------------
// void CLmkWaitDialogLauncher::StartWaitDialogL
// Starts the wait dialog
// -----------------------------------------------------------------------------
//
void CLmkWaitDialogLauncher::StartWaitDialogL()
    {
    if( IsActive())
        {
        User::Leave( KErrInUse );
        }

    // Issue a request for Wait dialog launching

    TRequestStatus* status = &iStatus;
    User::RequestComplete( status, KErrNone );
    SetActive();
    }

// -----------------------------------------------------------------------------
// void CLmkWaitDialogLauncher::StartWaitDialogL
// Stops the wait dialog
// -----------------------------------------------------------------------------
//
TInt CLmkWaitDialogLauncher::StopWaitDialogL()
    {
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
        iSchedWait->Start();
        }

    // Return the Button ID with which the dialog was dismissed
    return iButtonId;
    }

// -----------------------------------------------------------------------------
// void CLmkWaitDialogLauncher::DialogDismissedL
// Inherited from MProgressDialogCallback
// -----------------------------------------------------------------------------
//
void CLmkWaitDialogLauncher::DialogDismissedL( TInt aButtonId )
    {
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
    }

// -----------------------------------------------------------------------------
// void CLmkWaitDialogLauncher::RunL
// Inherited from CActive
// -----------------------------------------------------------------------------
//
void CLmkWaitDialogLauncher::RunL()
    {
    if( iStatus.Int() == KErrNone )
        {
        // Launch the Wait dialog
        iWaitNote = new (ELeave) CAknWaitDialog
                      ((REINTERPRET_CAST(CEikDialog**,&iWaitNote)), ETrue );
        iWaitNote->SetCallback( this );
        if ( !iWaitNote->ExecuteLD( R_LMK_REQUEST_LOCATION ) )
            {
            //making iWaitNote null, since framework destroys waitnote
            //dialog on pressing cancel.
            iWaitNote = NULL;
            }
        }
    }

// -----------------------------------------------------------------------------
// void CLmkWaitDialogLauncher::DoCancel
// Inherited from CActive
// -----------------------------------------------------------------------------
//
void CLmkWaitDialogLauncher::DoCancel()
    {
    TRAP_IGNORE( iButtonId = StopWaitDialogL() );
    }
    
// -----------------------------------------------------------------------------
// void CLmkWaitDialogLauncher::SetObserver
// -----------------------------------------------------------------------------
void CLmkWaitDialogLauncher::SetObserver( MLmkWaitDialogLauncherObserver* aObserver)
	{
	iObserver = aObserver;
	}

// End of File
