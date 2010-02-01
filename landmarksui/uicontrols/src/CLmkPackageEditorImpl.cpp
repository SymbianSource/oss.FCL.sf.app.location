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
 * Description:    LandmarksUi Package viewer implementation
 *
 */

// INCLUDE FILES
#include <featmgr.h>
#include <aknnotewrappers.h>
#include <AknWaitDialog.h>
#include <aknlists.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <lmkui.rsg>
#include <eikmenup.h>
#include <hlplch.h>
#include <sendnorm.rsg> // R_SENDUI_MENU
#include <CLmkEditorDlg.h> // from system include!
#include "CLmkParser.h"
#include "landmarks.hrh"
#include "LmkUID.h"
#include <csxhelp/lmmes.hlp.hrh>
#include "CLmkUiUtils.h"
#include "CLmkEditorFieldArray.h"
#include "MLmkEditorField.h"
#include "LmkNotes.h"
#include "LmkConsts.h"
#include "CLmkDbUtils.h"
#include "CLmkLocationService.h"
#include "CLmkDlgCategorySelectorImpl.h"
#include "CLmkSender.h"
#include "CLmkLandMarkCategoriesName.h"
#include "CLmkPackageEditorImpl.h"
#include "CLmkSaveLandmarkUtils.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkPackageEditorImpl::CLmkPackageEditorImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkPackageEditorImpl::CLmkPackageEditorImpl(CPosLandmarkDatabase& aDb,
        CLmkParser& aParser, CLmkSender& aSender, TInt aItemIndex) :
    CLmkEditorImpl(aDb, aSender, CLmkEditorDlg::ELmkViewer), // viewing only mode!
            iParser(aParser), iItemIndex(aItemIndex)
    {
    TCoeHelpContext help(TUid::Uid( KLmkMsgViewerAppUID3),
            KLM_HLP_RECEIVED_LM);
    SetHelpContext(help);
    }

// -----------------------------------------------------------------------------
// CLmkPackageEditorImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkPackageEditorImpl::ConstructL()
    {
    iLandmark = iParser.LandmarkLC(iItemIndex);
    iCategoryNameArray = iParser.CategoryNameLC(iItemIndex);
    iCategoryNameArray->GetCategoryNames(iCategoryNames);
    iCalegorySelectionFlag = ETrue;
    CleanupStack::Pop(2); //iLandmark ,iCategoryNames
    TLmkEditorAttributes arguments(CLmkEditorDlg::ELmkAll);
    iIsLmkReceiveMode = ETrue;
    CLmkEditorImpl::ConstructL(iLandmark, arguments);
    }

// -----------------------------------------------------------------------------
// CLmkPackageEditorImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkPackageEditorImpl* CLmkPackageEditorImpl::NewL(
        CPosLandmarkDatabase& aDb, CLmkParser& aParser, CLmkSender& aSender,
        TInt aItemIndex)
    {
    CLmkPackageEditorImpl* self = new (ELeave) CLmkPackageEditorImpl(aDb,
            aParser, aSender, aItemIndex);

    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); //self
    return self;
    }

// -----------------------------------------------------------------------------
// CLmkPackageEditorImpl::~CLmkPackageEditorImpl()
// -----------------------------------------------------------------------------
//
CLmkPackageEditorImpl::~CLmkPackageEditorImpl()
    {
    if (iWaitNote)
        {
        TRAPD(err, iWaitNote->ProcessFinishedL());
        if (err != KErrNone)
            {
            delete iWaitNote;
            iWaitNote = NULL;
            }
        }
    delete iLandmark;
    delete iCategoryNameArray;
    }

// -----------------------------------------------------------------------------
// CLmkPackageEditorImpl::HandleOperationL
// -----------------------------------------------------------------------------
//
void CLmkPackageEditorImpl::HandleOperationL(TOperationTypes aType,
        TReal32 /*aProgress*/, TInt aStatus)
    {
    if (aType == EImport)
        {
        if (aStatus != KPosLmOperationNotComplete && iWaitNote)
            {
            iWaitNote->ProcessFinishedL();
            iWaitNote = NULL;
            }
        }
    }

// -----------------------------------------------------------------------------
// CLmkPackageEditorImpl::DynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CLmkPackageEditorImpl::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    switch( aResourceId )
        {
        case R_AVKON_FORM_MENUPANE:
            {
            CAknForm::DynInitMenuPaneL( aResourceId, aMenuPane );

            // delete the show on map & Navigate To options
            aMenuPane->DeleteMenuItem( ELmkShowOnMapPlaceHolder );
            aMenuPane->DeleteMenuItem( ELmkNavigateToPlaceHolder );
            
			aMenuPane->SetItemDimmed( ELmkCmdSendDummy, ETrue);
            aMenuPane->SetItemDimmed( ELmkCmdSaveLm, ETrue );
            
            if ( FeatureManager::FeatureSupported( KFeatureIdHelp ) &&
                    !iIsHideHelp )
				{
				aMenuPane->SetItemDimmed( EAknCmdHelp, EFalse );
				}
			else
				{
				aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
				}

			DimmMenuItemsL(aMenuPane);

			//always dimmed
            aMenuPane->SetItemDimmed( EAknFormCmdEdit, ETrue );
		    aMenuPane->SetItemDimmed( EAknFormCmdSave, ETrue );
		    aMenuPane->SetItemDimmed( EAknFormCmdLabel, ETrue );
		    aMenuPane->SetItemDimmed( EAknFormCmdAdd, ETrue );
		    aMenuPane->SetItemDimmed( EAknFormCmdDelete, ETrue );
            break;
            }
        case R_SENDUI_MENU: // Dynamically created send ui menu
            {
            if ( FeatureManager::FeatureSupported( KFeatureIdLandmarksConverter ) )
	            {
            	iSender.DisplaySendCascadeMenuL( *aMenuPane );
	            }
            break;
            }
        case R_LMK_EDITOR_MENU:
	        {
            TBool hideMN=EFalse;
            if( iIsHideCoordinate )         
                {
                if(IsLandmarkDataEmptyL(iLandmark))
                    {
                    hideMN = ETrue;
                    }
                }       
            else if( ArePositionFieldEmptyL() )
                {
                hideMN = ETrue;
                }
            
            if( hideMN )
                {
                // delete the show on map & Navigate To options
                aMenuPane->DeleteMenuItem( ELmkShowOnMapPlaceHolder );
                aMenuPane->DeleteMenuItem( ELmkNavigateToPlaceHolder );
                }
            iMapNavInterface->AttachMenuPaneL(aMenuPane, R_LMK_EDITOR_MENU, ELmkCmdMnNav);

            // Send menu is handled by the sender:
            if ( FeatureManager::FeatureSupported( KFeatureIdLandmarksConverter ) )
				{
				aMenuPane->SetItemDimmed(ELmkCmdSendDummy, EFalse);

                // Use default "Send" item text from SendUI
				iSender.DisplaySendMenuL( *aMenuPane, 1);
				}
	       	DimmMenuItemsL(aMenuPane);
	        break;
	        }
        default:
            {
            break;
            }
        }
    iMapNavInterface->HandleMenuOperationL(aResourceId, aMenuPane, ELmkCmdMnNav);
    }

// -----------------------------------------------------------------------------
// CLmkPackageEditorImpl::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CLmkPackageEditorImpl::ProcessCommandL(TInt aCommandId)
    {
    CAknDialog::ProcessCommandL(aCommandId);
    iContextMenuBar->StopDisplayingMenuBar();
    // Override just a couple of CLmkEditorImpl implementations:
    switch (aCommandId)
        {
        case ELmkCmdSaveLm:
            {
            SaveLandmarkCmdL();
            break;
            }
            // fall through all send cmds:
        case ELmkCmdSendVia1:
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
            {
            SendLandmarkCmdL(aCommandId);
            break;
            }
        case EAknSoftkeyContextOptions:
            {
            iContextMenuBar->TryDisplayContextMenuBarL();
            break;
            }
        default:
            {
            CLmkEditorImpl::ProcessCommandL(aCommandId);
            break;
            }
        }
    }

// ----------------------------------------------------
// CLmkPackageEditorImpl::SaveLandmarkCmdL
// ----------------------------------------------------
//
void CLmkPackageEditorImpl::SaveLandmarkCmdL()
    {
    // Only one item is stored but API requires an array:
    RArray<TUint> importIndexes(1);
    CleanupClosePushL(importIndexes);
    User::LeaveIfError(importIndexes.Append(iItemIndex));
    CLmkSaveLandmarkUtils* saveLmUtils = CLmkSaveLandmarkUtils::NewL(iDb,
            iParser);
    CleanupStack::PushL(saveLmUtils);
    RArray<TCategoryName> newCategories;
    CleanupClosePushL(newCategories);
    TCategoryName newCategoryFound;
    TInt noOfNewCategories = saveLmUtils->CheckForNewCategoriesL(
            importIndexes, newCategoryFound, newCategories);

    if (saveLmUtils->CategoryAddConfirmationQueryL(noOfNewCategories,
            newCategoryFound))
        {
        iParser.StartImportingL(importIndexes, ETrue);// Save landmarks with new categories
        }
    else
        {
        iParser.StartImportingL(importIndexes, EFalse); //Save landmarks without new categories
        }
    CleanupStack::PopAndDestroy(); // newCategories
    // wait note
    iWaitNote = new (ELeave) CAknWaitDialog(NULL, ETrue);
    if (!iWaitNote->ExecuteLD(R_LMK_IMPORTING_WAIT_NOTE))
        {
        //making iWaitNote null, since framework destroys waitnote
        //dialog on pressing cancel.
        iWaitNote = NULL;
        iParser.CancelImporting();
        CleanupStack::PopAndDestroy(2);// saveLmUtils, importIndexes
        return;
        }
    // Saved confirmation note
    saveLmUtils->LmsSavedConfirmationNoteL(importIndexes, iItemIndex);
    CleanupStack::PopAndDestroy(2);// saveLmUtils, importIndexes
    }

// ----------------------------------------------------
// CLmkPackageEditorImpl::SendLandmarkCmdL
// ----------------------------------------------------
//
void CLmkPackageEditorImpl::SendLandmarkCmdL(TInt aCommandId)
    {
    CLmkLandmark* lmk = iParser.LmkLandmarkLC(iItemIndex);
    iSender.SendSingleLandmarkL(aCommandId, *lmk);
    CleanupStack::PopAndDestroy();//lmk
    }

// ----------------------------------------------------
// CLmkPackageEditorImpl::UpdateMskOnArrowMoveForViewerL
// ----------------------------------------------------
//
void CLmkPackageEditorImpl::UpdateMskOnArrowMoveForViewerL()
    {
    MLmkEditorField* field = iEditorFieldArray->Find(IdOfFocusControl());
    TUint type = field->LandmarkItemField().FieldType();
    switch (type)
        {
        case ECategories:
            {
            CEikButtonGroupContainer* cba =
                    CEikButtonGroupContainer::Current();
            cba->RemoveCommandFromStack(KMskCommandPos,
                    EAknSoftkeyContextOptions);
            AddMskCommandL(R_LM_MSK_SHOW, ELmkCmdShow);
            break;
            }
        default:
            {
            EnableMskMenuL();
            break;
            }
        }
    }

// ----------------------------------------------------
// CLmkPackageEditorImpl::EnableMskMenuL
// ----------------------------------------------------
//
void CLmkPackageEditorImpl::EnableMskMenuL()
    {
    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    cba->RemoveCommandFromStack(KMskCommandPos, ELmkCmdShow);
    cba->SetCommandSetL(
            R_LMK_RECEIVED_VIEWER_SOFTKEYS_OPTIONS_BACK_CONTEXTOPTIONS);
    cba->MakeCommandVisible(EAknSoftkeyContextOptions, ETrue);
    iContextMenuBar->SetContextMenuTitleResourceId(
            R_LMK_MSGVIEWER_CONTEXT_MENUBAR);
    }

// ----------------------------------------------------
// CLmkPackageEditorImpl::PostLayoutDynInitL
// ----------------------------------------------------
//
void CLmkPackageEditorImpl::PostLayoutDynInitL()
    {
    CLmkEditorImpl::PostLayoutDynInitL();
    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    EnableMskMenuL();
    }
// ----------------------------------------------------
// CLmkPackageEditorImpl::DimmMenuItems
// ----------------------------------------------------
//
void CLmkPackageEditorImpl::DimmMenuItemsL(CEikMenuPane* aMenuPane)
    {
    //not shown in package editor mode:
    aMenuPane->SetItemDimmed(ELmkCmdCategories, ETrue);
    aMenuPane->SetItemDimmed(ELmkCmdDeleteLm, ETrue);
    aMenuPane->SetItemDimmed(ELmkCmdSelectCategories, ETrue);
    aMenuPane->SetItemDimmed(ELmkCmdCurrentLocation, ETrue);
    aMenuPane->SetItemDimmed(ELmkCmdChangeIcon, ETrue);
    aMenuPane->SetItemDimmed(ELmkCmdEditTextLm, ETrue);
    //additional commands always dimmed in this class:
    aMenuPane->SetItemDimmed(ELmkCmdEditLm, ETrue);
    aMenuPane->SetItemDimmed(ELmkCmdCall, ETrue);
    aMenuPane->SetItemDimmed(ELmkCmdGoToUrl, ETrue);
    aMenuPane->SetItemDimmed(ELmkCmdClearPosInfo, ETrue);
    }

#ifdef RD_SCALABLE_UI_V2
// -----------------------------------------------------------------------------
// CLmkPackageEditorImpl::HandleDialogPageEventL
// -----------------------------------------------------------------------------
//
void CLmkPackageEditorImpl::HandleDialogPageEventL(TInt /*aEventID*/)
    {
    // No Impl
    }

// -----------------------------------------------------------------------------
// CLmkPackageEditorImpl::HandlePointerEventL
// -----------------------------------------------------------------------------
//
void CLmkPackageEditorImpl::HandlePointerEventL( const TPointerEvent& aPointerEvent )
    { 
    CAknForm::HandlePointerEventL(aPointerEvent);
    
    if (aPointerEvent.iType == TPointerEvent::EButton1Up && iIsDragging
            == EFalse)
        {
        iContextMenuBar->TryDisplayContextMenuBarL();
        }    
    if (aPointerEvent.iType == TPointerEvent::EButton1Down)
        {
        iIsDragging = EFalse;
        }
    if (aPointerEvent.iType == TPointerEvent::EDrag)
        {
        iIsDragging = ETrue;
        }
    }
#endif//RD_SCALABLE_UI_V2
//  End of File
