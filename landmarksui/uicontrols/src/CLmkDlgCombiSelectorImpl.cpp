/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -    This contains methods implementing a combined dialog for Landmarks
*                by name view and categories view
*
*/







// INCLUDE FILES

#include <eikapp.h>
#include <eikenv.h>
#include <StringLoader.h>
#include <AknsUtils.h>
#include <akntabgrp.h>
#include <eikspane.h>
#include <aknnavide.h>
#include <aknnavi.h>
#include <barsread.h>
#include <lmkui.rsg>
#include <EPos_CPosLmCategoryCriteria.h>
#include "LmkConsts.h"
#include "landmarks.hrh"
#include "CLmkDbUtils.h"
#include "CLmkLmItemListProvider.h"
#include "CLmkUiUtils.h"
#include "CLmkSelectorDialog.h"
#include "CLmkEmptyDialog.h"
#include "CLmkDlgLmSelectorImpl.h"
#include "CLmkDlgCategorySelectorImpl.h"
#include "CLmkDlgCombiSelectorImpl.h"
#include "CLmkLmItemListMemento.h"
#include <lmkerrors.h>


// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

const TInt KLmkNumberOfSelectors = 3;

#if defined(_DEBUG)
_LIT( KPanicMsg, "CLmkDlgCombiSelectorImpl" );
void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
#endif
}  // namespace
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkDlgCombiSelectorImpl::CLmkDlgCombiSelectorImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkDlgCombiSelectorImpl::CLmkDlgCombiSelectorImpl( CPosLandmarkDatabase& aDb )
    : CLmkDlgSelectorImplBase( aDb ),
      iSelectorState( ELmkInitialState ),
      iSelectors( KLmkNumberOfSelectors ),
      iMementos( KLmkNumberOfSelectors ),
      iNavigating( EFalse ),
      iAlwaysModify( EFalse ),
      iCustomLandmarksTitle(0)
    {
    }

// -----------------------------------------------------------------------------
// CLmkDlgCombiSelectorImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkDlgCombiSelectorImpl::ConstructL()
    {
    BaseConstructL();

    // Default status pane:
    CEikStatusPane* sp = iEnv->AppUiFactory()->StatusPane();

    // Fetch pointer to the default navi pane control:
    iNaviPane = static_cast<CAknNavigationControlContainer*>(
                        sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    __ASSERT_DEBUG( iNaviPane, Panic( KLmkPanicNullMember ) );
    iNaviDeco = iNaviPane->CreateTabGroupL();
   	iNaviDeco->SetControlType( CAknNavigationDecorator::ETabGroup );
    iTabGroup = static_cast<CAknTabGroup*>( iNaviDeco->DecoratedControl() );
    __ASSERT_DEBUG( iTabGroup, Panic( KLmkPanicNullMember ) );

    // Add Tab Icons
    AddTabsL( *iTabGroup );

    // Set observer for CAknTabGroup
    iTabGroup->SetObserver( this );

    // Create mementos and initialize selector array with NULL pointers:
    MLmkListMemento* memento = NULL;
    for ( TInt i( 0 ); i < KLmkNumberOfSelectors; ++i )
        {
        memento = MementoL();
        User::LeaveIfError( iMementos.Append( memento ) );
        User::LeaveIfError( iSelectors.Append( NULL ) );
        }
    }

// -----------------------------------------------------------------------------
// CLmkDlgCombiSelectorImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkDlgCombiSelectorImpl* CLmkDlgCombiSelectorImpl::NewL(
    CPosLandmarkDatabase& aDb )
    {
    CLmkDlgCombiSelectorImpl* self =
        new( ELeave ) CLmkDlgCombiSelectorImpl( aDb );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CLmkDlgCombiSelectorImpl::~CLmkDlgCombiSelectorImpl
// ----------------------------------------------------
//
CLmkDlgCombiSelectorImpl::~CLmkDlgCombiSelectorImpl()
    {
    iSelectors.ResetAndDestroy();
    iMementos.ResetAndDestroy();
    delete iNaviDeco;
    if( iCustomLandmarksTitle )
    	delete iCustomLandmarksTitle;
    }

// ----------------------------------------------------
// CLmkDlgCombiSelectorImpl::StopExecutionL
// ----------------------------------------------------
//
void CLmkDlgCombiSelectorImpl::StopExecutionL()
    {
    __ASSERT_DEBUG( iSelectors.Count() == KLmkNumberOfSelectors,
                    Panic( KLmkPanicSelectorArrayIndex ) );

    for ( TInt i( 0 ); i < KLmkNumberOfSelectors; ++i )
        {
        if ( iSelectors[i] )
            {
            iSelectors[i]->StopExecutionL();
            }
        }
    iNaviPane->Pop( iNaviDeco );
    // Dialog ExecuteL returns and we must consider it in final state
    iSelectorState = ELmkFinalState;
    }

// -----------------------------------------------------------------------------
// CLmkDlgCombiSelectorImpl::ExecuteL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLmkDlgCombiSelectorImpl::ExecuteL(
    TPosLmItemId& aOpenedItem,
    TBool aAlwaysModifyOpened )
    {
    // Ensure that ExecuteL is not already running:
    __ASSERT_DEBUG( iSelectorState == ELmkFinalState ||
                    iSelectorState == ELmkInitialState,
                    Panic( KLmkPanicIllegalMethodCall ) );

    iSelectorState = ELmkInitialState;
    iOpenedItem = &aOpenedItem;
    iAlwaysModify = aAlwaysModifyOpened;
    CLmkEmptyDialog * lDlg = new(ELeave)CLmkEmptyDialog();
    lDlg->SetMopParent(iMopParent);
	lDlg->ExecuteLD(R_EMPTY_DIALOG);

    TLmkSelectorStateEvent nextEvent( ELmkSelectorStart );
    do
        {
        nextEvent = HandleAnyStateEventL( nextEvent );

        } while ( nextEvent != ELmkSelectorNoEvent );

    lDlg->EmptyDialogExitL();
    return iDlgRetVal;
    }

// -----------------------------------------------------------------------------
// CLmkDlgCombiSelectorImpl::ExecuteL
//
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLmkDlgCombiSelectorImpl::ExecuteL(
     RArray<TPosLmItemId>& aSelectedItems,
     TBool aAlwaysModifySelected )
    {
    // Ensure that ExecuteL is not already running:
    __ASSERT_DEBUG( iSelectorState == ELmkFinalState ||
                    iSelectorState == ELmkInitialState,
                    Panic( KLmkPanicIllegalMethodCall ) );

    iSelectorState = ELmkInitialState;
    iSelectedItems = &aSelectedItems;
    iAlwaysModify = aAlwaysModifySelected;


    TLmkSelectorStateEvent nextEvent( ELmkSelectorStart );
    do
        {
        nextEvent = HandleAnyStateEventL( nextEvent );

        } while ( nextEvent != ELmkSelectorNoEvent );

    return iDlgRetVal;
    }

// ----------------------------------------------------
// CLmkDlgCombiSelectorImpl::HandleAnyStateEventL
// ----------------------------------------------------
//
CLmkDlgCombiSelectorImpl::TLmkSelectorStateEvent
CLmkDlgCombiSelectorImpl::HandleAnyStateEventL(
    TLmkSelectorStateEvent aEvent )
    {
    TLmkSelectorStateEvent nextEvent( ELmkSelectorNoEvent );

    switch ( iSelectorState )
        {
        case ELmkInitialState:
            {
            nextEvent = HandleInitialStateEventL( aEvent );
            break;
            }
        case ELmkLandmarkSelector:
            {
            nextEvent = HandleLandmarkSelectorStateEventL( aEvent );
            break;
            }
        case ELmkCategorySelector:
            {
            nextEvent = HandleCategorySelectorStateEventL( aEvent );
            break;
            }
        case ELmkFilteredLmSelector:
            {
            nextEvent = HandleFilteredLmSelectorStateEventL( aEvent );
            break;
            }
        default: // ELmkFinalState
            {
            nextEvent = HandleDefaultStateStateEventL( aEvent );
            }
        }
    if ( iNavigating )
        { // navigation variable overrides dialog return value
        iNavigating = EFalse;
        nextEvent = ELmkSelectorNavigating;
        }

    return nextEvent;
    }

// ----------------------------------------------------
// CLmkDlgCombiSelectorImpl::HandleInitialStateEventL
// ----------------------------------------------------
//
CLmkDlgCombiSelectorImpl::TLmkSelectorStateEvent
CLmkDlgCombiSelectorImpl::HandleInitialStateEventL(
    TLmkSelectorStateEvent /*aEvent*/ )
    {

    iTabGroup->SetActiveTabById( ETabLandmarkList );
    iNaviPane->PushL( *iNaviDeco );
    iSelectorState = ELmkLandmarkSelector;
    CLmkDlgSelectorImplBase& selector = CurrentStateSelectorL();

    if ( iSelectedItems )
        { // multi selector mode
        iDlgRetVal = selector.ExecuteL( *iSelectedItems, iAlwaysModify );
        }
    else
        { // single selector mode
        iDlgRetVal = selector.ExecuteL( *iOpenedItem, iAlwaysModify );
        }
    return ELmkSelectorStop;
    }

// ----------------------------------------------------
// CLmkDlgCombiSelectorImpl::HandleAnyStateEventL
// ----------------------------------------------------
//
CLmkDlgCombiSelectorImpl::TLmkSelectorStateEvent
CLmkDlgCombiSelectorImpl::HandleLandmarkSelectorStateEventL(
    TLmkSelectorStateEvent aEvent )
    {
    __ASSERT_DEBUG( aEvent == ELmkSelectorNavigating ||
                    aEvent == ELmkSelectorStop,
                    Panic( KLmkPanicUnexpectedEvent ) );

    if ( aEvent == ELmkSelectorNavigating )
        {
        iSelectorState = ELmkCategorySelector;
        CLmkDlgSelectorImplBase& selector = CurrentStateSelectorL();
        iDlgRetVal = selector.ExecuteL( iCategoryIndex );
        return ELmkSelectorStop;
        }
    else
        { // aEvent == ELmkSelectorStop
        iSelectorState = ELmkFinalState;
        return ELmkSelectorNoEvent;
        }
    }

// ----------------------------------------------------
// CLmkDlgCombiSelectorImpl::HandleCategorySelectorStateEventL
// ----------------------------------------------------
//
CLmkDlgCombiSelectorImpl::TLmkSelectorStateEvent
CLmkDlgCombiSelectorImpl::HandleCategorySelectorStateEventL(
    TLmkSelectorStateEvent aEvent )
    {
    __ASSERT_DEBUG( aEvent == ELmkSelectorNavigating ||
                    aEvent == ELmkSelectorStop,
                    Panic( KLmkPanicUnexpectedEvent ) );

    if ( aEvent == ELmkSelectorNavigating )
        {
        iSelectorState = ELmkLandmarkSelector;
        CLmkDlgSelectorImplBase& selector = CurrentStateSelectorL();

        if ( iSelectedItems )
            { // multi selector mode
            iDlgRetVal = selector.ExecuteL( *iSelectedItems, iAlwaysModify );
            }
        else
            { // single selector mode
            iDlgRetVal = selector.ExecuteL( *iOpenedItem, iAlwaysModify );
            }
        return ELmkSelectorStop;
        }
    else if ( aEvent == ELmkSelectorStop && iDlgRetVal != 0 )
        { // -> open category contents
        TPosLmItemId categoryId( KPosLmNullItemId );
        static_cast<const CLmkLmItemListProvider&>( ListProvider() ).
            GetSelectedItemId( categoryId, iCategoryIndex );

		HBufC* catName = NULL;

        if (categoryId == KPosLmNullItemId) // 'uncategorised' category
        {
	        // 'uncategorised' category
	        // set criteria for searchnig 'Uncategorised' landmarks
			catName = StringLoader::LoadL(
			        R_LMK_LM_LIST_UNCATEGORISED,
			        const_cast<CEikonEnv *>( iEnv ));
        }
        else
        {
           //HBufC* catName = CLmkDbUtils::CategoryNameL( iDb, categoryId );
           catName = CLmkDbUtils::CategoryNameL( iDb, categoryId );
        }

        CleanupStack::PushL( catName );
        //Store old and create new navi pane
        iLmkUiUtils->StoreNaviPaneL();
        iLmkUiUtils->CreateNaviLabelL( *catName );
        CleanupStack::PopAndDestroy(); //catName

        iSelectorState = ELmkFilteredLmSelector;
        CLmkDlgSelectorImplBase& selector = CurrentStateSelectorL( categoryId );
		if(iMementos[iSelectorState])
	    	{
	    	CLmkLmItemListMemento* memento =
				 static_cast<CLmkLmItemListMemento*>(iMementos[iSelectorState]);
	    	memento->UpdateModelAvailability(EFalse);
	    	}
        if ( iSelectedItems )
            { // Return value is stored even if "Back" was pressed:
            iDlgRetVal = selector.ExecuteL( *iSelectedItems, ETrue );
            }
        else
            { // Return value is not stored if "Back" was pressed:
            iDlgRetVal = selector.ExecuteL( *iOpenedItem, EFalse );
            }
        return ELmkSelectorStop;
        }
    else // aEvent == ELmkSelectorStop && dialog canceled
        {
        iSelectorState = ELmkFinalState;
        return ELmkSelectorNoEvent;
        }
    }

// ----------------------------------------------------
// CLmkDlgCombiSelectorImpl::HandleFilteredLmSelectorStateEventL
// ----------------------------------------------------
//
CLmkDlgCombiSelectorImpl::TLmkSelectorStateEvent
CLmkDlgCombiSelectorImpl::HandleFilteredLmSelectorStateEventL(
    TLmkSelectorStateEvent aEvent )
    {
    __ASSERT_DEBUG( aEvent == ELmkSelectorStop,
                    Panic( KLmkPanicUnexpectedEvent ) );

    // filtered lm selector shouldn't be removed earlier
    CLmkDlgSelectorImplBase*& selector = iSelectors[ELmkFilteredLmSelector];
    delete selector;
    selector = NULL; // iSelector's pointer points to NULL now!

    //Restore navi panel
    iLmkUiUtils->RestoreOldNaviPaneL();

    if ( aEvent == ELmkSelectorStop && iDlgRetVal != 0 )
        {
        iSelectorState = ELmkFinalState;
        return ELmkSelectorNoEvent;
        }
    else // aEvent == ELmkSelectorStop & dialog canceled
        {
        iSelectorState = ELmkCategorySelector;
        CLmkDlgSelectorImplBase& dlgselector = CurrentStateSelectorL();
        TInt categoryIndex( -1 );
        iDlgRetVal = dlgselector.ExecuteL( categoryIndex );
        iCategoryIndex = categoryIndex;
        return ELmkSelectorStop;
        }
    }

// ----------------------------------------------------
// CLmkDlgCombiSelectorImpl::HandleDefaultStateStateEventL
// ----------------------------------------------------
//
CLmkDlgCombiSelectorImpl::TLmkSelectorStateEvent
CLmkDlgCombiSelectorImpl::HandleDefaultStateStateEventL(
    TLmkSelectorStateEvent /*aEvent*/ )
    {
    return ELmkSelectorNoEvent;
    }

// ----------------------------------------------------
// CLmkDlgCombiSelectorImpl::SetupListProviderL
// ----------------------------------------------------
//
void CLmkDlgCombiSelectorImpl::SetupListProviderL()
    {
    // This class does not create own provider
    }

// ----------------------------------------------------
// CLmkDlgCombiSelectorImpl::ListProvider
// ----------------------------------------------------
//
const CLmkListProviderBase& CLmkDlgCombiSelectorImpl::ListProvider() const
    {
    if ( iSelectorState ==  ELmkCategorySelector )
        {
        return iSelectors[ELmkCategorySelector]->ListProvider();
        }
    else if ( iSelectorState ==  ELmkFilteredLmSelector )
        {
        return iSelectors[ELmkFilteredLmSelector]->ListProvider();
        }
    else // ELmkLandmarkSelector
        {
        return iSelectors[ELmkLandmarkSelector]->ListProvider();
        }
    }

// ----------------------------------------------------
// CLmkDlgCombiSelectorImpl::GetDlgResources
// ----------------------------------------------------
//
void CLmkDlgCombiSelectorImpl::GetDlgResources(
    TBool /*aIsSingleSelector*/,
    TInt& aTitlePaneResource,
    TInt& aMenuBarResource,
    TInt& aDialogResource ) const
    {
    // This class does not define resources
    aTitlePaneResource = NULL;
    aMenuBarResource = NULL;
    aDialogResource = NULL;
    }

// ----------------------------------------------------
// CLmkDlgCombiSelectorImpl::StoreMemento
// ----------------------------------------------------
//
void CLmkDlgCombiSelectorImpl::StoreMemento()
    {
    // This class does not store
    }

// ----------------------------------------------------
// CLmkDlgCombiSelectorImpl::RestoreMemento
// ----------------------------------------------------
//
void CLmkDlgCombiSelectorImpl::RestoreMemento()
    {
    // This class does not restore
    }

// ----------------------------------------------------
// CLmkDlgCombiSelectorImpl::ProcessCommandL
// ----------------------------------------------------
//
void CLmkDlgCombiSelectorImpl::ProcessCommandL( TInt /*aCommandId*/ )
    {
    // This class does not process cmds
    }

// ----------------------------------------------------
// CLmkDlgCombiSelectorImpl::HandleNavigationEventL
// ----------------------------------------------------
//
TKeyResponse CLmkDlgCombiSelectorImpl::HandleNavigationEventL(
    const TKeyEvent& aKeyEvent )
    {
    __ASSERT_DEBUG( iTabGroup, Panic( KLmkPanicNullMember ) );
    // tab scroll key event handling is not needed 'Landmark Contents' view
    if ( iSelectorState ==  ELmkFilteredLmSelector)
	    {
	    return EKeyWasNotConsumed;
	    }

    TInt active = iTabGroup->ActiveTabId();
    TBool shift = EFalse;

    if ( active == ETabCategoryList )
        {
        if(AknLayoutUtils::LayoutMirrored())
        	{
        	if(aKeyEvent.iCode == EKeyRightArrow)
        		{
        		shift = ETrue;
        		}
        	else
	        	{//aKeyEvent.iCode == EKeyLeftArrow
	        	shift = EFalse;
	        	}
        	}
        else
        	{
        	if(aKeyEvent.iCode == EKeyLeftArrow)
        		{
        		shift = ETrue;
        		}
        	else
	        	{//aKeyEvent.iCode == EKeyRightArrow
	        	shift = EFalse;
	        	}
        	}

        if(shift)
        	{
        	iTabGroup->SetActiveTabById( ETabLandmarkList );
	        iNaviPane->PushL(*iNaviDeco);
	        iNavigating = ETrue;
        	}
        else
	        {// no need to set the tab id
	        return EKeyWasNotConsumed;
	        }
        }
    else if ( active == ETabLandmarkList )
        {
        TBool shift = EFalse;
        if(AknLayoutUtils::LayoutMirrored())
        	{
        	if(aKeyEvent.iCode == EKeyLeftArrow)
        		{
        		shift = ETrue;
        		}
        	else //aKeyEvent.iCode == EKeyRightArrow
	        	{
	        	shift = EFalse;
	        	}
        	}
        else
        	{
        	if(aKeyEvent.iCode == EKeyRightArrow)
        		{
        		shift = ETrue;
        		}
        	else //aKeyEvent.iCode == EKeyLeftArrow
	        	{
	        	shift = EFalse;
	        	}
        	}

        if(shift)
        	{
        	iTabGroup->SetActiveTabById( ETabCategoryList );
	        iNaviPane->PushL(*iNaviDeco);
	        iNavigating = ETrue;
        	}
        else // no need to set the tab id
	        {
	        return EKeyWasNotConsumed;
	        }
        }
    else
        {
        return EKeyWasNotConsumed;
        }
    return EKeyWasConsumed;
    }

// ----------------------------------------------------
// CLmkDlgCombiSelectorImpl::HandleLaunchingEventL
// ----------------------------------------------------
//
void CLmkDlgCombiSelectorImpl::HandleLaunchingEventL()
    {
    // This class does not do anything with this event
    }

// ----------------------------------------------------
// CLmkDlgCombiSelectorImpl::HandleClosingEventL
// ----------------------------------------------------
//
void CLmkDlgCombiSelectorImpl::HandleClosingEventL()
    {
    // This class does not do anything with this event
    }

CLmkDlgSelectorImplBase& CLmkDlgCombiSelectorImpl::CurrentStateSelectorL(
    TPosLmItemId aCategoryId )
    {
    __ASSERT_DEBUG( iSelectors.Count() == KLmkNumberOfSelectors,
                    Panic( KLmkPanicSelectorArrayIndex ) );

    CLmkDlgSelectorImplBase*& selector = iSelectors[iSelectorState];
    if ( selector == NULL )
        { // iSelector's pointer will be modified!
        if ( iSelectorState == ELmkLandmarkSelector )
            {
            selector = CLmkDlgLmSelectorImpl::NewL( iDb );
            if (iCustomLandmarksTitle) 
            		{
            		selector->CLmkDlgSelectorImplBase::SetDialogTitleL(*iCustomLandmarksTitle);
            		}
            }
        else if ( iSelectorState == ELmkCategorySelector )
            {
            // Don't show empty categories -> 2nd parameter EFalse
            selector = CLmkDlgCategorySelectorImpl::NewL( iDb, EFalse );
            }
        else //ELmkFilteredLmSelector
            {
            __ASSERT_DEBUG( iSelectorState == ELmkFilteredLmSelector,
                            Panic( KLmkPanicWrongSelector ) );
            CPosLmCategoryCriteria* criteria = CPosLmCategoryCriteria::NewLC();
            criteria->SetCategoryItemId( aCategoryId );
            selector = CLmkDlgLmSelectorImpl::NewL( iDb, criteria );
            if (iCustomLandmarksTitle) 
            		{
            		selector->CLmkDlgSelectorImplBase::SetDialogTitleL(*iCustomLandmarksTitle);	
            		}
            CleanupStack::Pop(); // criteria, ownership was transferred
            }

        selector->SetMopParent(iMopParent);
        selector->SetSelectorImplParent( this );
        selector->SetMemento( *( iMementos[iSelectorState] ) );
        selector->SetEmptyDlgLabel(iCurrentLabelPrimResource , iCurrentLabelSecResource);
        }

    return *selector;
    }

// ----------------------------------------------------
// CLmkDlgCombiSelectorImpl::AddTabsL
// ----------------------------------------------------
//
void CLmkDlgCombiSelectorImpl::AddTabsL(CAknTabGroup& aTabGroup)
    {
    aTabGroup.SetTabFixedWidthL(EAknTabWidthWithTwoTabs);

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    TFileName filename( KLmkSystemMbmPath );
    filename.Append(KLandmarksIconFileName);
	CLmkUiUtils::GetFileWithCorrectDriveL( filename );

    //Add tab icon 1
    CFbsBitmap* lmkListBitmap = NULL;
    CFbsBitmap* lmkBitmapMask = NULL;
    
    AknsUtils::CreateIconLC( skin,
                            KAknsIIDQgnPropLmNamesTab2,
                            lmkListBitmap,
                            lmkBitmapMask,
                            /**filename*/filename,
                            EMbmLandmarksQgn_prop_lm_names_tab2,
                            EMbmLandmarksQgn_prop_lm_names_tab2_mask );

    CleanupStack::Pop(2); //lmkListBitmap, lmkBitmapMask

    //Add tab icon 2
    CFbsBitmap* lmkCatBitmap = NULL;
    CFbsBitmap* lmkCatBitmapMask = NULL;

    AknsUtils::CreateIconLC( skin,
                            KAknsIIDQgnPropLmCategoriesTab2,
                            lmkCatBitmap,
                            lmkCatBitmapMask,
                            /* *filename */filename,
                            EMbmLandmarksQgn_prop_lm_categories_tab2,
                            EMbmLandmarksQgn_prop_lm_categories_tab2_mask );

    CleanupStack::Pop(2); //lmkCatBitmap, lmkCatBitmapMask

    if(AknLayoutUtils::LayoutMirrored())
    	{
    	aTabGroup.AddTabL(ETabCategoryList, lmkCatBitmap, lmkCatBitmapMask); // ELmkByCategoryView
    	aTabGroup.AddTabL( ETabLandmarkList, lmkListBitmap, lmkBitmapMask ); //ELmkByLmView
    	}
    else
    	{
    	aTabGroup.AddTabL( ETabLandmarkList, lmkListBitmap, lmkBitmapMask ); //ELmkByLmView
    	aTabGroup.AddTabL(ETabCategoryList, lmkCatBitmap, lmkCatBitmapMask); // ELmkByCategoryView
    	}
    }

// ----------------------------------------------------------------------------
// CLmkDlgCombiSelectorImpl::GetBitmapFilenameL
// ----------------------------------------------------------------------------
//
HBufC* CLmkDlgCombiSelectorImpl::GetBitmapFilenameLC()
	{
	// Path and file name:
	HBufC* filename = HBufC::NewLC( KMaxFileName);
	TPtr fileNamePtr = 	filename->Des();
	fileNamePtr.Append(KLmkSystemMbmPath);
	fileNamePtr.Append(KLandmarksIconFileName);

	TBuf<20> aApp; // Holds drive name where the file is stored
	CEikAppUi *appUi = (CEikAppUi *)(iEnv->AppUi());
	HBufC* fullPath = HBufC::NewLC( KMaxFileName);
	*fullPath = appUi->Application()->AppFullName();
	TPtr fullPathPtr =  fullPath->Des();
	TParsePtr ptr(fullPathPtr);
	aApp.Copy(ptr.Drive());

	// Add drive to path & file name:
	TParse parse;
	User::LeaveIfError( parse.Set( fileNamePtr, &aApp, NULL ) );
	CleanupStack::PopAndDestroy(2); // fullPath, filename
	return parse.FullName().AllocLC();
	}

// ----------------------------------------------------------------------------
// CLmkDlgCombiSelectorImpl::TabChangedL
// ----------------------------------------------------------------------------
//
void CLmkDlgCombiSelectorImpl::TabChangedL( TInt /*aIndex*/ )
	{
	iNaviPane->PushL(*iNaviDeco);
	iNavigating = ETrue;

	TInt active = iTabGroup->ActiveTabId();

	if ( active == ETabCategoryList ) // After tab change, ETabCategoryList tab is active
		{
		iSelectors[ETabLandmarkList]->StopExecutionL();
		iSelectorState = ELmkLandmarkSelector;
		}
	else if( active == ETabLandmarkList ) // After tab change, ETabLandmarkList tab is active
		{
		iSelectors[ETabCategoryList]->StopExecutionL();
		iSelectorState = ELmkCategorySelector;
		}
	}
	
// ----------------------------------------------------------------------------
// CLmkDlgCombiSelectorImpl::SetDialogTitleL
// ----------------------------------------------------------------------------
//
void CLmkDlgCombiSelectorImpl::SetDialogTitleL(const TDesC& aTitle)
    {
    delete iCustomLandmarksTitle;
    iCustomLandmarksTitle = NULL;
    iCustomLandmarksTitle = aTitle.AllocL();
    }

		
//  End of File
