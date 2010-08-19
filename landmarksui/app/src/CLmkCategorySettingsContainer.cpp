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
 * Description:    Category Settings View (Edit Categories View) Class Implementation
 *
 */

// INCLUDE FILES

#include <aknlists.h>
#include <aknview.h>
// For RD_AVKON_UNBRANCH
#include <bldvariant.hrh>
#include <AknDef.h>
#include <landmarks.rsg>
#include <lmkui.rsg>
#include "landmarks.hrh"
//#include "Landmarks.hlp.hrh"
#include <csxhelp/lm.hlp.hrh>
#include "CLmkAppUi.h"
#include "CLmkDocument.h"
#include "CLmkUiUtils.h"
#include "CLmkAppCategorySettingsImpl.h"
#include "CLmkCategorySettingsView.h"
#include "CLmkCategorySettingsContainer.h"
#include <lmkerrors.h>

#if defined(_DEBUG)
// CONSTANTS
namespace
    {
    _LIT( KPanicText, "CLmkCategorySettingsContainer" );

    void Panic( TPanicCode aReason )
        {
        User::Panic( KPanicText, aReason );
        }
    } // namespace
#endif
// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CLmkCategorySettingsContainer::NewL
// ----------------------------------------------------------------------------
//

// FOr landscape support
CLmkCategorySettingsContainer* CLmkCategorySettingsContainer::NewL(
        MLmkKeyProcessor& aKeyProcessor, MObjectProvider* aParent,
        const TRect& aRect, CAknView& aView)
    {
    CLmkCategorySettingsContainer* self =
            new (ELeave) CLmkCategorySettingsContainer(aKeyProcessor, aView);
    CleanupStack::PushL(self);
    self->SetMopParent(aParent);
    self->ConstructL(aRect);
    CleanupStack::Pop(); // self
    return self;
    }

// ----------------------------------------------------------------------------
// CLmkCategorySettingsContainer::CLmkCategorySettingsContainer
// ----------------------------------------------------------------------------
//
CLmkCategorySettingsContainer::CLmkCategorySettingsContainer(
        MLmkKeyProcessor& aKeyProcessor, CAknView& aView) :
    CLmkBaseContainer(aKeyProcessor, KLM_HLP_EDIT_CATEGORIES), iView(aView),
            iCurrentIndex(0)
    {
    }

// ---------------------------------------------------------
// CLmkCategorySettingsContainer::ConstructL
// Symbian 2nd phase constructor can leave.
// ---------------------------------------------------------
//
void CLmkCategorySettingsContainer::ConstructL(const TRect& aRect)
    {
    CLmkBaseContainer::BaseConstructL();
    CreateWindowL();

    CreateNaviPaneL(0);
    CLmkUiUtils::ChangeTitlePaneL( R_LMK_CATEGORY_SETTINGS_TITLE);

    CLmkAppUi* lmkAppUi = static_cast<CLmkAppUi*> (ViewAppUi());
    CPosLandmarkDatabase& db = lmkAppUi->Document().LmDbL();
    CLmkSender& sender = lmkAppUi->LmkSender();

    iSelector = CLmkAppCategorySettingsImpl::NewL(db, sender, EFalse);
    iSelector->SetMskObserver(this);
    iListBox = &(iSelector->CreateListBoxL(this, EAknCtDoubleGraphicListBox,
#ifdef RD_SCALABLE_UI_V2
            EAknListBoxStylusMarkableList,
#else
            EAknListBoxMarkableList,
#endif //RD_SCALABLE_UI_V2
            R_LMK_EMPTY_NO_CATEGORIES, R_LMK_GUIDE_NEW_CATEGORY));

    // enable scroll arrows
    CEikScrollBarFrame* sBFrame = iListBox->CreateScrollBarFrameL(ETrue);
    sBFrame->SetScrollBarVisibilityL(CEikScrollBarFrame::EOn,
            CEikScrollBarFrame::EAuto);

    SetRect(aRect);
    ActivateL();
    }

// ----------------------------------------------------------------------------
// CLmkCategorySettingsContainer::~CLmkCategorySettingsContainer
// ----------------------------------------------------------------------------
//
CLmkCategorySettingsContainer::~CLmkCategorySettingsContainer()
    {
    }

// ----------------------------------------------------------------------------
// CLmkCategorySettingsContainer::SelectorImpl
// ----------------------------------------------------------------------------
//
CLmkAppCategorySettingsImpl& CLmkCategorySettingsContainer::SelectorImpl()
    {
    return *static_cast<CLmkAppCategorySettingsImpl*> (iSelector);
    }

// ---------------------------------------------------------
// CLmkCategorySettingsContainer::ListBox()
// ---------------------------------------------------------
//
CEikTextListBox& CLmkCategorySettingsContainer::ListBox()
    {
    __ASSERT_DEBUG(iListBox, Panic(KLmkPanicNullMember));
    return *iListBox;
    }

// ---------------------------------------------------------
// CLmkCategorySettingsContainer::OfferKeyEventL()
// ---------------------------------------------------------
//
TKeyResponse CLmkCategorySettingsContainer::OfferKeyEventL(
        const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    if (aType == EEventKey && aKeyEvent.iCode == EKeyBackspace
            && MenuBar()->ItemSpecificCommandsEnabled())
        {
        CLmkAppSelectorImplBase& selector = SelectorImpl();
        TInt markedCount(selector.ListMarkedItemCountL());
        //TInt visibleCount( selector.ListVisibleItemCount() );
        // if there is atleast one landmark and only one or less
        // is market then EKeyOK means edit command
        if (markedCount > 0)
            {
            selector.ProcessCommandL(ELmkCmdDeleteCat);
            return EKeyWasConsumed;
            }
        }
    return CLmkBaseContainer::OfferKeyEventL(aKeyEvent, aType);
    }

// ---------------------------------------------------------
// CLmkCategorySettingsContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CLmkCategorySettingsContainer::SizeChanged()
    {
    __ASSERT_DEBUG(iListBox, Panic(KLmkPanicNullMember));
    iListBox->SetRect(Rect());
    }

// ---------------------------------------------------------
// CLmkCategorySettingsContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CLmkCategorySettingsContainer::CountComponentControls() const
    {
    //only one control allways
    return 1; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CLmkCategorySettingsContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CLmkCategorySettingsContainer::ComponentControl(TInt /*aIndex*/) const
    {
    return iListBox;
    }

// ---------------------------------------------------------
// CLmkCategorySettingsContainer::MenuBar
// ---------------------------------------------------------
//
CEikMenuBar* CLmkCategorySettingsContainer::MenuBar()
    {
    return ViewAppUi()->View(TUid::Uid(ELmkCategorySettingsView))->MenuBar();
    }

// ---------------------------------------------------------
// CLmkCategorySettingsContainer::ContextSpecificMenuBar
// ---------------------------------------------------------
//
TInt CLmkCategorySettingsContainer::ContextSpecificMenuBar()
    {
    return R_LMK_CATEGORY_SETTINGS_OK_MENUBAR;
    }

// ---------------------------------------------------------
// CLmkCategorySettingsContainer::OptionsMenuBar
// ---------------------------------------------------------
//
TInt CLmkCategorySettingsContainer::OptionsMenuBar()
    {
    return R_LMK_CATEGORY_SETTINGS_MENUBAR;
    }

// -----------------------------------------------------------------------------
//CLmkCategorySettingsContainer::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CLmkCategorySettingsContainer::HandleResourceChange(TInt aType)
    {
    CCoeControl::HandleResourceChange(aType);
    if (aType == KEikDynamicLayoutVariantSwitch || (aType
            == KAknsMessageSkinChange))
        {
        (static_cast<CLmkCategorySettingsView *> (&iView))->Update();
        }
    if (iListBox)
        {
        iListBox->DrawNow();
        }
    }

// ----------------------------------------------------------------------------
// CLmkCategorySettingsContainer::FocusChanged()
// ----------------------------------------------------------------------------
//
void CLmkCategorySettingsContainer::FocusChanged(TDrawNow aDrawNow)
    {
    if (iListBox)
        {
        iListBox->SetFocus(IsFocused(), aDrawNow);
        }
    }
// ----------------------------------------------------------------------------
// CLmkCategorySettingsContainer::UpdateMskContainer()
// ----------------------------------------------------------------------------
//
void CLmkCategorySettingsContainer::UpdateMskContainerL()
    {
    CLmkBaseView* view = reinterpret_cast<CLmkBaseView*> (&iView);
    view->UpdateMskViewL(EContextMenu);
    }

// ----------------------------------------------------------------------------
// CLmkCategorySettingsContainer::ResolveMskDisplayItem()
// ----------------------------------------------------------------------------
//
TLmkMskDispItem CLmkCategorySettingsContainer::ResolveMskDisplayItem()
    {
    TLmkMskDispItem dispItem = EContextMenu;
    if (SelectorImpl().ListVisibleItemCount() == 0)
        {
        dispItem = ENoMsk;
        }
    return dispItem;
    }

#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------
// CLmkCategorySettingsContainer::SetListBoxObserver
// ---------------------------------------------------------
//
void CLmkCategorySettingsContainer::SetListBoxObserver(MEikListBoxObserver* aObserver)
    {
    __ASSERT_DEBUG( aObserver, Panic( KLmkPanicNullMember ) );
    iListBox->SetListBoxObserver(aObserver);
    }

// ---------------------------------------------------------
// CLmkCategorySettingsContainer::HandlePoineteventL
// ---------------------------------------------------------
//
void CLmkCategorySettingsContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    CCoeControl::HandlePointerEventL(aPointerEvent);
    //    if(aPointerEvent.iType == TPointerEvent::EButton1Up)
    //        {        
    //        TInt currIndex1 = iListBox->CurrentItemIndex();
    //        if( currIndex1 >=0 )
    //            {
    //            CEikMenuBar* menubar = MenuBar();
    //            if ( menubar )
    //                {
    //                menubar->SetContextMenuTitleResourceId(R_LMK_CATEGORY_SETTINGS_OK_MENUBAR);
    //                TRAP_IGNORE( menubar->TryDisplayContextMenuBarL() );
    //                }   
    //            }
    //        }
    }
#endif//RD_SCALABLE_UI_V2 	
// End of File
