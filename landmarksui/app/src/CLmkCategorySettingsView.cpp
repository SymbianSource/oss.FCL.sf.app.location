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
#include <eikmenup.h>
#include <eiktxlbx.h>
#include <Landmarks.rsg>
#include "landmarks.hrh"
#include "CLmkAppUi.h"
#include "CLmkAppCategorySettingsImpl.h"
#include "CLmkCategorySettingsContainer.h"
#include "CLmkCategorySettingsView.h"
#include "CLmkCategoryContentsView.h"

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CLmkCategorySettingsView::NewLC( MLmkKeyProcessor& aKeyProcessor )
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CLmkCategorySettingsView* CLmkCategorySettingsView::NewLC(
        MLmkKeyProcessor& aKeyProcessor)
    {
    CLmkCategorySettingsView* self = new (ELeave) CLmkCategorySettingsView(
            aKeyProcessor);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
// ----------------------------------------------------------------------------
// CLmkCategorySettingsView::CLmkCategorySettingsView
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CLmkCategorySettingsView::CLmkCategorySettingsView(
        MLmkKeyProcessor& aKeyProcessor) :
    CLmkBaseView(aKeyProcessor)
    {
    }

// ----------------------------------------------------------------------------
// CLmkCategorySettingsView::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CLmkCategorySettingsView::ConstructL()
    {
    BaseConstructL(R_LMK_CATEGORY_SETTINGS_VIEW);
    }

// ----------------------------------------------------------------------------
// CLmkCategorySettingsView::~CLmkCategorySettingsView
// ----------------------------------------------------------------------------
//
CLmkCategorySettingsView::~CLmkCategorySettingsView()
    {
    if (iContainer)
        {
        AppUi()->RemoveFromViewStack(*this, iContainer);
        }
    delete iContainer;
    }

// ----------------------------------------------------------------------------
// CLmkCategorySettingsView::ProcessKeyEventL
// ----------------------------------------------------------------------------
//
TBool CLmkCategorySettingsView::ProcessKeyEventL(const TKeyEvent& aKeyEvent,
        TEventCode aType)
    {
    return CLmkBaseView::ProcessKeyEventL(aKeyEvent, aType);
    }

// ----------------------------------------------------------------------------
// TUid CLmkCategorySettingsView::Id
// ----------------------------------------------------------------------------
//
TUid CLmkCategorySettingsView::Id() const
    {
    return TUid::Uid(ELmkCategorySettingsView);
    }

// ----------------------------------------------------------------------------
// CLmkCategorySettingsView::HandleCommandL
// ----------------------------------------------------------------------------
//
void CLmkCategorySettingsView::HandleCommandL(TInt aCommand)
    {
    AknSelectionService::HandleMarkableListProcessCommandL(aCommand,
            &(iContainer->ListBox()));

    switch (aCommand)
        {
        case EAknSoftkeyBack:
            {
            if (iPrevViewId == ELmkCategoryContentsView)
                {
                CLmkCategoryContentsView
                        * view =
                                static_cast<CLmkCategoryContentsView*> (AppUi()->View(
                                        TUid::Uid(iPrevViewId)));
                if (view->IsContainerPtrValid())
                    {
                    AppUi()->ActivateLocalViewL(TUid::Uid(iPrevViewId));
                    }
                else
                    {
                    AppUi()->ActivateLocalViewL(TUid::Uid(ELmkByCategoryView));
                    }
                }
            else
                {
                AppUi()->ActivateLocalViewL(TUid::Uid(iPrevViewId));
                }
            break;
            }
        case ELmkCmdNewCategory:
        case ELmkCmdChangeIcon:
        case ELmkCmdRenameCat:
        case ELmkCmdDeleteCat:
            {
            iContainer->SelectorImpl().ProcessCommandL(aCommand);
            break;
            }
        default:
            {
            (static_cast<CLmkAppUi*> (AppUi()))->HandleCommandL(aCommand);
            break;
            }
        }
    }

// ---------------------------------------------------------
// CLmkCategorySettingsView::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CLmkCategorySettingsView::DynInitMenuPaneL(TInt aResourceId,
        CEikMenuPane* aMenuPane)
    {
    CEikTextListBox& listbox = iContainer->ListBox();
    AknSelectionService::HandleMarkableListDynInitMenuPane(aResourceId,
            aMenuPane, &listbox);

    TInt count(listbox.Model()->NumberOfItems());
    TInt markedCount = listbox.SelectionIndexes()->Count();
    CLmkAppCategorySettingsImpl& LCategorySettingsImpl =
            iContainer->SelectorImpl();
    // Checkif the category is a global category only when some items present
    TBool retVal = EFalse;

    if (count > 0)
        {
        retVal = LCategorySettingsImpl.IsPredefinedCategoryL();
        }

    switch (aResourceId)
        {
        case R_LMK_CATEGORY_SETTINGS_MENU1:
            {
            if (retVal || count == 0)
                {
                aMenuPane->SetItemDimmed(ELmkCmdRenameCat, ETrue);
                aMenuPane->SetItemDimmed(ELmkCmdDeleteCat, ETrue);
                }
            else
                {
                if (markedCount > 0)
                    {
                    aMenuPane->SetItemDimmed(ELmkCmdRenameCat, ETrue);
                    }
                }
            break;
            }
        case R_LMK_CATEGORY_SETTINGS_MENU2:
            {
            if (count == 0)
                {
                aMenuPane->SetItemDimmed(ELmkCmdChangeIcon, ETrue);
                }
            break;
            }
        case R_LMK_CATEGORY_SETTINGS_OK_MENU:
            {
            aMenuPane->SetItemDimmed(ELmkCmdNewCategory, ETrue);
            if (retVal)
                {
                aMenuPane->SetItemDimmed(ELmkCmdRenameCat, ETrue);
                aMenuPane->SetItemDimmed(ELmkCmdDeleteCat, ETrue);
                }
            if (count == 0)
                {
                aMenuPane->SetItemDimmed(ELmkCmdRenameCat, ETrue);
                aMenuPane->SetItemDimmed(ELmkCmdDeleteCat, ETrue);
                aMenuPane->SetItemDimmed(ELmkCmdChangeIcon, ETrue);
                }
            else
                {
                if (markedCount > 1)
                    {
                    aMenuPane->SetItemDimmed(ELmkCmdRenameCat, ETrue);
                    }
                }
            break;
            }
        case R_LMK_MAIN_MENU:
            {
            CLmkBaseView::HandleHelpFeature(aMenuPane);
            break;
            }
        default:
            {
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CLmkCategorySettingsView::DoActivateL
// ----------------------------------------------------------------------------
//
void CLmkCategorySettingsView::DoActivateL(const TVwsViewId& aPrevViewId,
        TUid /*aCustomMessageId*/, const TDesC8& /*aCustomMessage*/)
    {
    iPrevViewId = aPrevViewId.iViewUid.iUid;

#ifdef _DEBUG
    RDebug::Print(_L( "CLmkCategorySettingsView::DoActivateL() called" ));
#endif

    if (!iContainer)
        {
        iContainer = CLmkCategorySettingsContainer::NewL(*this, this,
                ClientRect(), *this);

        AppUi()->AddToStackL(*this, iContainer);
#ifdef RD_SCALABLE_UI_V2
        //for touch event
        iContainer->ListBox().SetListBoxObserver(this);
#endif//RD_SCALABLE_UI_V2
        }
    }

// ----------------------------------------------------------------------------
// CLmkCategorySettingsView::DoDeactivate
// ----------------------------------------------------------------------------
//
void CLmkCategorySettingsView::DoDeactivate()
    {
    if (iContainer)
        {
        AppUi()->RemoveFromViewStack(*this, iContainer);
        }
    delete iContainer;
    iContainer = NULL;
    }
// ----------------------------------------------------------------------------
// CLmkCategorySettingsView::Update()
// ----------------------------------------------------------------------------
//
void CLmkCategorySettingsView::Update()
    {
    iContainer->SetRect(ClientRect());
    }

// ---------------------------------------------------------
// CLmkBaseView::EnableMskMenuL()
// ---------------------------------------------------------
//
void CLmkCategorySettingsView::EnableMskMenuL(TBool aEnable)
    {
    CEikButtonGroupContainer* cbaGrp = Cba();
    if (aEnable)
        {
        cbaGrp->SetCommandSetL(R_LMK_SOFTKEYS_OPTIONS_BACK_CONTEXTOPTIONS);
        //cbaGrp->MakeCommandVisible(EAknSoftkeyContextOptions,ETrue);
        cbaGrp->DrawDeferred();
        MenuBar()->SetContextMenuTitleResourceId(
                R_LMK_CATEGORY_SETTINGS_CONTEXT_MENUBAR);
        }
    else
        {
        cbaGrp->MakeCommandVisible(EAknSoftkeyContextOptions, EFalse);
        }
    }

#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------
// CLmkCategorySettingsView::HandleListBoxEventL()
// ---------------------------------------------------------
//
void CLmkCategorySettingsView::HandleListBoxEventL(CEikListBox* aListBox,
        TListBoxEvent aEventType)
    {
    switch (aEventType)
        {
        //case EEventItemDoubleClicked:
        case EEventItemSingleClicked:
            {
            if( aListBox->SelectionIndexes()->Count() > 0 )
                {
                CEikMenuBar* menubar = MenuBar();
                if (menubar)
                    {
                    menubar->SetContextMenuTitleResourceId(R_LMK_CATEGORY_SETTINGS_OK_MENUBAR);
                    TRAP_IGNORE( menubar->TryDisplayContextMenuBarL() );
                    }            
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
            break;
            }
        default:
            break;
        }
    }
#endif//RD_SCALABLE_UI_V2
// End of File
