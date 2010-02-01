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
* Description:   This class provides functionality of searching new categories
*                in a landmark package received asynchronously.
*
*/







// INCLUDE FILES
#include "CLmkPkgFindNewCatOp.h"
#include "CLmkAOOperation.h"
#include <EPos_CPosLmCategoryManager.h>
#include <EPos_CPosLmOperation.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkPkgFindNewCatOp::CLmkPkgFindNewCatOp
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkPkgFindNewCatOp::CLmkPkgFindNewCatOp(
                                        TInt& aNoOfNewCategories,
                                        RArray<TCategoryName>& aNewCategories,
                                        TCategoryName& aNewCategoryFound,
                                        CLmkSaveLandmarkUtils*aSaveLmUtils )
                                        :iSaveLmUtils(aSaveLmUtils),
                                         iIsFirstStep(ETrue),
                                         iNewCategoryFound(aNewCategoryFound),
                                         iNewCategories(aNewCategories),
                                         iNoOfNewCategories(aNoOfNewCategories)

    {
    }

// -----------------------------------------------------------------------------
// CLmkPkgFindNewCatOp::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkPkgFindNewCatOp::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// CLmkPkgFindNewCatOp::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkPkgFindNewCatOp* CLmkPkgFindNewCatOp::NewL(
                                        TInt& aNoOfNewCategories,
                                        RArray<TCategoryName>& aNewCategories,
                                        TCategoryName& aNewCategoryFound,
                                        CLmkSaveLandmarkUtils*aSaveLmUtils)
    {
    CLmkPkgFindNewCatOp* self =
        new ( ELeave ) CLmkPkgFindNewCatOp(aNoOfNewCategories,
                                           aNewCategories,
                                           aNewCategoryFound,
                                           aSaveLmUtils);

    CleanupStack::PushL( self );
    self->ConstructL( );
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CLmkPkgFindNewCatOp::~CLmkPkgFindNewCatOp
// -----------------------------------------------------------------------------
//
CLmkPkgFindNewCatOp::~CLmkPkgFindNewCatOp()
    {
    }

// -----------------------------------------------------------------------------
// CLmkPkgFindNewCatOp::NextStep
// -----------------------------------------------------------------------------
//
void CLmkPkgFindNewCatOp::NextStep( TRequestStatus& aStatus,
                                    TReal32& aProgress )
    {
    aStatus = KRequestPending;
    iStatus = &aStatus;
    iProgress = &aProgress;
    TInt err( KErrNone );

    if (iIsFirstStep)
        {
        iIsFirstStep = EFalse;
        *iProgress = 1;
        err = KPosLmOperationNotComplete;
        }
    else
        {
        TRAP(err,iNoOfNewCategories = iSaveLmUtils->CheckForNewCategories2L(iNewCategoryFound,iNewCategories));
        }
        User::RequestComplete( iStatus,err);
    }

// -----------------------------------------------------------------------------
// CLmkPkgFindNewCatOp::ExecuteL
// -----------------------------------------------------------------------------
//
void CLmkPkgFindNewCatOp::ExecuteL()
    {
    // We never use asynchronous methods in synchronous way and therefore
    // don't need to support this method
    User::Leave( KErrNotSupported );
    }
//  End of File
