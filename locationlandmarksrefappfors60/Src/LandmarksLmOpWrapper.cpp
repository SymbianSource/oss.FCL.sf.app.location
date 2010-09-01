/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements the CLandmarksLmOpWrapper class
*
*/



#include <EPos_Landmarks.h>
#include <EPos_CPosLmOperation.h>

#include "LandmarksLmOpWrapper.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksLmOpWrapper::CLandmarksLmOpWrapper()
: CActive(CActive::EPriorityStandard)
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksLmOpWrapper::~CLandmarksLmOpWrapper()
    {
    Cancel();
    delete iLmOperation;
    iLmOperation = NULL;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksLmOpWrapper::StartOperation(
    CPosLmOperation* aLmOperation,
    TRequestStatus& aStatus,
    TBool aReportProgress)
    {
    delete iLmOperation;
    iLmOperation = aLmOperation;
    iCallerStatus = &aStatus;
    iReportProgress = aReportProgress;
    iProgress = 0;

    ExecuteNextStep();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CPosLmOperation* CLandmarksLmOpWrapper::LmOperationPtr()
    {
    return iLmOperation;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksLmOpWrapper::ExecuteNextStep(TRequestStatus& aStatus)
    {
    iCallerStatus = &aStatus;
    ExecuteNextStep();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TInt CLandmarksLmOpWrapper::Progress()
    {
    return (TInt) (iProgress * 100);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksLmOpWrapper::DoCancel()
    {
    delete iLmOperation;
    iLmOperation = NULL;
    User::RequestComplete(iCallerStatus, KErrCancel);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksLmOpWrapper::RunL()
    {
    if (iStatus == KErrNone)
        {
        // Operation finished.
        User::RequestComplete(iCallerStatus, KErrNone);
        }
    else if (iStatus == KPosLmOperationNotComplete)
        {
        if (iReportProgress)
            {
            User::RequestComplete(iCallerStatus, KPosLmOperationNotComplete);
            }
        else
            {
            ExecuteNextStep();
            }
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksLmOpWrapper::ExecuteNextStep()
    {
    iStatus = KRequestPending;
    iLmOperation->NextStep(iStatus, iProgress);
    SetActive();
    }


