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
#include "CLmkAddToCatOperation.h"
#include "CLmkAOOperation.h"
#include <EPos_CPosLmCategoryManager.h>
#include <EPos_CPosLmOperation.h>
#include <lmkerrors.h>

#if defined(_DEBUG)
// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkAddToCatOperation" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace
#endif
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkAddToCatOperation::CLmkAddToCatOperation
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkAddToCatOperation::CLmkAddToCatOperation(
    CPosLmCategoryManager& aCategoryMgr )
    : iCategoryMgr( aCategoryMgr )
    {
    }

// -----------------------------------------------------------------------------
// CLmkAddToCatOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkAddToCatOperation::ConstructL(
    const RArray<TPosLmItemId>& aLandmarkIdArray,
    const RArray<TPosLmItemId>& aCategoryIdArray )
    {
    TInt lmCount = aLandmarkIdArray.Count();
    for ( TInt i( 0 ); i < lmCount; ++i )
        {
        User::LeaveIfError( iLmIdArray.Append( aLandmarkIdArray[i] ) );
        }
    TInt catCount = aCategoryIdArray.Count();
    for ( TInt k( 0 ); k < catCount; ++k )
        {
        User::LeaveIfError( iCatIdArray.Append( aCategoryIdArray[k] ) );
        }
    }

// -----------------------------------------------------------------------------
// CLmkAddToCatOperation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkAddToCatOperation* CLmkAddToCatOperation::NewL(
    CPosLmCategoryManager& aCategoryMgr,
    const RArray<TPosLmItemId>& aLandmarkIdArray,
    const RArray<TPosLmItemId>& aCategoryIdArray )
    {
    CLmkAddToCatOperation* self =
        new ( ELeave ) CLmkAddToCatOperation( aCategoryMgr );

    CleanupStack::PushL( self );
    self->ConstructL( aLandmarkIdArray, aCategoryIdArray );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CLmkAddToCatOperation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkAddToCatOperation* CLmkAddToCatOperation::NewL(
    CPosLmCategoryManager& aCategoryMgr,
    TPosLmItemId aLandmarkId,
    const RArray<TPosLmItemId>& aCategoryIdArray )
    {
    CLmkAddToCatOperation* self =
        new ( ELeave ) CLmkAddToCatOperation( aCategoryMgr );

    CleanupStack::PushL( self );
    // For users of this class we provide two NewL interfaces but internally
    // we create an array also in case of just one landmark item.
    RArray<TPosLmItemId> tmpArray = RArray<TPosLmItemId>( 1 );
    CleanupClosePushL( tmpArray );
    User::LeaveIfError( tmpArray.Append( aLandmarkId ) );
    self->ConstructL( tmpArray, aCategoryIdArray );
    CleanupStack::PopAndDestroy(); // tmpArray
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CLmkAddToCatOperation::~CLmkAddToCatOperation
// -----------------------------------------------------------------------------
//
CLmkAddToCatOperation::~CLmkAddToCatOperation()
    {
    if (iStatus)
        {
        if (*iStatus == KRequestPending )
            {
            User::RequestComplete( iStatus, KErrCancel );
            }
        }
    iLmIdArray.Close();
    iCatIdArray.Close();
    delete iAddOperation;
    }

// -----------------------------------------------------------------------------
// CLmkAddToCatOperation::NextStep
// -----------------------------------------------------------------------------
//
void CLmkAddToCatOperation::NextStep( TRequestStatus& aStatus,
                                      TReal32& aProgress )
    {
    aStatus = KRequestPending;
    iStatus = &aStatus;
    iProgress = &aProgress;

    TInt count = iCatIdArray.Count();
    // Index should be smaller than count if NextStep called properly:
    __ASSERT_DEBUG( iCurrentIndex < count, Panic( KLmkPanicIllegalMethodCall ) );

    TInt err( KErrNone );

    // Do one step:
    if ( iCurrentIndex < count )
        {
        TRAP( err, DoAddToCatStepL( iCatIdArray[iCurrentIndex++] ) );
        }

    if ( err != KErrNone )
        { // error occurred already when preparing for asynch. operation
        --iCurrentIndex;
        *iProgress = 1;
        User::RequestComplete( iStatus, err );
        }
    }

// -----------------------------------------------------------------------------
// CLmkAddToCatOperation::ExecuteL
// -----------------------------------------------------------------------------
//
void CLmkAddToCatOperation::ExecuteL()
    {
    // We never use asynchronous methods in synchronous way and therefore
    // don't need to support this method
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CLmkAddToCatOperation::HandleOperationL
// -----------------------------------------------------------------------------
//
void CLmkAddToCatOperation::HandleOperationL(
    TOperationTypes /*aType*/,
    TReal32 /*aProgress*/,
    TInt aStatus )
    {
    // Operation (adding one category to landmarks) finished:
    delete iAddOperation;
    iAddOperation = NULL;

    TInt result( KErrNone );
    TInt count = iCatIdArray.Count();

    // Check result:
    if ( aStatus != KErrNone || iCurrentIndex == count )
        {
        *iProgress = 1;
        result = aStatus;
        }
    else
        {
        result = KPosLmOperationNotComplete;
        // Neither divider nor dividend can be zero in this if-branch:
        __ASSERT_DEBUG( ( iCurrentIndex > 0 ) && ( count > 0 ),
                        Panic( KLmkPanicMathError ) );
        *iProgress = static_cast<TReal32>( iCurrentIndex ) / count;
        }
    User::RequestComplete( iStatus, result );
    }

// -----------------------------------------------------------------------------
// CLmkAddToCatOperation::DoAddToCatStepL
// -----------------------------------------------------------------------------
//
void CLmkAddToCatOperation::DoAddToCatStepL( TPosLmItemId aCategoryId )
    {
    CPosLmOperation* operation =
        iCategoryMgr.AddCategoryToLandmarksL( aCategoryId,
                                              iLmIdArray );

    __ASSERT_DEBUG( !iAddOperation, Panic( KLmkPanicAlreadyActive ) );

    iAddOperation =
        CLmkAOOperation::NewL( operation,
                               *this,
                               MLmkAOOperationObserver::EAddToCategory,
                               EFalse );
    iAddOperation->StartOperation();
    }

//  End of File
