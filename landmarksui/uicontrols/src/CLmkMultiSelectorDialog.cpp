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
#include <featmgr.h>
#include <sendnorm.rsg> // R_SENDUI_MENU
#include <aknsfld.h>
#include <StringLoader.h>
#include <lmkui.rsg>
#include <lmkerrors.h>

#include "CLmkMultiSelectorDialog.h"
#include "MLmkDlgMenuObserver.h"
#include "MLmkNavigationObserver.h"
#include "LmkConsts.h"
#include "landmarks.hrh"
#include "CLmkSender.h"


// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

#if defined(_DEBUG)

_LIT( KPanicMsg, "CLmkMultiSelectorDialog" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
#endif
}  // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkMultiSelectorDialog::CLmkMultiSelectorDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkMultiSelectorDialog::CLmkMultiSelectorDialog(
    TInt& aOpenedItemIndex,
    CArrayFix<TInt>* aSelectionIndexArray,
    MDesCArray* aLbModel,
    TInt aMenuBarResourceId,
    TInt aOkOptionsMenuResourceId,
    MEikCommandObserver* aCmdObserver,
    MLmkDlgMenuObserver* aMenuObserver,
    MLmkListMemento* aMemento )
    : CLmkSelectorDialog( aOpenedItemIndex, aLbModel, aCmdObserver,aMemento ),
      iMenuBarResourceId( aMenuBarResourceId ),
      iOkOptionsMenuResourceId( aOkOptionsMenuResourceId ),
      iSendUiMenuPaneResourceId( KZeroResourceId ),
      iSelectionIndexArray( aSelectionIndexArray ),
      iMenuObserver( aMenuObserver )
    {
    iExited = EFalse;
    iIsToClearSelection = EFalse;
    }

// -----------------------------------------------------------------------------
// CLmkMultiSelectorDialog::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkMultiSelectorDialog* CLmkMultiSelectorDialog::NewL(
    TInt& aOpenedItemIndex,
    CArrayFix<TInt>* aSelectionIndexArray,
    MDesCArray* aLbModel,
    TInt aMenuBarResourceId,
    TInt aOkOptionsMenuResourceId,
    MEikCommandObserver* aCmdObserver,
    MLmkDlgMenuObserver* aMenuObserver,
    MLmkListMemento* aMemento )
    {
    CLmkMultiSelectorDialog* self =
        new( ELeave ) CLmkMultiSelectorDialog( aOpenedItemIndex,
                                               aSelectionIndexArray,
                                               aLbModel,
                                               aMenuBarResourceId,
                                               aOkOptionsMenuResourceId,
                                               aCmdObserver,
                                               aMenuObserver,
                                               aMemento );

    CleanupStack::PushL( self );
    self->ConstructL( aMenuBarResourceId );
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CLmkMultiSelectorDialog::ConstructL
// ----------------------------------------------------
//
void CLmkMultiSelectorDialog::ConstructL(TInt aMenuBarResourceId)
	{
	CAknSelectionListDialog::ConstructL( aMenuBarResourceId );
	}

// ----------------------------------------------------
// CLmkMultiSelectorDialog::~CLmkMultiSelectorDialog
// ----------------------------------------------------
//
CLmkMultiSelectorDialog::~CLmkMultiSelectorDialog()
    {
    }

// ----------------------------------------------------
// CLmkMultiSelectorDialog::SetupSendMenu
// ----------------------------------------------------
//
void CLmkMultiSelectorDialog::SetupSendMenu(
    CLmkSender& aSender,
    TInt aSendUiMenuPaneResource,
    TInt aMenuTextResource )
    {
    iLmkSender = &aSender;
    iSendUiMenuPaneResourceId = aSendUiMenuPaneResource;
    iMenuTextResourceId = aMenuTextResource;
    }

// ----------------------------------------------------
// CLmkMultiSelectorDialog::HandleListBoxEventL
// ----------------------------------------------------
//
void CLmkMultiSelectorDialog::HandleListBoxEventL(
    CEikListBox* /*aListBox*/,
    TListBoxEvent aEventType )
    {
    switch( aEventType )
        {
        case EEventEnterKeyPressed:
        case EEventItemDoubleClicked:
            {
	        // Do nothing -- offerkeyevent handles this
            }
            break;
    #ifdef RD_SCALABLE_UI_V2
        case EEventItemClicked:
        	{
        	UpdateMskL();
        	break;
        	}
    #endif//RD_SCALABLE_UI_V2
        default:
            {
            break;
            }
        };
    }


// ----------------------------------------------------
// CLmkMultiSelectorDialog::SelectionListProcessCommandL
// ----------------------------------------------------
//
void CLmkMultiSelectorDialog::SelectionListProcessCommandL( TInt aCommand )
    {
    AknSelectionService::HandleMarkableListProcessCommandL( aCommand,
                                                            ListBox() );
    }

// -----------------------------------------------------------------------------
// CLmkMultiSelectorDialog::OfferKeyEventL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TKeyResponse CLmkMultiSelectorDialog::OfferKeyEventL(
                                     const TKeyEvent& aKeyEvent,
                                     TEventCode aType )
	{
	// Do not process navigation handling till UI gets from engine
	if (!ListBox())
		{
		return EKeyWasConsumed;
		}
    if (aType == EEventKey &&
        	(aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == EKeyEnter || aKeyEvent.iScanCode == EStdKeyEnter) )
		{
		ListBox()->View()->ToggleItemL(ListBox()->CurrentItemIndex());
		return EKeyWasConsumed;
		}
	if (aKeyEvent.iRepeats == 0)
        {
        if (KeyEventForNavigationL(aKeyEvent, aType) == EKeyWasConsumed)
            {
            iIsToClearSelection = ETrue;
            TryExitL(EAknSoftkeyOk);
            return EKeyWasConsumed;
            }
        CAknSearchField* findBox = FindBox();
        if	(findBox && !(findBox->IsVisible()))
            {
            return 	EKeyWasConsumed;
            }
        }

	TKeyResponse keyResponce;
	keyResponce = CAknSelectionListDialog::OfferKeyEventL(aKeyEvent, aType);
	//CEikButtonGroupContainer &buttonContainer = *(CEikButtonGroupContainer::Current());//ButtonGroupContainer();
	if(!iExited)
		{
		UpdateMskL();
		}
	return keyResponce;
	}

// ----------------------------------------------------
// CLmkMultiSelectorDialog::PreLayoutDynInitL
// ----------------------------------------------------
//
void CLmkMultiSelectorDialog::PreLayoutDynInitL()
    {
    CLmkSelectorDialog::PreLayoutDynInitL();
    __ASSERT_DEBUG( iSelectionIndexArray, Panic( KLmkPanicNullMember ) );
    ListBox()->SetSelectionIndexesL( iSelectionIndexArray );
    }
// ----------------------------------------------------
// CLmkMultiSelectorDialog::PostLayoutDynInitL
// ----------------------------------------------------

void CLmkMultiSelectorDialog::PostLayoutDynInitL()
	{
	CLmkSelectorDialog::PostLayoutDynInitL();
	UpdateMskL();

	}
// -----------------------------------------------------------------------------
// CLmkMultiSelectorDialog::OkToExitL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CLmkMultiSelectorDialog::OkToExitL( TInt aButtonId )
    {
    TBool b = CAknSelectionListDialog::OkToExitL( aButtonId );
    if ( iSelectionIndexArray )
	    {
	    const CArrayFix<TInt>* array = ListBox()->SelectionIndexes();
	    if ( FindBox() )
	        {
            CAknFilteredTextListBoxModel* model =
	         static_cast<CAknFilteredTextListBoxModel*>( ListBox()->Model() );
            model->Filter()->UpdateSelectionIndexesL();
	        array = model->Filter()->SelectionIndexes();
	        }
	    TInt numberOfItems( array->Count() );
	    iSelectionIndexArray->Reset();
	    for ( TInt i( 0 ); i < numberOfItems; ++i )
            {
	        iSelectionIndexArray->AppendL( array->At( i ) );
            }
	    }
    if(iIsToClearSelection && iSelectionIndexArray)
        {
        iSelectionIndexArray->Reset();
        }
    if ( b && iNavigationObserver )
        {
        iNavigationObserver->HandleClosingEventL();
        }

    iExited = b;
    return b;
    }

// ----------------------------------------------------
// CLmkMultiSelectorDialog::DynInitMenuPaneL
// ----------------------------------------------------
//
void CLmkMultiSelectorDialog::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    AknSelectionService::HandleMarkableListDynInitMenuPane( aResourceId,
                                                    aMenuPane, ListBox() );

    if ( iMenuObserver )
        {
        iMenuObserver->DynInitDlgMenuPaneL( aResourceId, aMenuPane );
        }

    TInt visibleCount = ListBox()->Model()->NumberOfItems();

    if ( aResourceId == iSendUiMenuPaneResourceId )
        {
        if ( iLmkSender )
            {
            // Send menu is handled by iLmkSender:
            if ( FeatureManager::FeatureSupported( KFeatureIdLandmarksConverter ) )
				{
				aMenuPane->SetItemDimmed(ELmkCmdSendDummy, EFalse);
				}
			else
				{
				aMenuPane->SetItemDimmed(ELmkCmdSendDummy, ETrue);
				}
            }
        }
    else if ( aResourceId == R_SENDUI_MENU )
        { // Dynamically created send ui menu
        // Sender always exists if R_SENDUI_MENU exists:
        __ASSERT_DEBUG( iLmkSender, Panic( KLmkPanicSenderNotExist ) );

        if ( FeatureManager::FeatureSupported( KFeatureIdLandmarksConverter ) )
	        {
	        iLmkSender->DisplaySendCascadeMenuL( *aMenuPane );
	        }
        }

    CAknSelectionListDialog::DynInitMenuPaneL( aResourceId, aMenuPane );
    }


// ----------------------------------------------------
// CLmkMultiSelectorDialog::UpdateMskL
// ----------------------------------------------------
//
 void CLmkMultiSelectorDialog::UpdateMskL()
	 {

 	CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
 	HBufC* mskTxt = NULL;
 	if (ListBox()->Model()->NumberOfItems() > 0)
 		{
 		cba->MakeCommandVisibleByPosition(CEikButtonGroupContainer::EMiddleSoftkeyPosition,ETrue);
 		cba->MakeCommandVisible(EAknSoftkeyDone, ETrue);
		cba->MakeCommandVisible(EAknSoftkeyOk, ETrue);
 		if(IsCurrentItemMarked())
			{
			mskTxt = StringLoader::LoadLC( R_LM_MSK_UNMARK, iEikonEnv );
			cba->AddCommandToStackL(KMskCommandPos,EAknSoftkeyUnmark,mskTxt->Des());
			CleanupStack::PopAndDestroy(mskTxt);

			}
		else
			{
			mskTxt = StringLoader::LoadLC( R_LM_MSK_MARK, iEikonEnv );
			cba->AddCommandToStackL(KMskCommandPos,EAknSoftkeyMark,mskTxt->Des());
			CleanupStack::PopAndDestroy(mskTxt);
			}
 		}
 	else
 		{
 		cba->MakeCommandVisible(EAknSoftkeyDone, EFalse);
		cba->MakeCommandVisible(EAknSoftkeyOk, EFalse);
 		cba->MakeCommandVisibleByPosition(CEikButtonGroupContainer::EMiddleSoftkeyPosition,EFalse);
 		}
 	 cba->DrawDeferred();
	 }


// ----------------------------------------------------
// CLmkMultiSelectorDialog::IsCurrentItemMarked
// ----------------------------------------------------
//
TBool CLmkMultiSelectorDialog::IsCurrentItemMarked()
	{
	TBool ret(EFalse);
	TInt itemIndex = ListBox()->View()->CurrentItemIndex();
	if (itemIndex != KErrNotFound)
		{
		 ret = ListBox()->View()->ItemIsSelected(itemIndex);
		}
	return ret;
	}
//  End of File
