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
 * Description:    Provides Categorized Landmarks view class methods.
 *
 */

// INCLUDE FILES
#include <EPos_CPosLandmarkDatabase.h>
#include "CLmkDocument.h"
#include <eiktxlbx.h>
#include <eikmenup.h>
#include <aknViewAppUi.h>
#include <featmgr.h>
#include <sendnorm.rsg> // R_SENDUI_MENU
#include <Landmarks.rsg>
#include <EPos_CPosLandmark.h>
#include <lbsposition.h>
#include "CLmkCategoryContentsView.h"
#include "landmarks.hrh"
#include "CLmkCategoryContentsContainer.h"
#include "CLmkAppSelectorImplBase.h"
#include "MLmkListMemento.h"
#include "CLmkAppUi.h"
#include "CLmkSender.h"
#include "CLmkLmItemListMemento.h"
#include "CLmkByCategoryView.h"
#include "clmkmapnavigationinterface.h"
#include <StringLoader.h>
#include "Debug.h"

// ================= LOCAL FUNCTIONS =======================
static void CleanupArray(TAny* aArray)
    {
    (static_cast<RPointerArray<CPosLandmark>*> (aArray))->ResetAndDestroy();
    }

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CLmkCategoryContentsView::NewLC
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CLmkCategoryContentsView* CLmkCategoryContentsView::NewLC(
        MLmkKeyProcessor& aKeyProcessor, CLmkSender& aLmkSender)
    {
    CLmkCategoryContentsView* self = new (ELeave) CLmkCategoryContentsView(
            aKeyProcessor, aLmkSender);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsView::CLmkCategoryContentsView
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CLmkCategoryContentsView::CLmkCategoryContentsView(
        MLmkKeyProcessor& aKeyProcessor, CLmkSender& aLmkSender) :
    CLmkBaseView(aKeyProcessor), iLmkSender(aLmkSender)
    {
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsView::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CLmkCategoryContentsView::ConstructL()
    {
    BaseConstructL(R_LMK_CATEGORY_CONTENTS_VIEW);
    CLmkAppUi* lmkAppUi = static_cast<CLmkAppUi*> (AppUi());
    CPosLandmarkDatabase& db = lmkAppUi->Document().LmDbL();
    iMapNavInterface = CLmkMapNavigationInterface::NewL(db);
    AttachAIWMenuInterestL();
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsView::~CLmkCategoryContentsView
// ----------------------------------------------------------------------------
//
CLmkCategoryContentsView::~CLmkCategoryContentsView()
    {
    if (iContainer)
        {
        AppUi()->RemoveFromViewStack(*this, iContainer);
        }

    delete iContainer;
    delete iListMemento;
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsView::AttachAIWMenuInterestL
// ----------------------------------------------------------------------------
//
void CLmkCategoryContentsView::AttachAIWMenuInterestL()
    {
    iMapNavInterface->AttachAIWInterestL(R_LMK_NEW_LANDMARK_SUBMENU,
            R_LMK_AIW_INTEREST_SELECTFROMMAP);
    iMapNavInterface->AttachAIWInterestL(R_LMK_BYLM_MENU1,
            R_LMK_AIW_INTEREST_SHOWONMAP);
    iMapNavInterface->AttachAIWInterestL(R_LMK_BYLM_MENU1,
            R_LMK_AIW_INTEREST_NAVIGATETO);
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsView::ProcessKeyEventL
// ----------------------------------------------------------------------------
//
TBool CLmkCategoryContentsView::ProcessKeyEventL(const TKeyEvent& aKeyEvent,
        TEventCode aType)
    {
    return CLmkBaseView::ProcessKeyEventL(aKeyEvent, aType);
    }

// ----------------------------------------------------------------------------
// TUid CLmkCategoryContentsView::Id
// ----------------------------------------------------------------------------
//
TUid CLmkCategoryContentsView::Id() const
    {
    return TUid::Uid(ELmkCategoryContentsView);
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsView::HandleCommandL
// ----------------------------------------------------------------------------
//
void CLmkCategoryContentsView::HandleCommandL(TInt aCommand)
    {
    DEBUG1( CLmkCategoryContentsView::HandleCommandL entered with aCommand=%d, aCommand);

    AknSelectionService::HandleMarkableListProcessCommandL(aCommand,
            &(iContainer->ListBox()));
    CLmkAppSelectorImplBase& selector = iContainer->SelectorImpl();
    //first give a chance to M&N framework.
    TInt aiwCmd = iMapNavInterface->GetServiceCmdByMenuCmd(aCommand);
    if (KAiwCmdNone == aiwCmd)
        {
        aiwCmd = selector.ServiceCmdByMenuCmd(aCommand);
        }
    //Handle AIW command
    if (HandleAIWserviceCommandsL(aiwCmd, aCommand))
        {
        iContainer->ListBox().ClearSelection();
        EnableMskLabelL(ETrue);
        return;
        }

    switch (aCommand)
        {
        case ELmkCmdNewLmCurrentLocation:
        case ELmkCmdNewLmEmpty:
        case ELmkCmdEditLm:
        case ELmkCmdDeleteLm:
        case ELmkCmdAddToCat:
        case ELmkCmdChangeIcon:
        case ELmkCmdSendVia1: // These commands are used by SendUi
        case ELmkCmdSendVia2:
        case ELmkCmdSendVia3:
        case ELmkCmdSendVia4:
        case ELmkCmdSendVia5:
        case ELmkCmdSendVia6:
        case ELmkCmdSendVia7:
        case ELmkCmdSendVia8:
        case ELmkCmdSendVia9:
        case ELmkCmdSendVia10:
        case ELmkCmdSend:
        case ELmkCmdOpenLm:
        case ELmkCmdGoToUrl:
        case ELmkCmdCall:
        case EAknCmdMark:
        case EAknCmdUnmark:
        case EAknMarkAll:
        case EAknUnmarkAll:
        case ELmkCmdLaunchPositionSettings:
            {
            TInt markedCount(
                    iContainer->SelectorImpl().ListMarkedItemCountL());
            DEBUG1( CLmkCategoryContentsView::HandleCommandL markedcount=%d,markedCount );

            if (markedCount > 0 && aCommand == ELmkCmdOpenLm
                    && !iContainer->IsEditorOpened())
                {
                DEBUG( CLmkCategoryContentsView::HandleCommandL ELmkCmdOpenLm showing c menu);
                if (MenuBar())
                    {
                    MenuBar()->SetContextMenuTitleResourceId(R_LMK_OK_MENUBAR);
                    MenuBar()->TryDisplayContextMenuBarL();
                    }
                }
            else if (aCommand == ELmkCmdOpenLm
                    && !iContainer->IsEditorOpened())
                {
                DEBUG( CLmkCategoryContentsView::HandleCommandL ELmkCmdOpenLm );
                iContainer->SelectorImpl().ProcessCommandL(aCommand);
                iContainer->SetEditorOpenedBool(ETrue);
                }
            else
                {
                DEBUG( CLmkCategoryContentsView::HandleCommandL ProcessCommandL aCommand );
                iContainer->SelectorImpl().ProcessCommandL(aCommand);
                }

            if (static_cast<CTextListBoxModel*> (iContainer->ListBox().Model())->ItemTextArray()->MdcaCount()
                    == 0)
                {
                DEBUG( CLmkCategoryContentsView::HandleCommandL listbx count = 0 );
                AppUi()->ActivateLocalViewL(TUid::Uid(ELmkByCategoryView));
                }
            break;
            }
        case EAknSoftkeyBack:
            {
            DEBUG( CLmkCategoryContentsView::HandleCommandL Back Button );
            AppUi()->ActivateLocalViewL(TUid::Uid(ELmkByCategoryView));
            break;
            }

        case ERemoveFromCat:
            {
            DEBUG( CLmkCategoryContentsView::HandleCommandL ERemoveFromCat );
            iContainer->RemoveFromThisCategoryL();
            if (static_cast<CTextListBoxModel*> (iContainer->ListBox().Model())->ItemTextArray()->MdcaCount()
                    == 0)
                {
                AppUi()->ActivateLocalViewL(TUid::Uid(ELmkByCategoryView));
                }
            break;
            }
        default:
            {
            DEBUG( CLmkCategoryContentsView::HandleCommandL default );
            (static_cast<CLmkAppUi*> (AppUi()))->HandleCommandL(aCommand);
            break;
            }
        }
    DEBUG( CLmkCategoryContentsView::HandleCommandL End );
    }

// ---------------------------------------------------------
// CLmkCategoryContentsView::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CLmkCategoryContentsView::DynInitMenuPaneL(TInt aResourceId,
        CEikMenuPane* aMenuPane)
    {
    DEBUG( CLmkCategoryContentsView::DynInitMenuPaneL entered );
    iMapNavInterface->HandleMenuOperationL(aResourceId, aMenuPane,
            ELmkCmdMnNav);
    AknSelectionService::HandleMarkableListDynInitMenuPane(aResourceId,
            aMenuPane, &(iContainer->ListBox()));

    CLmkAppSelectorImplBase& selector = iContainer->SelectorImpl();
    TInt markedCount(selector.ListMarkedItemCountL());
    TInt visibleCount(selector.ListVisibleItemCount());
    DEBUG1( CLmkCategoryContentsView::DynInitMenuPaneL markedCount=%d,markedCount );
    DEBUG1( CLmkCategoryContentsView::DynInitMenuPaneL visibleCount=%d,visibleCount );

    switch (aResourceId)
        {
        case R_LMK_APP_CALL_SUBMENU:
            {
            // Initialise the call UI AIW menu via selector
            selector.AttachToAIWMenuL(R_LMK_APP_CALL_SUBMENU,
                    R_LMK_APP_AIW_INTEREST);
            selector.InitializeMenuPaneL(*aMenuPane, aResourceId);
            selector.AttachInterestL(R_LMK_APP_AIW_INTEREST);
            break;
            }
        case R_LMK_BYLM_MENU1:
            {
            aMenuPane->SetItemDimmed(ELmkCmdSendDummy, ETrue);
            aMenuPane->SetItemDimmed(ELmkAppMenuAiwId, ETrue);
            aMenuPane->SetItemDimmed(ELmkCmdGoToUrl, ETrue);

            /*
             If multiple landmarks are selected then we have to dim
             Navigate To option.
             if only one landmark is in focus then we have to dim
             Navigate To & Show On Map options if the particular landmark
             does not either have address or coordinates.
             */

            TBool isLandmarkDataEmpty = EFalse;
            if (visibleCount == 0)
                {
                isLandmarkDataEmpty = ETrue;
                }
            else
                {
                if (markedCount == 1 || markedCount == 0)
                    {
                    // Get the selected landmark and check if it is empty
                    RPointerArray<CPosLandmark> lmkArray;
                    TInt retVal = iContainer->GetSelectedLandmarksL(lmkArray);
                    if (retVal != KErrNotFound)
                        {
                        CleanupStack::PushL(TCleanupItem(CleanupArray,
                                &lmkArray));
                        isLandmarkDataEmpty
                                = iContainer->IsLandmarkDataEmptyL(
                                        lmkArray[0]);
                        CleanupStack::PopAndDestroy();// lmkArray
                        }
                    }
                }

            TInt showOnMapCmd = -1;
            TInt navigateToCmd = -1;

            TInt cnt = aMenuPane->NumberOfItemsInPane();

            for (TInt i = 0; i < cnt; ++i)
                {
                CEikMenuPaneItem::SData& itemData =
                        aMenuPane->ItemDataByIndexL(i);

                if (iMapNavInterface->GetServiceCmdByMenuCmd(
                        itemData.iCommandId) == KAiwCmdMnShowMap)
                    {
                    showOnMapCmd = itemData.iCommandId;
                    }
                if (iMapNavInterface->GetServiceCmdByMenuCmd(
                        itemData.iCommandId) == KAiwCmdMnNavigateTo)
                    {
                    navigateToCmd = itemData.iCommandId;
                    }
                }
            if (showOnMapCmd < 0 && navigateToCmd < 0)
                {
                break;
                }

            if (isLandmarkDataEmpty || markedCount > 1)
                {
                aMenuPane->SetItemDimmed(showOnMapCmd, ETrue);
                aMenuPane->SetItemDimmed(navigateToCmd, ETrue);
                }
            else
                {
                aMenuPane->SetItemDimmed(showOnMapCmd, EFalse);
                aMenuPane->SetItemSpecific(showOnMapCmd, ETrue);
                if (iContainer->IsUncategorizedCategory())
                    {
                    aMenuPane->SetItemDimmed(navigateToCmd, EFalse);
                    aMenuPane->SetItemSpecific(navigateToCmd, ETrue);
                    }
                else
                    aMenuPane->SetItemDimmed(navigateToCmd, ETrue);
                }
            break;
            }
        case R_LMK_BYLM_MENU2:
            {
            aMenuPane->SetItemDimmed(ELmkCmdChangeIcon, ETrue);
            if (iContainer->IsUncategorizedCategory() || visibleCount == 0)
                {
                aMenuPane->SetItemDimmed(ERemoveFromCat, ETrue);
                }
            break;
            }
        case R_LMK_OK_MENU:
            {
            DEBUG( CLmkCategoryContentsView::DynInitMenuPaneL R_LMK_OK_MENU );
            // Let send ui add the send menu if MTMs are available:            
            if (FeatureManager::FeatureSupported(KFeatureIdLandmarksConverter))
                {
                aMenuPane->SetItemDimmed(ELmkCmdSendDummy, EFalse);
                iLmkSender.DisplaySendMenuL(*aMenuPane, visibleCount);
                }
            else
                {
                aMenuPane->SetItemDimmed(ELmkCmdSendDummy, ETrue);
                }

            if (visibleCount == 0)
                {
                aMenuPane->SetItemDimmed(ELmkCmdAddToCat, ETrue);
                aMenuPane->SetItemDimmed(ELmkCmdDeleteLm, ETrue);
                aMenuPane->SetItemDimmed(ELmkCmdNewLm, EFalse);
                }
            break;
            }
        case R_LMK_MAIN_MENU:
            {
            CLmkBaseView::HandleHelpFeature(aMenuPane);
            break;
            }
        case R_SENDUI_MENU: // Dynamically created send ui menu
            {
            // Send menu is handled by the sender:
            if (FeatureManager::FeatureSupported(KFeatureIdLandmarksConverter))
                {
                iLmkSender.DisplaySendCascadeMenuL(*aMenuPane);
                }
            break;
            }
        case R_LMK_NEW_LANDMARK_SUBMENU:
            {
            if (FeatureManager::FeatureSupported(
                    KFeatureIdLocationFrameworkCore))
                {
                aMenuPane->SetItemDimmed(ELmkCmdNewLmCurrentLocation, EFalse);
                }
            else
                {
                aMenuPane->SetItemDimmed(ELmkCmdNewLmCurrentLocation, ETrue);
                }

            if (iIsHideCoordinate)
                {
                aMenuPane->SetItemDimmed(ELmkCmdNewLmEmpty, ETrue);
                }
            else
                {
                aMenuPane->SetItemDimmed(ELmkCmdNewLmEmpty, EFalse);
                }
            break;
            }
        default:
            {
            break;
            }
        }
    DEBUG( CLmkCategoryContentsView::DynInitMenuPaneL end );
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsView::DoActivateL
// ----------------------------------------------------------------------------
//
void CLmkCategoryContentsView::DoActivateL(const TVwsViewId& /*aPrevViewId*/,
        TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
    {
    DEBUG( CLmkCategoryContentsView::DoActivateL entered );
    if (!iContainer)
        {
        // FOr Full landscape support
        TRAPD( err, iContainer = CLmkCategoryContentsContainer::NewL(
                        *this, this, ClientRect(), *this ) );
        if (err != KErrNone)
            { // Cagegory deleted, go back to "by category" view
            AppUi()->ActivateLocalViewL(TUid::Uid(ELmkByCategoryView));
            return;
            }

        CLmkAppSelectorImplBase& selector = iContainer->SelectorImpl();
        AppUi()->AddToStackL(*this, iContainer);
        // Attach CAll UI base service Interest
        selector.AttachInterestL(R_LMK_APP_AIW_INTEREST);

#ifdef RD_SCALABLE_UI_V2
        //for touch event
        iContainer->SetListBoxObserver(this);
#endif//RD_SCALABLE_UI_V2
        if (!iListMemento)
            { // Memento not created yet, create it now:
            iListMemento = selector.MementoL();
            }
        selector.SetMemento(*iListMemento);
        selector.RestoreMemento();
        EnableMskMenuL(EFalse);
        EnableMskLabelL(EFalse);
        }
    DEBUG( CLmkCategoryContentsView::DoActivateL End );
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsView::DoDeactivate
// ----------------------------------------------------------------------------
//
void CLmkCategoryContentsView::DoDeactivate()
    {
    if (iContainer)
        {
        iContainer->SelectorImpl().StoreMemento();
        AppUi()->RemoveFromViewStack(*this, iContainer);
        }
    TRAP_IGNORE(EnableMskMenuL(EFalse));
    TRAP_IGNORE(EnableMskLabelL(EFalse));
    delete iContainer;
    iContainer = NULL;
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsView::Update()
// ----------------------------------------------------------------------------
//
void CLmkCategoryContentsView::Update()
    {
    iContainer->SetRect(ClientRect());
    }

// ----------------------------------------------------------------------------
// CLmkCategoryContentsView::IsContainerPtrValid()
// ----------------------------------------------------------------------------
//
TBool CLmkCategoryContentsView::IsContainerPtrValid()
    {
    TBool lResult = ETrue;
    if (!iContainer)
        {
        // FOr Full landscape support
        TRAPD( err, iContainer = CLmkCategoryContentsContainer::NewL(
                        *this, this, ClientRect(), *this ) );
        if (err != KErrNone)
            { // Cagegory deleted, go back to "by category" view
            lResult = EFalse;
            }
        else
            {
            lResult = ETrue;
            delete iContainer;
            iContainer = NULL;
            }
        }
    return lResult;
    }

// ---------------------------------------------------------
// CLmkCategoryContentsView::EnableMskMenuL()
// ---------------------------------------------------------
//
void CLmkCategoryContentsView::EnableMskMenuL(TBool aEnable)
    {
    CEikButtonGroupContainer* cbaGrp = Cba();
    if (cbaGrp)
        {
        if (aEnable)
            {
            cbaGrp->RemoveCommandFromStack(KMskCommandPos, ELmkCmdOpenLm);
            cbaGrp->SetCommandSetL(R_LMK_SOFTKEYS_OPTIONS_BACK_CONTEXTOPTIONS);
            cbaGrp->MakeCommandVisible(EAknSoftkeyContextOptions, ETrue);
            if (MenuBar())
                MenuBar()->SetContextMenuTitleResourceId(R_LMK_OK_MENUBAR);
            }
        else
            {
            cbaGrp->MakeCommandVisible(EAknSoftkeyContextOptions, EFalse);
            }
        }
    }

// ---------------------------------------------------------
// CLmkCategoryContentsView::EnableMskLabelL()
// ---------------------------------------------------------
//
void CLmkCategoryContentsView::EnableMskLabelL(TBool aEnable)
    {
    CEikButtonGroupContainer* cbaGrp = Cba();
    if (cbaGrp)
        {
        if (aEnable)
            {
            HBufC* mskOpenTxt = NULL;
            mskOpenTxt = StringLoader::LoadLC(R_LMK_MSK_OPEN, iEikonEnv);
            cbaGrp->RemoveCommandFromStack(KMskCommandPos,
                    EAknSoftkeyContextOptions);
            cbaGrp->AddCommandToStackL(KMskCommandPos, ELmkCmdOpenLm,
                    mskOpenTxt->Des());
            cbaGrp->MakeCommandVisible(ELmkCmdOpenLm, ETrue);
            CleanupStack::PopAndDestroy(mskOpenTxt);
            }
        else
            {
            cbaGrp->MakeCommandVisible(ELmkCmdOpenLm, EFalse);
            }
        }
    }

#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------
// CLmkCategoryContentsView::HandleListBoxEventL()
// ---------------------------------------------------------
//
void CLmkCategoryContentsView::HandleListBoxEventL(CEikListBox* /*aListBox*/,
        TListBoxEvent aEventType)
    {
    TLmkMskDispItem dispItem = iContainer->ResolveMskDisplayItem();

    switch (aEventType)
        {
        //case EEventItemDoubleClicked:
        case EEventItemSingleClicked:
            {
            if (dispItem == EContextMenu)
                {
                UpdateMskViewL(dispItem);
                CEikMenuBar
                        * menubar =
                                static_cast<CAknViewAppUi*> (iAvkonAppUi)->View(
                                        TUid::Uid(ELmkCategoryContentsView))->MenuBar();
                if (menubar)
                    {
                    TRAPD( err, menubar->TryDisplayContextMenuBarL() );
                    User::LeaveIfError(err);
                    }
                }
            else if (dispItem == ELabel)
                {
                UpdateMskViewL(dispItem);
                HandleCommandL(ELmkCmdOpenLm);
                }
            break;
            }

            //case EEventItemClicked:
        case EEventPanningStarted:
        case EEventPanningStopped:
        case EEventFlickStarted:
        case EEventFlickStopped:
        case EEventPenDownOnItem:
        case EEventItemDraggingActioned:
            {
            if (dispItem == EContextMenu)
                {
                UpdateMskViewL(dispItem);
                }
            else if (dispItem == ELabel)
                {
                UpdateMskViewL(dispItem);
                }
            break;
            }
        }
    }
#endif//RD_SCALABLE_UI_V2

// ---------------------------------------------------------
// CLmkCategoryContentsView::HandleAIWserviceCommandsL()
// ---------------------------------------------------------
//
TBool CLmkCategoryContentsView::HandleAIWserviceCommandsL(
        TInt aServiceCommand, TInt aMenuCommand)
    {
    TBool result = ETrue;
    RPointerArray<CPosLandmark> lmkArray;
    switch (aServiceCommand)
        {
        case KAiwCmdMnNavigateTo:
            {
            iContainer->GetSelectedLandmarksL(lmkArray);
            CleanupStack::PushL(TCleanupItem(CleanupArray, &lmkArray));
            iMapNavInterface->NavigateToLandmarkL(lmkArray[0], aMenuCommand);
            CleanupStack::PopAndDestroy(); // lmkArray
            break;
            }
        case KAiwCmdMnShowMap:
            {
            TInt retval = iContainer->GetSelectedLandmarksL(lmkArray);
            CleanupStack::PushL(TCleanupItem(CleanupArray, &lmkArray));
            iMapNavInterface->ShowLandmarksOnMapL(lmkArray, aMenuCommand,
                    CLmkMapNavigationInterface::EByLmkView);
            CleanupStack::PopAndDestroy(); // lmkArray
            break;
            }
        case KAiwCmdMnSelectFromMap:
            {
            // category view
            CLmkByCategoryView* categoryView =
                    static_cast<CLmkByCategoryView*> (AppUi()->View(
                            TUid::Uid(ELmkByCategoryView)));
            const CLmkLmItemListMemento
                    & categoryMemento =
                            static_cast<const CLmkLmItemListMemento&> (categoryView->BorrowMemento());
            iMapNavInterface->SetObserver(&iContainer->SelectorImpl());
            TPosLmItemId categoryId = categoryMemento.MemorizedItemId();
            iMapNavInterface->GetLandmarkFromMapForCategoryL(aMenuCommand,
                    categoryId);
            break;
            }
        case KAiwCmdCall:
            {
            // Handle AIW specific service commands
            CLmkAppSelectorImplBase& selector = iContainer->SelectorImpl();
            if (selector.ListVisibleItemCount() > 0)
                {
                selector.ExecuteAIWCallCmdL(aMenuCommand);
                }
            break;
            }
        default:
            {
            result = EFalse;
            break;
            }
        }
    return result;
    }

// End of File
