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
 * Description:    Provides LM category contents view's container class methods.
 *
 */

// INCLUDE FILES
#include <aknsfld.h>
#include <aknlists.h>
#include <Landmarks.rsg>
#include <eikmenub.h>
#include <layoutmetadata.cdl.h> // To Check TOUCH Support
#include <bldvariant.hrh>
#include <AknDef.h>
#include <EPos_CPosLandmark.h>
#include "CLmkUiUtils.h"
#include "CLmkAppUi.h"
#include "CLmkSender.h"
#include "landmarks.hrh"
#include <csxhelp/lm.hlp.hrh>
#include "CLmkCategoryContentsView.h"
#include "CLmkCategoryContentsContainer.h"
#include "CLmkAppLmSelectorImpl.h"
#include "CLmkByCategoryView.h"
#include <lmkui.rsg>
#include "Debug.h"

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CLmkCategoryContentsContainer::NewL
// ----------------------------------------------------------------------------
//
CLmkCategoryContentsContainer* CLmkCategoryContentsContainer::NewL(
        MLmkKeyProcessor& aKeyProcessor, MObjectProvider* aParent,
        const TRect& aRect, CAknView& aView)
    {
    CLmkCategoryContentsContainer* self =
            new (ELeave) CLmkCategoryContentsContainer(aKeyProcessor, aView);
    CleanupStack::PushL(self);
    self->SetMopParent(aParent);
    self->ConstructL(aRect);
    CleanupStack::Pop(); // self
    return self;
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsContainer::ConstructL
// ----------------------------------------------------------------------------
//
void CLmkCategoryContentsContainer::ConstructL(const TRect& aRect)
    {
    CLmkLbWithFilterContainer::ConstructL(aRect);
    iFindBox->SetObserver(this);
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsContainer::CLmkCategoryContentsContainer
// ----------------------------------------------------------------------------
//

CLmkCategoryContentsContainer::CLmkCategoryContentsContainer(
        MLmkKeyProcessor& aKeyProcessor, CAknView& aView) :
    CLmkLbWithFilterContainer(aKeyProcessor, KLM_HLP_CATEGORY_CONTENTS),
            iView(aView)
    {
    iIsEditorOpened = EFalse;
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsContainer::~CLmkCategoryContentsContainer
// ----------------------------------------------------------------------------
//
CLmkCategoryContentsContainer::~CLmkCategoryContentsContainer()
    {
    NaviPane().Pop(iNaviDeco); // Pop category name from navi pane
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsContainer::SetupTitlePaneL
// ----------------------------------------------------------------------------
//
void CLmkCategoryContentsContainer::SetupTitlePaneL()
    {
    CLmkUiUtils::ChangeTitlePaneL(R_LMK_BYLM_VIEW_TITLE);
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsContainer::SetupSelectorAndListL
// ----------------------------------------------------------------------------
//
void CLmkCategoryContentsContainer::SetupSelectorAndListL(
        CPosLandmarkDatabase& aDb, CLmkSender& aSender)
    {
    CLmkByCategoryView* categoryView =
            static_cast<CLmkByCategoryView*> (ViewAppUi()->View(TUid::Uid(
                    ELmkByCategoryView)));

    // Create this classes selector based on category selector:
    iSelector = CLmkAppLmSelectorImpl::NewL(aDb,
            categoryView->BorrowMemento(), aSender, ETrue);
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
// CLmkCategoryContentsContainer::SetupNaviPaneL
// ----------------------------------------------------------------------------
//
void CLmkCategoryContentsContainer::SetupNaviPaneL()
    {
    // Show category name in navi pane:
    CAknNavigationControlContainer& naviPane = NaviPane();
    CLmkAppLmSelectorImpl* selector =
            static_cast<CLmkAppLmSelectorImpl*> (iSelector);
    iNaviDeco = naviPane.CreateNavigationLabelL(selector->CategoryNameL());
    naviPane.PushL(*iNaviDeco); // activate navi label in navi pane
    }

// ---------------------------------------------------------
// CLmkCategoryContentsContainer::OfferKeyEventL()
// ---------------------------------------------------------
//
TKeyResponse CLmkCategoryContentsContainer::OfferKeyEventL(
        const TKeyEvent& aKeyEvent, TEventCode aType)
    {
    DEBUG( CLmkCategoryContentsContainer::OfferKeyEventL entered );
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
    DEBUG1( CLmkCategoryContentsContainer::OfferKeyEventL markedCount=%d,markedCount );
    DEBUG1( CLmkCategoryContentsContainer::OfferKeyEventL visibleCount=%d,visibleCount );

    if (aKeyEvent.iCode == EKeyOK && !shiftKeyPressed && aType == EEventKey)
        {
        if (visibleCount > 0 && markedCount < 1)
            {
            // Launch landmark viewer here
            selector.ProcessCommandL(ELmkCmdOpenLm);
            iIsEditorOpened = ETrue;
            return EKeyWasConsumed;
            }
        else
            {
            DEBUG( CLmkCategoryContentsContainer::OfferKeyEventL EKeyOK show menubar );
            CEikMenuBar* menubar =
                    static_cast<CAknViewAppUi*> (iAvkonAppUi)->View(
                            TUid::Uid(ELmkCategoryContentsView))->MenuBar();
            if (menubar)
                {
                menubar->SetMenuTitleResourceId(R_LMK_OK_MENUBAR);

                menubar->StopDisplayingMenuBar();
                TRAPD(err,menubar->TryDisplayMenuBarL());
                DEBUG1( CLmkCategoryContentsContainer::OfferKeyEventL menubar->TryDisplayMenuBarL() = %d,err );
                // set ordinary menubar back even in case of error:                                         
                menubar->SetMenuTitleResourceId(R_LMK_BYLM_MENUBAR);
                User::LeaveIfError(err);
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
        DEBUG( CLmkCategoryContentsContainer::OfferKeyEventL send event key & other keys handling );
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
            DEBUG( CLmkCategoryContentsContainer::OfferKeyEventL iIsEditorOpened entered );
            SelectorImpl().HandleListProviderEvent(ELmkEventListReady);
            }
        }
    iIsEditorOpened = EFalse;
    TKeyResponse response = CLmkLbWithFilterContainer::OfferKeyEventL(
            aKeyEvent, aType);
    
    DEBUG( CLmkCategoryContentsContainer::OfferKeyEventL End );
    return response;
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsContainer::HandleControlEventL()
// Called by framework when the view size is changed
// ----------------------------------------------------------------------------
//
void CLmkCategoryContentsContainer::HandleControlEventL(
        CCoeControl* aControl, TCoeEvent aEventType)
    {
    if (aEventType == EEventStateChanged && aControl == iFindBox)
        {
        SelectorImpl().ProcessCommandL(ELmkCmdFindBoxSearchAfresh);
        }
    if (aEventType == EEventStateChanged && aControl == iListBox )
        {
        if (static_cast<CTextListBoxModel*> (ListBox().Model())->ItemTextArray()->MdcaCount()
                == 0)
            {
            (static_cast<CLmkCategoryContentsView *> (&iView))->HandleCommandL(
                    EAknSoftkeyBack);
            }
        }
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsContainer::GetSelectedLandmarksL
// ----------------------------------------------------------------------------
//
TInt CLmkCategoryContentsContainer::GetSelectedLandmarksL(RPointerArray<
        CPosLandmark> &aArray)
    {
    return (static_cast<CLmkAppLmSelectorImpl*> (iSelector))->GetSelectedLandmarksL(
            aArray);
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsContainer::IsLandmarkDataEmptyL
// ----------------------------------------------------------------------------
//
TBool CLmkCategoryContentsContainer::IsLandmarkDataEmptyL(
        CPosLandmark* aLandmark)
    {
    return (static_cast<CLmkAppLmSelectorImpl*> (iSelector))->IsLandmarkDataEmptyL(
            aLandmark);
    }

// -----------------------------------------------------------------------------
//CLmkCategorySettingsContainer::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CLmkCategoryContentsContainer::HandleResourceChange(TInt aType)
    {
    CCoeControl::HandleResourceChange(aType);
    if (aType == KEikDynamicLayoutVariantSwitch)
        {
        (static_cast<CLmkCategoryContentsView *> (&iView))->Update();
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
// CLmkByLmContainer::FocusChanged()
// ----------------------------------------------------------------------------
//
void CLmkCategoryContentsContainer::FocusChanged(TDrawNow aDrawNow)
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
// CLmkCategoryContentsContainer::IsEditorOpened()
// ----------------------------------------------------------------------------
//
TBool CLmkCategoryContentsContainer::IsEditorOpened()
    {
    return iIsEditorOpened;
    }

// ----------------------------------------------------------------------------
// CLmkByLmContainer::SetEditorOpenedBool()
// ----------------------------------------------------------------------------
//
void CLmkCategoryContentsContainer::SetEditorOpenedBool(TBool aBoolvalue)
    {
    iIsEditorOpened = aBoolvalue;
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsContainer::UpdateMskContainerL()
// ----------------------------------------------------------------------------
//
void CLmkCategoryContentsContainer::UpdateMskContainerL()
    {
    CLmkBaseView* view = reinterpret_cast<CLmkBaseView*> (&iView);
    view->UpdateMskViewL(ResolveMskDisplayItem());
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsContainer::RemoveFromThisCategory()
// ----------------------------------------------------------------------------
//
void CLmkCategoryContentsContainer::RemoveFromThisCategoryL()
    {
    iIsRemoveOperation = ETrue;
    CLmkBaseView* view = reinterpret_cast<CLmkBaseView*> (&iView);
    view->UpdateMskViewL(ENoMsk);
    (static_cast<CLmkAppLmSelectorImpl*> (iSelector))->RemoveLandmarksFromCategoryL();
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsContainer::IsUncategorizedCategory()
// ----------------------------------------------------------------------------
//
TBool CLmkCategoryContentsContainer::IsUncategorizedCategory()
    {
    CLmkAppLmSelectorImpl* selector =
            static_cast<CLmkAppLmSelectorImpl*> (iSelector);
    return selector->IsCriteriaUnCategorizedCat();
    }

// ---------------------------------------------------------
// CLmkCategoryContentsContainer::ResolveMskDisplayItem()
// ---------------------------------------------------------
//
TLmkMskDispItem CLmkCategoryContentsContainer::ResolveMskDisplayItem()
    {
    TLmkMskDispItem dispItem = ELabel;
    if ((!SelectorImpl().ListVisibleItemCount()))
        {
        dispItem = ENoMsk;
        }
    else
        {
        const CArrayFix<TInt>* markedIndexes = NULL;
        markedIndexes = SelectorImpl().MarkedIndexes();
        if (markedIndexes->Count() > 0)
            {
            dispItem = EContextMenu;
            }
        }
    return dispItem;
    }

// ----------------------------------------------------------------------------
// CLmkByLmContainer::UpdateMskContainerForFilterL()
// ----------------------------------------------------------------------------
//
void CLmkCategoryContentsContainer::UpdateMskContainerForFilterL()
    {
    iSearchInitiated = ETrue;
    UpdateMskContainerL();
    iSearchInitiated = EFalse;
    }
// End of File
