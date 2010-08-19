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
 * Description:    Provides LM main view class methods.
 *
 */

// INCLUDE FILES

#include <EPos_CPosLandmarkDatabase.h>
#include <aknViewAppUi.h>
#include <landmarks.rsg>
#include <eikmenup.h>
#include <featmgr.h>
#include <StringLoader.h>
#include <lmkerrors.h>  // for landmarksui panic codes
#include "CLmkDocument.h"
#include "CLmkAppUi.h"
#include "CLmkByCategoryView.h"
#include "CLmkByCategoryContainer.h"
#include "CLmkAppSelectorImplBase.h"
#include "landmarks.hrh"
#include "MLmkListMemento.h"
#include "Debug.h"
#include "LmkConsts.h"
#include "clmkmapnavigationinterface.h"

#if defined(_DEBUG)
// CONSTANTS
/// Unnamed namespace for local definitions
namespace
    {
    _LIT( KPanicMsg, "CLmkByCategoryView" );

    void Panic(TPanicCode aReason)
        {
        User::Panic(KPanicMsg, aReason);
        }
    } // namespace
#endif

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CLmkByCategoryView::NewLC( MLmkKeyProcessor& aKeyProcessor )
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CLmkByCategoryView* CLmkByCategoryView::NewLC(MLmkKeyProcessor& aKeyProcessor)
    {
    CLmkByCategoryView* self = new (ELeave) CLmkByCategoryView(aKeyProcessor);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
// ----------------------------------------------------------------------------
// CLmkByCategoryView::CLmkByCategoryView
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CLmkByCategoryView::CLmkByCategoryView(MLmkKeyProcessor& aKeyProcessor) :
    CLmkBaseView(aKeyProcessor)
    {
    }

// ----------------------------------------------------------------------------
// CLmkByCategoryView::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CLmkByCategoryView::ConstructL()
    {
    if (iEikonEnv->StartedAsServerApp())
        {
        BaseConstructL(R_LMK_BYCATEGORY_EMBEDDED_VIEW);
        isEmbeddedLaunch = ETrue;
        }
    else
        {
        BaseConstructL(R_LMK_BYCATEGORY_VIEW);
        }

    CLmkAppUi* lmkAppUi = static_cast<CLmkAppUi*> (AppUi());
    CPosLandmarkDatabase& db = lmkAppUi->Document().LmDbL();
    iMapNavInterface = CLmkMapNavigationInterface::NewL(db);
    AttachAIWMenuInterestL();
    }

// ----------------------------------------------------------------------------
// CLmkByCategoryView::~CLmkByCategoryView
// ----------------------------------------------------------------------------
//
CLmkByCategoryView::~CLmkByCategoryView()
    {
    if (iContainer)
        {
        AppUi()->RemoveFromViewStack(*this, iContainer);
        }
    delete iContainer;
    delete iListMemento;
    }

// ----------------------------------------------------------------------------
// CLmkByCategoryView::AttachAIWMenuInterestL
// ----------------------------------------------------------------------------
//
void CLmkByCategoryView::AttachAIWMenuInterestL()
    {
    iMapNavInterface->AttachAIWInterestL(R_LMK_NEW_LANDMARK_SUBMENU,
            R_LMK_AIW_INTEREST_SELECTFROMMAP);
    iMapNavInterface->AttachAIWInterestL(R_LMK_BYCATEGORY_MENU,
            R_LMK_AIW_INTEREST_SHOWONMAP);
    }

// ----------------------------------------------------------------------------
// CLmkByCategoryView::BorrowMemento
// ----------------------------------------------------------------------------
//
const MLmkListMemento& CLmkByCategoryView::BorrowMemento()
    {
    // This method gets never called before memento is created:
    __ASSERT_DEBUG( iListMemento, Panic( KLmkPanicIllegalMementoAccess ) );
    return *iListMemento;
    }

// ----------------------------------------------------------------------------
// CLmkByCategoryView::ProcessKeyEventL
// ----------------------------------------------------------------------------
//
TBool CLmkByCategoryView::ProcessKeyEventL(const TKeyEvent& aKeyEvent,
        TEventCode aType)
    {
    return CLmkBaseView::ProcessKeyEventL(aKeyEvent, aType);
    }

// ----------------------------------------------------------------------------
// TUid CLmkByCategoryView::Id
// ----------------------------------------------------------------------------
//
TUid CLmkByCategoryView::Id() const
    {
    return TUid::Uid(ELmkByCategoryView);
    }

// ----------------------------------------------------------------------------
// CLmkByCategoryView::HandleCommandL
// ----------------------------------------------------------------------------
//
void CLmkByCategoryView::HandleCommandL(TInt aCommand)
    {
    TInt aiwCmd = iMapNavInterface->GetServiceCmdByMenuCmd(aCommand);

    // Handles Map and Navigation commands
    if (KAiwCmdNone != aiwCmd)
        {
        switch (aiwCmd)
            {
            case KAiwCmdMnShowMap:
                {
                TInt catId = iContainer->GetCurrentCategoryId();
                iMapNavInterface->ShowCategoryOnMapL(catId, aCommand,
                        CLmkMapNavigationInterface::EByCategoryView);
                break;
                }
            case KAiwCmdMnSelectFromMap:
                {
                iMapNavInterface->SetObserver(&iContainer->SelectorImpl());
                iMapNavInterface->GetLandmarkFromMapL(aCommand);
                break;
                }
            }
        return;
        }
    switch (aCommand)
        {
        case ELmkCmdNewLmCurrentLocation:
        case ELmkCmdNewLmEmpty:
        case ELmkCmdLaunchPositionSettings:
            {
            iContainer->SelectorImpl().ProcessCommandL(aCommand);
            break;
            }
        case ELmkCmdOpenCat:
            {
            // Store current status before activating category contents view:
            iContainer->SelectorImpl().StoreMemento();
            AppUi()->ActivateLocalViewL(TUid::Uid(ELmkCategoryContentsView));
            break;
            }
        default:
            {
            AppUi()->HandleCommandL(aCommand);
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CLmkByCategoryView::DynInitMenuPaneL
// ----------------------------------------------------------------------------
//
void CLmkByCategoryView::DynInitMenuPaneL(TInt aResourceId,
        CEikMenuPane* aMenuPane)
    {
    TInt count(iContainer->SelectorImpl().ListVisibleItemCount());
    iMapNavInterface->HandleMenuOperationL(aResourceId, aMenuPane,
            ELmkCmdMnNav);
    switch (aResourceId)
        {
        case R_LMK_BYCATEGORY_MENU:
            {
            if (count == 0)
                {
                aMenuPane->SetItemDimmed(ELmkCmdOpenCat, ETrue);
                TInt count = aMenuPane->NumberOfItemsInPane();
                // Dim show on map when no categories
                for (TInt i = 0; i < count; ++i)
                    {
                    CEikMenuPaneItem::SData& itemData =
                            aMenuPane->ItemDataByIndexL(i);
                    if (iMapNavInterface->GetServiceCmdByMenuCmd(
                            itemData.iCommandId) == KAiwCmdMnShowMap)
                        {
                        aMenuPane->SetItemDimmed(itemData.iCommandId, ETrue);
                        }
                    }
                }
            else
                {
                TInt cnt = aMenuPane->NumberOfItemsInPane();
                for (TInt i = 0; i < cnt; ++i)
                    {
                    CEikMenuPaneItem::SData& itemData =
                            aMenuPane->ItemDataByIndexL(i);
                    if (iMapNavInterface->GetServiceCmdByMenuCmd(
                            itemData.iCommandId) == KAiwCmdMnShowMap)
                        {
                        aMenuPane->SetItemDimmed(itemData.iCommandId, EFalse);
                        aMenuPane->SetItemSpecific(itemData.iCommandId, ETrue);
                        }
                    }
                }
            break;
            }
        case R_LMK_MAIN_MENU:
            {
            CLmkBaseView::HandleHelpFeature(aMenuPane);
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
    }

// ----------------------------------------------------------------------------
// CLmkByCategoryView::DoActivateL
// ----------------------------------------------------------------------------
//
void CLmkByCategoryView::DoActivateL(const TVwsViewId& /*aPrevViewId*/,
        TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
    {
    DEBUG( CLmkByCategoryView::DoActivateL entered );
    TRAP_IGNORE(EnableMskMenuL(EFalse));
    TRAP_IGNORE(EnableMskLabelL(EFalse));
    if (!iContainer)
        {
        iContainer = CLmkByCategoryContainer::NewL(*this, this, ClientRect(),
                *this);

        CLmkAppSelectorImplBase& selector = iContainer->SelectorImpl();
        AppUi()->AddToStackL(*this, iContainer);
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
        EnableMskLabelL(EFalse);
        }
    }

// ----------------------------------------------------------------------------
// CLmkByCategoryView::DoDeactivate
// ----------------------------------------------------------------------------
//
void CLmkByCategoryView::DoDeactivate()
    {
    if (iContainer)
        {
        iContainer->SelectorImpl().StoreMemento();
        AppUi()->RemoveFromViewStack(*this, iContainer);
        }
    TRAP_IGNORE( EnableMskMenuL( EFalse ) );
    TRAP_IGNORE( EnableMskLabelL( EFalse ) );
    delete iContainer;
    iContainer = NULL;
    }

// ---------------------------------------------------------
// CLmkByCategoryView::Update()
// ---------------------------------------------------------
//
void CLmkByCategoryView::Update()
    {
    iContainer->SetRect(ClientRect());
    }

// ---------------------------------------------------------
// CLmkByCategoryView::EnableMskMenuL()
// ---------------------------------------------------------
//
void CLmkByCategoryView::EnableMskMenuL(TBool aEnable)
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
            }
        //This context menu will always be dimmed.
        cbaGrp->MakeCommandVisible(EAknSoftkeyContextOptions, EFalse);
        }
    }

// ---------------------------------------------------------
// CLmkByCategoryView::EnableMskLabelL()
// ---------------------------------------------------------
//
void CLmkByCategoryView::EnableMskLabelL(TBool aEnable)
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
            cbaGrp->AddCommandToStackL(KMskCommandPos, ELmkCmdOpenCat,
                    mskOpenTxt->Des());
            cbaGrp->MakeCommandVisible(ELmkCmdOpenCat, ETrue);
            CleanupStack::PopAndDestroy(mskOpenTxt);
            }
        else
            {
            cbaGrp->MakeCommandVisible(ELmkCmdOpenCat, EFalse);
            }
        }
    }

#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------
// CLmkByCategoryView::HandleListBoxEventL()
// ---------------------------------------------------------
//
void CLmkByCategoryView::HandleListBoxEventL(CEikListBox* /*aListBox*/,
        TListBoxEvent aEventType)
    {
    switch (aEventType)
        {
        // Add enter key handling
        case EEventEnterKeyPressed:
            {
            // Store current status before activating category contents view:
            if (iContainer->SelectorImpl().ListVisibleItemCount() > 0)
                {
                iContainer->SelectorImpl().StoreMemento();
                static_cast<CAknViewAppUi*> (iAvkonAppUi)-> ActivateLocalViewL(
                        TUid::Uid(ELmkCategoryContentsView));
                }
            break;
            }
        //case EEventItemDoubleClicked:
        case EEventItemSingleClicked:
            {
            HandleCommandL(ELmkCmdOpenCat);
            break;
            }
        default:
            break;
        }
    }
#endif//RD_SCALABLE_UI_V2

// End of File
