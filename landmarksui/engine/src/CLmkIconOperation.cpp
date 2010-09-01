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
#include "CLmkIconOperation.h"
#include <EPos_CPosLmCategoryManager.h>
#include "LmkConsts.h"
#include <lmkerrors.h>

#if defined(_DEBUG)
// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkIconOperation" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace
#endif
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkIconOperation::CLmkIconOperation
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkIconOperation::CLmkIconOperation(
    CPosLandmarkDatabase& aDb,
    TLmkItemType aItemsType,
    TInt aIconIndex )
    : iDb( aDb ),
      iIconIndex( aIconIndex )
    {
    // Set method pointer:
    if ( aItemsType == ELmkItemTypeLandmark )
        {
        iStepMethodL = &CLmkIconOperation::DoLandmarkStepL;
        }
    else
        {
        __ASSERT_DEBUG( aItemsType == ELmkItemTypeCategory,
                        Panic( KLmkPanicUnknownItemType ) );
        iStepMethodL = &CLmkIconOperation::DoCategoryStepL;
        }
    }

// -----------------------------------------------------------------------------
// CLmkIconOperation::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkIconOperation::ConstructL(
    const RArray<TPosLmItemId>& aIdArray,
    TLmkItemType aItemsType,
    const TDesC& aMbmFileName )
    {
    TInt count = aIdArray.Count();
    for ( TInt i( 0 ); i < count; ++i )
        {
        User::LeaveIfError( iIdArray.Append( aIdArray[i] ) );
        }

    if ( aItemsType == ELmkItemTypeCategory )
        {
        iCategoryMgr = CPosLmCategoryManager::NewL( iDb );
        }
    iMbmFileName = aMbmFileName.AllocL();
    }

// -----------------------------------------------------------------------------
// CLmkIconOperation::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkIconOperation* CLmkIconOperation::NewL(
    CPosLandmarkDatabase& aDb,
    const RArray<TPosLmItemId>& aIdArray,
    TLmkItemType aItemsType,
    const TDesC& aMbmFileName,
    TInt aIconIndex )
    {
    CLmkIconOperation* self = new ( ELeave ) CLmkIconOperation( aDb,
                                                                aItemsType,
                                                                aIconIndex );

    CleanupStack::PushL( self );
    self->ConstructL( aIdArray, aItemsType, aMbmFileName );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CLmkIconOperation::~CLmkIconOperation
// -----------------------------------------------------------------------------
//
CLmkIconOperation::~CLmkIconOperation()
    {
    iIdArray.Close();
    delete iCategoryMgr;
    delete iMbmFileName;
    }

// -----------------------------------------------------------------------------
// CLmkIconOperation::NextStep
// -----------------------------------------------------------------------------
//
void CLmkIconOperation::NextStep( TRequestStatus& aStatus, TReal32& aProgress )
    {
    aStatus = KRequestPending;
    aProgress = 0;

    TInt count = iIdArray.Count();
    // Index should be smaller than count if NextStep called properly:
    __ASSERT_DEBUG( iCurrentIndex < count, Panic( KLmkPanicIllegalMethodCall ) );

    TInt result( KErrNone );

    // Do one step:
    if ( iCurrentIndex < count )
        {
        TRAP( result, ( this->*iStepMethodL )( iIdArray[iCurrentIndex++] ) );
        }

    // Check result:
    if ( result != KErrNone || iCurrentIndex == count )
        {
        --iCurrentIndex;
        aProgress = 1;
        }
    else
        {
        result = KPosLmOperationNotComplete;
        // Neither divider nor dividend can be zero in this if-branch:
        __ASSERT_DEBUG( ( iCurrentIndex > 0 ) && ( count > 0 ),
                        Panic( KLmkPanicMathError ) );
        aProgress = static_cast<TReal32>( iCurrentIndex ) / count;
        }
    TRequestStatus* status = &aStatus;
    User::RequestComplete( status, result );
    }

// -----------------------------------------------------------------------------
// CLmkIconOperation::ExecuteL
// -----------------------------------------------------------------------------
//
void CLmkIconOperation::ExecuteL()
    {
    // We never use asynchronous methods in synchronous way and therefore
    // don't need to support this method
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CLmkIconOperation::DoLandmarkStepL
// -----------------------------------------------------------------------------
//
void CLmkIconOperation::DoLandmarkStepL( TPosLmItemId aId )
    {

    CPosLandmark* landmark = iDb.ReadLandmarkLC( aId );
    landmark->SetIconL( *iMbmFileName, iIconIndex, KLmkDefaultId+1 );
	iDb.UpdateLandmarkL( *landmark );
    CleanupStack::PopAndDestroy(); // landmark
    }

// -----------------------------------------------------------------------------
// CLmkIconOperation::DoCategoryStepL
// -----------------------------------------------------------------------------
//
void CLmkIconOperation::DoCategoryStepL( TPosLmItemId aId )
    {

    CPosLandmarkCategory* category =
        iCategoryMgr->ReadCategoryLC( aId );
    category->SetIconL( *iMbmFileName, iIconIndex, KCategoryDefaultId+1);
	iCategoryMgr->UpdateCategoryL( *category );
    CleanupStack::PopAndDestroy(); // category
    }

//  End of File
