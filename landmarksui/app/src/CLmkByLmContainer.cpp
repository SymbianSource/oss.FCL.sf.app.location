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
 * Description:    Provides LM compass view's container class methods.
 *
 */

// INCLUDE FILES
#include <aknlists.h>
#include <aknview.h>
#include <eikmenub.h>
#include <aknsfld.h>
#include <layoutmetadata.cdl.h> // To Check TOUCH Support
// For RD_AVKON_UNBRANCH
#include <bldvariant.hrh>
#include <AknDef.h>
#include <landmarks.rsg>
#include "CLmkAppUi.h"
#include "CLmkUiUtils.h"
#include "CLmkSender.h"
#include <csxhelp/lm.hlp.hrh>
#include "CLmkByLmView.h"
#include "CLmkByLmContainer.h"
#include "CLmkAppLmSelectorImpl.h"
#include "landmarks.hrh"
#include <lmkui.rsg>
#include "Debug.h"

// ----------- Touch feedback additions start
#ifdef RD_TACTILE_FEEDBACK
#include <touchfeedback.h>
#endif //RD_TACTILE_FEEDBACK
// ----------- Touch feedback additions end

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CLmkByLmContainer::NewL
// ----------------------------------------------------------------------------
//
CLmkByLmContainer* CLmkByLmContainer::NewL(MLmkKeyProcessor& aKeyProcessor,
        MObjectProvider* aParent, const TRect& aRect, CAknView& aView)
    {
    CLmkByLmContainer* self = new (ELeave) CLmkByLmContainer(aKeyProcessor,
            aView);
    CleanupStack::PushL(self);
    self->SetMopParent(aParent);
    self->ConstructL(aRect);
    CleanupStack::Pop(); // self
    return self;
    }

// ----------------------------------------------------------------------------
// CLmkByLmContainer::ConstructL
// ----------------------------------------------------------------------------
//
void CLmkByLmContainer::ConstructL(const TRect& aRect)
    {
    CLmkLbWithFilterContainer::ConstructL(aRect);
    iFindBox->SetObserver(this);
    }

// ----------------------------------------------------------------------------
// CLmkByLmContainer::CLmkByLmContainer
// ----------------------------------------------------------------------------
//
CLmkByLmContainer::CLmkByLmContainer(MLmkKeyProcessor& aKeyProcessor,
        CAknView& aView) :
    CLmkLbWithFilterContainer(aKeyProcessor, KLM_HLP_LM_BY_NAME),
            iView(aView)
    {
    iIsEditorOpened = EFalse;
    iHandlePointerEvent = ETrue;
    }
// ----------------------------------------------------------------------------
// CLmkByLmContainer::~CLmkByLmContainer
// ----------------------------------------------------------------------------
//
CLmkByLmContainer::~CLmkByLmContainer()
    {
    }

// ----------------------------------------------------------------------------
// CLmkByLmContainer::SetupTitlePaneL
// ----------------------------------------------------------------------------
//
void CLmkByLmContainer::SetupTitlePaneL()
    {
    CLmkUiUtils::ChangeTitlePaneL( R_LMK_BYLM_VIEW_TITLE);
    }

// ----------------------------------------------------------------------------
// CLmkByLmContainer::SetupSelectorAndListL
// ----------------------------------------------------------------------------
//
void CLmkByLmContainer::SetupSelectorAndListL(CPosLandmarkDatabase& aDb,
        CLmkSender& aSender)
    {
    iSelector = CLmkAppLmSelectorImpl::NewL(aDb, aSender, ETrue);
    iSelector->SetMskObserver(this);
    iListBox = &(iSelector->CreateListBoxL(this, EAknCtSingleGraphicListBox,
#ifdef RD_SCALABLE_UI_V2
            EAknListBoxStylusMarkableList,
#else
            EAknListBoxMarkableList,
#endif //RD_SCALABLE_UI_V2
            R_LMK_EMPTY_NO_LANDMARKS, R_LMK_GUIDE_NEW_LANDMARK));
    }

// ----------------------------------------------------------------------------
// CLmkByLmContainer::SetupNaviPaneL
// ----------------------------------------------------------------------------
//
void CLmkByLmContainer::SetupNaviPaneL()
    {
    iNaviDeco = CreateNaviPaneL(R_LMK_TAB_GROUP);
    ActivateTabL(*iNaviDeco, ELmkByLmView);
    }

// ----------------------------------------------------------------------------
// CLmkByLmContainer::OfferKeyEventL
// ----------------------------------------------------------------------------
//
TKeyResponse CLmkByLmContainer::OfferKeyEventL(const TKeyEvent& aKeyEvent,
        TEventCode aType)
    {
    DEBUG( CLmkByLmContainer::OfferKeyEventL entered );
    DEBUG2( aKeyEvent.iCode=%d aKeyEvent.iScanCode=%d,
            aKeyEvent.iCode,
            aKeyEvent.iScanCode
    );
    DEBUG3( aKeyEvent.iModifiers=%d aKeyEvent.iRepeats=%d aType=%d,
            aKeyEvent.iModifiers,
            aKeyEvent.iRepeats,
            aType
    );

    TBool shiftKeyPressed = (aKeyEvent.iModifiers & EModifierShift)
            || (aKeyEvent.iModifiers & EModifierLeftShift)
            || (aKeyEvent.iModifiers & EModifierRightShift);

    if (aKeyEvent.iScanCode == EStdKeyUpArrow || aKeyEvent.iScanCode
            == EStdKeyDownArrow)
        {
        if (aType == EEventKey)
            {
            if (iListBox->OfferKeyEventL(aKeyEvent, aType) == EKeyWasConsumed)
                {
                if (shiftKeyPressed)
                    UpdateMskContainerL();
                return EKeyWasConsumed;
                }
            }
        return EKeyWasNotConsumed;
        }

    CLmkAppSelectorImplBase& selector = SelectorImpl();
    TInt markedCount(selector.ListMarkedItemCountL());
    TInt visibleCount(selector.ListVisibleItemCount());
    DEBUG1( CLmkByLmContainer::OfferKeyEventL markedCount=%d,markedCount );
    DEBUG1( CLmkByLmContainer::OfferKeyEventL visibleCount=%d,visibleCount );

    if (aKeyEvent.iCode == EKeyOK && !shiftKeyPressed && aType == EEventKey)
        {
        DEBUG( CLmkByLmContainer::OfferKeyEventL EKeyOK );
        if (visibleCount > 0 && markedCount < 1)
            {
            DEBUG( CLmkByLmContainer::OfferKeyEventL EKeyOK ELmkCmdOpenLm );
            // Launch landmark viewer here
            selector.ProcessCommandL(ELmkCmdOpenLm);
            iIsEditorOpened = ETrue;
            return EKeyWasConsumed;
            }
        else
            {
            DEBUG( CLmkByLmContainer::OfferKeyEventL EKeyOK show menubar );
            CEikMenuBar* menubar =
                    static_cast<CAknViewAppUi*> (iAvkonAppUi)->View(
                            TUid::Uid(ELmkByLmView))->MenuBar();
            if (menubar)
                {
                menubar->SetMenuTitleResourceId(R_LMK_OK_MENUBAR);

                menubar->StopDisplayingMenuBar();
                TRAPD(err, menubar->TryDisplayMenuBarL());
                DEBUG1( CLmkByLmContainer::OfferKeyEventL menubar->TryDisplayMenuBarL() = %d,err );
                // set ordinary menubar back even in case of error
                menubar->SetMenuTitleResourceId(R_LMK_BYLM_MENUBAR);
                User::LeaveIfError( err);
                }

            return EKeyWasConsumed;
            }
        }
    else if (shiftKeyPressed && aType == EEventKeyUp)
        {
        UpdateMskContainerL();
        }
    else
        {
        DEBUG( CLmkByLmContainer::OfferKeyEventL send event key & other keys handling );
        // 'send' eveny key processing        
        if (visibleCount > 0 && markedCount < 1)
            {
            // Check TOUCH Support
            if (aKeyEvent.iCode == EKeyPhoneSend
                    && (!Layout_Meta_Data::IsPenEnabled()))
                {
                selector.ProcessCommandL(ELmkCmdCall);
                }
            }
        if (iIsEditorOpened)
            {
            DEBUG( CLmkByLmContainer::OfferKeyEventL iIsEditorOpened entered );
            SelectorImpl().HandleListProviderEvent(ELmkEventListReady);
            }
        }
    iIsEditorOpened = EFalse;
    DEBUG( CLmkByLmContainer::OfferKeyEventL end );
    return CLmkLbWithFilterContainer::OfferKeyEventL(aKeyEvent, aType);
    }

// ----------------------------------------------------------------------------
// CLmkByLmContainer::HandleControlEventL()
// Called by framework when the view size is changed
// ----------------------------------------------------------------------------
//

void CLmkByLmContainer::HandleControlEventL(CCoeControl* aControl,
        TCoeEvent aEventType)
    {
    if (aEventType == EEventStateChanged && aControl == iFindBox)
        {
        SelectorImpl().ProcessCommandL(ELmkCmdFindBoxSearchAfresh);
        }
    }

// -----------------------------------------------------------------------------
//CLmkByLmContainer::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CLmkByLmContainer::HandleResourceChange(TInt aType)
    {
    CCoeControl::HandleResourceChange(aType);
    if (aType == KEikDynamicLayoutVariantSwitch)
        {
        (static_cast<CLmkByLmView *> (&iView))->Update();
        }

    if (iListBox)
        {
        iListBox->DrawNow();
        }
    if (iFindBox && iFindBox->IsVisible())
        {
        iFindBox->DrawNow();
        }
    }

// ----------------------------------------------------------------------------
// CLmkByLmContainer::GetSelectedLandmarksL
// ----------------------------------------------------------------------------
//
TInt CLmkByLmContainer::GetSelectedLandmarksL(
        RPointerArray<CPosLandmark> &aArray)
    {
    return (static_cast<CLmkAppLmSelectorImpl*> (iSelector))->GetSelectedLandmarksL(
            aArray);
    }

// ----------------------------------------------------------------------------
// CLmkByLmContainer::IsLandmarkEmptyL
// ----------------------------------------------------------------------------
//
TBool CLmkByLmContainer::IsLandmarkDataEmptyL(CPosLandmark* aLandmark)
    {
    return (static_cast<CLmkAppLmSelectorImpl*> (iSelector))->IsLandmarkDataEmptyL(
            aLandmark);
    }

// ----------------------------------------------------------------------------
// CLmkByLmContainer::FocusChanged()
// ----------------------------------------------------------------------------
//
void CLmkByLmContainer::FocusChanged(TDrawNow aDrawNow)
    {
    if (iListBox)
        {
        iListBox->SetFocus(IsFocused(), aDrawNow);
        }
    if (iFindBox && iFindBox->IsVisible())
        {
        iFindBox->SetFocus(IsFocused(), aDrawNow);
        }
    }

// ----------------------------------------------------------------------------
// CLmkByLmContainer::IsEditorOpened()
// ----------------------------------------------------------------------------
//
TBool CLmkByLmContainer::IsEditorOpened()
    {
    return iIsEditorOpened;
    }

// ----------------------------------------------------------------------------
// CLmkByLmContainer::SetEditorOpenedBool()
// ----------------------------------------------------------------------------
//
void CLmkByLmContainer::SetEditorOpenedBool(TBool aBoolvalue)
    {
    iIsEditorOpened = aBoolvalue;
    }

// ----------------------------------------------------------------------------
// CLmkByLmContainer::UpdateMskContainerL()
// ----------------------------------------------------------------------------
//
void CLmkByLmContainer::UpdateMskContainerL()
    {
    CLmkBaseView* view = reinterpret_cast<CLmkBaseView*> (&iView);
    view->UpdateMskViewL(ResolveMskDisplayItem());
    }

// ---------------------------------------------------------
// CLmkByLmContainer::MenuBar
// ---------------------------------------------------------
//
CEikMenuBar* CLmkByLmContainer::MenuBar()
    {
    return ViewAppUi()->View(TUid::Uid(ELmkByLmView))->MenuBar();
    }

// ---------------------------------------------------------
// CLmkByLmContainer::ContextSpecificMenuBar
// ---------------------------------------------------------
//
TInt CLmkByLmContainer::ContextSpecificMenuBar()
    {
    return R_LMK_OK_MENUBAR;
    }

// ---------------------------------------------------------
// CLmkByLmContainer::SetHandlePointerEventBool
// ---------------------------------------------------------
//
void CLmkByLmContainer::SetHandlePointerEventBool(TBool aBoolValue)
    {
    iHandlePointerEvent = aBoolValue;
    }

#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------
// CLmkByLmContainer::HandlePoineteventL
// ---------------------------------------------------------
//
void CLmkByLmContainer::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    {
    CCoeControl::HandlePointerEventL(aPointerEvent);
    }

#endif//RD_SCALABLE_UI_V2
// End of File
