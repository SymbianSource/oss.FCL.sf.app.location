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
* Description:   This file contains the methods implementing the core
*                functionality of landmarks editor
*
*/

// INCLUDE FILES
#include <AknSettingCache.h>
#include <AiwServiceHandler.h>      // AIW
#include <AiwCommon.hrh>            // AIW
#include <featmgr.h>
#include <aknlayoutscalable_avkon.cdl.h>
#include <layoutmetadata.cdl.h>
#include <aknmessagequerydialog.h>
#include <AknIconUtils.h>
#include <AknWaitDialog.h>
#include <aknlists.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <eikcapc.h>
#include <eikmenup.h>
#include <hlplch.h>
#include <sendnorm.rsg> // R_SENDUI_MENU
#include <lmkui.rsg>
#include <CLmkEditorDlg.h>
#include <EPos_CPosLandmark.h>
#include <EPos_CPosLandmarkDatabase.h>
#include <epos_poslandmarkserialization.h>
#include <bldvariant.hrh>
#include <AknDef.h>
#include <sysutil.h>

#include <baclipb.h>                        // for CClipboard
#include <txtrich.h>
#include <txtetext.h>
#include <s32std.h>

#include "CLmkUiUtils.h"
#include "landmarks.hrh"
#include "LmkConsts.h"
#include "LmkUID.h"
#include "CLmkLocationService.h"
#include "CLmkDbUtils.h"
#include "CLmkEditorFieldArray.h"
#include "MLmkEditorField.h"
#include "LmkNotes.h"
#include "CLmkDlgCategorySelectorImpl.h"
#include <csxhelp/lm.hlp.hrh>
#include "CLmkSender.h"
#include "clmkgotourlcmd.h"
#include "clmkcallcmd.h"
#include "CLmkEditorImpl.h"
#include "CLmkEditorTextField.h"
#include "lmkicondialog.h"
#include "CLmkEditorListField.h"
#include "CLmkEditorCoordinateField.h"
#include <Landmarks.rsg>
#include <uriutils.h>
#include "LmkListProviderFactory.h"
#include "CLmkListProviderBase.h"
#include <locnotprefplugindomaincrkeys.h>
#include "clmkcentralrepository.h"
#include "CLmkEditorNumberField.h"
#include "lmkwaitdlglauncher.h" // Wait dialog launcher
#include "LmkEditorFieldFactory.h"
#include <locationvariationdomaincrkeys.h>
#include <lmkerrors.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {
 //const TInt KMaxCatCntForInfoPopUp (3);
 const TInt KHOffsetCatInfoPopup ( 100 );
 const TInt KVOffsetCatInfoPopup ( 30 );
 //const TInt KMinCatLenForInfoPopUp ( 11 );
 //const TInt KMaxCatSizeForInfoPopUp ( 135 );
 #if defined(_DEBUG)
_LIT( KPanicMsg, "CLmkEditorImpl");

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
 #endif
}  // namespace

// ================= LOCAL FUNCTIONS =======================
static void CleanupArray(TAny* aArray)
    {
    ( static_cast<RPointerArray<CPosLandmark>*>( aArray ) )->ResetAndDestroy();
    }
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkEditorImpl::CLmkEditorImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkEditorImpl::CLmkEditorImpl(
    CPosLandmarkDatabase& aDb,
    CLmkSender& aSender,
    TLmkEditorMode aEditorMode )
    : iDb( aDb ),
      iSender( aSender ),
      iEditorMode( aEditorMode ),
      iIsEditing(EFalse),
      iResetWebAddrSize(ETrue),
      iIsCategorySelctorOpen(EFalse)
    {
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::ConstructL(
    TPosLmItemId aLandmarkId,
    TLmkEditorAttributes aAttributes )
    {
    CommonConstructL(aAttributes);
    
    iLandmark = iDb.ReadLandmarkLC(aLandmarkId);  
    
    iLmkFields = CLmkFields::NewL( iLabelArray, 
                                   iDb,
                                   aAttributes,
                                   aLandmarkId, 
                                   iLandmark ,
                                   iJapaneseInputMode);  
    ChangeContextImageL();
    iEditorFieldArray = CLmkEditorFieldArray::NewL( *iLmkFields, *this );    
    CleanupStack::Pop(iLandmark);
    
    if(iLandmark)
    	{
    	iIsLmOwned = ETrue;
    	}
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::ConstructL(
    CPosLandmark* aLandmark,
    TLmkEditorAttributes aAttributes )
    {
    CommonConstructL(aAttributes);

    iLmkFields = CLmkFields::NewL( iLabelArray,
                                   iDb,
                                   aAttributes,
                                   aLandmark->LandmarkId(),
                                   aLandmark,
                                   iJapaneseInputMode );
    iLandmark = aLandmark;

    ChangeContextImageL();
    iEditorFieldArray = CLmkEditorFieldArray::NewL( *iLmkFields, *this );
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::ConstructL(
    TPosLmItemId* aLandmarkId,
    CPosLandmark* aLandmark,
    TLmkEditorAttributes aAttributes )
    {
    CommonConstructL(aAttributes);

    iLmkFields = CLmkFields::NewL( iLabelArray,
                                   iDb,
                                   aAttributes,
                                   *aLandmarkId,
                                   aLandmark,
                                   iJapaneseInputMode );
	iLandmarkItemId = aLandmarkId;
	iLandmark = aLandmark;
    ChangeContextImageL();
    iEditorFieldArray = CLmkEditorFieldArray::NewL( *iLmkFields, *this );
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::CommonConstructL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::CommonConstructL(TLmkEditorAttributes& aAttributes)
    {
	iLandmarkItemId = NULL; // Set to NULL by default. Use only where needed.
    iLmkUiUtils = CLmkUiUtils::NewL();
    iLmkUiUtils->ReadLmkUiResourceFileL();
    CAknForm::ConstructL( R_LMK_EDITOR_MENUBAR );

    iLmkUiUtils->StoreNaviPaneL(); // store old navi pane
    iLmkUiUtils->CreateDefaultNaviPaneL();
    // For setting Japanese input mode
    CAknSettingCache& cache = CAknEnv::Static()->SettingCache();
	TLanguage currentLanguage = cache.InputLanguage();
	if ( currentLanguage == ELangJapanese )
    	{
    	iJapaneseInputMode = ETrue;
    	}
    else
        {
        iJapaneseInputMode = EFalse;
        }
    CreateLabelArrayL();
    ConstructContextMenuL();

    // launching browser
    iLmkGoToURLCmd = CLmkGoToURLCmd::NewL();
    // Launching Call UI
    iLmkCallCmd = CLmkCallCmd::NewL();
    iLmkCallCmd->AttachMenuPaneL( R_LMK_EDITOR_CALL_SUBMENU, R_LMK_EDITOR_AIW_INTEREST);
	// Create AIW interest
	iLmkCallCmd->AttachInterestL( R_LMK_EDITOR_AIW_INTEREST );
	// By default enable map and navigation feature
    iMapNavFeature = ETrue;

    iMapNavInterface = CLmkMapNavigationInterface::NewL( iDb);
    iCatInfoPopupNote = NULL;
    iRepository = CLmkCentralRepository::NewL( KLocSystemofMeasurements,this);

    // Create the wait dialog launcher
    iWaitDialogLauncher = CLmkWaitDialogLauncher::NewL();
    iWaitDialogLauncher->SetObserver(this);

    iIsLmOwned = EFalse;
    
    // Check if the Coordinates has to be hidden
	CheckHideCoordinateL();
    
    if( iIsHideCoordinate )
        {
        aAttributes&= ~CLmkEditorDlg::ELmkLatitude;
        aAttributes&= ~CLmkEditorDlg::ELmkLongitude;
        aAttributes&= ~CLmkEditorDlg::ELmkPositionAccuracy;
        aAttributes&= ~CLmkEditorDlg::ELmkAltitude;
        aAttributes&= ~CLmkEditorDlg::ELmkAltitudeAccuracy;
        }
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkEditorImpl* CLmkEditorImpl::NewL(
    CPosLandmarkDatabase& aDb,
    CLmkSender& aSender,
    TLmkEditorAttributes aAttributes,
    TLmkEditorMode aEditorMode,
    TPosLmItemId aLandmarkId )
    {
    CLmkEditorImpl* self =
        new( ELeave ) CLmkEditorImpl( aDb, aSender, aEditorMode );

    CleanupStack::PushL( self );
    self->ConstructL( aLandmarkId, aAttributes );
    CleanupStack::Pop(); //self
    return self;
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkEditorImpl* CLmkEditorImpl::NewL(
    CPosLandmarkDatabase& aDb,
    CLmkSender& aSender,
    TLmkEditorAttributes aAttributes,
    TLmkEditorMode aEditorMode,
    CPosLandmark* aLandmark )
    {
    CLmkEditorImpl* self =
        new( ELeave ) CLmkEditorImpl( aDb, aSender, aEditorMode );

    CleanupStack::PushL( self );
    self->ConstructL( aLandmark, aAttributes );
    CleanupStack::Pop(); //self
    return self;
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkEditorImpl* CLmkEditorImpl::NewL(
    CPosLandmarkDatabase& aDb,
    CLmkSender& aSender,
    TLmkEditorAttributes aAttributes,
    TLmkEditorMode aEditorMode,
    TPosLmItemId* aLandmarkId,
    CPosLandmark* aLandmark )
    {
    CLmkEditorImpl* self =
        new( ELeave ) CLmkEditorImpl( aDb, aSender, aEditorMode );

    CleanupStack::PushL( self );
    self->ConstructL( aLandmarkId, aLandmark, aAttributes );
    CleanupStack::Pop(); //self
    return self;
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::~CLmkEditorImpl()
// -----------------------------------------------------------------------------
//
CLmkEditorImpl::~CLmkEditorImpl()
    {
    delete iWaitDialogLauncher;
		
	CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
	cba->RemoveCommandObserver(KMskCommandPos);
        
    if( iLmkUiUtils )
        {
        iLmkUiUtils->SetOldContextIcon();
        iLmkUiUtils->RestoreOldTitlePane();
        }

    delete iLmkUiUtils;
    delete iLmkFields;
    iLabelArray.ResetAndDestroy();
    iCategoryNames.Close();
    iLabelArray.Close();
    delete iEditorFieldArray;
    delete iLocationService;

    if (iContextMenuBar)
        {
        iEikonEnv->EikAppUi()->RemoveFromStack(iContextMenuBar);
        delete iContextMenuBar;
        }
    delete iLmkGoToURLCmd;
    delete iLmkCallCmd;
    iMapNavInterface->Release();
	delete iCatInfoPopupNote;
	delete iRepository;
	if(iIsLmOwned && iLandmark)
		delete iLandmark;
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::SetMopParent
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkEditorImpl::SetMopParent(
    MObjectProvider* aParent )
    {
    CCoeControl::SetMopParent( aParent );
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::SetHelpContext
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkEditorImpl::SetHelpContext( TCoeHelpContext aContext )
    {
    iHelpContext = aContext;
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::ExecuteL
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CLmkEditorImpl::ExecuteLD()
    {
    CleanupStack::PushL( this );
    AttachAIWMenuInterestL();
    CleanupStack::Pop();//this
    if (iEditorMode == CLmkEditorDlg::ELmkEditor)
		{
		iIsEditing = ETrue;
		return CAknForm::ExecuteLD( R_LMK_EDITOR_DIALOG );
		}
    else // viewer/receive state of editor
	    {
		iIsEditing = EFalse;
		return CAknForm::ExecuteLD( R_LMK_VIEWER_DIALOG );
	    }
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::DisableMapAndNavigationMenuOptions
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkEditorImpl::DisableMapAndNavigationMenuOptions()
	{
	iMapNavFeature = EFalse;
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::DynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    switch( aResourceId )
        {
        case R_AVKON_FORM_MENUPANE:
            {
            CAknForm::DynInitMenuPaneL( aResourceId, aMenuPane );
            TBool isLandmarkDataEmpty = EFalse;
            TBool dimClearPos = ETrue;

            // Disable Map and Navigation feature when opted not to show	
		    if( !iMapNavFeature )
		    	{		    	
		    	isLandmarkDataEmpty = ETrue;
		    	}	
		    else	    
	        	{
	        	isLandmarkDataEmpty = ArePositionFieldEmptyL();
	        	}
	                    
            if( isLandmarkDataEmpty )
            	{
		    	// delete the show on map & Navigate To options
		    	aMenuPane->DeleteMenuItem( ELmkShowOnMapPlaceHolder );
            	aMenuPane->DeleteMenuItem( ELmkNavigateToPlaceHolder );
            	}
            else if( IsEditable() )
            	{
		    	// delete the Navigate To option for editor
            	aMenuPane->DeleteMenuItem( ELmkNavigateToPlaceHolder );
            	}

		    if( IsEditable() )
		    	{
		    	dimClearPos = ArePositionFieldEmptyL();
		    	}
    
            if( dimClearPos || iIsHideCoordinate)
	            {
	            aMenuPane->SetItemDimmed( ELmkCmdClearPosInfo, ETrue );
	            }

            if( iMapNavFeature )
            	{
            	iMapNavInterface->AttachMenuPaneL( aMenuPane, R_LMK_EDITOR_MENU, ELmkCmdMnNav );
            	}

            // Send menu is handled by the sender:
            if ( FeatureManager::FeatureSupported( KFeatureIdLandmarksConverter ) )
				{
				aMenuPane->SetItemDimmed(ELmkCmdSendDummy, EFalse);
				iSender.DisplaySendMenuL( *aMenuPane, 1 );
				}
			else
				{
				aMenuPane->SetItemDimmed(ELmkCmdSendDummy, ETrue);
				}

			if ( FeatureManager::FeatureSupported( KFeatureIdLocationFrameworkCore ))
				{
				aMenuPane->SetItemDimmed( ELmkCmdCurrentLocation,!IsEditable());
				}
			else
				{
				aMenuPane->SetItemDimmed( ELmkCmdCurrentLocation, ETrue );
				}

			if ( FeatureManager::FeatureSupported( KFeatureIdHelp )&&
                        !iIsHideHelp )
				{
				aMenuPane->SetItemDimmed( EAknCmdHelp, EFalse );
				}
			else
				{
				aMenuPane->SetItemDimmed( EAknCmdHelp, ETrue );
				}

            //only in edit mode
            aMenuPane->SetItemDimmed( ELmkCmdSelectCategories, !IsEditable() );
            aMenuPane->SetItemDimmed( ELmkCmdChangeIcon, !IsEditable() );


            //additional commands always dimmed in this class:
            aMenuPane->SetItemDimmed( ELmkCmdSaveLm, ETrue );
            // Only in view mode
            aMenuPane->SetItemDimmed( ELmkCmdEditLm, IsEditable() );
             //allways dimmed
            aMenuPane->SetItemDimmed( EAknFormCmdEdit, ETrue );
		    aMenuPane->SetItemDimmed( EAknFormCmdSave, ETrue );
		    aMenuPane->SetItemDimmed( EAknFormCmdLabel, ETrue );
		    aMenuPane->SetItemDimmed( EAknFormCmdAdd, ETrue );
		    aMenuPane->SetItemDimmed( EAknFormCmdDelete, ETrue );
            aMenuPane->SetItemDimmed( ELmkCmdCategories, ETrue );
			aMenuPane->SetItemDimmed( ELmkCmdEditTextLm, ETrue );

			if(IsEditable())
				{
				aMenuPane->SetItemDimmed( ELmkCmdCall, ETrue );
				aMenuPane->SetItemDimmed( ELmkCmdGoToUrl, ETrue );
				}
			else
				{
				MLmkFieldData* field = iLmkFields->GetField(EPhoneNumber);
				if(field)
					{
	    			TInt id = field->UniqueFieldIdentity();
					MLmkEditorField* control = iEditorFieldArray->Find( id );
					if ((!control) || GetFieldTextLengthL( *control ) <= 0 )
						{
						aMenuPane->SetItemDimmed( ELmkCmdCall, ETrue );
						}
					
					MLmkFieldData* field2 = iLmkFields->GetField(EWebAddress);
					if( field2 )
                        {
                        TInt id2 = field2->UniqueFieldIdentity();
                        control = iEditorFieldArray->Find( id2 );
                        if ( (!control) || GetFieldTextLengthL( *control ) <= 0 )
                            {
                            aMenuPane->SetItemDimmed( ELmkCmdGoToUrl, ETrue );
                            }
                        }
					}
				}
            break;
            }
        case R_SENDUI_MENU: // Dynamically created send ui menu
            {
            // Send menu is handled by the sender:
            if ( FeatureManager::FeatureSupported( KFeatureIdLandmarksConverter ) )
				{
				iSender.DisplaySendCascadeMenuL( *aMenuPane );
				}
            break;
            }
		case R_LMK_EDITOR_CONTEXT_MENU:
			{
			MLmkEditorField* field =
			iEditorFieldArray->Find( IdOfFocusControl() );
			TUint type = field->LandmarkItemField().FieldType();

			if ( type != EPhoneNumber )
				{
				aMenuPane->SetItemDimmed( ELmkEditMenuAiwId, ETrue );
				}
			if ( type != EWebAddress )
				{
				aMenuPane->SetItemDimmed( ELmkCmdGoToUrl, ETrue );
				}

			if ( type == EPhoneNumber )
				{
				if ( GetFieldTextLengthL( *field ) <= 0 )
					{
					aMenuPane->SetItemDimmed( ELmkEditMenuAiwId, ETrue );
					}
				}
			if ( type == EWebAddress )
				{
				if ( GetFieldTextLengthL( *field ) <= 0 )
					{
					aMenuPane->SetItemDimmed( ELmkCmdGoToUrl, ETrue );
					}
				}
			break;
			}
        case R_LMK_EDITOR_CALL_SUBMENU:
        	{
        	// Initialize the AIW menu service
        	iLmkCallCmd->InitializeMenuPaneL(
                 *aMenuPane,
                 aResourceId,
                 ELmkCmdLast // Must not overlap with the other menu ids!
                 );
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
// CLmkEditorImpl::IsLandmarkDataEmptyL
// -----------------------------------------------------------------------------
//
TBool CLmkEditorImpl::IsLandmarkDataEmptyL(CPosLandmark* aLandmark)
    {
    // verify that destination landmark has coordinates
    TLocality loc;
    TBool isValid = (KErrNone == aLandmark->GetPosition(loc));
    isValid &= !Math::IsNaN(loc.Latitude()) && !Math::IsNaN(loc.Longitude());
    return isValid;
    
/*    if (!isValid)
        {
        // verify that destination landmark has address info
        TPositionFieldId fieldId = aLandmark->FirstPositionFieldId();
        while (fieldId != EPositionFieldNone)
            {
            if (fieldId > EPositionFieldAddressCapabilitiesBegin && fieldId
                    < EPositionFieldBuildingTelephone)
                {
                TPtrC field;
                aLandmark->GetPositionField(fieldId, field);
                if (field.Length())
                    {
                    isValid = ETrue;
                    break;
                    }
                }
            fieldId = aLandmark->NextPositionFieldId(fieldId);
            }
        }
    return !isValid;*/
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::ProcessCommandL( TInt aCommandId )
    {
    // Hide the context sensitive menu here
    HideMenu();
    iContextMenuBar->StopDisplayingMenuBar();

    CAknDialog::ProcessCommandL( aCommandId );
    TInt aiwCmd = iMapNavInterface->GetServiceCmdByMenuCmd(aCommandId);

    // Handles Map and Navigation commands
    if(KAiwCmdNone != aiwCmd)
    	{
    	// Landmark array to pass to M&N interface
    	RPointerArray<CPosLandmark> lmkArray;
    	switch(aiwCmd)
    		{
    		case KAiwCmdMnShowMap:
    			{
    			CPosLandmark* landmark = GetCurrentEditedLandmarkL();
    		    lmkArray.Append(landmark);
			    CleanupStack::PushL( TCleanupItem( CleanupArray, &lmkArray ) );
				iMapNavInterface->ShowLandmarksOnMapL(lmkArray, aCommandId,
				 									 CLmkMapNavigationInterface::EByLmkView);
				CleanupStack::PopAndDestroy(); //lmkArray
    			break;
    			}
    		case KAiwCmdMnNavigateTo:
    			{
    			CPosLandmark* landmark = GetCurrentEditedLandmarkL();
				lmkArray.Append(landmark);
	    		CleanupStack::PushL( TCleanupItem( CleanupArray, &lmkArray ) );
	    		iMapNavInterface->NavigateToLandmarkL(lmkArray[0],aCommandId);
	    		CleanupStack::PopAndDestroy(); //lmkArray
    			break;
    			}
    		}
    	lmkArray.Reset();
    	return;
    	}
    switch ( aCommandId )
        {
        case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( iEikonEnv->WsSession(),
                static_cast<CAknAppUi*>(
                    iCoeEnv->AppUi() )->AppHelpContextL() );
            break;
            }
        // these all are same: save & exit
        case EAknSoftkeyBack:
        case EAknSoftkeyExit:
        case EEikCmdExit:
        case EAknCmdExit:
            {
            CAknForm::OkToExitL( aCommandId );
            iLmkUiUtils->RestoreOldNaviPaneL();
            CEikAppUi* appUi = iEikonEnv->EikAppUi();
            // Close this dialog first
            delete this;
            // Exit application
            static_cast<MEikCommandObserver*>(appUi)->ProcessCommandL(aCommandId);
            break;
            }
        case ELmkCmdDeleteLm:
            {
            if ( LmkNotes::LandmarkConfirmationQueryL( iEikonEnv ) )
                {
                iLmkFields->DeleteLandmarkL();
                iLmkUiUtils->RestoreOldNaviPaneL();
                iIsEditing = EFalse;
                delete this;                
                }
            break;
            }
        case ELmkCmdSelectCategories:
            {
			if(!iIsCategorySelctorOpen)
				{
				iIsCategorySelctorOpen = ETrue;
				SelectCategoriesCmdL();
				}
            break;
            }
        case ELmkCmdChangeIcon:
            {
            ChangeIconCmdL();
            break;
            }
        case ELmkCmdCurrentLocation:
            {
            CurrentLocationCmdL();
            break;
            }
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
            if ( !IsNameFieldEmptyL() )
                {
                // Obtain the current landmark object
                CPosLandmark* lmk = GetCurrentEditedLandmarkL();
                CleanupStack::PushL( lmk );
                iSender.SendSingleLandmarkL(*lmk);
                CleanupStack::PopAndDestroy( lmk );
                }
            else
                {
                DoNotSaveFormDataL();
                }
            break;
            }
        case ELmkCmdGoToUrl:
			{
			// Get URL field
			MLmkFieldData* field = iLmkFields->GetField(EWebAddress);
			if(field)
				{
				TInt id = field->UniqueFieldIdentity();
				MLmkEditorField* control = iEditorFieldArray->Find( id );
				if(control)
					{
					HBufC* fieldText =  control->ControlTextL();
					CleanupStack::PushL( fieldText );
					TPtr urlPtr = fieldText->Des();
					if ( urlPtr.Length() > 0 )
						{
						iLmkGoToURLCmd->LaunchBrowserL( urlPtr );
						}
					CleanupStack::PopAndDestroy( fieldText );
					}
				}
			break;
			}
        case ELmkCmdEditLm:
	        {
	        if(!iIsEditing)
	        	{
	        	LaunchLmkEditorL();
	        	}
	        break;
	        }
        case ELmkCmdCall:
	    case ELmkCmdLast:
		    {
			MLmkFieldData* field = iLmkFields->GetField(EPhoneNumber);
			if(field)
				{
				TInt id = field->UniqueFieldIdentity();
				MLmkEditorField* control = iEditorFieldArray->Find( id );
				if(control)
					{
					HBufC* fieldText = control->ControlTextL();
					CleanupStack::PushL( fieldText );
		            if ( fieldText && fieldText->Length() > 0 )
		                {
						//Check if any active character or not
						TPtr ptr = fieldText->Des();
						ptr.Trim();
						if (ptr.Length() > 0)
							{
							//modified for 3.2, since   command is not coming
							//from menu item, hence need to pass diff command.
							//this code needs some more modification to make it
						 	//genericly work for all the commands.
						    iLmkCallCmd->ExecuteAIWCallCmdL( ELmkCmdCall, ptr );
							}
						}
	            	CleanupStack::PopAndDestroy( fieldText );
					}
			    }
			break;
		    }
        case ELmkCmdShow:
	        {
	        MLmkEditorField* field =
			iEditorFieldArray->Find( IdOfFocusControl() );
			TUint type = field->LandmarkItemField().FieldType();
			if (type == ECategories )
				{
				if(iCalegorySelectionFlag) // Receive mode
					{
					CategoriesCmdFromXmlL();
					}
				else
					{
					CategoriesCmdL();		// View mode
					}
				}
			break;
	        }
        case ELmkCmdClearPosInfo:
        	{
        	ClearPositionInfoL();
        	break;
        	}        	
        default:
            {
            CAknForm::ProcessCommandL( aCommandId );
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::HandleControlStateChangeL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::HandleControlStateChangeL( TInt aControlId )
    {
    MLmkEditorField* changedField = iEditorFieldArray->Find( aControlId );
    if ( changedField )
        {
        if ( changedField->LandmarkItemField().IsTitleField() )
            {
            HBufC* title = changedField->ControlTextL();
            CleanupStack::PushL( title );
            if ( !title )
                {
                title = HBufC::New(0);
                }
            iLmkUiUtils->ChangeTitlePaneL( title );
            CleanupStack::Pop(); //title
            }
        }
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::DoNotSaveFormDataL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::DoNotSaveFormDataL()
    {
    if ( !iIsEndKey )
		{
	  	LmkNotes::InformationNoteL( iEikonEnv, R_LMK_EMPTY_LANDMARK_NAME_NOTE );
	    }
    MLmkFieldData* field = iLmkFields->GetField(EName);
    if( field )
        {
        TInt id = field->UniqueFieldIdentity();
        TryChangeFocusL(id);        
        }    
    CAknForm::SetInitialCurrentLine();
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::GetHelpContext
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    if ( iHelpContext.iContext.Length() > 0 )
        {
        aContext.iContext = iHelpContext.iContext;
        aContext.iMajor = iHelpContext.iMajor;
        }
    else if ( iEditorMode == CLmkEditorDlg::ELmkViewer )
        {
        aContext.iContext = KLM_HLP_LM_VIEWER;
        aContext.iMajor = TUid::Uid( KLmkAppUID3 );
        }
    else
        {
        aContext.iContext = KLM_HLP_LM_EDITOR;
        aContext.iMajor = TUid::Uid( KLmkAppUID3 );
        }
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CLmkEditorImpl::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                             TEventCode aType )
    {
   /**
    * By def, dialog opens in view mode
    * selecting 'edit' option from (either from context menu/options menu)
    * to edit landmark fields
    *
    */
    if (IsEditable())
    	{
    	//always dimmed for the editor
		DimmMiddleSoftKey();
    	}
	if ( aKeyEvent.iCode == EKeyOK )
		{
		MLmkEditorField* field =
		iEditorFieldArray->Find( IdOfFocusControl() );
		TUint type = field->LandmarkItemField().FieldType();
		// check the editor mode
		if (IsEditable()) // edit mode
			{
			if ( type == ECategory || type == ECategories )
				{
				if(!iIsCategorySelctorOpen)
					{
					iIsCategorySelctorOpen = ETrue;
					SelectCategoriesCmdL();
					}

				return EKeyWasConsumed;
				}
			}
		else
			{// view or receive mode
			if ( type == ECategory || type == ECategories )
				{
				if(iCalegorySelectionFlag) // Receive mode
					{
					CategoriesCmdFromXmlL();
					return EKeyWasConsumed;
					}
				else
					{
					CategoriesCmdL();		// View mode
					return EKeyWasConsumed;
					}
				}
			else // focus is on any other landmark field
				{
				if ( !iCalegorySelectionFlag )// No context menu for receive mode
					{
		            if(!iIsEditing)
		                {
		                LaunchLmkEditorL();
		                return EKeyWasConsumed;
		                }
/*					
					//Launch context sensitive menu bar here
					if ( iContextMenuBar )
						{
						iContextMenuBar->TryDisplayMenuBarL();
						return EKeyWasConsumed;
						}
*/						
					}
				}
			}
		}
	else if( aKeyEvent.iCode == EKeyBackspace )
		{
		if( !IsEditable() && iIsLmkReceiveMode == EFalse )
			{
			ProcessCommandL( ELmkCmdDeleteLm );
			return EKeyWasConsumed;
			}
		}
	else if ( aKeyEvent.iCode == EKeyPhoneEnd )
			{
			iIsEndKey = ETrue;
			OkToExitL(EAknSoftkeyDone);
			}
	else // for 'sendkey' handling
		{
		if ( aKeyEvent.iCode == EKeyPhoneSend &&  (!IsEditable()) && (!Layout_Meta_Data::IsPenEnabled()) )
			{
			// Get Landmark's telephone number
			TPosLmItemId lmId = iLmkFields->LandmarkId();
			CPosLandmark* landmark = iDb.ReadLandmarkLC(lmId);
			// Get phone num field
			if (landmark->IsPositionFieldAvailable(
			              ELmkPositionFieldPhoneNumber) )
				{
				TPtrC telnumPtr;
				if ( (landmark->GetPositionField(
				                ELmkPositionFieldPhoneNumber,
				                telnumPtr ) == KErrNone) && (telnumPtr.Length() > 0 ))
					{
					iLmkCallCmd->ExecuteAIWCallCmdL( ELmkCmdCall,telnumPtr );
					}
				else
					{
					// Get name
					TPtrC lmkNamePtr;
					if ( landmark->GetLandmarkName ( lmkNamePtr ) == KErrNone )
						{
						LmkNotes::InformationNotewithTextL( iEikonEnv,
						                 R_LMK_NOTE_NO_NUMBER_TO_NAME,
						                 lmkNamePtr);
						}
					}
				}
			CleanupStack::PopAndDestroy(landmark);
			}
		}

	if ( aType == EEventKey && ( aKeyEvent.iCode == EKeyEnter || aKeyEvent.iScanCode == EStdKeyEnter ))
		{
		TBool isCategoryField = EFalse;
		MLmkEditorField* field = iEditorFieldArray->Find( IdOfFocusControl() );
		TUint type = field->LandmarkItemField().FieldType();

		if ( type == ECategory || type == ECategories )
			isCategoryField = ETrue;

		// check the editor mode and category field focus
		if ( IsEditable() == EFalse || isCategoryField )
			{
			TKeyEvent enterKeyEvent( aKeyEvent );
			enterKeyEvent.iCode = EKeyOK;
			CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
			return cba->OfferKeyEventL( enterKeyEvent, aType );
			}
		}

    TKeyResponse response= CAknForm::OfferKeyEventL( aKeyEvent, aType );
    return response;
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::NotifyL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::NotifyL( const TInt aStatus )
	{
    // Stop the running wait dialog
 	TInt buttonId = iWaitDialogLauncher->StopWaitDialogL();
 	if( EEikBidCancel != buttonId )
 	    {
	    switch( aStatus )
    		{
            case KErrNone:
            case KPositionPartialUpdate:
                {
                if(iLocationService)
                	{
                	TPosition position = iLocationService->CurrentPosition();
			    	SetCurrentLocationL( position );
                	}
                break;
                }
            default:
                {
                break;
                }
    		}
 	    }
    delete iLocationService;
    iLocationService = NULL;
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::NotifyErrorL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::NotifyErrorL( TInt aErrorCode )
    {

    // Stop the running wait dialog
 	TInt buttonId = iWaitDialogLauncher->StopWaitDialogL();

    delete iLocationService;
    iLocationService = NULL;

 	if( EEikBidCancel != buttonId )
 	    {
	    switch( aErrorCode )
    		{
	        case KPositionQualityLoss:
    		case KErrTimedOut:
    		case KErrNotFound: // No PSY selected.
    		case KErrUnknown:
    		case KErrCancel:
    		case KErrArgument:
    		default:
    		    // The above errors are because of location request failures
		        // Ask user to retry location request
    			{
    			if ( LmkNotes::MessageQueryWithLinkL(iEikonEnv,R_LMK_MSG_QUERY_HEADING_PANE_TEXT,
    												R_LMK_CONF_RETRY_LOCATION_REQUEST,R_LMK_REQUEST_FAIL_MORE_INFO,
    												MessageQueryCallBack) )
    				{
    				if( iLocationService )
    					{
    					delete iLocationService;
    					iLocationService = NULL;
    					}
    				CurrentLocationCmdL();
    				}
    			}
    		}
        }
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::CreateCustomControlL
// -----------------------------------------------------------------------------
//
SEikControlInfo CLmkEditorImpl::CreateCustomControlL( TInt /*aControlType*/ )
    {
    SEikControlInfo ctrl;
    ctrl.iControl = NULL;
    ctrl.iTrailerTextId = 0;
    ctrl.iFlags = 0;
    /*switch( aControlType )
        {
        case EDistanceEditorType:
            {
            ctrl.iControl = new(ELeave) CLmkDistanceEditor();
            break;
            }
        default:
            {
            break;
            }
        }*/
    return ctrl;
    }

// ----------------------------------------------------
// CLmkEditorImpl::ConvertCustomControlTypeToBaseControlType
// ----------------------------------------------------
//
MEikDialogPageObserver::TFormControlTypes
CLmkEditorImpl::ConvertCustomControlTypeToBaseControlType(
    TInt aControlType ) const
    {
    switch( aControlType )
        {
        case EDistanceEditorType:
            {
            return MEikDialogPageObserver::EMfneDerived;
            //break;
            }
        default:
            {
            return MEikDialogPageObserver::EUnknownType;
            //break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::PreLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::PreLayoutDynInitL()
    {
    CAknForm::PreLayoutDynInitL();

    // When adding fields the form must be editable:
    //SetEditableL(ETrue);
    if (iCalegorySelectionFlag && iCategoryNames.Count() >= 1)
        iEditorFieldArray->SetCategoryName(iCategoryNames[0]);
    iEditorFieldArray->CreateFieldsFromLmkL();
    //SetEditableL(EFalse);
    //CAknForm::SetInitialCurrentLine();
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::PostLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::PostLayoutDynInitL()
    {
    CAknForm::PostLayoutDynInitL();

    MLmkFieldData* fields = iLmkFields->GetField( EName );
    if ( fields )
        {
        TPtrC name =  fields->TextData();

        //ChangeTitle
        HBufC* title = name.Alloc();
        CleanupStack::PushL( title );
        iLmkUiUtils->StoreTitlePaneL(); // save old title pane
        iLmkUiUtils->ChangeTitlePaneL( title ); //take ownership
        CleanupStack::Pop( title ); //title
        }

    SetEditableL(  iEditorMode == CLmkEditorDlg::ELmkEditor );
    CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
    cba->UpdateCommandObserverL(KMskCommandPos,*this);
    if (!IsEditable())
	    {
	    AddMskCommandL(R_LM_MSK_EDIT,ELmkCmdEditLm);
	    }
	else
		{
		//only for editor
		DimmMiddleSoftKey();
		if (iFieldTypeToFocus == ECategory || iFieldTypeToFocus == ECategories)
			{
			AddMskCommandL(R_LM_MSK_EDIT,ELmkCmdSelectCategories);
			}
		}
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CLmkEditorImpl::OkToExitL( TInt aKeyCode )
    {
    TBool ret = EFalse;
    switch ( aKeyCode )
        {
	    case EAknSoftkeyDone:
		    {
            if ( IsEditable() )
                {
                if ( iEditorFieldArray->AreAllFieldsEmptyL() )
                    {
                    
                    if( iLandmark && iIsHideCoordinate )
                    	{
                    	TLocality locality;
                    	iLandmark->GetPosition( locality ); 
                    	if( !Math::IsNaN( locality.Latitude() ))                        		
                    		{
					        DoNotSaveFormDataL(); 
					        iIsEndKey = EFalse;
                        	// Stay back in the editor mode
                    		break;
                    		}
                    	}
                    	
                    TRAPD(err,CPosLandmark* landmark = iDb.ReadLandmarkLC(iLmkFields->LandmarkId());CleanupStack::PopAndDestroy( landmark ));
                    if (err == KErrNotFound)
                        {
						// The Landmark is not in the database.Dont save the 
						// landmark. Just return
						// Don't save when all landmark fields are empty 
						iLmkUiUtils->RestoreOldNaviPaneL();                     
						ret = ETrue;                       	
                        }
                    else
                        {
                        // The landmark is in the database. Hence, confirm
                        // from the user whether he wants to delete the
                        // landmark
                        if ( LmkNotes::LandmarkConfirmationQueryL( iEikonEnv ) )
                            {
                            iLmkFields->DeleteLandmarkL();
                            iLmkUiUtils->RestoreOldNaviPaneL();
                            ret = ETrue;
                            }
                        else
                            {
                            // Stay back in the editor mode
                            ret = EFalse;
                            }
                        }

                    }
                else
                    {
                    if(!ValidateContentL())
                    	{
                    	ret = EFalse;
                        return ret;
                    	}
                    else
                        {
                        /**
                        * Do not save landmark if disc space is not
                        * enough
                        */
                        TRAPD( err, iEditorFieldArray->SaveFieldsL() );
                        if ( err == KErrDiskFull )
                            {
                            ret = EFalse;
                            LmkNotes::OutOfMemoryNoteL( iEikonEnv );
                            delete this;
                            return ret;
                            }

                        if ( iLandmarkItemId )
                        	{
                        	*iLandmarkItemId = iLmkFields->LandmarkId();
                        	}
                        iLmkUiUtils->RestoreOldNaviPaneL();
                        ret = ETrue;
                        }
                    }
                }
            else
                {
                iLmkUiUtils->RestoreOldNaviPaneL();
                ret = ETrue;
                }
            break;
            }
        default:
            break;
        }
    if ( !ret )
        {
        ret = CAknForm::OkToExitL( aKeyCode );
        }
    return ret;
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::CreateLineL
// -----------------------------------------------------------------------------
//
CCoeControl* CLmkEditorImpl::CreateLineL(
    const TDesC& aCaption,
	TInt aControlId,
	TInt aControlType )
	{
	return CreateLineByTypeL(
			aCaption, aControlId, aControlType, NULL );
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::LineControl
// -----------------------------------------------------------------------------
//
CEikCaptionedControl* CLmkEditorImpl::LineControl( TInt aControlId ) const
	{
	return Line( aControlId );
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::DeleteControl
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::DeleteControl( TInt aControlId )
	{
	DeleteLine( aControlId );
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::TryChangeFocusL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::TryChangeFocusL( TInt aControlId )
	{
	if ( ControlOrNull( aControlId ) )
		{
		TryChangeFocusToL( aControlId );
		}
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::SetCurrentLineCaptionL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::SetCurrentLineCaptionL(
    TInt aControlId,
    const TDesC& aText )
	{
	CEikCaptionedControl* ctl = Line(aControlId);
	if (ctl)
	    {
	    ctl->SetCaptionL(aText);
	    ctl->DrawDeferred();
	    }
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::Control
// -----------------------------------------------------------------------------
//
CCoeControl* CLmkEditorImpl::Control( TInt aControlId ) const
	{
	return ControlOrNull( aControlId );
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::SetEditableL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::SetEditableL( TBool aState )
	{
    CAknForm::SetEditableL( aState );
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::Database
// -----------------------------------------------------------------------------
//
CPosLandmarkDatabase& CLmkEditorImpl::Database()
    {
    return iDb;
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::GetCurrentEditedLandmarkL
// -----------------------------------------------------------------------------
//
CPosLandmark* CLmkEditorImpl::GetCurrentEditedLandmarkL()
	{
	iEditorFieldArray->SaveFieldsToBufferL();
	CArrayPtrFlat<MLmkFieldData>& fieldArray = iLmkFields->Fields();
	TInt count( fieldArray.Count() );
	CPosLandmark* lmk = NULL;
	if( iLandmark )
		{
		HBufC8* lmBuf = PosLandmarkSerialization::PackL( *iLandmark );
		CleanupStack::PushL( lmBuf );
		lmk = PosLandmarkSerialization::UnpackL( lmBuf->Des() );
		CleanupStack::PopAndDestroy( lmBuf );
		}
	else
		{
		lmk = CPosLandmark::NewL();
		}

	TBool isPosFieldPresent=EFalse;
	
	TLocationData locationData; // to store editor position data

    //Position data
	TLocality locality = TLocality(); // empty locality

	locationData.iLatitude             = locality.Latitude();
	locationData.iLongitude            = locality.Longitude();
	locationData.iAltitude             = locality.Altitude();
	locationData.iHorizontalAccuracy   = locality.HorizontalAccuracy();
	locationData.iVerticalAccuracy     = locality.VerticalAccuracy();
		
    for ( TInt i = 0; i < count; i++ )
        {
        switch( fieldArray[i]->FieldType() )
            {
            case EName:
                {
                lmk->SetLandmarkNameL( fieldArray[i]->TextData() );
                HBufC* path = fieldArray[i]->IconPath();
                if ( path && path->Length() > 0 &&
                    fieldArray[i]->IconId() != KErrNotFound )
                    {
                    // Get the default Mask index
                    lmk->SetIconL( *path, fieldArray[i]->IconId() , KLmkDefaultId+1);
                    }

                break;
                }
            case ECategory:
            case ECategories:
                {
                //Add categories to landmark
                RArray<TPosLmItemId>& categories =
                    fieldArray[i]->Categories();
                TInt Tcount( categories.Count() );
                for ( TInt j=0; j < Tcount; j++ )
                    {
                    lmk->AddCategoryL( categories[j] );
                    }
                break;
                }
			case EDescription:
				{
				lmk->SetLandmarkDescriptionL( fieldArray[i]->TextData() );
				break;
				}
            //these all are same
            case EStreet:
            case EPostCode:
            case ECity:
            case EStateProvince:
            case ECountry:
			case EPhoneNumber:
			case EWebAddress:
                {
                lmk->SetPositionFieldL(
                        fieldArray[i]->PositionFieldId(),
                        fieldArray[i]->TextData() );
                break;
                }
            case ELatitude:
                {
                isPosFieldPresent = ETrue;
                locationData.iLatitude = fieldArray[i]->DoubleData();
                break;
                }
            case ELongitude:
                {
                isPosFieldPresent = ETrue;
                locationData.iLongitude = fieldArray[i]->DoubleData();
                break;
                }
            case EPositionAccuracy:
                {
                locationData.iHorizontalAccuracy =
                    fieldArray[i]->FloatData();
                break;
                }
            case EAltitude:
                {
                locationData.iAltitude = fieldArray[i]->FloatData();
                break;
                }
            case EAltitudeAccuracy:
                {
                locationData.iVerticalAccuracy = fieldArray[i]->FloatData();
                break;
                }
            default:
                {
                break;
                }
            }
        }

    if( isPosFieldPresent )
        {
        // remove the old values to set new values       
        lmk->RemoveLandmarkAttributes(CPosLandmark::EPosition);

    	if (!Math::IsNaN(locationData.iLatitude) &&
        	!Math::IsNaN(locationData.iLongitude))
            {
            if (!Math::IsNaN(locationData.iAltitude))
                {
                locality.SetCoordinate(
                    locationData.iLatitude,
                    locationData.iLongitude,
                    locationData.iAltitude);

                if (!Math::IsNaN(locationData.iVerticalAccuracy))
                    {
                    if( locationData.iVerticalAccuracy > 0 )
                        locality.SetVerticalAccuracy(locationData.iVerticalAccuracy);
                    }
                }
            else
                {
                locality.SetCoordinate(
                    locationData.iLatitude,
                    locationData.iLongitude);
                }

            if (!Math::IsNaN(locationData.iHorizontalAccuracy))
                {
                if( locationData.iHorizontalAccuracy > 0 )
                    locality.SetHorizontalAccuracy(locationData.iHorizontalAccuracy);
                }
            // save position if at least lat/lon are entered
            lmk->SetPositionL(locality);            
            }
        }

    return lmk;
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::CreateLabelArrayL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::CreateLabelArrayL()
    {

    if ( iJapaneseInputMode )
		{
		// Display in Japanese order
	    CreateLabelArrayForJapaneseModeL();
		}
	else
		{ // Normal mode
		CreateLabelArrayForNormalModeL();
		}
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::CreateLabelArrayForNormalModeL()
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::CreateLabelArrayForNormalModeL()
	{
    CreateLabelL( R_LMK_NAME_LABEL, EName );
    CreateLabelL( R_LMK_CATEGORY_LABEL, ECategory );
    CreateLabelL( R_LMK_CATEGORIES_LABEL, ECategories );
    CreateLabelL( R_LMK_DESCRIPTION_LABEL, EDescription );
    CreateLabelL( R_LMK_STREET_LABEL, EStreet );
    CreateLabelL( R_LMK_POST_CODE_LABEL, EPostCode );
    CreateLabelL( R_LMK_CITY_LABEL, ECity );
    CreateLabelL( R_LMK_STATE_PROVINCE_LABEL, EStateProvince );
    CreateLabelL( R_LMK_COUNTRY_LABEL, ECountry );
    CreateLabelL( R_LMK_PHONE_NUMBER_LABEL, EPhoneNumber  );
    CreateLabelL( R_LMK_WEB_ADDRESS_LABEL, EWebAddress );
    CreateLabelL( R_LMK_LATITUDE_LABEL, ELatitude );
    CreateLabelL( R_LMK_LONGITUDE_LABEL, ELongitude );
    CreateLabelL( R_LMK_POSITION_ACCURACY_LABEL, EPositionAccuracy );
    CreateLabelL( R_LMK_ALTITUDE_LABEL, EAltitude );
    CreateLabelL( R_LMK_ALTITUDE_ACCURACY_LABEL, EAltitudeAccuracy );
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::CreateLabelArrayForJapaneseModeL()
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::CreateLabelArrayForJapaneseModeL()
	{
	CreateLabelL( R_LMK_NAME_LABEL, ENameJapaneseMode);
	CreateLabelL( R_LMK_CATEGORY_LABEL, ECategoryJapaneseMode );
	CreateLabelL( R_LMK_CATEGORIES_LABEL, ECategoriesJapaneseMode );
	CreateLabelL( R_LMK_DESCRIPTION_LABEL, 	EDescriptionJapaneseMode );
	CreateLabelL( R_LMK_POST_CODE_LABEL, EPostCodeJapaneseMode );
	CreateLabelL( R_LMK_STATE_PROVINCE_LABEL, EStateProvinceJapaneseMode );
	CreateLabelL( R_LMK_CITY_LABEL, ECityJapaneseMode );
	CreateLabelL( R_LMK_STREET_LABEL, EStreetJapaneseMode );
	CreateLabelL( R_LMK_COUNTRY_LABEL, ECountryJapaneseMode );
	CreateLabelL( R_LMK_PHONE_NUMBER_LABEL, EPhoneNumberJapaneseMode );
    CreateLabelL( R_LMK_WEB_ADDRESS_LABEL, EWebAddressJapaneseMode );
	CreateLabelL( R_LMK_LATITUDE_LABEL, ELatitudeJapaneseMode );
	CreateLabelL( R_LMK_LONGITUDE_LABEL, ELongitudeJapaneseMode );
	CreateLabelL( R_LMK_POSITION_ACCURACY_LABEL, EPositionAccuracyJapaneseMode );
	CreateLabelL( R_LMK_ALTITUDE_LABEL, EAltitudeJapaneseMode );
	CreateLabelL( R_LMK_ALTITUDE_ACCURACY_LABEL, EAltitudeAccuracyJapaneseMode );
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::CreateLabelL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::CreateLabelL( TInt iResourceId, TInt iPos )
    {
    HBufC* label = iCoeEnv->AllocReadResourceL( iResourceId );
    CleanupStack::PushL( label );
    User::LeaveIfError( iLabelArray.Insert( label, iPos ) );
    CleanupStack::Pop(); //label
    }


// -----------------------------------------------------------------------------
// CLmkEditorImpl::IsNameFieldEmptyL
// -----------------------------------------------------------------------------
//
TBool CLmkEditorImpl::IsNameFieldEmptyL()
    {
    const MLmkEditorField* field = &iEditorFieldArray->FieldAt( 0 );
    HBufC* fieldText = field->ControlTextL();
    TBool ret( ETrue );
    if ( fieldText && fieldText->Length() > 0 )
        {
		//Check if any active character or not
		TPtr16 ptr = fieldText->Des();
		ptr.Trim();
		if (ptr.Length() == 0)
		 	{
		 	ret = ETrue;
		 	}
		 	else
		 	{
		 	ret = EFalse;
		 	}
        }
    delete fieldText;
    return ret;
    }


// -----------------------------------------------------------------------------
// CLmkEditorImpl::ValidateContentL()
// -----------------------------------------------------------------------------
//
TBool CLmkEditorImpl::ValidateContentL()
    {
    TBool result = ETrue;
    if(IsFieldEmptyL( ELatitude ) && !IsFieldEmptyL( ELongitude ))
   		{
        LmkNotes::InformationNoteL( iEikonEnv,R_LMK_ERROR_CHECK_NOTE );
        DoNotSaveDataL(ELatitude);
		result = EFalse;
   		}
    else if (!IsFieldEmptyL( ELatitude ) && IsFieldEmptyL( ELongitude ))
   		{
        LmkNotes::InformationNoteL( iEikonEnv, R_LMK_ERROR_CHECK_NOTE );
        DoNotSaveDataL(ELongitude);
	  	result = EFalse;
   		}
    else if(IsFieldEmptyL( ELatitude ) && !IsFieldEmptyL( EPositionAccuracy ))
	    {
	    LmkNotes::InformationNoteL( iEikonEnv, R_LMK_ERROR_POS_ACC_CHECK_NOTE );
	    DoNotSaveDataL(ELatitude);
    	result = EFalse;
	    }
	else if (IsFieldEmptyL( ELongitude ) && !IsFieldEmptyL( EPositionAccuracy ))
   		{
        LmkNotes::InformationNoteL( iEikonEnv, R_LMK_ERROR_POS_ACC_CHECK_NOTE );
	    DoNotSaveDataL(ELongitude);
    	result = EFalse;
   		}
	else if (IsFieldEmptyL( EAltitude ) && !IsFieldEmptyL( EAltitudeAccuracy ))
   		{
        LmkNotes::InformationNoteL( iEikonEnv, R_LMK_ERROR_ALT_ACC_CHECK_NOTE );
        DoNotSaveDataL(EAltitude);
    	result = EFalse;
   		}
	else if ((IsFieldEmptyL( ELatitude ) || IsFieldEmptyL( ELongitude )) && !IsFieldEmptyL( EAltitude ))
   		{
        LmkNotes::InformationNoteL( iEikonEnv, R_LMK_ERROR_ALTITUDE_CHECK_NOTE );
        DoNotSaveDataL(ELatitude);
    	result = EFalse;
   		}
    else if (IsInvalidUriL())
   		{
        LmkNotes::InformationNoteL( iEikonEnv, R_LMK_ERROR_URL_ILLEGAL_NOTE );
        DoNotSaveDataL(EWebAddress);
    	result = EFalse;
   		}
    else if ( IsNameFieldEmptyL() )
        {
        DoNotSaveFormDataL();
        iIsEndKey = EFalse;
        result = EFalse;
        }
    return result;

    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::IsFieldEmptyL()
// -----------------------------------------------------------------------------
//
TBool CLmkEditorImpl::IsFieldEmptyL(const TUint16 aFieldType)
	{
	TInt fldId;
	MLmkFieldData* field = iLmkFields->GetField(aFieldType);
	if(field)
	    {
	    fldId = field->UniqueFieldIdentity();
	    }
    else
        {
        return EFalse;
        }

	MLmkEditorField* fld = iEditorFieldArray->Find(fldId );
	if(!fld)
		{
		return ETrue;
		}
	
	TReal val;
	switch(aFieldType)
		{
		case ELatitude:
		case ELongitude:
			{
			val = fld->FieldValue();
			}
		break;
		case EPositionAccuracy:
		case EAltitudeAccuracy:
		case EAltitude:
			{
			val = fld->FieldValue();
			}
		break;
		}
	return Math::IsNaN(val);
	}


// -----------------------------------------------------------------------------
// CLmkEditorImpl::DoNotSaveDataL()
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::DoNotSaveDataL(const TUint16 aFieldType)
	{
	if( iIsEditing )
		{
		MLmkFieldData* fields = iLmkFields->GetField(aFieldType);
		if(fields)
			{
			TInt id = fields->UniqueFieldIdentity();
			TryChangeFocusL(id);
			}
		}
    }
// -----------------------------------------------------------------------------
// CLmkEditorImpl::OkToExitL
// -----------------------------------------------------------------------------
//
TBool CLmkEditorImpl::OkToExitL()
    {
    TBool result = ETrue;

    if ( iEditorFieldArray->AreAllFieldsEmptyL() )
        {
        iLmkFields->DeleteLandmarkL();
        }
    else
        {
        if ( IsNameFieldEmptyL() )
            {
            DoNotSaveFormDataL();
            result = EFalse;
            }
        else
            {
            TRAPD( err, iEditorFieldArray->SaveFieldsL() );
            if ( err == KErrDiskFull )
                {
                result = EFalse;
                LmkNotes::OutOfMemoryNoteL( iEikonEnv );
                delete this;
                return result;
                }
            if ( iLandmarkItemId )
            	{
            	*iLandmarkItemId = iLmkFields->LandmarkId();
            	}
            }
        }

    return result;
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::SelectCategoriesCmdL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::SelectCategoriesCmdL()
    {
    if(iCatInfoPopupNote)
		 {
		 iCatInfoPopupNote->HideInfoPopupNote();
		 }
    MLmkFieldData* fields = iLmkFields->GetField( ECategory );
    if ( !fields )
        {
        fields = iLmkFields->GetField( ECategories );
        }

    if ( fields )
        {
        CLmkDlgCategorySelectorImpl* dlg = CLmkDlgCategorySelectorImpl::NewL(
                                    iDb, ETrue );
        CleanupStack::PushL( dlg );
        dlg->SetEmptyDlgLabel(R_LMK_EMPTY_NO_CATEGORIES ,R_LMK_GUIDE_CREATE_LANDMARK);
        RArray<TPosLmItemId>& categories = fields->Categories();
        if ( dlg->ExecuteL( categories ) )
            {
            iEditorFieldArray->UpdateFieldsL( ECategory );
            iEditorFieldArray->UpdateFieldsL( ECategories );
            }
        CleanupStack::PopAndDestroy( dlg );
        iIsCategorySelctorOpen = EFalse;
        DrawNow();
       // ShowMultipleCategoriesInfoPopupL(ETrue);
        }
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::CurrentLocationCmdL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::CurrentLocationCmdL()
    {
    if(ShowQueryIfLocationFieldsAlreadyFilledL())
    	{
    	return;
    	}
    HBufC* srvName = StringLoader::LoadLC(
                      R_LMK_SERVICE_RULE_NAME,
                      const_cast< CCoeEnv *>( iCoeEnv ));
    iLocationService = CLmkLocationService::NewL(*srvName);
    CleanupStack::PopAndDestroy( srvName ); //srvName
    iLocationService->SetObserver( *this );
    iLocationService->LocationRequestL();

    TRAPD( error, iWaitDialogLauncher->StartWaitDialogL());
    if( error )
        {
        delete iLocationService;
        iLocationService = NULL;

        User::Leave( error );
        }
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::ChangeIconCmdL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::ChangeIconCmdL()
    {
    TInt lSelectedIconIndex(0);
    CLmkIconMapDialog* dialog = new(ELeave) CLmkIconMapDialog( lSelectedIconIndex );
    MLmkFieldData* nameField;
    if (dialog->ExecuteLD(R_LMK_ICON_TABLE_DIALOG) )
        {
        // icon file also contains masks, therefore 2*
        TInt iconIndex =  2*lSelectedIconIndex;
	    RFs fs;
	    User::LeaveIfError(fs.Connect());
	    CleanupClosePushL( fs );

	    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &fs, sizeof( CPosLandmark ), EDriveC ) )
	        {
	        CleanupStack::PopAndDestroy(); // fs
	        User::LeaveNoMemory();
	        }
		else
			{            
            nameField = iLmkFields->GetField( EName );
            TFileName* iconFile = CLmkUiUtils::LmkUiIconFileLC();
            if( nameField && iconFile )                
                {                
                nameField->SetIconId( iconIndex + EMbmLmkuiQgn_prop_lm_transport);
                nameField->SetIconPathL( *iconFile );
        
                //Update context icon
                CEikImage* image = new(ELeave) CEikImage();
                CleanupStack::PushL( image );
                image->CreatePictureFromFileL( *iconFile,
                                               nameField->IconId(),
                                               iconIndex + EMbmLmkuiQgn_prop_lm_transport_mask);
        
                iLmkUiUtils->UpdateContextIconL( image );
        
                // Added for appending Icon to Name label
                CLmkUiUtils::ChangeLmNameLabelIconL (*this, *nameField);
                CleanupStack::Pop(); //image
                CleanupStack::PopAndDestroy();//iconFile
                DrawNow();
                }
			}
		CleanupStack::PopAndDestroy(); // fs
        }
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::CategoriesCmdL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::CategoriesCmdL()
    {
    MLmkFieldData* categoryField = iLmkFields->GetField( ECategories );
    if ( !categoryField )
        {
        categoryField = iLmkFields->GetField( ECategory );
        if ( !categoryField )
            {
            return;
            }
        }
    else    
        {
        RArray<TPosLmItemId> categories = categoryField->Categories();
        TInt count( categories.Count() );

        // Do not launch the popup for displaying one category name
        if (count <=1 )
            {
            //Launch context sensitive menu bar here
            if ( iContextMenuBar )
                {
                iContextMenuBar->TryDisplayMenuBarL();
                }
            return;
            }

        HBufC* catName;
        // show the category list in a message query
        HBufC* message = HBufC::NewLC( (KPosLmMaxCategoryNameLength * count) + 1);
        TPtr msgPtr = message->Des();
        CDesC16ArrayFlat* desArr = new (ELeave) CDesC16ArrayFlat(count);
        CleanupStack::PushL(desArr);
        // fill up the array with category´s names from the engine.
        for ( TInt i = 0; i < count; i++ )
            {
            catName = CLmkDbUtils::CategoryNameL( iDb, categories[i] );
            CleanupStack::PushL( catName );
            desArr->AppendL(catName->Des());
            CleanupStack::PopAndDestroy( catName ); //name
            }
            desArr->Sort();
            for ( TInt i = 0; i < count; i++ )
            {
            msgPtr.Append((*desArr)[i]);
            if ( i != ( count - 1 ) )
                {
                msgPtr.Append(_L("\n"));
                }
            }
        LmkNotes::MessageQueryL(  iEikonEnv, msgPtr, count );
        CleanupStack::Pop();//desArr
        delete  desArr;
        CleanupStack::PopAndDestroy( message ); //message
        }
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::SetCurrentLocationL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::SetCurrentLocationL( const TPosition& aPosition )
    {
    MLmkFieldData* latitude = iLmkFields->GetField( ELatitude );
    if(latitude)
	    {
	    latitude->SetTReal( aPosition.Latitude() );

	    MLmkFieldData* longitude = iLmkFields->GetField( ELongitude );
	    if(longitude)
	        longitude->SetTReal( aPosition.Longitude() );
	    }

    MLmkFieldData* altitude = iLmkFields->GetField( EAltitude );
    if(altitude)
	    {
	    altitude->SetTReal( aPosition.Altitude() );
	    }

    MLmkFieldData* horizontalAccuracy =
                        iLmkFields->GetField( EPositionAccuracy );
    if(horizontalAccuracy)
	    {
	    horizontalAccuracy->SetTReal( aPosition.HorizontalAccuracy() );
	    }

    MLmkFieldData* verticalAccuracy =
                        iLmkFields->GetField( EAltitudeAccuracy );
    if(verticalAccuracy)
	    {
	    verticalAccuracy->SetTReal( aPosition.VerticalAccuracy() );
	    }
	
	TRAP_IGNORE(iLandmark->SetPositionL(aPosition));    
	    
    iEditorFieldArray->UpdateFieldsL( EAllEditorItems );
    
    DrawNow();
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::ChangeContextImageL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::ChangeContextImageL()
    {
    MLmkFieldData* nameField = iLmkFields->GetField( EName );
    if( nameField )
        {
        CEikImage* image = new(ELeave) CEikImage();
        CleanupStack::PushL( image );
        TFileName* defaultIconFile = CLmkUiUtils::LmkUiIconFileLC();
        // No need to add iconFile to cleanupstack since it is a member variable from CLmkFieldData class
        HBufC *iconFile = nameField->IconPath();
        TInt err = KErrNotFound;
        CFbsBitmap* bitmap;
        CFbsBitmap* mask;
        if ( nameField->IconId() != KErrNotFound )
            {
            TRAP(err,AknIconUtils::CreateIconL( bitmap, mask,
                                   *iconFile, nameField->IconId(),
                                   nameField->IconId()+1 ););
            }
        // If the Icon File doesn't exist the CreateIconL in the if statement above will
        // Leave with KErrNotFound in which case the default Icon should be loaded.
        // Also when the Landmarks itself doesn't have a valid Icon, the default Icon is
        // loaded.
        if ( err != KErrNone )
            {
            AknIconUtils::CreateIconL( bitmap, mask,
                                   *defaultIconFile, KLmkDefaultId,
                                   KLmkDefaultId+1 );
            }
        image->SetBitmap( bitmap );
        image->SetMask( mask );
        iLmkUiUtils->SwapNewContextIconL( image );
        CleanupStack::PopAndDestroy(); //defaultIconFile
        CleanupStack::Pop(); //image
        }
    }

// ----------------------------------------------------------------------------
// CLmkEditorImpl::AttachAIWMenuInterestL
// ----------------------------------------------------------------------------
//
void CLmkEditorImpl::AttachAIWMenuInterestL()
	{
	if(iMapNavFeature)
		{
		iMapNavInterface->AttachAIWInterestL(R_LMK_EDITOR_MENU, R_LMK_EDITOR_AIW_INTEREST_SHOWONMAP);
		if (iEditorMode != CLmkEditorDlg::ELmkEditor)
			{
			iMapNavInterface->AttachAIWInterestL(R_LMK_EDITOR_MENU, R_LMK_EDITOR_AIW_INTEREST_NAVIGATETO);
			}
		}
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::HandleResourceChange()
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::HandleResourceChange (TInt aType)
	{	
	CAknForm::HandleResourceChange(aType);
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::CategoriesCmdFromXmlL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::CategoriesCmdFromXmlL()
    {
    TInt count = iCategoryNames.Count();
    if ( count <= 1 )
        {
        return;
        }
    CDesC16ArrayFlat* desArr = new (ELeave) CDesC16ArrayFlat(count);
    CleanupStack::PushL(desArr);
	// fill up the array with category´s names from the engine.
	for ( TInt i = 0; i < count; i++ )
        {
	    desArr->AppendL(iCategoryNames[i]);
		}

	desArr->Sort();
	// show the category list in a message query
	HBufC* message = HBufC::NewLC((KPosLmMaxCategoryNameLength * iCategoryNames.Count()) + 1);
	TPtr msgPtr = message->Des();
	for ( TInt i = 0; i < count; i++ )
		{
        msgPtr.Append((*desArr)[i]);
		if ( i != ( count - 1 ))
    		{
    		msgPtr.Append(_L("\n"));
    		}
		}
    LmkNotes::MessageQueryL( iEikonEnv, msgPtr, count );
    CleanupStack::PopAndDestroy( 2 ); //message ,desArr
    }

// -----------------------------------------------------------------------------
// CLmkEditorImpl::ConstructContextMenuL()
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::ConstructContextMenuL()
    {
	CEikMenuBar* newMenuBar = new(ELeave) CEikMenuBar();
	CleanupStack::PushL(newMenuBar);
	newMenuBar->ConstructL(this, NULL, R_LMK_EDITOR_CONTEXT_MENUBAR);
	iEikonEnv->EikAppUi()->AddToStackL(newMenuBar, ECoeStackPriorityMenu, ECoeStackFlagRefusesFocus);
	iContextMenuBar = newMenuBar;
	CleanupStack::Pop(newMenuBar);
    }

// -----------------------------------------------------------------------------
//  CLmkEditorImpl::GetFieldTextLengthL
// -----------------------------------------------------------------------------
//
TInt  CLmkEditorImpl::GetFieldTextLengthL( MLmkEditorField& afield)
	{
	HBufC* fieldText =  afield.ControlTextL();
	TInt fieldLen (0);
	if ( fieldText )
		{
		if ( fieldText->Length() > 0)
			{// Remove any blank spaces if any
			TPtr16 ptr = fieldText->Des();
			ptr.Trim();
			fieldLen = ptr.Length();
			}
		}
	delete fieldText;
	return fieldLen;
	}
// -----------------------------------------------------------------------------
// CLmkEditorImpl::UpdateViewDlgL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::UpdateViewDlgL()
	{

	CPosLandmark* landmark;
	TInt id = iLmkFields->LandmarkId();
	landmark = iDb.ReadLandmarkLC(id);
	TLocality locality;
	landmark->GetPosition(locality);
    TRAP_IGNORE(iLandmark->SetPositionL(locality));
	CArrayPtrFlat<MLmkFieldData>& fieldArray =  iLmkFields->Fields();
	CArrayPtrFlat<MLmkEditorField>& editorFieldArray =
									iEditorFieldArray->GetFieldArray();
	TInt count = fieldArray.Count();
	TInt postn = KErrNotFound;
	TInt ctrlid = KErrNotFound;
	for (TInt i(0);i<count;i++)
		{
		TUint fieldType = fieldArray[i]->FieldType();
		MLmkEditorField* fld = iEditorFieldArray->Find(fieldArray[i]->UniqueFieldIdentity(),&postn,&ctrlid);

		switch(fieldType)
			{
	        case EName:
	            {
	            UpdateNameEditorControlL(fld,landmark,fieldArray[i]);
	            break;
	            }
	        case ECategory:
	        case ECategories:
	            {
	            UpdateCategoryEditorControlL( fld, fieldArray[i],ctrlid,postn,*landmark);
	            break;
	            }
			case EDescription:
				{
				UpdateDesEditorControlL(fld,landmark,fieldArray[i],ctrlid,postn);
				break;
				}
	        //these all are same
	        case EStreet:
	        case EPostCode:
	        case ECity:
	        case EStateProvince:
	        case ECountry:
			case EPhoneNumber:
			case EWebAddress:
	            {
	            UpdateTextEditorControlL(fld,landmark,fieldArray[i],
	            						fieldType,ctrlid,postn);
	            break;
	            }
	        case ELatitude:
	            {
	        	UpdateCoordinateEditorControlL(fld,fieldArray[i],
	        								   locality.Latitude(),
	        								   ctrlid,postn);
	            break;
	            }
	        case ELongitude:
	            {
	        	UpdateCoordinateEditorControlL(fld,fieldArray[i],
	        								   locality.Longitude(),
	        								   ctrlid,postn);
	            break;
	            }
	        case EPositionAccuracy:
	            {
	            UpdateNumberEditorControlL(fld,fieldArray[i],
	        								   locality.HorizontalAccuracy(),ctrlid,postn);
	            break;
	            }
	        case EAltitude:
	            {
	        	UpdateNumberEditorControlL(fld,fieldArray[i],
	        								   locality.Altitude(),ctrlid,postn);
	            break;
	            }
	        case EAltitudeAccuracy:
	            {
	            UpdateNumberEditorControlL(fld,fieldArray[i],
	        								   locality.VerticalAccuracy(),ctrlid,postn);
	            break;
	            }
	        default:
	            {
	            break;
	            }
			}
		}
	iEditorFieldArray->UpdateFieldsL();
	CleanupStack::PopAndDestroy( landmark );
	DrawNow();
	if(!IsEditable())
		{
		//only for landmarks viewer
		UpdateMskOnArrowMoveForViewerL();
		//ShowMultipleCategoriesInfoPopupL(ETrue);
		}
	}
// -----------------------------------------------------------------------------
// CLmkEditorImpl::ActivateL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::ActivateL()
	{
	if (iEditorMode == CLmkEditorDlg::ELmkEditor
		&& iIsEditing)
		{
    	MLmkFieldData* fields = iLmkFields->GetField(iFieldTypeToFocus);
    	if(fields)
	    	{
	    	TInt id = fields->UniqueFieldIdentity();
	    	TryChangeFocusL(id);	    	
			if (iFieldTypeToFocus == ECategories || iFieldTypeToFocus == ECategory)
		    	{
		    	// ShowMultipleCategoriesInfoPopupL(EFalse);	
		    	}	
	    	}
	    }
	CCoeControl::ActivateL();
	}
// -----------------------------------------------------------------------------
// CLmkEditorImpl::UpdateTextEditorControlL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::UpdateTextEditorControlL
										(MLmkEditorField* aEditorField,
										CPosLandmark*	aLandmark,
										MLmkFieldData*  fieldData,
										TUint aFiledType,
										TInt aPrevControlId,
							   			TInt aPos
										)
	{

	TPositionFieldId fieldId = EPositionFieldNone;
	switch(aFiledType)
	{
	case EStreet:
	fieldId = EPositionFieldStreet;
	break;
    case EPostCode:
    fieldId = EPositionFieldPostalCode ;
    break;
    case ECity:
    fieldId = EPositionFieldCity;
    break;
    case EStateProvince:
    fieldId = EPositionFieldState;
    break;
    case ECountry:
    fieldId = EPositionFieldCountry;
    break;
	case EPhoneNumber:
	fieldId = ELmkPositionFieldPhoneNumber;
	break;
	case EWebAddress:
	fieldId = ELmkPositionFieldWebAddress;
	break;
	}
	TPtrC text;
	if ( aLandmark->GetPositionField(
    		fieldId, text ) == KErrNone )
		{
	   	if (IsEditable() && text.Length() == 0 && aFiledType == EWebAddress)
	   		{
	   		//return if web address field and field is empty and its an editor
	   		return;
	   		}
	   	fieldData->SetTextL(text);
	   	if(!IsEditable())
			{
			aEditorField = AddOrDeleteLineL( aEditorField,fieldData,aPrevControlId,
			 								 fieldData->UniqueFieldIdentity(), aPos);
			}
		if(aEditorField)
			{
			//now modify the text for display
	   	CLmkEditorTextField* field =	static_cast<CLmkEditorTextField*>(aEditorField);
	   	HBufC* buff = HBufC::NewLC(text.Length());
	   	TPtr dispTxt = buff->Des();
	    if(!IsEditable())
		    {
		    TPtr des1 = CLmkFields::RemoveEnterCharacter( text );
			dispTxt.Copy(des1);
		    }
	    else
		    {
			dispTxt.Copy(text);
		    }
		if(aFiledType == EWebAddress)
			{
			TInt position;
			if((position =  field->IsWebUrlProtocolPreFixedL(dispTxt))!=
				KErrNotFound)
				{
				field->TrimPrefixesFromWebUrlL( dispTxt, position +1 );
				}
			}
		   	TPtrC ptr;
		   	ptr.Set(dispTxt);
		   	field->SetControlTextL(ptr);
		   	CleanupStack::PopAndDestroy( buff );
			}

		}
	}
// -----------------------------------------------------------------------------
// CLmkEditorImpl::UpdateDesEditorControlL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::UpdateDesEditorControlL
										(MLmkEditorField* aEditorField,
										CPosLandmark*	aLandmark,
										MLmkFieldData*  fieldData,
										TInt aPrevControlId,
							   			TInt aPos
										)
	{
		TPtrC text;
        aLandmark->GetLandmarkDescription(text);
        fieldData->SetTextL(text);
        TPtrC dispTxt;
	    if(!IsEditable())
		    {
		    TPtr des1 = CLmkFields::RemoveEnterCharacter( text );
			dispTxt.Set(des1);
			aEditorField = AddOrDeleteLineL( aEditorField,fieldData,aPrevControlId,
		 								 fieldData->UniqueFieldIdentity(), aPos);
		    }
	    else
		    {
			dispTxt.Set(text);
		    }
		CLmkEditorTextField* field =	static_cast<CLmkEditorTextField*>(aEditorField);
		if(aEditorField)
			{
			field->Control()->SetTextL(&dispTxt);
			field->Control()->DrawDeferred();
			}
	}
// -----------------------------------------------------------------------------
// CLmkEditorImpl::UpdateCategoryEditorControlL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::UpdateCategoryEditorControlL
							(
							MLmkEditorField* aEditorField,
							MLmkFieldData*  fieldData,
							TInt aPrevControlId,
							TInt aPos,
						    CPosLandmark&	aLandmark
						    )
	{
	RArray<TPosLmItemId>& categories = fieldData->Categories();
    aLandmark.GetCategoriesL(categories);
  	TInt count( categories.Count() );

    if ( count  <= 1 )
        {
        fieldData->SetFieldType( ECategory );
        }
    else
        {
        fieldData->SetFieldType( ECategories );
        }
    if(!IsEditable())
		{
		aEditorField = AddOrDeleteLineL( aEditorField,fieldData,aPrevControlId,
		 								 fieldData->UniqueFieldIdentity(), aPos);
		}
	}
// -----------------------------------------------------------------------------
// CLmkEditorImpl::UpdateCoordinateEditorControlL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::UpdateCoordinateEditorControlL
							  (
							  MLmkEditorField* aEditorField,
							   MLmkFieldData*  fieldData,
							   TReal aValue,
							   TInt aPrevControlId,
							   TInt aPos
							  )
	{
	fieldData->SetTReal( aValue );
	if(!IsEditable())
		{
		aEditorField = AddOrDeleteLineL( aEditorField,fieldData,aPrevControlId,
		 								 fieldData->UniqueFieldIdentity(), aPos);
		}
	}
// -----------------------------------------------------------------------------
// CLmkEditorImpl::UpdateNumberEditorControlL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::UpdateNumberEditorControlL
							  (
							  MLmkEditorField* aEditorField,
							   MLmkFieldData*  fieldData,
							   TReal32 aValue,
							   TInt aPrevControlId,
							   TInt aPos
							   )
	{
	fieldData->SetTReal( aValue );

	if(!IsEditable())
		{
		aEditorField = AddOrDeleteLineL( aEditorField,fieldData,aPrevControlId,
		 								 fieldData->UniqueFieldIdentity(), aPos);
		}
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::UpdateNameEditorControl
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::UpdateNameEditorControlL
							  (
							  MLmkEditorField* aEditorField,
							  CPosLandmark*	aLandmark,
							  MLmkFieldData*  fieldData
							  )
	{
	
	if( aEditorField && aLandmark && fieldData )
	    {
	    //name field icon updation
	    TInt iconIden = -1;
	    TInt iconMaskIndex = -1;
	    TPtrC iconFile;
	    CLmkEditorTextField* field = static_cast<CLmkEditorTextField*>(aEditorField);
	    if( aLandmark->GetIcon( iconFile, iconIden, iconMaskIndex ) && field )
	        {
	        if( iconIden > 0 )
	            {
	            fieldData->SetIconId( iconIden );
	            fieldData->SetIconPathL( iconFile );
	            CEikImage* image = new( ELeave ) CEikImage();
	            CleanupStack::PushL( image );
	            image->CreatePictureFromFileL( iconFile,
	                               iconIden, iconMaskIndex );
	            iLmkUiUtils->UpdateContextIconL( image );
	            CLmkUiUtils::ChangeLmNameLabelIconL ( *this, *fieldData );
	            CleanupStack::Pop(); //image
	            field->CaptionedControl()->DrawDeferred();
	            }
	        //text control updation
	        TPtrC text;
	        aLandmark->GetLandmarkName( text );
	        TPtrC dispTxt;
	        if( !IsEditable() )
	            {
	            TPtr des1 = CLmkFields::RemoveEnterCharacter( text );
	            dispTxt.Set(des1);
	            }
	        else
	            {
	            dispTxt.Set(text);
	            }
	        field->Control()->SetTextL( &dispTxt );
	        fieldData->SetTextL( text );
	        HBufC* buff = dispTxt.AllocL();
	        iLmkUiUtils->ChangeTitlePaneL( buff );
	        field->Control()->DrawDeferred();
	        }
	    }
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::SetFieldTypeToFocusInEditMode
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::SetFieldTypeToFocusInEditMode(TUint aFieldType)
	{
		iFieldTypeToFocus = aFieldType;
	}
// -----------------------------------------------------------------------------
// CLmkEditorImpl::IsDlgEditing
// -----------------------------------------------------------------------------
//
TBool CLmkEditorImpl::IsDlgEditing()
	{
	return iIsEditing;
	}
// -----------------------------------------------------------------------------
// CLmkEditorImpl::SetDlgEditing
// -----------------------------------------------------------------------------
//
void  CLmkEditorImpl::SetDlgEditing(TBool aEditing)
	{
	iIsEditing = aEditing;
	}
// -----------------------------------------------------------------------------
// CLmkEditorImpl::SetDlgEditing
// -----------------------------------------------------------------------------
//
TInt CLmkEditorImpl::EditMode()
    {
    return iEditorMode;
    }
// -----------------------------------------------------------------------------
// CLmkEditorImpl::UpdateMskOnArrowMoveForViewerL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::UpdateMskOnArrowMoveForViewerL()
	{
	TBool cmdAdded = EFalse;
    MLmkEditorField* field =
	iEditorFieldArray->Find( IdOfFocusControl() );
	TUint type = field->LandmarkItemField().FieldType();
	switch(type)
		{
		case ECategories:
			{
			AddMskCommandL(R_LM_MSK_SHOW,ELmkCmdShow);
			cmdAdded = ETrue;
			break;
			}
		case EPhoneNumber:
			{
			HBufC* fieldText = field->ControlTextL();
			CleanupStack::PushL( fieldText );
            if ( fieldText && fieldText->Length() > 0 )
	            {
	            AddMskCommandL(R_LM_MSK_CALL,ELmkCmdLast);
	            cmdAdded = ETrue;
	            }
	        CleanupStack::PopAndDestroy( fieldText );
			break;
			}
		case EWebAddress:
			{
			HBufC* fieldText =  field->ControlTextL();
			CleanupStack::PushL( fieldText );
			if ( fieldText && fieldText->Length() > 0 )
				{
				AddMskCommandL(R_LM_MSK_BROWSE,ELmkCmdGoToUrl);
				cmdAdded = ETrue;
				}
			CleanupStack::PopAndDestroy( fieldText );
			break;
			}
		default:
			{
			//nothing
			break;
			}
		}
		if (!cmdAdded)
			{
			AddMskCommandL(R_LM_MSK_EDIT,ELmkCmdEditLm);
			}
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::AddMskCommandL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::AddMskCommandL(TInt aResourceId,
								   TLmkAppCmdId aCommandId)
	{

	HBufC* mskTxt = NULL;
	mskTxt = StringLoader::LoadLC( aResourceId, iEikonEnv );
	CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
	cba->RemoveCommandFromStack(KMskCommandPos,iMskCmdId);
	iMskCmdId = aCommandId;
	iCurrentResourceId = aResourceId;
	cba->AddCommandToStackL(KMskCommandPos,iMskCmdId,mskTxt->Des());
	cba->MakeCommandVisible(iMskCmdId,ETrue);
	CleanupStack::PopAndDestroy( mskTxt );
	}
// -----------------------------------------------------------------------------
// CLmkEditorImpl::AddMskCommandL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::DimmMiddleSoftKey()
	{
	/*This is required, since there was an error,
	where the menu item appears if there are not items
	in the lis, after pressing the middle key.Hence, if
	dummy msk is added to the resource, it fixes the error,
	now this dummy button needs to be disabled.Check the
	dialog resource in editor.rss*/
	CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
	cba->MakeCommandVisible(EAknSoftkeyContextOptions,EFalse);
	}


// -----------------------------------------------------------------------------
// CLmkEditorImpl::ShowMultipleCategoriesInfoPopupL
// -----------------------------------------------------------------------------
//
 /*void CLmkEditorImpl::ShowMultipleCategoriesInfoPopupL(TBool aRefresh)
    {
    if (iCatInfoPopupNote && aRefresh)
	    {
	    delete iCatInfoPopupNote;
	    iCatInfoPopupNote = NULL;
	    }

    if (!iCatInfoPopupNote)
	    {

	    if(!IsCategoryField())
		    {
		    return;
		    }
	    MLmkFieldData* categoryField = iLmkFields->GetField( ECategories );
	    if ( !categoryField )
	        {
	        categoryField = iLmkFields->GetField( ECategory );
	        if ( !categoryField )
	            {
	            return;
	            }
	        }
	    RArray<TPosLmItemId> categories = categoryField->Categories();
	    
	    // This function is also called from message viewer, where the
	    // landmarks object is populated from xml file instead of db,
	    // hence the category id's will be dummy and must not be used,
	    // to get the category name, instead, get the category names
	    // from the xml file (iCategoryNames).
	    
	    TInt count( categories.Count() );
	    TBool isNotFromMessageViewer(EFalse);
	    count = iCategoryNames.Count();
	    if ( count == 0  ) // hack to determine from where the call coming.
	    	{
	    	//not called from message viewer
	    	isNotFromMessageViewer = ETrue;
	    	count = categories.Count();
	    	}
	    // Do not launch the popup for displaying one category name
		if (count <=1 )
		    {
			return;
		    }
   
		// ====================================================================	
		// Get Font for Formatting the infoPopUp strings 						
		// ====================================================================	

		// Get parameter and table limits for popup preview text window
		TAknLayoutScalableParameterLimits limits =
							AknLayoutScalable_Avkon::popup_preview_text_window_ParamLimits();

		TAknLayoutScalableTableLimits tableLimits =
							AknLayoutScalable_Avkon::popup_preview_text_window_t_Limits();

		// Get layout rects
		TRect rectScreen = iAvkonAppUi->ApplicationRect();
		TInt mainPaneVariety = Layout_Meta_Data::IsLandscapeOrientation() ? 4 : 1;

		TAknWindowLineLayout lineLayout =
							AknLayoutScalable_Avkon::main_pane( mainPaneVariety ).LayoutLine();
		TAknLayoutRect layoutRect;
		layoutRect.LayoutRect(rectScreen, lineLayout);
		TRect rectMainPane = layoutRect.Rect();

		// Use first variety to be able to get the font for text parsing
		TInt index = 0;
		if ( Layout_Meta_Data::IsLandscapeOrientation() )
			{
			index += 5;
			}
		TInt firstVariety = Min( Max( index, limits.FirstVariety() ), limits.LastVariety() );

		TAknWindowLineLayout lineLayout2 =
							AknLayoutScalable_Avkon::main_pane( mainPaneVariety ).LayoutLine();
		TAknLayoutRect layoutRect2;
		layoutRect2.LayoutRect(rectScreen, lineLayout2);
		TRect rectPopupWindow = layoutRect2.Rect();

		TInt firstIndex = tableLimits.FirstIndex();
		TInt firstLineVariety = AknLayoutScalable_Avkon::
		popup_preview_text_window_t_ParamLimits( firstIndex ).FirstVariety();

		TAknTextLineLayout popupTextLayout =
		AknLayoutScalable_Avkon::popup_preview_text_window_t(
		firstIndex, firstLineVariety );

		TAknLayoutText layoutText;
		layoutText.LayoutText( rectPopupWindow, popupTextLayout );
		TRect rectText = layoutText.TextRect();

		TInt infoPopupWidth = KMaxCatSizeForInfoPopUp;
		TInt infoPopupLength = KMinCatLenForInfoPopUp;

		if(rectText.Height() > 25)
			{
			infoPopupWidth = KMaxCatSizeForInfoPopUp + 80;
			}
		else if(rectText.Height() > 23)
			{
			infoPopupWidth = KMaxCatSizeForInfoPopUp + 55;
			}
		else if(rectText.Height() > 16)
			{
			infoPopupWidth = KMaxCatSizeForInfoPopUp + 10;
			infoPopupLength++;
			}

		// Prepare font to format the popupInfo string
		const CFont *font = layoutText.Font();

       	// =================================================================== 
       	// =================================================================== 

	    HBufC* catName = NULL;
	    // show the category list in a message query
		HBufC* message = HBufC::NewLC( (KPosLmMaxCategoryNameLength * count) + 1);
		TPtr msgPtr = message->Des();
	    CDesC16ArrayFlat* desArr = new (ELeave) CDesC16ArrayFlat(count);
	    CleanupStack::PushL(desArr);
		// fill up the array with category´s names from the engine.
		for ( TInt i = 0; i < count; i++ )
	        {
	        if ( !isNotFromMessageViewer )
	        	{
	        	catName = HBufC::NewL( KPosLmMaxCategoryNameLength + 1);
	        	catName->Des().Copy(iCategoryNames[i]);
	        	}
	        else
	        	{
	        	catName = CLmkDbUtils::CategoryNameL( iDb, categories[i] );
	        	}
	        CleanupStack::PushL( catName );

			TPtr des = catName->Des();

			TInt textWidth = font->TextWidthInPixels( des );
	        TInt textLength = des.Length();

	        // check the length of category name string to format for InfoPopUp.
			if(textLength > infoPopupLength && textWidth > infoPopupWidth)
				{

				// set the string as per KMinCatLenForInfoPopUp
				if(textLength > infoPopupLength + 9)
					{
					textLength = infoPopupLength + 9;
					des = des.Mid(0, textLength);
					}

	    		textWidth = font->TextWidthInPixels( des );

	    		for(TInt ctr=textLength; ctr >= (infoPopupLength-1);ctr--)
	    			{
	    			des = des.Mid(0, ctr);
	    			textWidth = font->TextWidthInPixels( des );
	    			if(textWidth <= infoPopupWidth)
	    				{
	    				break;
	    				}
	    			}

				// loading the "..." string
				HBufC* endString =  StringLoader::LoadLC(R_LM_EDITOR_MANY_CATEGORIES);

				// appending the "..." string at end
				des.Append(endString->Des());

				// appendig formatted string into category string array
				desArr->AppendL(des);
				CleanupStack::PopAndDestroy(endString); // endString
				}
			else
				{
		    	desArr->AppendL(des);
				}
	        CleanupStack::PopAndDestroy( catName ); //name
			}
		desArr->Sort();
		HBufC* catTxt = NULL;

		for ( TInt i = 0; i < desArr->Count(); i++ )
	        {
	        if (i <KMaxCatCntForInfoPopUp)
	            {
                msgPtr.Append((*desArr)[i]);
                if ( i != ( count - 1 ) )
                    {
                    msgPtr.Append(_L("\n"));
                    }
	            }
			}

		if (count > KMaxCatCntForInfoPopUp)
			{

			catTxt = StringLoader::LoadLC( R_LM_EDITOR_MANY_CATEGORIES, iEikonEnv );
            msgPtr.Append(catTxt->Des());
            CleanupStack::PopAndDestroy(catTxt );//catTxt
			}
		TPoint pt1;
		iCatInfoPopupNote = LmkNotes::ShowDelayedInfoPopupL(msgPtr,pt1);
		UpdateCatInfoNotePosition();
		iCatInfoPopupNote->ShowInfoPopupNote();
		CleanupStack::Pop();//desArr
	    delete	desArr;
		CleanupStack::PopAndDestroy( message ); //message
	    }
	else
	    {
	    UpdateCatInfoNotePosition();
	    iCatInfoPopupNote->ShowInfoPopupNote();
	    }
    }
*/

// -----------------------------------------------------------------------------
// CLmkEditorImpl::ShowQueryIfLocationFieldsAlreadyFilledL
// -----------------------------------------------------------------------------
//
TBool CLmkEditorImpl::ShowQueryIfLocationFieldsAlreadyFilledL()
  	{
  	TBool result = EFalse;
    TRealX lati;
    lati.SetNaN();
    TRealX longi;
    longi.SetNaN();

    MLmkFieldData* lat = iLmkFields->GetField(ELatitude);
	if(lat)
		{
        //get latitude control id
        TInt id = lat->UniqueFieldIdentity();
        MLmkEditorField* latcontrol = iEditorFieldArray->Find( id );
        //get latitude control current text
        lati = static_cast<CLmkEditorCoordinateField*>(latcontrol)->FieldValue();
        
        MLmkFieldData* lon = iLmkFields->GetField(ELongitude);
        if(lon)
            {
            //get longitude control id
            id = lon->UniqueFieldIdentity();
            MLmkEditorField* longcontrol = iEditorFieldArray->Find( id );
            //get longitude control current text
            longi = static_cast<CLmkEditorCoordinateField*>(longcontrol)->FieldValue();         
    		}
        
        if ( (!lati.IsNaN()) && (!longi.IsNaN()))
            {
            if ( !LmkNotes::ShowOwerriteLocationQueryL( iEikonEnv ) )
                {
                result = ETrue;
                }
            }
		}
	return result;
  	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::IsInvalidUriL
// -----------------------------------------------------------------------------
//
TBool CLmkEditorImpl::IsInvalidUriL()
	{
	TBool ret = EFalse;
	MLmkFieldData* fields = iLmkFields->GetField( EWebAddress );
	if(fields)
		{
		MLmkEditorField* fld = iEditorFieldArray->Find(fields->UniqueFieldIdentity());
		if(fld)
			{
			HBufC* webaddr = HBufC::NewLC(KMaxBufferLen);
			CLmkEditorTextField* field =	static_cast<CLmkEditorTextField*>(fld);
			TPtr ptr = webaddr->Des();
			field->Control()->GetText(ptr);
			ret = UriUtils::HasInvalidChars(ptr);
			CleanupStack::PopAndDestroy();//webaddr
			}
		}
	return ret;
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::HandleListProviderEvent
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::HandleListProviderEvent(TLmkListProviderEventType /*aEvent*/ )
	{
	TRAPD(err,CPosLandmark* landmark = iDb.ReadLandmarkLC(iLmkFields->LandmarkId());CleanupStack::PopAndDestroy( landmark ));
    if (err == KErrNotFound)
        {
        TRAP_IGNORE(DeleteSelfL());
        }
     else
     	{
     	//update the landmark viewer/editor
     	TRAP_IGNORE(UpdateViewDlgL());
     	}
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::HandleListProviderError
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::HandleListProviderError( TInt /*aError*/ )
	{
	//if error do nothing
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::LaunchLmkEditorL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::LaunchLmkEditorL()
	{
	CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
	cba->RemoveCommandFromStack( KMskCommandPos, iMskCmdId );
    // Launch editor
   TLmkEditorAttributes attributeFlags( CLmkEditorDlg::ELmkAll );
   // Check if the Landmark is alreayd present in the database. Incase,
   // a landmark is associated with the editor and if it is not
   // added to the Database, then the viewer would be launched with
   // the lanmark content
    if( KPosLmNullItemId == iLmkFields->LandmarkId() && iLandmark )
        {
        iEditor = CLmkEditorImpl::NewL( iDb,
                                    iSender,
                                    attributeFlags,
                                    CLmkEditorDlg::ELmkEditor,
                                    iLandmark );
        }
    else
        {
        iEditor = CLmkEditorImpl::NewL( iDb,
                                    iSender,
                                    attributeFlags,
                                    CLmkEditorDlg::ELmkEditor,
                                    iLmkFields->LandmarkId() );
        }


	MLmkEditorField* field = iEditorFieldArray->Find( IdOfFocusControl() );
	iFieldTypeToFocus = field->LandmarkItemField().FieldType();
    iEditor->SetFieldTypeToFocusInEditMode( iFieldTypeToFocus );
    iEditor->SetDlgEditing( ETrue );
    iIsEditing = ETrue;
    if( !iMapNavFeature )
	    {
	    iEditor->DisableMapAndNavigationMenuOptions();
	    }
    iEditor->ExecuteLD();
    iEditor->SetDlgEditing( EFalse );
    iIsEditing = EFalse;
    TInt err = KErrNone;
    TRAP( err, CPosLandmark* landmark = iDb.ReadLandmarkLC( iLmkFields->LandmarkId() ); CleanupStack::PopAndDestroy( landmark ) );
    if (err == KErrNotFound)
        {
        iLmkUiUtils->RestoreOldNaviPaneL();
        delete this;
        return;
        }
    AddMskCommandL( iCurrentResourceId, iMskCmdId );
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::DeleteSelfL
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::DeleteSelfL()
	{
	iLmkUiUtils->RestoreOldNaviPaneL();
    if(iEditor)
    	{
    	//delete only editor,if editor instance is present,
    	//viewer will be deleted in ProcessCommandL, where
    	//editor was created.
    	delete iEditor;
    	iEditor = NULL;
    	}
    else
    	{
    	delete this;
    	}
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::HandleCentralRepositoryChangeL()
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::HandleCentralRepositoryChangeL()
	{
	//change unit for position accuracy
	MLmkFieldData* fldData = iLmkFields->GetField( EPositionAccuracy );
	if(fldData)
		{
		TInt id1 = fldData->UniqueFieldIdentity();
		CLmkEditorNumberField* field = static_cast<CLmkEditorNumberField*>(iEditorFieldArray->Find( id1 ));
		if(field)
			field->HandleUnitChangeL();
		}

	//change altitude unit
	MLmkFieldData* fldData1 = iLmkFields->GetField( EAltitude );
	if(fldData1)
		{
		TInt id2 = fldData1->UniqueFieldIdentity();
		CLmkEditorNumberField* field1 = static_cast<CLmkEditorNumberField*>(iEditorFieldArray->Find( id2 ));
		if(field1)
			field1->HandleUnitChangeL();
		}

	//change altitude accuracy unit
	MLmkFieldData* fldData2 = iLmkFields->GetField( EAltitudeAccuracy );
	if(fldData2)
		{
		TInt id3 = fldData2->UniqueFieldIdentity();
		CLmkEditorNumberField* field2 = static_cast<CLmkEditorNumberField*>(iEditorFieldArray->Find( id3 ));
		if(field2)
			field2->HandleUnitChangeL();
		}
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::UpdateCatInfoNotePosition()
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::UpdateCatInfoNotePosition()
	{
	//this is only to set the position of the
	//cat info note.
	if(iCatInfoPopupNote && IsCategoryField())
		{
		CEikCaptionedControl* tmpline = CurrentLine();
		TPoint pt1 = tmpline->PositionRelativeToScreen();
		pt1.iX+=KHOffsetCatInfoPopup;
		pt1.iY+=KVOffsetCatInfoPopup;
		iCatInfoPopupNote->SetPositionAndAlignment(pt1,EHLeftVTop);
		}
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::IsCategoryField()
// -----------------------------------------------------------------------------
//
TBool CLmkEditorImpl::IsCategoryField()
	{
	//to check whether the current focussed field is a
	//category field.
	TBool result = ETrue;
	MLmkEditorField* field =
			iEditorFieldArray->Find( IdOfFocusControl() );
	TUint type = field->LandmarkItemField().FieldType();
    if(type != ECategories && type != ECategory)
	    {
	    result = EFalse;
	    }
	return result;
	}


#ifdef RD_SCALABLE_UI_V2
// -----------------------------------------------------------------------------
// CLmkEditorImpl::HandleDialogPageEventL
// -----------------------------------------------------------------------------
//

void CLmkEditorImpl::HandleDialogPageEventL(TInt /*aEventID*/)
	{
    /*
	if(IsEditable())
		{
		//editor
		HandleEditorCommandL();
		}
	*/	
	}

#endif //RD_SCALABLE_UI_V2

// -----------------------------------------------------------------------------
// CLmkEditorImpl::LineChangedL
// -----------------------------------------------------------------------------
//

void CLmkEditorImpl::LineChangedL(TInt /*aControlId*/)
	{

	//hide the category info pop-up if active
    if(iCatInfoPopupNote)
		{
		iCatInfoPopupNote->HideInfoPopupNote();
		}
	MLmkEditorField* field = iEditorFieldArray->Find( IdOfFocusControl() );
	__ASSERT_DEBUG( field, Panic( KLmkPanicNullMember ) );
	TUint type = field->LandmarkItemField().FieldType();
   	if (IsEditable())
    	{
    	//editor
    	if ( type == ECategory || type == ECategories )
			 {
			 AddMskCommandL(R_LM_MSK_EDIT,ELmkCmdSelectCategories);
			 }
		 else
			 {
			 CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
			 cba->MakeCommandVisible(iMskCmdId,EFalse);
			 }
    	}
    else
        {
        //viewer
        UpdateMskOnArrowMoveForViewerL();
        }
   	if ( type == ECategory || type == ECategories )
    	{
    	// ShowMultipleCategoriesInfoPopupL(IdOfFocusControl());
    	}

	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::ClearPositionInfoL()
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::ClearPositionInfoL()
	{
	//clear latitude
	MLmkFieldData* lat = iLmkFields->GetField(ELatitude);
	if(lat)
		{
		TInt id = lat->UniqueFieldIdentity();
		MLmkEditorField* latcontrol = iEditorFieldArray->Find( id );
		if(latcontrol)
			{
			latcontrol->ResetL();
			}
		}
	//clear longitude
	MLmkFieldData* lon = iLmkFields->GetField(ELongitude);
	if(lon)
		{
		TInt id = lon->UniqueFieldIdentity();
		MLmkEditorField* longcontrol = iEditorFieldArray->Find( id );
		if(longcontrol)
			{
			longcontrol->ResetL();
			}
		}
	//clear position accuracy
	MLmkFieldData* fldData = iLmkFields->GetField( EPositionAccuracy );
	if(fldData)
		{
		TInt id1 = fldData->UniqueFieldIdentity();
		CLmkEditorNumberField* field = static_cast<CLmkEditorNumberField*>(iEditorFieldArray->Find( id1 ));
		if(field)
			{
			field->ResetL();
			}
		}
	//clear altitude
	MLmkFieldData* fldData1 = iLmkFields->GetField( EAltitude );
	if(fldData1)
		{
		TInt id2 = fldData1->UniqueFieldIdentity();
		CLmkEditorNumberField* field1 = static_cast<CLmkEditorNumberField*>(iEditorFieldArray->Find( id2 ));
		if(field1)
			{
			field1->ResetL();
			}
		}
	//clear altitude accuracy
	MLmkFieldData* fldData2 = iLmkFields->GetField( EAltitudeAccuracy );
	if(fldData2)
		{
		TInt id3 = fldData2->UniqueFieldIdentity();
		CLmkEditorNumberField* field2 = static_cast<CLmkEditorNumberField*>(iEditorFieldArray->Find( id3 ));
			if(field2)
			{
			field2->ResetL();
			}
		}
		
	DrawNow();
	
	}	

// -----------------------------------------------------------------------------
// CLmkEditorImpl::ArePositionFieldEmptyL()
// -----------------------------------------------------------------------------
//
TBool CLmkEditorImpl::ArePositionFieldEmptyL()
	{
	return  (IsFieldEmptyL( ELatitude ) && IsFieldEmptyL( ELongitude ) && IsFieldEmptyL( EAltitude )
			  && IsFieldEmptyL( EAltitudeAccuracy ) && IsFieldEmptyL( EPositionAccuracy)) ? ETrue : EFalse;
	}

#ifdef RD_SCALABLE_UI_V2
// -----------------------------------------------------------------------------
// CLmkEditorImpl::HandleEditorCommandL()
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::HandleEditorCommandL()
	{
	MLmkEditorField* field = iEditorFieldArray->Find( IdOfFocusControl() );
	__ASSERT_DEBUG( field, Panic( KLmkPanicNullMember ) );

	TUint type = field->LandmarkItemField().FieldType();
	switch(type)
		{
		case ECategories:
		case ECategory:
			{
			ProcessCommandL( ELmkCmdSelectCategories );
			break;
			}
		}
	}
#endif //RD_SCALABLE_UI_V2

// -----------------------------------------------------------------------------
// CLmkAppLmItemSelectorImpl::HandleDialogDismissed
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::HandleDialogDismissed( TInt aButtonId )
	{
	if( EEikBidCancel == aButtonId )
		{
		delete iLocationService;
    	iLocationService = NULL;
		}
	}
// -----------------------------------------------------------------------------
// CLmkEditorImpl::InsertLineL()
// -----------------------------------------------------------------------------
//
MLmkEditorField* CLmkEditorImpl::InsertLineL(  MLmkFieldData*  aFieldData,
								   TInt aPrevControlId)
	{
	MLmkEditorField* editorField = NULL;
	//data present, but control not present
	TryChangeFocusL(aPrevControlId);
	editorField = LmkEditorFieldFactory::CreateFieldL(
	                                        *aFieldData, *this );
    CleanupStack::PushL( editorField );
    if ( editorField )
        {
        editorField->SetEditableL(EFalse);
        iEditorFieldArray->GetFieldArray().AppendL( editorField );
        editorField->ActivateL();
        }
    CleanupStack::Pop(); //editorField
    return editorField;
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::DeleteLineL()
// -----------------------------------------------------------------------------
//
void CLmkEditorImpl::DeleteLineL( MLmkEditorField* aEditorField,
								  TInt aCtrlId , TInt aPos)
	{
	//data not present but control present
	//iEditorFieldArray->GetFieldArray().Delete( aPos-1 );
	iEditorFieldArray->GetFieldArray().Delete( aPos );
	delete aEditorField;
	aEditorField=NULL;
	DeleteLine(aCtrlId);
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::AddOrDeleteLineL()
// -----------------------------------------------------------------------------
//
MLmkEditorField* CLmkEditorImpl::AddOrDeleteLineL( MLmkEditorField* aEditorField,
												   MLmkFieldData*  aFieldData ,
												   TInt aPrevControlId,
								  				   TInt aCtrlId , TInt aPos)
	{
	TBool valPresent = iEditorFieldArray->CheckIfFieldIsEmpty(*aFieldData);

	if(valPresent && !aEditorField)
		{
		aEditorField = InsertLineL( aFieldData, aPrevControlId);
		}
	else if(!valPresent && aEditorField)
		{
		DeleteLineL(aEditorField, aCtrlId,aPos);
		aEditorField = NULL;
		}
	return aEditorField;
	}

// -----------------------------------------------------------------------------
// CLmkEditorImpl::MessageQueryCallBack()
// -----------------------------------------------------------------------------
//
TInt CLmkEditorImpl::MessageQueryCallBack( TAny* /*aPtr*/ )
	{
	TInt result = KErrNone;
	CEikonEnv* env = CEikonEnv::Static();
	TRAP_IGNORE(result = LmkNotes::GenericMessageQueryL(env,R_LMK_REQUEST_FAIL_INFO_TEXT,
									R_LMK_MSG_QUERY_HEADING_PANE_TEXT));
	return result;
	}

// ---------------------------------------------------------
// CLmkEditorImpl::CheckHideCoordinateL()
// ---------------------------------------------------------
//
void CLmkEditorImpl::CheckHideCoordinateL()
	{
    TInt coordinateSettingValue = ELocCoordinatesNotHidden;
    TInt helpSettingValue = ELocHelpNotHidden;
    CRepository* repository = CRepository::NewL( TUid::Uid( KCRUidLocLocalVariation) );
    CleanupStack::PushL( repository );
    User::LeaveIfError( repository->Get( KLocHideCoordinates,
                                              coordinateSettingValue ));  
    User::LeaveIfError( repository->Get( KLocHideHelp,
                                              helpSettingValue ));  
    CleanupStack::PopAndDestroy( repository );
	
	if( ELocCoordinatesHidden == coordinateSettingValue )
		iIsHideCoordinate = ETrue;
	else
		iIsHideCoordinate = EFalse;
	
	if( ELocHelpHidden == helpSettingValue )
		iIsHideHelp = ETrue;
	else
		iIsHideHelp = EFalse;
	
	}

// ---------------------------------------------------------
// CLmkEditorImpl::HandlePointerEventL()
// ---------------------------------------------------------
//
void CLmkEditorImpl::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    if (aPointerEvent.iType == TPointerEvent::EButton1Up && iIsDragging
            == EFalse)
        {
        MLmkEditorField* field = iEditorFieldArray->Find(IdOfFocusControl());
        TUint type = field->LandmarkItemField().FieldType();

        // check the editor mode
        if (IsEditable()) // edit mode
            {
            if (type == ECategory || type == ECategories)
                {
                if (!iIsCategorySelctorOpen)
                    {
                    iIsCategorySelctorOpen = ETrue;
                    SelectCategoriesCmdL();
                    }
                }
            }
        else
            {// view or receive mode
            if (type == ECategory || type == ECategories)
                {
                if (iCalegorySelectionFlag) // Receive mode
                    {
                    CategoriesCmdFromXmlL();
                    }
                else
                    {
                    CategoriesCmdL(); // View mode
                    }
                }
            else // focus is on any other landmark field
                {
                if (!iCalegorySelectionFlag)// No context menu for receive mode
                    {
                    if (!iIsEditing)
                        {
                        LaunchLmkEditorL();
                        }
                    }
                }
            }
        }
    else
        {
        CAknForm::HandlePointerEventL(aPointerEvent);

        if (aPointerEvent.iType == TPointerEvent::EButton1Down)
            {
            iIsDragging = EFalse;
            }

        if (aPointerEvent.iType == TPointerEvent::EDrag)
            {
            iIsDragging = ETrue;
            }
        }
    }

//  End of File

