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
 * Description:    LandmarksUi Packaged Landmarks Selector View Implementation
 *
 */

// SYSTEM INCLUDE FILES
#include <featmgr.h>
#include <AknDef.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <EPos_CPosLmCategoryManager.h>
#include <AknsConstants.h>
#include <AknIconArray.h>
#include <eikmenup.h>
#include <eikspane.h>
#include <AknWaitDialog.h>
#include <e32math.h>
#include <hlplch.h>
#include <csxhelp/lmmes.hlp.hrh>
#include <avkon.mbg>
#include <lmkui.rsg>

// USER INCLUDE FILES
#include "CLmkSaveLandmarkUtils.h"
#include "CLmkDlgPackageSelectorImpl.h"
#include "CLmkLmItemListProvider.h"
#include "LmkListProviderFactory.h"
#include "CLmkMultiSelectorDialog.h"
#include "CLmkParser.h"
#include "LmkConsts.h"
#include "LmkUID.h"
#include "CLmkSender.h"
#include "CLmkPackageEditorImpl.h"
#include "CLmkDlgSelectorImplBase.h"
#include "CLmkUiUtils.h"
#include "CLmkSelectorDialog.h"
#include "CLmkSelectorLBModelBase.h"
#include "CLmkSingleGraphicLBModel.h"
#include "CLmkMultiSelectionLBModel.h"
#include "CLmkLmItemListMemento.h"
#include "CLmkSelectorIconMgr.h"
#include "CLmkAOOperation.h"

// ================= LOCAL FUNCTIONS =======================
static void CleanupArray(TAny* aArray)
    {
    ( static_cast<RPointerArray<CPosLandmark>*>( aArray ) )->ResetAndDestroy();
    }
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkDlgPackageSelectorImpl::CLmkDlgPackageSelectorImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkDlgPackageSelectorImpl::CLmkDlgPackageSelectorImpl(
    CPosLandmarkDatabase& aDb,
    CLmkParser& aParser,
    CLmkSender& aSender )
    : CLmkDlgLmSelectorImpl( aDb ),
      iParser( aParser ),
      iSender( aSender ),
      iDb(aDb)
    {
    }

// -----------------------------------------------------------------------------
// CLmkDlgPackageSelectorImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkDlgPackageSelectorImpl::ConstructL()
    {
    CLmkDlgLmSelectorImpl::ConstructL();
    iMapNavInterface = CLmkMapNavigationInterface::NewL( iDb );
    iMapNavInterface->AttachAIWInterestL(R_LMK_PKGVIEWER_MENU, R_LMK_PACKAGE_AIW_INTEREST_SHOWONMAP);
     for (TUint i(0); i < iParser.NumOfParsedLandmarks(); i++)
	    {
	    User::LeaveIfError( iTotalLms.Append(i) );
	    }
	iSaveLmUtils = CLmkSaveLandmarkUtils::NewL(iDb,iParser);
	//setting of observer is needed for the search operation.
	iSaveLmUtils->SetObserver(this);
    }

// -----------------------------------------------------------------------------
// CLmkDlgPackageSelectorImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkDlgPackageSelectorImpl* CLmkDlgPackageSelectorImpl::NewL(
    CPosLandmarkDatabase& aDb,
    CLmkParser& aParser,
    CLmkSender& aSender )
    {
    CLmkDlgPackageSelectorImpl* self =
        new( ELeave ) CLmkDlgPackageSelectorImpl( aDb, aParser, aSender );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CLmkDlgPackageSelectorImpl::~CLmkDlgPackageSelectorImpl
// ----------------------------------------------------
//
CLmkDlgPackageSelectorImpl::~CLmkDlgPackageSelectorImpl()
    {
    if(iMapNavInterface)
    	{
    	iMapNavInterface->Release();
    	}
    iTotalLms.Close();
    delete iSaveLmUtils;
    }

// ----------------------------------------------------
// CLmkDlgPackageSelectorImpl::HelpContext
// ----------------------------------------------------
//
TCoeHelpContext CLmkDlgPackageSelectorImpl::HelpContext() const
    {
    return TCoeHelpContext( TUid::Uid( KLmkMsgViewerAppUID3 ),
                            KLM_HLP_RECEIVED_PACKAGE );
    }

// ----------------------------------------------------
// CLmkDlgPackageSelectorImpl::SetupListProviderL
// ----------------------------------------------------
//
void CLmkDlgPackageSelectorImpl::SetupListProviderL()
    {
    if ( !iListProvider )
        {
        iListProvider =
            LmkListProviderFactory::CreateProviderL( iParser );
        iListProvider->AddObserverL( *this );
        iListProvider->InitializeL();
        }
    }

// ----------------------------------------------------
// CLmkDlgPackageSelectorImpl::GetDlgResources
// ----------------------------------------------------
//


void CLmkDlgPackageSelectorImpl::GetDlgResources(
    TBool /*aIsSingleSelector*/,
    TInt& aTitlePaneResource,
    TInt& aMenuBarResource,
    TInt& aDialogResource ) const
    {
    aTitlePaneResource = KZeroResourceId;
    aMenuBarResource = R_LMK_PKGVIEWER_MENUBAR;
    aDialogResource = R_LMK_PKGVIEWER_OPTIONS_BACK_DLG;
    }



// ----------------------------------------------------
// CLmkDlgPackageSelectorImpl::HandleLaunchingEventL
// ----------------------------------------------------
//
void CLmkDlgPackageSelectorImpl::HandleLaunchingEventL()
    {
    // Call base class implementation:
    CLmkDlgSelectorImplBase::HandleLaunchingEventL();

    // cast is safe, package selector is always multiselector
    static_cast<CLmkMultiSelectorDialog*>( iDialog )->SetupSendMenu(
                    iSender, R_LMK_PKGVIEWER_MENU, R_LMK_SENDUI_MENU_TEXT );
    }

// -----------------------------------------------------------------------------
// CLmkDlgPackageSelectorImpl::HandleOperationL
// -----------------------------------------------------------------------------
//
void CLmkDlgPackageSelectorImpl::HandleOperationL(
                                       TOperationTypes aType,
                                       TReal32 /*aProgress*/,
                                       TInt aStatus )
    {
    if(aType == ENewPkgCategory && aStatus != KPosLmOperationNotComplete)
    	{
    	if (iSaveLmUtils->CategoryAddConfirmationQueryL(iNoOfNewCategories,iNewCategoryFound) )
			{
			iType  = EImport;
			iParser.StartImportingL( iTotalLms, ETrue );// Save landmarks with new categories
			}
		else
			{
			iType  = EImport;
			iParser.StartImportingL( iTotalLms , EFalse); //Save landmarks without new categories
			}
    	}
    else if( (aType == EImport ) && (aStatus != KPosLmOperationNotComplete && iWaitNote ))
    	{
    	iWaitNote->ProcessFinishedL();
        iWaitNote = NULL;
    	}
    }

// ----------------------------------------------------
// CLmkDlgPackageSelectorImpl::GetLandmarksInPackageL
// ----------------------------------------------------
//
void CLmkDlgPackageSelectorImpl::GetLandmarksInPackageL(RPointerArray<CPosLandmark> &aLmkArray)
	{
	const CArrayFix<TInt>* indexes = iDialog->ListBox()->SelectionIndexes();
	CPosLandmark* lmk;
	if(indexes->Count() == 0)
		{
		lmk = iParser.LandmarkLC(iDialog->ListBox()->CurrentItemIndex());
		aLmkArray.Append(lmk);
		CleanupStack::Pop();//iParser.LandmarkLC
		}
	else
		{
		TInt count = indexes->Count();
		for(TInt i = 0;i < count; ++i)
			{
			TInt index = indexes->At(i);
			lmk = iParser.LandmarkLC(index);
			aLmkArray.Append(lmk);
			CleanupStack::Pop();//iParser.LandmarkLC
			}
		}
	}

// ----------------------------------------------------
// CLmkDlgPackageSelectorImpl::ProcessCommandL
// ----------------------------------------------------
//
void CLmkDlgPackageSelectorImpl::ProcessCommandL( TInt aCommandId )
    {
    TInt aiwCmd = iMapNavInterface->GetServiceCmdByMenuCmd(aCommandId);

    if(KAiwCmdNone != aiwCmd)
    	{
    	RPointerArray<CPosLandmark> lmkArray;
    	switch(aiwCmd)
    		{
    		case KAiwCmdMnShowMap:
    			{
    			GetLandmarksInPackageL(lmkArray);
    			CleanupStack::PushL( TCleanupItem( CleanupArray, &lmkArray ) );
    			iMapNavInterface->ShowLandmarksOnMapL(lmkArray, aCommandId,
    												CLmkMapNavigationInterface::EByLmkView);
    			CleanupStack::PopAndDestroy(); // lmkArray
    			break;
    			}
    		default:
    			{
    			break;
    			}
    		}
    	return;
    	}
    switch ( aCommandId )
        {
        case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( iEnv->WsSession(),
                                iEnv->EikAppUi()->AppHelpContextL() );
            break;
            }
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
            {
            SendSelectedLandmarksCmdL(aCommandId);
            break;
            }
        case ELmkCmdOpenLm:
            {
            OpenLandmarkCmdL();
            break;
            }
        case ELmkCmdSaveSelectedLms:
            {
            SaveLandmarksCmdL();
            break;
            }
        case ELmkCmdSaveAllLms:
            {
            SaveAllLandmarksCmdL();
            break;
            }
        case ELmkCmdSendSelectedLms:
            {
            SendSelectedLandmarksCmdL(aCommandId);
            break;
            }
        case ELmkCmdSendAllLms:
            {
            SendAllLandmarksCmdL(aCommandId);
            break;
            }
        default:
            {
            // Command not handled here, forward it to the app ui.
            static_cast<MEikCommandObserver*>(iEnv->EikAppUi())->ProcessCommandL( aCommandId );
            break;
            }
        }
    }

// ----------------------------------------------------
// CLmkDlgPackageSelectorImpl::DynInitDlgMenuPaneL
// ----------------------------------------------------
//
void CLmkDlgPackageSelectorImpl::DynInitDlgMenuPaneL(TInt aResourceId,
        CEikMenuPane* aMenuPane)
    {
    iMapNavInterface->HandleMenuOperationL(aResourceId, aMenuPane,
            ELmkCmdMnNav);

    TInt markedCount = ListMarkedItemCountL();

    switch (aResourceId)
        {
        case R_LMK_PKGVIEWER_MENU:
            {
            if (FeatureManager::FeatureSupported(KFeatureIdLandmarksConverter))
                {
                aMenuPane->SetItemDimmed(ELmkCmdSendDummy, EFalse);
                }
            else
                {
                aMenuPane->SetItemDimmed(ELmkCmdSendDummy, ETrue);
                }

            if (FeatureManager::FeatureSupported(KFeatureIdHelp)
                    && !iIsHideHelp)
                {
                aMenuPane->SetItemDimmed(EAknCmdHelp, EFalse);
                }
            else
                {
                aMenuPane->SetItemDimmed(EAknCmdHelp, ETrue);
                }

            if (markedCount < 1)
                {
                aMenuPane->SetItemDimmed(ELmkCmdOpenLm, ETrue);
                }
            else if (markedCount == 1)
                {
                aMenuPane->SetItemDimmed(ELmkCmdOpenLm, EFalse);
                }
            else if (markedCount > 1)
                {
                aMenuPane->SetItemDimmed(ELmkCmdOpenLm, ETrue);
                }

            TBool isLandmarkDataEmpty = EFalse;

            // check for show on map for single selected landmark
            if (markedCount == 1)
                {
                CLmkLandmark* lmklandmark = iParser.LmkLandmarkLC(
                        iDialog->CurrentItemIndex());
                if (IsLandmarkDataEmptyL(lmklandmark->PosLandmark()))
                    {
                    isLandmarkDataEmpty = ETrue;
                    }
                CleanupStack::PopAndDestroy(lmklandmark); //lmklandmark
                }
            else if (markedCount < 1)
                {
                isLandmarkDataEmpty = ETrue;
                }

            TInt showOnMapCmd = -1;
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
                }

            if (isLandmarkDataEmpty)
                {
                aMenuPane->SetItemDimmed(showOnMapCmd, ETrue);
                }
            else if (showOnMapCmd != -1)
                {
                aMenuPane->SetItemDimmed(showOnMapCmd, EFalse);
                }
            break;
            }
        case R_LMK_PKGVIEWER_SAVE_LANDMARK_SUBMENU:
            {
            if (markedCount < 1)
                {
                aMenuPane->SetItemDimmed(ELmkCmdSaveSelectedLms, ETrue);
                }
            break;
            }
        case R_LMK_PKGVIEWER_SEND_LANDMARK_SUBMENU:
            {
            if (markedCount < 1)
                {
                aMenuPane->SetItemDimmed(ELmkCmdSendSelectedLms, ETrue);
                }
            break;
            }
        default:
            break;
        }
    }

// ----------------------------------------------------
// CLmkDlgPackageSelectorImpl::OpenLandmarkCmdL
// ----------------------------------------------------
//
void CLmkDlgPackageSelectorImpl::OpenLandmarkCmdL()
    {
    CLmkPackageEditorImpl* editorImpl =
        CLmkPackageEditorImpl::NewL( iDb, iParser, iSender,
                                     iDialog->CurrentItemIndex() );
    iParser.ChangeObserver(*editorImpl);
    editorImpl->ExecuteLD();
    // Restore the parser's observer back to LmkPkg Selector
    iParser.ChangeObserver(*this);
    }

// ----------------------------------------------------
// CLmkDlgPackageSelectorImpl::SaveLandmarksCmdL
// ----------------------------------------------------
//
void CLmkDlgPackageSelectorImpl::SaveLandmarksCmdL()
    {
    const CArrayFix<TInt>* markedIndexes = iDialog->SelectionIndexes();
    RArray<TUint> importIndexes;
    CleanupClosePushL( importIndexes );
    TInt count( markedIndexes->Count() );
    TUint markedIndex (0);
    if ( count > 0 )
        {
        for ( TInt i( 0 ); i < count; ++i )
            {
            User::LeaveIfError(
                importIndexes.Append( markedIndexes->At( i ) ) );
            }
        markedIndex = importIndexes[0];
        }
    else
        { // only highlighted item, no marked items
        User::LeaveIfError(
            importIndexes.Append( iDialog->CurrentItemIndex() ) );
        markedIndex = iDialog->CurrentItemIndex();
        }


 	CLmkSaveLandmarkUtils* saveLmUtils = CLmkSaveLandmarkUtils::NewL(iDb,iParser);
	CleanupStack::PushL(saveLmUtils );

	RArray<TCategoryName> newCategories;
	CleanupClosePushL(newCategories);
	TCategoryName newCategoryFound;

	TInt noOfNewCategories = saveLmUtils->CheckForNewCategoriesL( importIndexes,newCategoryFound,newCategories);

	if (saveLmUtils->CategoryAddConfirmationQueryL( noOfNewCategories,newCategoryFound) )
		{
		iParser.StartImportingL( importIndexes, ETrue );// Save landmarks with new categories
		}
	else
		{
		iParser.StartImportingL( importIndexes , EFalse); //Save landmarks without new categories
		}
    // wait note
    iWaitNote = new (ELeave) CAknWaitDialog( NULL, ETrue );
    if ( !iWaitNote->ExecuteLD( R_LMK_IMPORTING_WAIT_NOTE ) )
        {
        //making iWaitNote null, since framework destroys waitnote
        //dialog on pressing cancel.
        iWaitNote = NULL;
        iParser.CancelImporting();
       	CleanupStack::PopAndDestroy(3); // newCategories, saveLmUtils,totalLms
		return;
        }

    // Saved confirmation note
    saveLmUtils->LmsSavedConfirmationNoteL(importIndexes, markedIndex);

	iDialog->ListBox()->ClearSelection();
	iDialog->UpdateMskL();
    CleanupStack::PopAndDestroy(3); // newCategories, saveLmUtils,importIndexes
    }

// ----------------------------------------------------
// CLmkDlgPackageSelectorImpl::SaveAllLandmarksCmdL
// ----------------------------------------------------
//
void CLmkDlgPackageSelectorImpl::SaveAllLandmarksCmdL()
	{

	RArray<TCategoryName> newCategories;
	CleanupClosePushL(newCategories);
	iSaveLmUtils->StartSearchNewCatL(iNoOfNewCategories,newCategories,iNewCategoryFound);
	iType  = ENewPkgCategory;
	// wait note
	iWaitNote = new (ELeave) CAknWaitDialog( NULL, ETrue );
	if ( !iWaitNote->ExecuteLD( R_LMK_IMPORTING_WAIT_NOTE ) )
		{
		//making iWaitNote null, since framework destroys waitnote
	    //dialog on pressing cancel.
	    iWaitNote = NULL;
		(iType == ENewPkgCategory )?iSaveLmUtils->CancelSearchNewCat():iParser.CancelImporting();
	    CleanupStack::PopAndDestroy();//newCategories
		return;
		}
	// Saved confirmation note
    iSaveLmUtils->LmsSavedConfirmationNoteL( iTotalLms );
    CleanupStack::PopAndDestroy();//newCategories
    }

// ----------------------------------------------------
// CLmkDlgPackageSelectorImpl::SendSelectedLandmarksCmdL
// ----------------------------------------------------
//
void CLmkDlgPackageSelectorImpl::SendSelectedLandmarksCmdL(TInt aCommandId)
    {
    RPointerArray<CLmkLandmark> lmkArray;
    const CArrayFix<TInt>* constMarkedIndexes = iDialog->SelectionIndexes();
    TUint index(0);
    TBool oneItemSelected = EFalse;
    if (constMarkedIndexes->Count() == 0)
        {
        oneItemSelected = ETrue;
        }
    TInt markedItemCount = constMarkedIndexes->Count();
    TInt i(0);
    while( markedItemCount >0 || oneItemSelected)
	    {
	    if(oneItemSelected)
		    {
		    index = iDialog->CurrentItemIndex();
		    oneItemSelected = EFalse;
		    }
		else
			{
			index = constMarkedIndexes->At(i);
			i++;
			}
		CLmkLandmark* lmklandmark = iParser.LmkLandmarkLC(index);
        User::LeaveIfError(lmkArray.Append(lmklandmark));
        CleanupStack::Pop();//lmklandmark
        --markedItemCount;
	    }
	iDialog->ListBox()->ClearSelection();
	iDialog->UpdateMskL();
    iSender.SendLandmarksL(aCommandId,iParser.PackageName(),lmkArray);
    lmkArray.ResetAndDestroy();
    }

// ----------------------------------------------------
// CLmkDlgPackageSelectorImpl::SendAllLandmarksCmdL
// ----------------------------------------------------
//
void CLmkDlgPackageSelectorImpl::SendAllLandmarksCmdL(TInt aCommandId)
    {

    RPointerArray<CLmkLandmark> lmkArray;
    for ( TInt i(0); i < iParser.NumOfParsedLandmarks(); i++ )
        {
        CLmkLandmark* lmklandmark = iParser.LmkLandmarkLC(i);
        User::LeaveIfError(lmkArray.Append(lmklandmark));
        CleanupStack::Pop();//lmklandmark
        }

	iSender.SendLandmarksL(aCommandId,iParser.PackageName(),lmkArray);
    lmkArray.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CLmkDlgPackageSelectorImpl::IsLandmarkDataEmptyL
// -----------------------------------------------------------------------------
//
TBool CLmkDlgPackageSelectorImpl::IsLandmarkDataEmptyL(
        CPosLandmark* aLandmark)
    {
    // verify that destination landmark has coordinates
    TLocality loc;
    TBool isValid = (KErrNone == aLandmark->GetPosition(loc));
    isValid &= !Math::IsNaN(loc.Latitude()) && !Math::IsNaN(loc.Longitude());
    return !isValid;
    
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

//  End of File
