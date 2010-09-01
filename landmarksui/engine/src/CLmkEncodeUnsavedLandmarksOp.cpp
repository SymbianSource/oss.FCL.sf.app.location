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
* Description:   This class provides functionality of encoding landmarks
*                both saved and unsaved landmarks (In Landmarks DB).
*
*/







// INCLUDE FILES
#include "CLmkEncodeUnsavedLandmarksOp.h"
#include "CLmkAOOperation.h"
#include <EPos_CPosLmCategoryManager.h>
#include <EPos_CPosLmOperation.h>
#include "CLmkDbUtils.h"


//CONSTANTS
const TReal32 KOperationDone = 1;
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkEncodeUnsavedLandmarksOp::CLmkEncodeUnsavedLandmarksOp
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkEncodeUnsavedLandmarksOp::CLmkEncodeUnsavedLandmarksOp(
           CPosLandmarkEncoder&  aLandmarkEncoder,
           CPosLandmarkDatabase::TTransferOptions  aTransferOptions)
           :iEncoder(aLandmarkEncoder),
            iTransferOptions(aTransferOptions),
            iCurrentIsLm(ETrue),
            iStatusFlag(KPosLmOperationNotComplete)

    {
    iNrOfSubOperations = 1;
    }

// -----------------------------------------------------------------------------
// CLmkEncodeUnsavedLandmarksOp::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkEncodeUnsavedLandmarksOp::ConstructL(const RPointerArray<CLmkLandmark>&  aLandmarks)
    {
    for (TInt i = 0; i < aLandmarks.Count(); i++)
        {
        CLmkLandmark* landmark = CLmkLandmark::NewL(*aLandmarks[i]);
        CleanupStack::PushL(landmark);
        User::LeaveIfError(iLandmarks.Append(landmark));
        CleanupStack::Pop();//landmark
        }
    }


// -----------------------------------------------------------------------------
// CLmkEncodeUnsavedLandmarksOp::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkEncodeUnsavedLandmarksOp* CLmkEncodeUnsavedLandmarksOp::NewL(
                              CPosLandmarkEncoder&  aLandmarkEncoder,
                              const RPointerArray<CLmkLandmark>&  aLandmarks,
                              CPosLandmarkDatabase::TTransferOptions  aTransferOptions )
    {
    CLmkEncodeUnsavedLandmarksOp* self =
        new ( ELeave ) CLmkEncodeUnsavedLandmarksOp(aLandmarkEncoder,
                                                    aTransferOptions);

    CleanupStack::PushL( self );
    self->ConstructL(aLandmarks );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CLmkEncodeUnsavedLandmarksOp::~CLmkEncodeUnsavedLandmarksOp
// -----------------------------------------------------------------------------
//
CLmkEncodeUnsavedLandmarksOp::~CLmkEncodeUnsavedLandmarksOp()
    {
    iLandmarks.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CLmkEncodeUnsavedLandmarksOp::NextStep
// -----------------------------------------------------------------------------
//
void CLmkEncodeUnsavedLandmarksOp::NextStep( TRequestStatus& aStatus,
                                    TReal32& aProgress )
    {
    aStatus = KRequestPending;
    iStatus = &aStatus;
    iProgress = &aProgress;
    TInt err(KErrNone);
    if (iCurrentIsLm)
        {
        CLmkLandmark* lmkLm  = iLandmarks[iCurrentLm++];
        CPosLandmark* lm = lmkLm->PosLandmark();
        TRAP(err,CLmkDbUtils::AddDefaultProtocolL(lm));
        if (iTransferOptions & CPosLandmarkDatabase::EIncludeCategories)
            {
            iCurrentCategory = 0;
            iCategories = lmkLm->Categories();
            if (iCategories.Count() != 0)
                {
                iCurrentIsLm = EFalse;
                }
            }

        TRAP(err,iEncoder.AddLandmarkL(*lm));
        }
    else
        {
        for (TInt i = 0;
            i < iNrOfSubOperations &&
                iCurrentCategory < iCategories.Count(); i++)
            {
            CPosLandmarkCategory* category =iCategories[iCurrentCategory++];
            if (iCurrentCategory == iCategories.Count())
                {
                iCurrentIsLm = ETrue;
                }
            TRAP(err,iEncoder.AddCategoryForLatestLandmarkL(*category));
            }
        }

    if(!err)
        {
        if (iCurrentLm == iLandmarks.Count() &&
            iCurrentCategory == iCategories.Count())
            {
            iStatusFlag = KErrNone;
            aProgress = KOperationDone;
            }
        else if (iCurrentIsLm)
            {
            aProgress = TReal32(iCurrentLm) / iLandmarks.Count();
            }
        else
            {
            aProgress = TReal32(iCurrentLm - 1) / iLandmarks.Count() +
                        TReal32(iCurrentCategory + 1) /
                        (iCategories.Count() + 1) /
                        iLandmarks.Count();
            }
        }
    else
        {
        iStatusFlag = err;
        aProgress = KOperationDone;
        }

    User::RequestComplete( iStatus,iStatusFlag);
    }

// -----------------------------------------------------------------------------
// CLmkEncodeUnsavedLandmarksOp::ExecuteL
// -----------------------------------------------------------------------------
//
void CLmkEncodeUnsavedLandmarksOp::ExecuteL()
    {
    // We never use asynchronous methods in synchronous way and therefore
    // don't need to support this method
    User::Leave( KErrNotSupported );
    }
//  End of File
