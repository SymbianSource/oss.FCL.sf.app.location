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
#include "TLmkDeletionHelper.h"
#include <eiklbx.h>
#include <aknlists.h>
#include <lmkerrors.h>


#if defined(_DEBUG)

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {
_LIT( KPanicMsg, "TLmkDeletionHelper" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace
#endif


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TLmkDeletionHelper::TLmkDeletionHelper
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TLmkDeletionHelper::TLmkDeletionHelper()
    : iUserRequestedDelete( EFalse ),
      iCurrentItemIndexBeforeRemoval( -1 ),
      iCurrentItemWasRemoved( EFalse ),
      iIndexesToRemovedItems( NULL ),
      iListBox( NULL )
    {
    }

// ----------------------------------------------------
// TLmkDeletionHelper::~TLmkDeletionHelper
// ----------------------------------------------------
//
TLmkDeletionHelper::~TLmkDeletionHelper()
    {
    }

// -----------------------------------------------------------------------------
// TLmkDeletionHelper::StoreListInformation
// -----------------------------------------------------------------------------
//
void TLmkDeletionHelper::StoreListInformation(
    CEikListBox& aListBox,
    TBool aMultipleItems )
    {
    iListBox = &aListBox;
    iUserRequestedDelete = ETrue;
    /*
    * After deletion, the focus must go to the first item,
    * commenting re-assigning the iCurrentItemIndexBeforeRemoval
    * with zero.
    */
	iCurrentItemIndexBeforeRemoval = iListBox->CurrentItemIndex();
	//iCurrentItemIndexBeforeRemoval = KTopListIndex;
    if ( aMultipleItems )
        {
        iIndexesToRemovedItems = iListBox->SelectionIndexes();
        // This parameter is not important in this case:
        iCurrentItemWasRemoved = EFalse;
        }
    else
        {
        iCurrentItemWasRemoved = ETrue;
        // This parameter is not important in this case:
        iIndexesToRemovedItems = NULL;
        }
    }

// -----------------------------------------------------------------------------
// TLmkDeletionHelper::UpdateListIfDeleteHappenedL
// -----------------------------------------------------------------------------
//
TBool TLmkDeletionHelper::UpdateListIfDeleteHappenedL()
    {
    // Avkon's utility seems to work nicely even if operation was cancelled
    // and some items were not deleted.

    if ( iUserRequestedDelete )
        {
        if( iIndexesToRemovedItems )
            {
            // We need to make a copy of the index array since AknListBoxUtils
            // requires a non-const parameter:
            TInt count = iIndexesToRemovedItems->Count();
            // If index array exists it always contains some items:
            __ASSERT_DEBUG( count > 0, Panic( KLmkPanicDeletionInfo ) );
            CArrayFix<TInt>* removedItems =
                new( ELeave ) CArrayFixFlat<TInt>( count );
            CleanupStack::PushL( removedItems );
            for ( TInt i( 0 ); i < count; ++i )
                {
                removedItems->AppendL( iIndexesToRemovedItems->At( i ) );
                }

            AknListBoxUtils::HandleItemRemovalAndPositionHighlightL(
                                                iListBox,
                                                iCurrentItemIndexBeforeRemoval,
                                                *removedItems );
            CleanupStack::PopAndDestroy( removedItems );
            }
        else
            {
            AknListBoxUtils::HandleItemRemovalAndPositionHighlightL(
                                                iListBox,
                                                iCurrentItemIndexBeforeRemoval,
                                                iCurrentItemWasRemoved );
            }
        // List update now done, set indicator to EFalse:
        iUserRequestedDelete = EFalse;
        return ETrue;
        }
    else
        { // Delete hasn't been performed by the user,
          // i.e. StoreListInformation() hasn't been called
        return EFalse;
        }
    }

//  End of File
