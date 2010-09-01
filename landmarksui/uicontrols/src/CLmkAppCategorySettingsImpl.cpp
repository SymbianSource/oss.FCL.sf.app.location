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
* Description:    Category settings implementation for LandmarksUi
*
*/


// INCLUDE FILES
#include "CLmkAppCategorySettingsImpl.h"
#include "LmkListProviderFactory.h"
#include "CLmkLmItemListProvider.h"
#include "CLmkUiItemBase.h"
#include "CLmkDbUtils.h"
#include "CLmkUiUtils.h"
#include "LmkConsts.h"
#include "LmkNotes.h"
#include <lmkui.rsg>
#include <eiktxlbx.h>
#include <EPos_CPosLandmarkCategory.h>
#include <EPos_CPosLmCategoryManager.h>
#include <EPos_CPosLandmarkCategory.h>
#include <AknWaitDialog.h>
#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLandmark.h>
#include <EPos_TPosLmSortPref.h>
#include <EPos_CPosLmItemIterator.h>
#include <lmkerrors.h>


// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

#if defined(_DEBUG)

_LIT( KPanicMsg, "CLmkAppCategorySettingsImpl" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
#endif

}  // namespace
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkAppCategorySettingsImpl::CLmkAppCategorySettingsImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkAppCategorySettingsImpl::CLmkAppCategorySettingsImpl(
    CPosLandmarkDatabase& aDb,
    CLmkSender& aSender,
    TBool aFindBox )
    : CLmkAppLmItemSelectorImpl( aDb, aSender, aFindBox )
    {
    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySettingsImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkAppCategorySettingsImpl::ConstructL()
    {
    CLmkAppLmItemSelectorImpl::BaseConstructL();
    iBuffer = HBufC::NewL( KMaxCategoryNameFieldLen );
    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySettingsImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkAppCategorySettingsImpl* CLmkAppCategorySettingsImpl::NewL(
    CPosLandmarkDatabase& aDb,
    CLmkSender& aSender,
    TBool aFindBox )
    {
    CLmkAppCategorySettingsImpl* self =
        new( ELeave ) CLmkAppCategorySettingsImpl( aDb, aSender, aFindBox );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySettingsImpl::~CLmkAppCategorySettingsImpl
// -----------------------------------------------------------------------------
//
CLmkAppCategorySettingsImpl::~CLmkAppCategorySettingsImpl()
    {
    if ( iWaitNote )
        {
        TRAPD( err, iWaitNote->ProcessFinishedL() );
        if (err != KErrNone)
    		{
		    delete iWaitNote;
            iWaitNote = NULL;
	    	}
        }
    delete iBuffer;
    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySettingsImpl::SetupListProviderL
// -----------------------------------------------------------------------------
//
void CLmkAppCategorySettingsImpl::SetupListProviderL()
    {
    if ( !iListProvider )
        {
        iListProvider =
            LmkListProviderFactory::CreateProviderL( iDb, ECategorySelector,
                                                  ELmkShowExtendedCategories );
        iListProvider->AddObserverL( *this );
        iListProvider->InitializeL();
        }
    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySettingsImpl::ListProvider
// -----------------------------------------------------------------------------
//
const CLmkListProviderBase& CLmkAppCategorySettingsImpl::ListProvider() const
    {
    __ASSERT_DEBUG( iListProvider, Panic( KLmkPanicNullMember ) );
    return *iListProvider;
    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySettingsImpl::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CLmkAppCategorySettingsImpl::ProcessCommandL( TInt aCommandId )
    {
    switch ( aCommandId )
        {
        case ELmkCmdNewCategory:
			{
            NewCategoryCmdL();
			break;
			}
		case ELmkCmdChangeIcon:
			{
            ChangeIconCmdL();
			break;
			}
		case ELmkCmdRenameCat:
			{
            RenameCategoryCmdL();
			break;
			}
		case ELmkCmdDeleteCat:
			{
            DeleteCategoryCmdL();
			break;
            }
        default:
            {
            // leave command handling to others
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySettingsImpl::HandleOperationL
// -----------------------------------------------------------------------------
//
void CLmkAppCategorySettingsImpl::HandleOperationL(
                                       TOperationTypes aType,
                                       TReal32 /*aProgress*/,
                                       TInt aStatus )
    {
    if ( aType == EDeleteCategories )
        {
        if ( aStatus != KPosLmOperationNotComplete && iWaitNote )
            {
            iWaitNote->ProcessFinishedL();
            iWaitNote = NULL;
            }
        }
    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySettingsImpl::ChangeIconL
// -----------------------------------------------------------------------------
//
void CLmkAppCategorySettingsImpl::ChangeIconL(
    TPosLmItemId aId,
    TInt aIconIndex,
    TInt aMaskIndex)
    {
    TFileName* iconFile = CLmkUiUtils::LmkUiIconFileLC();
    iDbUtils->ChangeIconL( iDb, aId, ELmkItemTypeCategory,
                           *iconFile, aIconIndex, aMaskIndex);
    CleanupStack::PopAndDestroy();//iconFile
    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySettingsImpl::ChangeIconsL
// -----------------------------------------------------------------------------
//
void CLmkAppCategorySettingsImpl::ChangeIconsL(
    RArray<TPosLmItemId>& aIdArray,
    TInt aIconIndex,
    TInt aMaskIndex)
    {
    TFileName* iconFile = CLmkUiUtils::LmkUiIconFileLC();
    iDbUtils->ChangeIconsL( aIdArray, ELmkItemTypeCategory,
                            *iconFile, aIconIndex, aMaskIndex);
    CleanupStack::PopAndDestroy();//iconFile
    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySettingsImpl::NewCategoryCmdL
// -----------------------------------------------------------------------------
//
void CLmkAppCategorySettingsImpl::NewCategoryCmdL()
    {
    TPtr catName( iBuffer->Des() );
    catName.Zero();
    CPosLandmarkCategory* newCategory = CPosLandmarkCategory::NewLC();

    while( LmkNotes::CategoryNameQueryL( catName, R_LMK_CATEGORY_NAME_QUERY ) )
        {
        newCategory->SetCategoryNameL( catName );
        TRAPD( err, iDbUtils->AddCategoryL( *newCategory, iDb ) );
        if( err == KErrNone )
            {
            iNewListBoxItemAdded = ETrue;      
            break;
            }
        else if ( err == KErrAlreadyExists )
            {            
            LmkNotes::InformationNoteL( iEnv,R_LMK_CATEGORY_NAME_EXISTS_NOTE );
            }
        else
            {
            User::LeaveIfError( err ); // other error cases
            break; // exit while loop if no error
            }
        }    
    CleanupStack::PopAndDestroy(); // newCategory
    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySettingsImpl::RenameCategoryCmdL
// -----------------------------------------------------------------------------
//
void CLmkAppCategorySettingsImpl::RenameCategoryCmdL()
    {
    __ASSERT_DEBUG( iListProvider, Panic( KLmkPanicNullMember ) );
    TInt index = iListBox->CurrentItemIndex();
    CLmkUiItemBase* item =
        iListProvider->ItemAtL( index );
    CleanupStack::PushL( item );

    TPtr catName( iBuffer->Des() );
    catName.Zero();
    catName.Append( item->Text() );

    while( LmkNotes::CategoryNameQueryL( catName, R_LMK_CATEGORY_NAME_QUERY ) )
        {
        TRAPD( err,
               iDbUtils->RenameCategoryL( catName, SelectedIdL(), iDb ) );
        if ( err == KErrAlreadyExists )
            {
            LmkNotes::InformationNoteL( iEnv, R_LMK_CATEGORY_NAME_EXISTS_NOTE );
            }
        else if ( err == KErrNone)
        	{
        	iNewListBoxItemAdded = ETrue;
        	break;
        	}
        else
            {
            User::LeaveIfError( err ); // other error cases
            break; // exit while loop if no error
            }
        }

    CleanupStack::PopAndDestroy(); //item
    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySettingsImpl::IsGlobalCategory
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CLmkAppCategorySettingsImpl::IsPredefinedCategoryL()
	{

	TBool isGlobalCategory = EFalse;

	RArray<TPosLmItemId>& markedItemIds = MarkedItemIdsL();
    TInt markedCount = markedItemIds.Count();

    if (markedCount)
        {
        CPosLmCategoryManager* categoryMgr = CPosLmCategoryManager::NewL( iDb );
        CleanupStack::PushL( categoryMgr );

        for(TInt i =0; i < markedCount; i++)
        	{
        	if(!isGlobalCategory)
        		{
        		CPosLandmarkCategory* category = categoryMgr->ReadCategoryLC(markedItemIds[i]);
        		if (category->GlobalCategory() != KPosLmNullGlobalCategory)
		        	{
		           	isGlobalCategory = ETrue;
		        	}
		        CleanupStack::PopAndDestroy(1); //category
        		}
        	}
        CleanupStack::PopAndDestroy(1); //categoryMgr
	    }
    else
        {
        TPosLmItemId categoryId = SelectedIdL();
        CPosLmCategoryManager* categoryMgr = CPosLmCategoryManager::NewL( iDb );

        CleanupStack::PushL( categoryMgr );
        CPosLandmarkCategory* category = categoryMgr->ReadCategoryLC( categoryId );

        // Check for Global category
        if (category->GlobalCategory() != KPosLmNullGlobalCategory)
        	{
           	isGlobalCategory = ETrue;
        	}

        CleanupStack::PopAndDestroy( 2 ); //category, categoryMgr
	    }
	return isGlobalCategory;
	}
// -----------------------------------------------------------------------------
// CLmkAppCategorySettingsImpl::DeleteCategoryCmdL
// -----------------------------------------------------------------------------
//
void CLmkAppCategorySettingsImpl::DeleteCategoryCmdL()
    {
    RArray<TPosLmItemId>& markedItemIds = MarkedItemIdsL();
    TInt markedCount = markedItemIds.Count();

    if(IsPredefinedCategoryL())
    	return;

    if ( markedCount )
        {
        if ( LmkNotes::CategoryConfirmationQueryL( iEnv, markedCount ) )
            {
            StoreIdTobeFocussedAfterDeletion( markedItemIds );
            iDeletionHelper.StoreListInformation( *iListBox, ETrue );
            iDbUtils->DeleteCategoriesL( markedItemIds );
            // wait note
            iWaitNote = new( ELeave ) CAknWaitDialog( NULL, ETrue );
            if ( !iWaitNote->ExecuteLD( R_LMK_DELETING_WAIT_NOTE ) )
                {
                //making iWaitNote null, since framework destroys waitnote
        		//dialog on pressing cancel.
        		iWaitNote = NULL;
                iDbUtils->CancelLmOperation();
                }
            }

        }
    else
        {
        // One item only
        RArray<TPosLmItemId> categoryIdArray( 1 );
        CleanupClosePushL( categoryIdArray );
        User::LeaveIfError( categoryIdArray.Append( SelectedIdL() ) );
        if ( LmkNotes::CategoryConfirmationQueryL( iEnv, 1 ) )
            {
            iDeletionHelper.StoreListInformation( *iListBox, EFalse );
            iDbUtils->DeleteCategoriesL( categoryIdArray  );
            }
        CleanupStack::PopAndDestroy( 1 ); // categoryIdArray

        }
    }

// -----------------------------------------------------------------------------
// CLmkAppCategorySettingsImpl::HandleEventListReadyL
// -----------------------------------------------------------------------------
//
TBool CLmkAppCategorySettingsImpl::HandleEventListReadyL()
    {
    // Possibly user-initiated delete finished --> if yes, update the list:
    TBool retVal;
	if( iNewListBoxItemAdded )
		{
	    TPosLmItemId lmkItemId = iDbUtils->CategoryIdL(iDb, *iBuffer);
	    TInt listboxIndex = LmItemListProvider().SelectedIndex(lmkItemId);
	    if ( listboxIndex == KLmkInValidIndex )
		    {
		    retVal = EFalse;
		    return retVal;
		    }
		/* This line is added to hold the current before changing
		 * the selection with the new one.
		 */
		iListBox->HandleItemAdditionL();
		iListBox->SetCurrentItemIndex( listboxIndex );
		UpdateMarkIndexAfterAdditionL( listboxIndex );
		iListBox->DrawDeferred();
		iNewListBoxItemAdded = EFalse;
		retVal = ETrue;
		}
    else
    	{
    	retVal = iDeletionHelper.UpdateListIfDeleteHappenedL();
    	if( retVal )
			{
			//delete has been performed, reset the focus.
			SetFocusToItemAfterDeletionOfMarkedItems();
			}
    	iListBox->DrawDeferred();
    	}
    return retVal;
    }
// -----------------------------------------------------------------------------
// CLmkAppCategorySettingsImpl::HandleLmCreated
// -----------------------------------------------------------------------------
//
void CLmkAppCategorySettingsImpl::HandleLmCreated(TPosLmItemId aNewLmkItemId)
    {
    iNewLmkItemId = aNewLmkItemId;
    iNewListBoxItemAdded = ETrue;
    }
//  End of File
