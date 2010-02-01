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
#include <StringLoader.h>
#include <aknlists.h>
#include <aknsfld.h>
#include <avkon.mbg>
#include <AknsConstants.h>
#include <AknIconArray.h>
#include <lmkui.rsg>
#include <locationvariationdomaincrkeys.h>
#include <centralrepository.h>
#include "LmkConsts.h"
#include "CLmkUiUtils.h"
#include "CLmkLmItemListProvider.h"
#include "CLmkSelectorLBModelBase.h"
#include "CLmkSingleGraphicLBModel.h"
#include "CLmkMultiSelectionLBModel.h"
#include "CLmkLmItemListMemento.h"
#include "CLmkSelectorIconMgr.h"
#include "CLmkSelectorDialog.h"
#include "CLmkMultiSelectorDialog.h"
#include "CLmkDlgSelectorImplBase.h"
#include <lmkerrors.h>


#if defined(_DEBUG)

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkDlgSelectorImplBase" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace
#endif
_LIT(KFetchingData, " ");
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkDlgSelectorImplBase::CLmkDlgSelectorImplBase
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkDlgSelectorImplBase::CLmkDlgSelectorImplBase( CPosLandmarkDatabase& aDb )
    : CLmkSelectorImplBase( aDb ),
      iDlgSelectorImplBaseLatestListEvent( ELmkEventNull ),
      iTitle(0)
    {
    }

// -----------------------------------------------------------------------------
// CLmkDlgSelectorImplBase::BaseConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkDlgSelectorImplBase::BaseConstructL()
    {
    CLmkSelectorImplBase::BaseConstructL();
	
    // Check if the Coordinates has to be hidden
	CheckHideCoordinateL();

    // CommonUi selector API promises that construction of a dialog leaves with
    // KErrNotSupported if the framework is not available. This is satisfied
    // since providers try to initialize the db during the construction.
    // However, it is not really even important since the selector API cannot
    // be even called if framework isn't available since the API requires
    // a landmark db reference, and user can't get it without the framework!
    }

// ----------------------------------------------------
// CLmkDlgSelectorImplBase::~CLmkDlgSelectorImplBase
// ----------------------------------------------------
//
CLmkDlgSelectorImplBase::~CLmkDlgSelectorImplBase()
    {
    if( iTitle )
    	delete iTitle;
    }

// ----------------------------------------------------
// CLmkDlgSelectorImplBase::SetSelectorImplParent
// ----------------------------------------------------
//
void CLmkDlgSelectorImplBase::SetSelectorImplParent(
    CLmkDlgSelectorImplBase* aParent )
    {
    iSelectorImplParent = aParent;
    }

// -----------------------------------------------------------------------------
// CLmkDlgSelectorImplBase::SetMopParent
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkDlgSelectorImplBase::SetMopParent( MObjectProvider* aParent )
    {
    iMopParent = aParent;
    }

// ----------------------------------------------------
// CLmkDlgSelectorImplBase::ExecuteL
// ----------------------------------------------------
//
TInt CLmkDlgSelectorImplBase::ExecuteL(
    TPosLmItemId& aOpenedItem,
    TBool aAlwaysModifyOpened )
    {
    const CLmkLmItemListProvider& provider =
        static_cast<const CLmkLmItemListProvider&>( ListProvider() );

    // First item in the list will be highlighted when launching the
    // single item selector, regardless of aOpenedItem input value:
    TInt index( 0 );
    TInt result = ExecuteL( index );
    if ( result || aAlwaysModifyOpened )
        {
        provider.GetSelectedItemId( aOpenedItem, index );
        }
    return result;
    }

// ----------------------------------------------------
// CLmkDlgSelectorImplBase::ExecuteL
// ----------------------------------------------------
//
TInt CLmkDlgSelectorImplBase::ExecuteL(
    RArray<TPosLmItemId>& aSelectedItems,
    TBool aAlwaysModifySelected )
    {
    const CLmkLmItemListProvider& provider =
        static_cast<const CLmkLmItemListProvider&>( ListProvider() );

    // If provider is not ready yet then selectedIndexes will not
    // contain any/all pre-selected indexes but this is still
    // a safe way to create an array for selections:
    CArrayFix<TInt>* selectedIndexes =
        provider.SelectedIndexesL( aSelectedItems );

    // If provider is not ready yet we will need to set pre-selected
    // indexes again later when it gets ready:
    if ( iDlgSelectorImplBaseLatestListEvent != ELmkEventListReady )
        {
        // Pre-selected items will be set marked when provider is ready:
        iInputSelectedItems = &aSelectedItems;
        }

    CleanupStack::PushL( selectedIndexes );

    TInt result = ExecuteL( *selectedIndexes );

    if ( result || aAlwaysModifySelected )
        {
        provider.GetSelectedItemIdsL( aSelectedItems,
                                      *selectedIndexes );
        }
    else
        {
        //Below code, if user might have passed some
        //pre-selection, which needs to be retunred
        //back to the user in case of cancel.
        RArray<TPosLmItemId> tempIdArray;
        TInt count = aSelectedItems.Count();
        for ( TInt i = 0; i < count; i++ )
            {
            TInt found = provider.SelectedIndex(aSelectedItems[i]);
            if ( found < 0 )
                {
                tempIdArray.Append(aSelectedItems[i]);
                }
            }
        count = tempIdArray.Count();
        for ( TInt i = 0; i < count; i++ )
            {
            TInt found = aSelectedItems.Find(tempIdArray[i]);
            if ( found != KErrNotFound )
                {
                aSelectedItems.Remove(found);
                }
            }
        tempIdArray.Close();
        }
    CleanupStack::PopAndDestroy( selectedIndexes );
    return result;
    }

// -----------------------------------------------------------------------------
// CLmkDlgSelectorImplBase::ExecuteL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLmkDlgSelectorImplBase::ExecuteL( TInt& aOpenedItem )
    {
    TInt menuBarResource( KZeroResourceId );
    TInt dialogResource( KZeroResourceId );
    TInt titleResource( KZeroResourceId );
    GetDlgResources( ETrue, titleResource, menuBarResource, dialogResource );

    CLmkSelectorLBModelBase* lbModel =
        CLmkSingleGraphicLBModel::NewL( *iListProvider );
    CleanupStack::PushL( lbModel );

    CLmkSelectorDialog* dialog = CLmkSelectorDialog::NewL( aOpenedItem,
                                                           lbModel,
                                                           menuBarResource,
                                                           this,iMemento, iListProvider );

    dialog->SetNavigationObserver( *this );
    dialog->SetMopParent( iMopParent );
    dialog->PrepareLC( dialogResource );
    dialog->SetHelpContext( HelpContext() );
    dialog->SetDlgEmptyResource(iCurrentLabelPrimResource,iCurrentLabelSecResource);

	HBufC* textForFetchingData;
	textForFetchingData = HBufC::NewL(20);
	CleanupStack::PushL(textForFetchingData);  // textForFetchingData
	*textForFetchingData = KFetchingData;
    dialog->ListBox()->View()->SetListEmptyTextL( *textForFetchingData);

    CleanupStack::PopAndDestroy();  // textForFetchingData.

    CAknIconArray* icons = SetupIconMgrLC();

    dialog->SetIconArrayL( icons );
    CleanupStack::Pop(); // icons

    // pointer needs to be stored in a member during exection:
    iDialog = dialog;
    iLmkUiUtils->StoreTitlePaneL();
    if (iTitle) {
        HBufC* titleTextBuf = iTitle->AllocL();
        iLmkUiUtils->ChangeTitlePaneL(titleTextBuf); // iLmkUiUtils takes ownership
    }
    else {
        iLmkUiUtils->ChangeTitlePaneL( titleResource );
    }
    TInt result = iDialog->RunLD();
    iDialog = NULL; // dialog was popped and destroyed
    iLmkUiUtils->RestoreOldTitlePane();
    CleanupStack::PopAndDestroy( lbModel );

    return result;
    }

// -----------------------------------------------------------------------------
// CLmkDlgSelectorImplBase::ExecuteL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLmkDlgSelectorImplBase::ExecuteL( CArrayFix<TInt>& aSelectedItems )
    {
    TInt openedItemIndex( 0 );
    TInt menuBarResource( KZeroResourceId );
    TInt dialogResource( KZeroResourceId );
    TInt titleResource( KZeroResourceId );


    GetDlgResources( EFalse, titleResource, menuBarResource, dialogResource );

		CLmkSelectorLBModelBase* lbModel =
        CLmkMultiSelectionLBModel::NewL( *iListProvider );
    CleanupStack::PushL( lbModel );

    CLmkSelectorDialog* dialog = CLmkMultiSelectorDialog::NewL(
                            openedItemIndex, &aSelectedItems, lbModel,
                            menuBarResource, NULL, this, this,iMemento );

    dialog->SetNavigationObserver( *this );
    dialog->SetMopParent( iMopParent );
    dialog->PrepareLC( dialogResource );
    dialog->SetHelpContext( HelpContext() );
    dialog->SetDlgEmptyResource(iCurrentLabelPrimResource,iCurrentLabelSecResource);

  	HBufC* textForFetchingData;
	textForFetchingData = HBufC::NewL(20);
	CleanupStack::PushL(textForFetchingData);  // textForFetchingData
	*textForFetchingData = KFetchingData;
    dialog->ListBox()->View()->SetListEmptyTextL( *textForFetchingData);
    CleanupStack::PopAndDestroy();  // textForFetchingData.


    CAknIconArray* icons = SetupIconMgrLC();
	TFileName* iconFile = CLmkUiUtils::AvkonIconFileLC();
    iIconMgr->SetIconAtIndexZeroL( *iconFile,
                                   KAknsIIDQgnPropCheckboxOn,
                                   EMbmAvkonQgn_prop_checkbox_on,
                                   EMbmAvkonQgn_prop_checkbox_on_mask );

    iIconMgr->SetIconAtIndexOneL( *iconFile,
                                  KAknsIIDQgnPropCheckboxOff,
                                  EMbmAvkonQgn_prop_checkbox_off,
                                  EMbmAvkonQgn_prop_checkbox_off_mask );

    dialog->SetIconArrayL( icons );
    CleanupStack::PopAndDestroy(); // iconFile
    CleanupStack::Pop(); // icons

    // pointer needs to be stored in a member during exection:
    iDialog = dialog;
    iLmkUiUtils->StoreTitlePaneL();
    if (iTitle) {
        HBufC* titleTextBuf = iTitle->AllocL();
        iLmkUiUtils->ChangeTitlePaneL(titleTextBuf); // iLmkUiUtils takes ownership
    }
    else {
        iLmkUiUtils->ChangeTitlePaneL( titleResource );
    }

    TInt result = iDialog->RunLD();
    iDialog = NULL; // dialog was popped and destroyed
    iLmkUiUtils->RestoreOldTitlePane();
    CleanupStack::PopAndDestroy( lbModel );

    return result;
    }

// -----------------------------------------------------------------------------
// CLmkDlgSelectorImplBase::ExecuteL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLmkDlgSelectorImplBase::ExecuteL( CArrayFix<TInt>& aSelectedItems, TPtrC aTitlePaneText )
    {
    TInt openedItemIndex( 0 );
    TInt menuBarResource( KZeroResourceId );
    TInt dialogResource( KZeroResourceId );
    TInt titleResource( KZeroResourceId );

    GetDlgResources( EFalse, titleResource, menuBarResource, dialogResource );


    CLmkSelectorLBModelBase* lbModel =
        CLmkMultiSelectionLBModel::NewL( *iListProvider );
    CleanupStack::PushL( lbModel );

    CLmkSelectorDialog* dialog = CLmkMultiSelectorDialog::NewL(
                            openedItemIndex, &aSelectedItems, lbModel,
                            menuBarResource, NULL, this, this,iMemento );

    dialog->SetNavigationObserver( *this );
    dialog->SetMopParent( iMopParent );
    dialog->PrepareLC( dialogResource );
    dialog->SetHelpContext( HelpContext() );
	dialog->SetDlgEmptyResource(iCurrentLabelPrimResource,iCurrentLabelSecResource);

    HBufC* textForFetchingData;
	textForFetchingData = HBufC::NewL(20);
	CleanupStack::PushL(textForFetchingData);  // textForFetchingData
	*textForFetchingData = KFetchingData;
    dialog->ListBox()->View()->SetListEmptyTextL( *textForFetchingData);
    CleanupStack::PopAndDestroy();  // textForFetchingData.

    CAknIconArray* icons = SetupIconMgrLC();
	TFileName* iconFile = CLmkUiUtils::AvkonIconFileLC();
    iIconMgr->SetIconAtIndexZeroL( *iconFile,
                                   KAknsIIDQgnPropCheckboxOn,
                                   EMbmAvkonQgn_prop_checkbox_on,
                                   EMbmAvkonQgn_prop_checkbox_on_mask );

    iIconMgr->SetIconAtIndexOneL( *iconFile,
                                  KAknsIIDQgnPropCheckboxOff,
                                  EMbmAvkonQgn_prop_checkbox_off,
                                  EMbmAvkonQgn_prop_checkbox_off_mask );

    dialog->SetIconArrayL( icons );
    CleanupStack::PopAndDestroy(); // iconFile
    CleanupStack::Pop(); // icons

    // pointer needs to be stored in a member during exection:
    iDialog = dialog;
    iLmkUiUtils->StoreTitlePaneL();

    HBufC* titleText  = NULL;

    titleText  = aTitlePaneText.AllocL();

    iLmkUiUtils->ChangeTitlePaneL(titleText );

    TInt result = iDialog->RunLD();
    iDialog = NULL; // dialog was popped and destroyed
    iLmkUiUtils->RestoreOldTitlePane();
    CleanupStack::PopAndDestroy( lbModel );

    return result;
    }


// ----------------------------------------------------
// CLmkDlgSelectorImplBase::HelpContext
// ----------------------------------------------------
//
TCoeHelpContext CLmkDlgSelectorImplBase::HelpContext() const
    {
    // Default null help context:
    return TCoeHelpContext();
    }

// ----------------------------------------------------
// CLmkDlgSelectorImplBase::StopExecutionL
// ----------------------------------------------------
//
void CLmkDlgSelectorImplBase::StopExecutionL()
    {
    delete iDialog;
    iDialog = NULL;
    }

// -----------------------------------------------------------------------------
// CLmkDlgSelectorImplBase::MementoL
// -----------------------------------------------------------------------------
//
MLmkListMemento* CLmkDlgSelectorImplBase::MementoL()
    {
    CLmkLmItemListMemento* memento = CLmkLmItemListMemento::NewL();
    return memento;
    }

// -----------------------------------------------------------------------------
// CLmkDlgSelectorImplBase::ListVisibleItemCount
// -----------------------------------------------------------------------------
//
TInt CLmkDlgSelectorImplBase::ListVisibleItemCount()
    {
    TInt retval( 0 );
    if ( iDialog )
        {
        retval = iDialog->NumberOfItems();
        }
    return retval;
    }

// -----------------------------------------------------------------------------
// CLmkDlgSelectorImplBase::ListMarkedItemCountL
// -----------------------------------------------------------------------------
//
TInt CLmkDlgSelectorImplBase::ListMarkedItemCountL()
    {
    TInt retval( 0 );
    if ( iDialog )
        {
        const CArrayFix<TInt>* markedIndexes = iDialog->SelectionIndexes();
        retval = markedIndexes->Count();
        }
    return retval;
    }

// ----------------------------------------------------
// CLmkDlgSelectorImplBase::ProcessCommandL
// ----------------------------------------------------
//
void CLmkDlgSelectorImplBase::ProcessCommandL( TInt aCommandId )
    {
    if ( iSelectorImplParent )
        {
        iSelectorImplParent->ProcessCommandL( aCommandId );
        }
    }

// ----------------------------------------------------
// CLmkDlgSelectorImplBase::DynInitDlgMenuPaneL
// ----------------------------------------------------
//
void CLmkDlgSelectorImplBase::DynInitDlgMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    if ( iSelectorImplParent )
        {
        iSelectorImplParent->DynInitDlgMenuPaneL( aResourceId, aMenuPane );
        }
    }

// ----------------------------------------------------
// CLmkDlgSelectorImplBase::HandleListProviderEvent
// ----------------------------------------------------
//
void CLmkDlgSelectorImplBase::HandleListProviderEvent(
    TLmkListProviderEventType aEvent )
    {
    iDlgSelectorImplBaseLatestListEvent = aEvent;

    if ( ( iDlgSelectorImplBaseLatestListEvent == ELmkEventListReady ||
    		iDlgSelectorImplBaseLatestListEvent == ELmkEventItemAdditionComplete ) &&
         	iInputSelectedItems )
        { // First attempt to set pre-selected items wasn't successful,
          // but now list provider is ready so let's try again:
        TRAPD( ignore, RetrySetPreSelectedL() );
        if (ignore != KErrNone)
        {
        ignore = KErrNone;
        }
        iInputSelectedItems = NULL;
        }
        if (iDialog)
	        {
	        TRAP_IGNORE(iDialog->UpdateMskL());
	        }
    if ( iSelectorImplParent )
        {
        iSelectorImplParent->HandleListProviderEvent( aEvent );
        }
    // It's better do this as last step as base class restores memento
    // which should be done AFTER event has been handled otherwise:
    if(iDialog && iDialog->ListBox()->View() && iDialog->ListBox()->Model())
        {
        CLmkSelectorImplBase::HandleListProviderEvent( aEvent );
        }
    if( iDialog )
        {
        TInt index = iDialog->ListBox()->CurrentItemIndex();
        TInt count = iDialog->ListBox()->Model()->NumberOfItems();

        if ( index == KErrNotFound && count > 0)
            {
            iDialog->ListBox()->SetCurrentItemIndex(count - 1);
            }
        }
    if(iDialog && (iDialog->ListBox()->Model()->NumberOfItems() == 0))
    	{
    	if(iMemento)
	    	{
	    	CLmkLmItemListMemento* memento =
	    							static_cast<CLmkLmItemListMemento*>(iMemento);
	    	memento->UpdateModelAvailability(ETrue);
	    	}

	    if(aEvent != ELmkEventFindListReady)
	    	{
	    	iDialog->ShowFindBox(EFalse);
	    	}
	    iDialog->EnableLeftSoftKey(EFalse);
    	}
    else if	(iDialog && (iDialog->ListBox()->Model()->NumberOfItems() > 0))
    	{
    	if(iMemento)
	    	{
	    	CLmkLmItemListMemento* memento =
	    							static_cast<CLmkLmItemListMemento*>(iMemento);
	    	memento->UpdateModelAvailability(ETrue);
	    	}
    	// Enable the find box
    	if(aEvent != ELmkEventFindListReady)
	    	{
	    	iDialog->ShowFindBox(ETrue);
	    	}
    	iDialog->EnableLeftSoftKey(ETrue);
    	}
    if(iDialog)
    	{
    	iDialog->ListBox()->DrawDeferred();
    	}
    }

// ----------------------------------------------------
// CLmkDlgSelectorImplBase::HandleListProviderError
// ----------------------------------------------------
//
void CLmkDlgSelectorImplBase::HandleListProviderError( TInt aError )
    {
    if ( iSelectorImplParent )
        {
        iSelectorImplParent->HandleListProviderError( aError );
        }
    }

// ----------------------------------------------------
// CLmkDlgSelectorImplBase::HandleNavigationEventL
// ----------------------------------------------------
//
TKeyResponse CLmkDlgSelectorImplBase::HandleNavigationEventL(
    const TKeyEvent& aKeyEvent )
    {
    if ( iSelectorImplParent )
        {
        return iSelectorImplParent->HandleNavigationEventL( aKeyEvent );
        }
    else
        {
        return EKeyWasNotConsumed;
        }
    }

// ----------------------------------------------------
// CLmkDlgSelectorImplBase::HandleLaunchingEventL
// ----------------------------------------------------
//
void CLmkDlgSelectorImplBase::HandleLaunchingEventL()
    {
    if ( iSelectorImplParent )
        {
        iSelectorImplParent->HandleLaunchingEventL();
        }
    // listbox always exists when HandleLaunchingEventL() is called:
    __ASSERT_DEBUG( iDialog && iDialog->ListBox(),
                    Panic( KLmkPanicNullListBox ) );
    Restore( *( iDialog->ListBox() ), *iListProvider, iDialog->HasFindBox() );
    }

// ----------------------------------------------------
// CLmkDlgSelectorImplBase::HandleClosingEventL
// ----------------------------------------------------
//
void CLmkDlgSelectorImplBase::HandleClosingEventL()
    {
    if ( iSelectorImplParent )
        {
        iSelectorImplParent->HandleClosingEventL();
        }
    // listbox always exists when HandleClosingEventL() is called:
    __ASSERT_DEBUG( iDialog->ListBox(), Panic( KLmkPanicNullListBox ) );
    Store( *( iDialog->ListBox() ), *iListProvider, iDialog->HasFindBox() );
    }

// ----------------------------------------------------
// CLmkDlgSelectorImplBase::RetrySetPreSelectedL
// ----------------------------------------------------
//
void CLmkDlgSelectorImplBase::RetrySetPreSelectedL()
    {
    const CLmkLmItemListProvider& provider =
        static_cast<const CLmkLmItemListProvider&>( ListProvider() );
    CArrayFix<TInt>* selectedIndexes =
        provider.SelectedIndexesL( *iInputSelectedItems );
    CleanupStack::PushL( selectedIndexes );
    __ASSERT_DEBUG( iDialog && iDialog->ListBox(),
                    Panic( KLmkPanicNullListBox ) );
    iDialog->ListBox()->SetSelectionIndexesL( selectedIndexes );
    CAknFilteredTextListBoxModel* model =
	         static_cast<CAknFilteredTextListBoxModel*>( iDialog->ListBox()->Model() );
            model->Filter()->UpdateSelectionIndexesL();
     model->Filter()->HandleItemArrayChangeL();
    // Listbox made a copy of selection indexes
    CleanupStack::PopAndDestroy( selectedIndexes );
    }

// ----------------------------------------------------
// CLmkDlgSelectorImplBase::SetEmptyDlgLabel
// ----------------------------------------------------
//
void CLmkDlgSelectorImplBase::SetEmptyDlgLabel(TInt aLabelPrimRes, TInt aLabelSecRes)
	{
	iCurrentLabelPrimResource = aLabelPrimRes;
	iCurrentLabelSecResource = aLabelSecRes;
	}

// ---------------------------------------------------------
// CLmkDlgSelectorImplBase::CheckHideCoordinateL()
// ---------------------------------------------------------
//
void CLmkDlgSelectorImplBase::CheckHideCoordinateL()
	{
    TInt helpSettingValue = ELocHelpNotHidden;
    CRepository* repository = CRepository::NewL( TUid::Uid( KCRUidLocLocalVariation) );
    CleanupStack::PushL( repository ); 
    User::LeaveIfError( repository->Get( KLocHideHelp,
                                              helpSettingValue ));  
    CleanupStack::PopAndDestroy( repository );
	
	if( ELocHelpHidden == helpSettingValue )
		iIsHideHelp = ETrue;
	else
		iIsHideHelp = EFalse;
	 
	}

// ---------------------------------------------------------
// CLmkDlgSelectorImplBase::SetDialogTitleL
// ---------------------------------------------------------
//
void CLmkDlgSelectorImplBase::SetDialogTitleL(const TDesC& aTitle)
    {
    delete iTitle;
    iTitle = NULL;
    iTitle = aTitle.AllocL();
    }
//  End of File
