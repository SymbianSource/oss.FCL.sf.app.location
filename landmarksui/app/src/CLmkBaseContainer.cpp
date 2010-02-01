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
* Description:    LandmarksUi Content File -    Provides LM request container class methods.
*
*/







// INCLUDE FILES
#include "CLmkBaseContainer.h"
#include "MLmkKeyProcessor.h"
#include "LmkUID.h"
#include "LmkConsts.h"
#include <aknViewAppUi.h>
#include <aknnavide.h>
#include <barsread.h>
#include <akntabgrp.h>
#include <eikmenub.h>
#include "landmarks.hrh"
#include <AknsUtils.h>
#include <akntabgrp.h>
#include <eikapp.h>
#include <AknsSkinInstance.h>
#include <f32file.h>
#include <eikenv.h>
#include <StringLoader.h>
#include <eikenv.h>
#include <aknViewAppUi.h>
#include <apparc.h>
#include "CLmkAppSelectorImplBase.h"
#include <lmkerrors.h>
#include "Debug.h"

#if defined(_DEBUG)
// CONSTANTS
namespace {
_LIT( KPanicText, "CLmkBaseContainer" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicText, aReason );
    }
}  // namespace
#endif

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------
// CLmkBaseContainer::CLmkBaseContainer
// ----------------------------------------------------
//
CLmkBaseContainer::CLmkBaseContainer(
    MLmkKeyProcessor& aKeyProcessor,
    const TDesC& aHelpContext )
    :iKeyProcessor( aKeyProcessor ),
     iContextName( aHelpContext )
    {
    }

// ----------------------------------------------------
// CLmkBaseContainer::BaseConstructL
// ----------------------------------------------------
//
void CLmkBaseContainer::BaseConstructL()
    {
    CEikStatusPane* sp = StatusPane();
    // Fetch pointer to the default navi pane control
    iNaviPane = static_cast<CAknNavigationControlContainer*>
        ( sp->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    }
// ----------------------------------------------------
// CLmkBaseContainer::~CLmkBaseContainer
// ----------------------------------------------------
//
CLmkBaseContainer::~CLmkBaseContainer()
    {
    delete iSelector;
    }

// ----------------------------------------------------------------------------
// CLmkBaseContainer::SelectorImpl()
// ----------------------------------------------------------------------------
//
CLmkAppSelectorImplBase& CLmkBaseContainer::SelectorImpl()
    {
    __ASSERT_DEBUG( iSelector, Panic( KLmkPanicNullMember ) );
    return *iSelector;
    }
// ----------------------------------------------------
// CLmkBaseContainer::ActivateTabL
// ----------------------------------------------------
//
void CLmkBaseContainer::ActivateTabL(
    CAknNavigationDecorator& aNaviDecorator,
    TInt aTabId )
    {
    CEikStatusPane* sp = iEikonEnv->AppUiFactory()->StatusPane();
    CAknNavigationControlContainer* naviPane =
        static_cast<CAknNavigationControlContainer*>(
        sp->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));
    CAknTabGroup* tabGroup = static_cast<CAknTabGroup*>(
        aNaviDecorator.DecoratedControl() );
    tabGroup->SetActiveTabById( aTabId );
    naviPane->PushL( aNaviDecorator );
    }

// ----------------------------------------------------
// CLmkBaseContainer::GetHelpContext
// ----------------------------------------------------
//
void CLmkBaseContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iContext = iContextName;
    aContext.iMajor = TUid::Uid( KLmkAppUID3 );
    }

// ----------------------------------------------------
// CLmkBaseContainer::OfferKeyEventL
// ----------------------------------------------------
//
TKeyResponse CLmkBaseContainer::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
	DEBUG( CLmkBaseContainer::OfferKeyEventL entered );    	
    	
    CAknNavigationDecorator* naviDecorator = NaviPane().Top();

    // Check if navigation pane controls exists, because decorator does
    // not exist in every cases.
    if ( naviDecorator && naviDecorator->ControlType() ==
         CAknNavigationDecorator::ETabGroup )
        {
        if(aKeyEvent.iRepeats == 0)
			{
			if ( naviDecorator->DecoratedControl()->OfferKeyEventL(
			 aKeyEvent, aType ) == EKeyWasConsumed )
				{
				return EKeyWasConsumed;
				}
			}
        }

    if ( iKeyProcessor.ProcessKeyEventL( aKeyEvent, aType ) ==
         EKeyWasConsumed )
        {
        return EKeyWasConsumed;
        }

    if ( aType == EEventKey )
        {
	    TBool shiftKeyPressed = (aKeyEvent.iModifiers & EModifierShift) ||
    	                        (aKeyEvent.iModifiers & EModifierLeftShift) ||
        	                    (aKeyEvent.iModifiers & EModifierRightShift);
        
        if ( aKeyEvent.iCode == EKeyOK && !shiftKeyPressed && ContextSpecificMenuBar() )	        
            {
	        DEBUG( CLmkBaseContainer::OfferKeyEventL aKeyEvent.iCode == EKeyOK entered ); 	
            CEikMenuBar* menubar = MenuBar();
            if ( menubar )
                {
                menubar->SetMenuTitleResourceId( ContextSpecificMenuBar() );
                menubar->StopDisplayingMenuBar();
                TRAPD( err, menubar->TryDisplayMenuBarL() );
                // set ordinary menubar back even in case of error
                menubar->SetMenuTitleResourceId( OptionsMenuBar() );
                User::LeaveIfError( err );
                }
            return EKeyWasConsumed;
            }
		if ( aKeyEvent.iCode == EKeyEnter || aKeyEvent.iScanCode == EStdKeyEnter )
			{
			TKeyEvent enterKeyEvent( aKeyEvent );
			enterKeyEvent.iCode = EKeyOK;
			CEikButtonGroupContainer* cba = CEikButtonGroupContainer::Current();
			return cba->OfferKeyEventL( enterKeyEvent, aType );
			}
        }

    for ( TInt i( 0 ); i < CountComponentControls(); ++i )
        {
        if ( ComponentControl(i)->OfferKeyEventL( aKeyEvent, aType ) ==
             EKeyWasConsumed )
            {
            return EKeyWasConsumed;
            }
        }
	DEBUG( CLmkBaseContainer::OfferKeyEventL end );    	    
    return EKeyWasNotConsumed;    
    }

// ----------------------------------------------------
// CLmkBaseContainer::LmkAppUi
// ----------------------------------------------------
//
CAknViewAppUi* CLmkBaseContainer::ViewAppUi()
    {
    return static_cast<CAknViewAppUi*>( iAvkonAppUi );
    }

// ----------------------------------------------------
// CLmkBaseContainer::NaviPane
// ----------------------------------------------------
//
CAknNavigationControlContainer& CLmkBaseContainer::NaviPane()
    {
    // default navi pane should always exist:
    __ASSERT_DEBUG( iNaviPane, Panic( KLmkPanicNullNaviPane ) );
    return *iNaviPane;
    }

// ----------------------------------------------------
// CLmkBaseContainer::CreateNaviPaneL
// ----------------------------------------------------
//
CAknNavigationDecorator* CLmkBaseContainer::CreateNaviPaneL(
    TInt aResource )
    {
    CEikStatusPane* sp = iEikonEnv->AppUiFactory()->StatusPane();
    // Fetch pointer to the default navi pane control
    CAknNavigationControlContainer* naviPane =
        static_cast<CAknNavigationControlContainer*>(sp->ControlL(
        TUid::Uid(EEikStatusPaneUidNavi)));

    CAknNavigationDecorator* decorator = NULL;
    if ( aResource == KZeroResourceId ) // dummy tab
        {
        naviPane->PushDefaultL( EFalse );
        }
    else
        {
        decorator = naviPane->CreateTabGroupL(iKeyProcessor.TabObserver());
        decorator->SetControlType( CAknNavigationDecorator::ETabGroup );
        CAknTabGroup* tabGroup =
        static_cast< CAknTabGroup* > ( decorator->DecoratedControl() );
        AddTabsL( *tabGroup );
        }
    return decorator;
    }

// ----------------------------------------------------
// CLmkBaseContainer::StatusPane
// ----------------------------------------------------
//
CEikStatusPane* CLmkBaseContainer::StatusPane()
    {
    return iEikonEnv->AppUiFactory()->StatusPane();
    }

// ---------------------------------------------------------
// CLmkBaseContainer::MenuBar
// ---------------------------------------------------------
//
CEikMenuBar* CLmkBaseContainer::MenuBar()
    {
    return NULL;
    }

// ---------------------------------------------------------
// CLmkBaseContainer::ContextSpecificMenuBar
// ---------------------------------------------------------
//
TInt CLmkBaseContainer::ContextSpecificMenuBar()
    {
    return 0;
    }

// ---------------------------------------------------------
// CLmkBaseContainer::OptionsMenuBar
// ---------------------------------------------------------
//
TInt CLmkBaseContainer::OptionsMenuBar()
    {
    return 0;
    }

// Code added by gaurav fro Tab Icons -start-
// ----------------------------------------------------
// CLmkBaseContainer::AddTabsL
// ----------------------------------------------------
//
void CLmkBaseContainer::AddTabsL(CAknTabGroup& aTabGroup)
    {
    aTabGroup.SetTabFixedWidthL(EAknTabWidthWithTwoTabs);

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    HBufC* filename = GetBitmapFilenameLC();
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* bitmapMask = NULL;

    //Add tab icon 1
    AknsUtils::CreateIconLC( skin,
                            KAknsIIDQgnPropLmNamesTab2,
                            bitmap,
                            bitmapMask,
                            *filename,
                            EMbmLandmarksQgn_prop_lm_names_tab2,
                            EMbmLandmarksQgn_prop_lm_names_tab2_mask );
    aTabGroup.AddTabL( ELmkByLmView, bitmap, bitmapMask ); //ELmkByLmView
    CleanupStack::Pop(2); //bitmap, bitmapmask


    //Add tab icon 2
    AknsUtils::CreateIconLC( skin,
                            KAknsIIDQgnPropLmCategoriesTab2,
                            bitmap,
                            bitmapMask,
                            *filename,
                            EMbmLandmarksQgn_prop_lm_categories_tab2,
                            EMbmLandmarksQgn_prop_lm_categories_tab2_mask );
    aTabGroup.AddTabL(ELmkByCategoryView, bitmap, bitmapMask ); // ELmkByCategoryView
    CleanupStack::Pop(2); //bitmap, bitmapmask

    CleanupStack::PopAndDestroy(); //filename
    }

// ----------------------------------------------------------------------------
// CLmkBaseContainer::GetBitmapFilenameL
// ----------------------------------------------------------------------------
//
HBufC* CLmkBaseContainer::GetBitmapFilenameLC()
    {

    TBuf<2> aApp;
    CEikAppUi *appUi = (CEikAppUi *)(iCoeEnv->AppUi());
    TFileName* full = new( ELeave ) TFileName;
    CleanupStack::PushL( full );
    *full = appUi->Application()->AppFullName();
    TParsePtr ptr(*full);
    aApp.Copy(ptr.Drive());

	// Path and file name:
    TFileName* file = new( ELeave ) TFileName;
    CleanupStack::PushL( file );
    file->Append( KLmkSystemMbmPath ); // always safe
    file->Append( KLandmarksIconFileName ); // always safe

    // Add drive to path & file name:
    TParse* parse = new(ELeave) TParse;
    CleanupStack::PushL( parse );
    User::LeaveIfError( parse->Set( *file, &aApp, NULL ) );
	HBufC* retFileName = parse->FullName().AllocLC();
	CleanupStack::Pop();//retFileName
	CleanupStack::PopAndDestroy(3);
	CleanupStack::PushL( retFileName );
    return retFileName;
    }

// ---------------------------------------------------------
// CLmkBaseContainer::ResolveMskDisplayItem()
// ---------------------------------------------------------
//
TLmkMskDispItem CLmkBaseContainer::ResolveMskDisplayItem()
	{
	TLmkMskDispItem dispItem  = ELabel;
	if (iSearchInitiated && (!SelectorImpl().ListVisibleItemCount()))
		{
		dispItem = ENoMsk;
		}
	else
		{
		const CArrayFix<TInt>* markedIndexes = NULL;
		markedIndexes = SelectorImpl().MarkedIndexes();
        if(markedIndexes->Count() > 0 ||
        !SelectorImpl().ListVisibleItemCount())
            {
            dispItem = EContextMenu;
            }
		}

	return dispItem;
	}

#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------
// CLmkBaseContainer::SetListBoxObserver()
// ---------------------------------------------------------
//
void CLmkBaseContainer::SetListBoxObserver(MEikListBoxObserver* /*aObserver*/)
	{
	//Since CLmkCategorySettingsContainer is directly derived from
	// CLmkBaseContainer, is the reason this function is made virtual
	//in this class. Most of the other container classes are derived
	//from CLmkLbWithFilterContainer, which is derived from CLmkBaseContainer,
	//hence there will be two overriding of this function,
	//1. In 	CLmkCategorySettingsContainer
	//2. In     CLmkLbWithFilterContainer
	//since both the above classes holds the CEikListBox member, where the
	//observer needs to be set.
	}
#endif//RD_SCALABLE_UI_V2
// End of File
