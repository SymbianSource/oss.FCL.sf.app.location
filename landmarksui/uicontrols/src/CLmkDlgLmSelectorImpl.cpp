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
#include "CLmkDlgLmSelectorImpl.h"
#include "CLmkMultiSelectorDialog.h"
#include "CLmkLmItemListProvider.h"
#include "LmkListProviderFactory.h"
#include <lmkui.rsg>
#include "CLmkDbUtils.h"
#include "CLmkUiUtils.h"

#include <EPos_CPosLmSearchCriteria.h>
#include <EPos_CPosLmCategoryCriteria.h>
#include <EPos_CPosLmCategoryManager.h>
#include <eiktxlbx.h>
#include <lmkerrors.h>


#if defined(_DEBUG)

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkDlgLmSelectorImpl" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace
#endif
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkDlgLmSelectorImpl::CLmkDlgLmSelectorImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkDlgLmSelectorImpl::CLmkDlgLmSelectorImpl(
    CPosLandmarkDatabase& aDb )
    : CLmkDlgSelectorImplBase( aDb )
    {
    }

// -----------------------------------------------------------------------------
// CLmkDlgLmSelectorImpl::CLmkDlgLmSelectorImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkDlgLmSelectorImpl::CLmkDlgLmSelectorImpl(
    CPosLandmarkDatabase& aDb,
    CPosLmSearchCriteria* aCriteria )
    : CLmkDlgSelectorImplBase( aDb ),
      iCriteria( aCriteria )
    {
    }

// -----------------------------------------------------------------------------
// CLmkDlgLmSelectorImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkDlgLmSelectorImpl::ConstructL()
    {
    BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CLmkDlgLmSelectorImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkDlgLmSelectorImpl* CLmkDlgLmSelectorImpl::NewL(
    CPosLandmarkDatabase& aDb )
    {
    CLmkDlgLmSelectorImpl* self =
        new( ELeave ) CLmkDlgLmSelectorImpl( aDb );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CLmkDlgLmSelectorImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkDlgLmSelectorImpl* CLmkDlgLmSelectorImpl::NewL(
    CPosLandmarkDatabase& aDb,
    CPosLmSearchCriteria* aCriteria )
    {
    CLmkDlgLmSelectorImpl* self =
        new( ELeave ) CLmkDlgLmSelectorImpl( aDb, aCriteria );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CLmkDlgLmSelectorImpl::~CLmkDlgLmSelectorImpl
// ----------------------------------------------------
//
CLmkDlgLmSelectorImpl::~CLmkDlgLmSelectorImpl()
    {
    delete iCriteria;
    }

// ----------------------------------------------------
// CLmkDlgLmSelectorImpl::SetupListProviderL
// ----------------------------------------------------
//
void CLmkDlgLmSelectorImpl::SetupListProviderL()
    {
    if ( !iListProvider )
        {
        if ( iCriteria )
            {
            iListProvider =
                LmkListProviderFactory::CreateProviderL( iDb, *iCriteria );
            }
        else
            {
            iListProvider =
                LmkListProviderFactory::CreateProviderL( iDb,
                                                         ELandmarkSelector );
            }
        iListProvider->AddObserverL( *this );
        iListProvider->InitializeL();
        }
    }

// ----------------------------------------------------
// CLmkDlgLmSelectorImpl::ListProvider
// ----------------------------------------------------
//
const CLmkListProviderBase& CLmkDlgLmSelectorImpl::ListProvider() const
    {
    __ASSERT_DEBUG( iListProvider, Panic( KLmkPanicNullMember ) );
    return *iListProvider;
    }

// ----------------------------------------------------
// CLmkDlgLmSelectorImpl::GetDlgResources
// ----------------------------------------------------
//
void CLmkDlgLmSelectorImpl::GetDlgResources(
    TBool aIsSingleSelector,
    TInt& aTitlePaneResource,
    TInt& aMenuBarResource,
    TInt& aDialogResource ) const
    {
    aTitlePaneResource = R_LMK_LANDMARK_SELECTOR_TITLE;
    aMenuBarResource = R_LMK_EMPTY_MENUBAR;

    if ( aIsSingleSelector )
        {
        if ( iCriteria )
            { // this is a "sub-dialog" when search criteria exists
            aDialogResource = R_LMK_SELECTOR_OK_BACK_DLG;
            }
        else
            {
            aDialogResource = R_LMK_SELECTOR_OK_CANCEL_DLG;
            }
        }
    else
        {
        if ( iCriteria )
            { // this is a "sub-dialog" when search criteria exists
            aDialogResource = R_LMK_SELECTOR_OK_BACK_MULTI_DLG;
            }
        else
            {
            aDialogResource = R_LMK_SELECTOR_OK_CANCEL_MULTI_DLG;
            }
        }
    }

// ----------------------------------------------------
// CLmkDlgLmSelectorImpl::HandleListProviderEvent
// ----------------------------------------------------
//
void CLmkDlgLmSelectorImpl::HandleListProviderEvent(
    TLmkListProviderEventType aEvent )
    {
    // Give event to the dialog as the first step since others
    // may then interact with the updated dialog:
    if (iCriteria)
	    {
	    if ( aEvent == ELmkEventCategoryDeleted )
		    {
		    TBool result = ETrue;
		    TRAP_IGNORE( result = CheckDbIfCriteriaCategoryExistsL() );
		    if ( !result )
    		    {
    		    // Delete the dialog
    		    delete iDialog;
    		    iDialog = NULL;
    		    }
		    }
		else if ( aEvent == ELmkEventCategoryUpdated )
		    {
		    TRAP_IGNORE( UpdateNaviLabelL() );
		    }
	    }
    if ( iDialog )
        {
         iDialog->HandleListProviderEvent( aEvent );
        }

    // Base class implementation:
    CLmkDlgSelectorImplBase::HandleListProviderEvent( aEvent );
    }

// ----------------------------------------------------
// CLmkDlgLmSelectorImpl::HandleListProviderError
// ----------------------------------------------------
//
void CLmkDlgLmSelectorImpl::HandleListProviderError( TInt aError )
    {
    // Base class implementation:
    CLmkDlgSelectorImplBase::HandleListProviderError( aError );

    if ( iDialog )
        {
        iDialog->HandleListProviderError( aError );
        }
    }

// ----------------------------------------------------
// CLmkDlgLmSelectorImpl::UpdateNaviLabelL
// ----------------------------------------------------
//
void CLmkDlgLmSelectorImpl::UpdateNaviLabelL()
    {
    CPosLmCategoryCriteria* criteria =
    reinterpret_cast<CPosLmCategoryCriteria*>( iCriteria );
    TPosLmItemId categoryId( KPosLmNullItemId );
    categoryId = criteria->CategoryItemId();
    HBufC* catName = CLmkDbUtils::CategoryNameL( iDb, categoryId );
    CleanupStack::PushL( catName );
    iLmkUiUtils->StoreNaviPaneL();
    iLmkUiUtils->CreateNaviLabelL(catName->Des());
    CleanupStack::PopAndDestroy(); //catName
    }

// ----------------------------------------------------
// CLmkDlgLmSelectorImpl::CheckDbIfCriteriaCategoryExistsL
// ----------------------------------------------------
//
TBool CLmkDlgLmSelectorImpl::CheckDbIfCriteriaCategoryExistsL()
    {
    TBool result = ETrue;
    CPosLmCategoryCriteria* criteria =
    static_cast<CPosLmCategoryCriteria*>( iCriteria );
    TPosLmItemId categoryId( KPosLmNullItemId );
    categoryId = criteria->CategoryItemId();
    if ( categoryId )
        {
        // Construction must fail if category has been deleted:
		CPosLmCategoryManager* mgr = CPosLmCategoryManager::NewL( iDb );
		CleanupStack::PushL( mgr );
		// Leaves with KErrNotFound if category doesn't exist
		TInt err = KErrNone;
		TRAP(err,CPosLandmarkCategory* category = mgr->ReadCategoryLC( categoryId );CleanupStack::PopAndDestroy( category ));
	    if(err == KErrNotFound)
		    {
		    result = EFalse;
		    }
		 CleanupStack::PopAndDestroy( 1 );//mgr
        }
    return result;
    }
//  End of File
