/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -
*
*/








// INCLUDE FILES
#include "CLmkAOOperation.h"
#include "MLmkAOOperationObserver.h"
#include <EPos_CPosLmOperation.h>
#include <EPos_Landmarks.h>

// CONSTANTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkAOOperation::CLmkAOOperation
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkAOOperation::CLmkAOOperation(
    CPosLmOperation* aLmOperation,
    MLmkAOOperationObserver& aObserver,
    MLmkAOOperationObserver::TOperationTypes aType,
    TBool aProgressNote )
    : CActive( EPriorityStandard ),
      iLmOperation( aLmOperation ),
      iObserver( aObserver ),
      iType( aType ),
      iProgressNote( aProgressNote )
    {
    }

// -----------------------------------------------------------------------------
// CLmkAOOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkAOOperation::ConstructL()
    {
    CActiveScheduler::Add( this );
    iTimer.CreateLocal();
    }

// -----------------------------------------------------------------------------
// CLmkAOOperation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkAOOperation* CLmkAOOperation::NewL(
    CPosLmOperation* aLmOperation,
    MLmkAOOperationObserver& aObserver,
    MLmkAOOperationObserver::TOperationTypes aType,
    TBool aProgressNote )
    {
    CLmkAOOperation* self =
        new ( ELeave ) CLmkAOOperation( aLmOperation, aObserver,
                                       aType, aProgressNote );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CLmkAOOperation::~CLmkAOOperation
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkAOOperation::~CLmkAOOperation()
    {
    iTimer.Close();
    Cancel();
    delete iLmOperation;
    }

// -----------------------------------------------------------------------------
// CLmkAOOperation::StartOperation
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkAOOperation::StartOperation()
    {
    iLmOperation->NextStep( iStatus, iProgress );
    SetActive();
    }

// -----------------------------------------------------------------------------
// CLmkAOOperation::StopOperation
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkAOOperation::StopOperation()
    {
    iTimer.Close();
    if (iLmOperation)
	    {
	    delete iLmOperation;
	    iLmOperation = NULL;
	    }
    }

// -----------------------------------------------------------------------------
// CLmkAOOperation::RunL
// -----------------------------------------------------------------------------
//
void CLmkAOOperation::RunL()
    {
    if ( iStatus == KPosLmOperationNotComplete || iIsTimerSet)
        {
        if ( iProgressNote && (!iIsTimerSet ))
            {
            iObserver.HandleOperationL( iType, iProgress, iStatus.Int() );
            }
        if(iLmOperation)
        	{
	        iLmOperation->NextStep( iStatus, iProgress );
	        iIsTimerSet = EFalse;
	        SetActive();
        	}
        }
    else if (iStatus == KErrLocked)
        {
           iTimer.After(iStatus,1);
           iIsTimerSet = ETrue;
        SetActive();
        }
    else
        {
        /*
        * The current way, in which the active object handled is, once the HandleOperationL
        * is called, it destroys this active object inside the HandleOperationL, hence,
        * if HandleOperationL leaves, AcvtiveScheduler will try to call RunError, in turn will crash,
        * so implementer of HandleOperationL must delete the current active object after all the
        * leavable codes, also if it is required, iStatus can be set as KErrLocked by the
        * HandleOperationL code to re-issue the request in case of failures.
        */
        TRAP_IGNORE(iObserver.HandleOperationL( iType, iProgress, iStatus.Int() ));
        }
    }

// -----------------------------------------------------------------------------
// CLmkAOOperation::DoCancel
// -----------------------------------------------------------------------------
//
void CLmkAOOperation::DoCancel()
    {
    iTimer.Close();
    if (iLmOperation)
	    {
	    delete iLmOperation;
	    iLmOperation = NULL;
	    }
    }

//  End of File
