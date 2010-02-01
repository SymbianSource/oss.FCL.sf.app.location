/*
 * Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:    Provides Landmarksui main view class methods.
 *
 */

// INCLUDE FILES
#include <EPos_CPosLandmarkDatabase.h>
#include <aknViewAppUi.h>
#include <Landmarks.rsg>
#include <sendnorm.rsg> // R_SENDUI_MENU
#include <eiktxlbx.h>
#include <eikmenup.h>
#include <featmgr.h>
#include "landmarks.hrh"
#include "CLmkByLmContainer.h"
#include "CLmkAppSelectorImplBase.h"
#include "MLmkListMemento.h"
#include "CLmkAppUi.h"
#include "CLmkByLmView.h"
#include "CLmkSender.h"
#include "Debug.h"
#include "landmarks.hrh"
#include "clmkmapnavigationinterface.h"
#include "CLmkDocument.h"
#include <StringLoader.h>

// ================= LOCAL FUNCTIONS =======================
static void CleanupArray(TAny* aArray)
    {
    (static_cast<RPointerArray<CPosLandmark>*> (aArray))->ResetAndDestroy();
    }

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CLmkByLmView::NewLC
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CLmkByLmView* CLmkByLmView::NewLC(MLmkKeyProcessor& aKeyProcessor,
        CLmkSender& aLmkSender)
    {
    CLmkByLmView* self = new (ELeave) CLmkByLmView(aKeyProcessor, aLmkSender);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ----------------------------------------------------------------------------
// CLmkByLmView::CLmkByLmView
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CLmkByLmView::CLmkByLmView(MLmkKeyProcessor& aKeyProcessor,
        CLmkSender& aLmkSender) :
    CLmkBaseView(aKeyProcessor), iLmkSender(aLmkSender)
    {
    }

// ----------------------------------------------------------------------------
// CLmkByLmView::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CLmkByLmView::ConstructL()
    {
    CLmkAppUi* lmkAppUi = static_cast<CLmkAppUi*> (AppUi());
    if (iEikonEnv->StartedAsServerApp())
        {
        BaseConstructL( R_LMK_BYLM_EMBEDDED_VIEW);
        isEmbeddedLaunch = ETrue;
        }
    else
        {
        BaseConstructL( R_LMK_BYLM_VIEW);
        }
    CPosLandmarkDatabase& db = lmkAppUi->Document().LmDbL();
    iMapNavInterface = CLmkMapNavigationInterface::NewL(db);
    AttachAIWMenuInterestL();
    }

// ----------------------------------------------------------------------------
// CLmkByLmView::~CLmkByLmView
// ----------------------------------------------------------------------------
//
CLmkByLmView::~CLmkByLmView()
    {
    if (iContainer)
        {
        AppUi()->RemoveFromViewStack(*this, iContainer);
        }
    delete iContainer;
    delete iListMemento;
    }

// ----------------------------------------------------------------------------
// CLmkByLmView::ProcessKeyEventL
// ----------------------------------------------------------------------------
//
TBool CLmkByLmView::ProcessKeyEventL(const TKeyEvent& aKeyEvent,
        TEventCode aType)
    {
    return CLmkBaseView::ProcessKeyEventL(aKeyEvent, aType);
    }

// ----------------------------------------------------------------------------
// TUid CLmkByLmView::Id
// ----------------------------------------------------------------------------
//
TUid CLmkByLmView::Id() const
    {
    return TUid::Uid(ELmkByLmView);
    }

// ----------------------------------------------------------------------------
// CLmkByLmView::AttachAIWMenuInterestL
// ----------------------------------------------------------------------------
//
void CLmkByLmView::AttachAIWMenuInterestL()
    {
    iMapNavInterface->AttachAIWInterestL(R_LMK_NEW_LANDMARK_SUBMENU,
            R_LMK_AIW_INTEREST_SELECTFROMMAP);
    iMapNavInterface->AttachAIWInterestL(R_LMK_BYLM_MENU1,
            R_LMK_AIW_INTEREST_SHOWONMAP);
    iMapNavInterface->AttachAIWInterestL(R_LMK_BYLM_MENU1,
            R_LMK_AIW_INTEREST_NAVIGATETO);
    }

// ----------------------------------------------------------------------------
// CLmkByLmView::HandleCommandL
// ----------------------------------------------------------------------------
//
void CLmkByLmView::HandleCommandL(TInt aCommand)
    {
    DEBUG1( CLmkByLmView::HandleCommandL entered with aCommand=%d, aCommand);

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
        EnableMskLabelL( ETrue);
        return;
        }
    switch (aCommand)
        {
        case ELmkCmdNewLmCurrentLocation:
        case ELmkCmdNewLmEmpty:
        case ELmkCmdEditLm:
        case ELmkCmdDeleteLm:
        case ELmkCmdChangeIcon:
        case ELmkCmdAddToCat:
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
            // Set pointer event handling EFalse, to handle the multiplt tap ,
            // Second tap becomes a pointer event for container, which has to be ignored
            // fix for bug ECYU-7C24HQ
            iContainer->SetHandlePointerEventBool(EFalse);

            TInt markedCount(
                    iContainer->SelectorImpl().ListMarkedItemCountL());
            DEBUG1( CLmkByLmView::HandleCommandL markedcount=%d,markedCount );

            if (markedCount > 0 && aCommand == ELmkCmdOpenLm
                    && !iContainer->IsEditorOpened())
                {
                DEBUG( CLmkByLmView::HandleCommandL ELmkCmdOpenLm showing c menu);
                if (MenuBar())
                    {
                    MenuBar()->SetContextMenuTitleResourceId(R_LMK_OK_MENUBAR);
                    MenuBar()->TryDisplayContextMenuBarL();
                    }
                }
            else if (aCommand == ELmkCmdOpenLm
                    && !iContainer->IsEditorOpened())
                {
                DEBUG( CLmkByLmView::HandleCommandL ELmkCmdOpenLm );
                iContainer->SelectorImpl().ProcessCommandL(aCommand);
                iContainer->SetEditorOpenedBool(ETrue);
                }
            else
                {
                DEBUG( CLmkByLmView::HandleCommandL ProcessCommandL aCommand );
                iContainer->SelectorImpl().ProcessCommandL(aCommand);
                }
            // Set pointer event handling ETrue	
            iContainer->SetHandlePointerEventBool(ETrue);
            break;
            }
        default:
            {
            DEBUG( CLmkByLmView::HandleCommandL default );
            (static_cast<CLmkAppUi*> (AppUi()))->HandleCommandL(aCommand);
            break;
            }
        }
DEBUG    ( CLmkByLmView::HandleCommandL End );
    }

// ---------------------------------------------------------
// CLmkByLmView::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CLmkByLmView::DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane)
    {
    DEBUG( CLmkByLmView::DynInitMenuPaneL entered );
    iMapNavInterface->HandleMenuOperationL(aResourceId, aMenuPane,
            ELmkCmdMnNav);

    AknSelectionService::HandleMarkableListDynInitMenuPane(aResourceId,
            aMenuPane, &(iContainer->ListBox()));

    CLmkAppSelectorImplBase& selector = iContainer->SelectorImpl();
    TInt markedCount(selector.ListMarkedItemCountL());
    TInt visibleCount(selector.ListVisibleItemCount());
    DEBUG1( CLmkByLmView::DynInitMenuPaneL markedCount=%d,markedCount );
    DEBUG1( CLmkByLmView::DynInitMenuPaneL visibleCount=%d,visibleCount );

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

                aMenuPane->SetItemDimmed(navigateToCmd, EFalse);
                aMenuPane->SetItemSpecific(navigateToCmd, ETrue);
                }
            break;
            }
        case R_LMK_BYLM_MENU2:
            {
            //always dimmed
            aMenuPane->SetItemDimmed(ELmkCmdChangeIcon, ETrue);
            aMenuPane->SetItemDimmed(ERemoveFromCat, ETrue);
            break;
            }
        case R_LMK_OK_MENU:
            {
            DEBUG( CLmkByLmView::DynInitMenuPaneL R_LMK_OK_MENU );
            // Let send ui add the send menu if MTMs are available:                             
            if (FeatureManager::FeatureSupported(KFeatureIdLandmarksConverter))
                {
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
DEBUG    ( CLmkByLmView::DynInitMenuPaneL end );
    }

// ----------------------------------------------------------------------------
// CLmkByLmView::DoActivateL
// ----------------------------------------------------------------------------
//
void CLmkByLmView::DoActivateL(const TVwsViewId& /*aPrevViewId*/,
        TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
    {
    DEBUG( CLmkByLmView::DoActivateL entered );
    if (!iContainer)
        {
        iContainer
                = CLmkByLmContainer::NewL(*this, this, ClientRect(), *this);
        CLmkAppSelectorImplBase& selector = iContainer->SelectorImpl();
        AppUi()->AddToStackL(*this, iContainer);

#ifdef RD_SCALABLE_UI_V2
        //for touch event
        iContainer->SetListBoxObserver(this);
#endif//RD_SCALABLE_UI_V2
        // Attach CAll UI base service Interest
        selector.AttachInterestL(R_LMK_APP_AIW_INTEREST);
        if (!iListMemento)
            { // Memento not created yet, create it now:
            iListMemento = selector.MementoL();
            }
        selector.SetMemento(*iListMemento);
        selector.RestoreMemento();
        EnableMskMenuL( EFalse);
        EnableMskLabelL(EFalse);
        }
DEBUG    ( CLmkByLmView::DoActivateL End );
    }

// ----------------------------------------------------------------------------
// CLmkByLmView::DoDeactivate
// ----------------------------------------------------------------------------
//
void CLmkByLmView::DoDeactivate()
    {
    if (iContainer)
        {
        iContainer->SelectorImpl().StoreMemento();
        AppUi()->RemoveFromViewStack(*this, iContainer);
        }
    TRAP_IGNORE( EnableMskMenuL(EFalse));
    TRAP_IGNORE( EnableMskLabelL(EFalse));
    delete iContainer;
    iContainer = NULL;
    }

// ----------------------------------------------------------------------------
// CLmkByLmView::Update()
// ----------------------------------------------------------------------------
//
void CLmkByLmView::Update()
    {
    iContainer->SetRect(ClientRect());
    }

// ---------------------------------------------------------
// CLmkBaseView::EnableMskMenuL()
// ---------------------------------------------------------
//
void CLmkByLmView::EnableMskMenuL(TBool aEnable)
    {
    CEikButtonGroupContainer* cbaGrp = Cba();
    if (cbaGrp)
        {
        if (aEnable)
            {
            cbaGrp->RemoveCommandFromStack(KMskCommandPos, ELmkCmdOpenLm);
            TInt
                    cbaRes =
                            isEmbeddedLaunch
                                             ? R_LMK_SOFTKEYS_OPTIONS_BACK_CONTEXTOPTIONS
                                                : R_LMK_SOFTKEYS_OPTIONS_EXIT_CONTEXTOPTIONS;
            cbaGrp->SetCommandSetL(cbaRes);
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
// CLmkBaseView::EnableMskLabelL()
// ---------------------------------------------------------
//
void CLmkByLmView::EnableMskLabelL(TBool aEnable)
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
// CLmkByLmView::HandleListBoxEventL()
// ---------------------------------------------------------
//
void CLmkByLmView::HandleListBoxEventL(CEikListBox* /*aListBox*/,
        TListBoxEvent aEventType)
    {
    TLmkMskDispItem dispItem = iContainer->ResolveMskDisplayItem();

    switch (aEventType)
        {
        //case EEventItemDoubleClicked:
        case EEventEmptyListClicked:
        case EEventItemSingleClicked:
            {
            if (dispItem == EContextMenu)
                {
                UpdateMskViewL(dispItem);
                CEikMenuBar* menubar =
                static_cast<CAknViewAppUi*> (iAvkonAppUi)->View(
                        TUid::Uid(ELmkByLmView))->MenuBar();
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
        default:
        break;
        }
    }
#endif//RD_SCALABLE_UI_V2
// ---------------------------------------------------------
// CLmkByLmView::HandleAIWserviceCommandsL()
// ---------------------------------------------------------
//
TBool CLmkByLmView::HandleAIWserviceCommandsL(TInt aServiceCommand,
        TInt aMenuCommand)
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
            iMapNavInterface->SetObserver(&iContainer->SelectorImpl());
            iMapNavInterface->GetLandmarkFromMapL(aMenuCommand);
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
