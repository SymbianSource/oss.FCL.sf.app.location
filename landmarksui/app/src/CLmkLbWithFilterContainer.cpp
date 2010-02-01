/*
* Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains methods for implementing container with
*				 find/searchbox
*
*/

// INCLUDE FILES

#include <aknlists.h>
#include <aknsfld.h>
#include <AknsBasicBackgroundControlContext.h>
#include "CLmkDocument.h"
#include "landmarks.hrh"
#include "CLmkAppUi.h"
#include "CLmkAppSelectorImplBase.h"
#include "CLmkLbWithFilterContainer.h"
#include <lmkerrors.h>

// CONSTANTS
namespace {
#if defined(_DEBUG)
_LIT( KPanicText, "CLmkLbWithFilterContainer" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicText, aReason );
    }
#endif
}  // namespace
// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------------------------------
// CLmkLbWithFilterContainer::CLmkBaseContainer
// ----------------------------------------------------------------------------
//
CLmkLbWithFilterContainer::CLmkLbWithFilterContainer(
    MLmkKeyProcessor& aKeyProcessor,
    const TDesC& aHelpContext )
    :CLmkBaseContainer( aKeyProcessor, aHelpContext )
    {
    }

// ----------------------------------------------------------------------------
// CLmkLbWithFilterContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CLmkLbWithFilterContainer::ConstructL( const TRect& aRect )
    {
    CLmkBaseContainer::BaseConstructL();
    CreateWindowL();

    // Get the skin instance
	iSkinInstance = AknsUtils::SkinInstance();

	// Create background control context for skinning the background
	iBackgroundSkinContext = CAknsBasicBackgroundControlContext::NewL(
			KAknsIIDQsnBgAreaMain, aRect, ETrue );

    iIsNeedToReDraw = ETrue;

    CLmkAppUi* lmkAppUi = static_cast<CLmkAppUi*>( ViewAppUi() );
    CPosLandmarkDatabase& db = lmkAppUi->Document().LmDbL();
    CLmkSender& sender = lmkAppUi->LmkSender();
    SetupSelectorAndListL( db, sender );

    SetupTitlePaneL();
    SetupNaviPaneL();
    SetupFindBoxL();

    // enable scroll arrows
    CEikScrollBarFrame* sBFrame = iListBox->CreateScrollBarFrameL( ETrue );
    sBFrame->SetScrollBarVisibilityL( CEikScrollBarFrame::EOn,
                                      CEikScrollBarFrame::EAuto );

    SetRect( aRect );
    ActivateL();
    }

// ----------------------------------------------------------------------------
// CLmkLbWithFilterContainer::~CLmkLbWithFilterContainer
// ----------------------------------------------------------------------------
//
CLmkLbWithFilterContainer::~CLmkLbWithFilterContainer()
    {
    delete iNaviDeco;
    delete iFindBox;
    delete iBackgroundSkinContext;
    }
// ----------------------------------------------------------------------------
// CLmkLbWithFilterContainer::ListBox()
// ----------------------------------------------------------------------------
//
CEikTextListBox& CLmkLbWithFilterContainer::ListBox()
    {
    __ASSERT_DEBUG( iListBox, Panic( KLmkPanicNullMember ) );
    return *iListBox;
    }

// ----------------------------------------------------------------------------
// CLmkLbWithFilterContainer::OfferKeyEventL
// ----------------------------------------------------------------------------
//
TKeyResponse CLmkLbWithFilterContainer::OfferKeyEventL(
                                     const TKeyEvent& aKeyEvent,
                                     TEventCode aType )
    {
    TKeyResponse result( EKeyWasNotConsumed );
    if ( ( aType == EEventKey || aType == EEventKeyUp) )
        {
        if(( iFindBox->TextLength() > 0 ||
             aKeyEvent.iCode != EKeyBackspace ) && iFindBox->IsVisible())
            {
			result = iFindBox->OfferKeyEventL(aKeyEvent, aType);
            }
        else if( aKeyEvent.iCode == EKeyBackspace )
            {
			CLmkAppSelectorImplBase& selector = SelectorImpl();
			TInt markedCount( selector.ListMarkedItemCountL() );
			
			CEikMenuBar* menuBar = MenuBar();
			if( menuBar )
			    {
                if( MenuBar()->ItemSpecificCommandsEnabled() 
                        && markedCount <= 0 )
                    {
                    return EKeyWasConsumed;
                    }			
			    }

			selector.ProcessCommandL( ELmkCmdDeleteLm );
            return EKeyWasConsumed;
            }
        }
    if ( result == EKeyWasNotConsumed )
        {
	    result = CLmkBaseContainer::OfferKeyEventL( aKeyEvent, aType );
	    if(  aKeyEvent.iScanCode == EStdKeyHash )
	        {
	        UpdateMskContainerL();
	        }
        }
    return result;
    }

// ----------------------------------------------------------------------------
// CLmkLbWithFilterContainer::SizeChanged()
// Called by framework when the view size is changed
// ----------------------------------------------------------------------------
//
void CLmkLbWithFilterContainer::SizeChanged()
    {
    __ASSERT_DEBUG( iListBox, Panic( KLmkPanicNullMember ) );
	const TRect rect(Rect());

    AknFind::HandleFixedFindSizeChanged(
        this,
        static_cast<CAknColumnListBox*>( iListBox ),
        iFindBox );

	if (iListBox && iFindBox && iFindBox->IsVisible())
		{
		AknLayoutUtils::LayoutControl(iListBox, rect,
		AKN_LAYOUT_WINDOW_list_gen_pane(1));
		AknLayoutUtils::LayoutControl(iFindBox, rect,
		AKN_LAYOUT_WINDOW_find_pane);
		if (iFindBox->IsVisible() && iListBox->IsVisible())
			{
			// The correct line position to use is 2, which corresponds
			// EABColumn in Avkon (not a public enumeration,
			// hence hard-coding used here)
			const TInt KSeparatorLinePos = 2;
			iFindBox->SetLinePos(KSeparatorLinePos);
			}
		}
	else if (iListBox)
		{
		AknLayoutUtils::LayoutControl(iListBox, rect,
		AKN_LAYOUT_WINDOW_list_gen_pane(0));
		}
	if (iListBox)
		{
		STATIC_CAST(CAknFilteredTextListBoxModel*,iListBox->Model())->Filter()->SetParentControl(this);
		}
	SelectorImpl().HandleScreenSizeChange();
    }

// ----------------------------------------------------------------------------
// CLmkLbWithFilterContainer::CountComponentControls
// ----------------------------------------------------------------------------
//
TInt CLmkLbWithFilterContainer::CountComponentControls() const
    {
    return 2; // return nbr of controls inside this container
    }

// ----------------------------------------------------------------------------
// CLmkLbWithFilterContainer::ComponentControl
// ----------------------------------------------------------------------------
//
CCoeControl* CLmkLbWithFilterContainer::ComponentControl( TInt aIndex ) const
    {
    switch ( aIndex )
        {
        case 0:
            {
            return iListBox;
            }
        case 1:
            {
            return iFindBox;
            }
        default:
            {
            return NULL;
            }
        }
    }

// ----------------------------------------------------------------------------
// CLmkLbWithFilterContainer::SetupFindBoxL
// ----------------------------------------------------------------------------
//
void CLmkLbWithFilterContainer::SetupFindBoxL()
	{
	iFindBox = SelectorImpl().CreateFindBoxL(*this);
	}


// ---------------------------------------------------------
// CLmkLbWithFilterContainer::Draw(const TRect& aRect) const
// ---------------------------------------------------------
//
void CLmkLbWithFilterContainer::Draw(const TRect& /*aRect*/) const
    {
    if(iIsNeedToReDraw)
        {
        CWindowGc& gc = SystemGc();

        AknsDrawUtils::Background( iSkinInstance,
            iBackgroundSkinContext, this, gc, Rect() );

        iIsNeedToReDraw = EFalse;
        }
    }

#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------
// CLmkLbWithFilterContainer::SetListBoxObserver
// ---------------------------------------------------------
//
void CLmkLbWithFilterContainer::SetListBoxObserver(MEikListBoxObserver* aObserver)
	{
	__ASSERT_DEBUG( aObserver, Panic( KLmkPanicNullMember ) );
	iListBox->SetListBoxObserver(aObserver);
	}
#endif//RD_SCALABLE_UI_V2

// ----------------------------------------------------------------------------
// CLmkLbWithFilterContainer::UpdateMskContainerForFilterL()
// ----------------------------------------------------------------------------
//
void CLmkLbWithFilterContainer::UpdateMskContainerForFilterL()
	{
	iSearchInitiated = ETrue;
	UpdateMskContainerL();
	iSearchInitiated = EFalse;
	}
// End of File
