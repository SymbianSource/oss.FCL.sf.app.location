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
#include <aknlists.h>
#include "CLmkDlgCategorySelectorImpl.h"
#include "CLmkMultiSelectorDialog.h"
#include "CLmkLmItemListProvider.h"
#include "LmkListProviderFactory.h"
#include "CLmkUiUtils.h"
#include <lmkui.rsg>
#include <lmkerrors.h>


#if defined(_DEBUG)

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkDlgCategorySelectorImpl" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace
#endif
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkDlgCategorySelectorImpl::CLmkDlgCategorySelectorImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkDlgCategorySelectorImpl::CLmkDlgCategorySelectorImpl(
    CPosLandmarkDatabase& aDb,
    TBool aShowEmptyCategories,
    TBool aShowBlankNaviPane
     )
    : CLmkDlgSelectorImplBase( aDb ),
      iShowEmptyCategories( aShowEmptyCategories ),
      iShowBlankNaviPane (aShowBlankNaviPane)
    {
    }

// -----------------------------------------------------------------------------
// CLmkDlgCategorySelectorImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkDlgCategorySelectorImpl::ConstructL()
    {
    BaseConstructL();
    // Hide the navi pane contents of the dialog Launcher
    if (iShowBlankNaviPane)
	    {
	    iLmkUiUtils->StoreNaviPaneL();
	    iLmkUiUtils->CreateDefaultNaviPaneL();
	    }
    }

// -----------------------------------------------------------------------------
// CLmkDlgCategorySelectorImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkDlgCategorySelectorImpl* CLmkDlgCategorySelectorImpl::NewL(
    CPosLandmarkDatabase& aDb,
    TBool aShowEmptyCategories, TBool aShowBlankNaviPane)
    {
    CLmkDlgCategorySelectorImpl* self =
        new( ELeave ) CLmkDlgCategorySelectorImpl( aDb, aShowEmptyCategories, aShowBlankNaviPane );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CLmkDlgCategorySelectorImpl::~CLmkDlgCategorySelectorImpl
// ----------------------------------------------------
//
CLmkDlgCategorySelectorImpl::~CLmkDlgCategorySelectorImpl()
    {
    }

// ----------------------------------------------------
// CLmkDlgCategorySelectorImpl::SetupListProviderL
// ----------------------------------------------------
//
void CLmkDlgCategorySelectorImpl::SetupListProviderL()
    {
    if ( !iListProvider )
        {
        TLmkProviderType providerType( ELmkNotShowEmptyCategories );
        if ( iShowEmptyCategories )
            {
            providerType = ELmkShowEmptyCategories;
            }
        iListProvider = LmkListProviderFactory::CreateProviderL( iDb,
                                    ECategorySelector, providerType );

        iListProvider->AddObserverL( *this );
        iListProvider->InitializeL();
        }
    }

// ----------------------------------------------------
// CLmkDlgCategorySelectorImpl::ListProvider
// ----------------------------------------------------
//
const CLmkListProviderBase& CLmkDlgCategorySelectorImpl::ListProvider() const
    {
    __ASSERT_DEBUG( iListProvider, Panic( KLmkPanicNullMember ) );
    return *iListProvider;
    }

// ----------------------------------------------------
// CLmkDlgCategorySelectorImpl::GetDlgResources
// ----------------------------------------------------
//
void CLmkDlgCategorySelectorImpl::GetDlgResources(
    TBool aIsSingleSelector,
    TInt& aTitlePaneResource,
    TInt& aMenuBarResource,
    TInt& aDialogResource ) const
    {
    aTitlePaneResource = R_LMK_CATEGORY_SELECTOR_TITLE;
    aMenuBarResource = R_LMK_EMPTY_MENUBAR;

    if ( iSelectorImplParent )
        { // category selector as a part of landmark selector
        aDialogResource = R_LMK_SELECTOR_OPEN_CANCEL_DLG;
        }
    else
        {
        if ( aIsSingleSelector )
            {
            aDialogResource = R_LMK_SELECTOR_OK_CANCEL_DLG;
            }
        else
            {
            aDialogResource = R_LMK_CATEGORY_SELECTOR_OK_CANCEL_MULTI_DLG;
            }
        }
    }

// ----------------------------------------------------
// CLmkDlgCategorySelectorImpl::HandleListProviderEvent
// ----------------------------------------------------
//
void CLmkDlgCategorySelectorImpl::HandleListProviderEvent(
    TLmkListProviderEventType aEvent )
    {
    // Give event to the dialog as the first step since others
    // may then interact with the updated dialog:
    if ( iDialog )
        {
        iDialog->HandleListProviderEvent( aEvent );
        }

    // Base class implementation:
    CLmkDlgSelectorImplBase::HandleListProviderEvent( aEvent );
    }

// ----------------------------------------------------
// CLmkDlgCategorySelectorImpl::HandleListProviderError
// ----------------------------------------------------
//
void CLmkDlgCategorySelectorImpl::HandleListProviderError( TInt aError )
    {
    // Base class implementation:
    CLmkDlgSelectorImplBase::HandleListProviderError( aError );

    if ( iDialog )
        {
        iDialog->HandleListProviderError( aError );
        }
    }

//  End of File
