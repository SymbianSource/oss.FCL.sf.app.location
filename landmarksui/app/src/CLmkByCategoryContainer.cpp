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
* Description:    Provides LM compass view's container class methods.
*
*/


// INCLUDE FILES
#include <aknlists.h>
#include <aknsfld.h>
#include <landmarks.rsg>
// For RD_AVKON_UNBRANCH
#include <bldvariant.hrh>
#include <AknDef.h>
#include "CLmkUiUtils.h"
#include "landmarks.hrh"
#include <csxhelp/lm.hlp.hrh>
#include "CLmkByCategoryView.h"
#include "CLmkByCategoryContainer.h"
#include "CLmkAppCategorySelectorImpl.h"

#include <lmkui.rsg>

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CLmkByCategoryContainer::NewL
// ----------------------------------------------------------------------------
//
CLmkByCategoryContainer* CLmkByCategoryContainer::NewL(
    MLmkKeyProcessor& aKeyProcessor,
    MObjectProvider* aParent,
	const TRect& aRect, CAknView& aView )
	{
	CLmkByCategoryContainer* self =
		new( ELeave ) CLmkByCategoryContainer( aKeyProcessor, aView );
	CleanupStack::PushL( self );
	self->SetMopParent( aParent );
	self->ConstructL( aRect );
        CleanupStack::Pop(); // self
	return self;
	}

// ----------------------------------------------------------------------------
// CLmkByCategoryContainer::CLmkByCategoryContainer
// ----------------------------------------------------------------------------
//
CLmkByCategoryContainer::CLmkByCategoryContainer(
                          MLmkKeyProcessor& aKeyProcessor, CAknView& aView ) :
    CLmkLbWithFilterContainer( aKeyProcessor, KLM_HLP_LM_BY_CATEGORY ),
    iView(aView)
    {
    }

// ----------------------------------------------------------------------------
// CLmkByCategoryContainer::~CLmkByCategoryContainer
// ----------------------------------------------------------------------------
//
CLmkByCategoryContainer::~CLmkByCategoryContainer()
    {
    }

// ----------------------------------------------------------------------------
// CLmkByCategoryContainer::SetupTitlePaneL
// ----------------------------------------------------------------------------
//
void CLmkByCategoryContainer::SetupTitlePaneL()
    {
    CLmkUiUtils::ChangeTitlePaneL( R_LMK_BYCATEGORY_VIEW_TITLE );
    }

// ----------------------------------------------------------------------------
// CLmkByCategoryContainer::SetupSelectorAndListL
// ----------------------------------------------------------------------------
//
void CLmkByCategoryContainer::SetupSelectorAndListL(
    CPosLandmarkDatabase& aDb,
    CLmkSender& aSender )
    {
    iSelector = CLmkAppCategorySelectorImpl::NewL( aDb, aSender, ETrue );
    iSelector->SetMskObserver(this);
    iListBox = &( iSelector->CreateListBoxL( this,
                                             EAknCtSingleGraphicListBox,
                                             EAknListBoxSelectionList,
                                             R_LMK_EMPTY_NO_LANDMARKS ,R_LMK_GUIDE_NEW_LANDMARK ) );
    }

// ----------------------------------------------------------------------------
// CLmkByCategoryContainer::GetCurrentCategoryId
// ----------------------------------------------------------------------------
//
TInt CLmkByCategoryContainer::GetCurrentCategoryId()
	{
	return (static_cast<CLmkAppCategorySelectorImpl*>(iSelector))->CurrentCategoryId();
	}

// ----------------------------------------------------------------------------
// CLmkByCategoryContainer::SetupNaviPaneL
// ----------------------------------------------------------------------------
//
void CLmkByCategoryContainer::SetupNaviPaneL()
    {
    iNaviDeco = CreateNaviPaneL( R_LMK_TAB_GROUP );
    ActivateTabL( *iNaviDeco, ELmkByCategoryView );
    }

// -----------------------------------------------------------------------------
//CLmkByCategoryContainer::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CLmkByCategoryContainer::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        (static_cast <CLmkByCategoryView *> (&iView))->Update();
        }
	if (iListBox)
		{
		iListBox->DrawNow();
		}
	if( iFindBox && iFindBox->IsVisible() )
		{
		iFindBox->DrawNow();
		}
    }

// ----------------------------------------------------------------------------
// CLmkByCategoryContainer::FocusChanged()
// ----------------------------------------------------------------------------
//
void CLmkByCategoryContainer::FocusChanged(TDrawNow aDrawNow)
	{
	if (iListBox)
		{
		iListBox->SetFocus(IsFocused(), aDrawNow);
		}
	if( iFindBox && iFindBox->IsVisible() )
		{
		iFindBox->SetFocus( IsFocused(), aDrawNow );
		}
	}

// ----------------------------------------------------------------------------
// CLmkByCategoryContainer::UpdateMskContainer()
// ----------------------------------------------------------------------------
//
void CLmkByCategoryContainer::UpdateMskContainerL()
	{
	CLmkBaseView* view = reinterpret_cast<CLmkBaseView*>(&iView);
	 view->UpdateMskViewL(ResolveMskDisplayItem());
	}
// End of File
