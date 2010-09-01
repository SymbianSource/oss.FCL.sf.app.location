/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides Blid request container class methods.
*
*/


// INCLUDE FILES
#include <akncontext.h> 
#include <aknnavide.h> 
#include <eikspane.h> 
#include <barsread.h> 
#include <akntabgrp.h>
#include <AknIconArray.h>
#include <gulicon.h>
#include <StringLoader.h> 
#include <akntitle.h> 
#include <eiklbx.h> 
#include <avkon.hrh>
#include <coehelp.h>
#include <AknsUtils.h>
#include <akntabgrp.h>
#include <blid.mbg>
#include "CBlidBaseContainer.h"
#include "MKeyProcessor.h"
#include "BlidUID.h"
#include "bliduiconsts.h"
#include "Blidutils.h"
#include "Blid.hrh"
#include "CBlidAppUi.h"

TBool CBlidBaseContainer::iGPSAvailability = EFalse;
// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------
// CBlidBaseContainer::CBlidBaseContainer
// First phase constructor, can not leave
// ----------------------------------------------------
//
CBlidBaseContainer::CBlidBaseContainer(
    MKeyProcessor& aKeyProcessor,
    const TDesC& aHelpContext )
    :iKeyProcessor(aKeyProcessor)
    {
    iContextName = aHelpContext;
    }

// ----------------------------------------------------
// CBlidBaseContainer::~CBlidBaseContainer
// Destructor, frees allocated resources
// ----------------------------------------------------
//
CBlidBaseContainer::~CBlidBaseContainer()
    {
    }

// ----------------------------------------------------
// CBlidBaseContainer::ActivateTabL
// Activates a tab specified bt tab id
// ----------------------------------------------------
//
void CBlidBaseContainer::ActivateTabL( 
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
// CBlidBaseContainer::GetHelpContext
// Get the help context for the application
// ----------------------------------------------------
//
void CBlidBaseContainer::GetHelpContext( TCoeHelpContext& aContext ) const
    {
    aContext.iContext = iContextName;
    aContext.iMajor = TUid::Uid(KBLIDUID3);
    }

// ----------------------------------------------------
// CBlidBaseContainer::OfferKeyEventL
// Handles the key event
// ----------------------------------------------------
//
TKeyResponse CBlidBaseContainer::OfferKeyEventL(
                    const TKeyEvent& aKeyEvent,
                    TEventCode aType)
    {
    CEikStatusPane* sp = StatusPane();
    // Fetch pointer to the default navi pane control
    CAknNavigationControlContainer* naviPane = 
        static_cast<CAknNavigationControlContainer*>(sp->ControlL( 
        TUid::Uid(EEikStatusPaneUidNavi)));
    CAknNavigationDecorator* naviDecorator = naviPane->Top();

    // Check if navigation pane controls exists, because decorator does 
    // not exist in every cases. E.g. after contact fetch in "new rule" 
    // command operation naviDecorator is null. 
    if ( naviDecorator && naviDecorator->ControlType() == 
        CAknNavigationDecorator::ETabGroup )
        {
        if ( naviDecorator->DecoratedControl()->OfferKeyEventL( 
            aKeyEvent, aType ) == EKeyWasConsumed )
            {
            return EKeyWasConsumed;
            }
        }    

    if ( iKeyProcessor.ProcessKeyEventL( aKeyEvent, aType ) == 
        EKeyWasConsumed )
        {
        return EKeyWasConsumed;
        }

    for ( TInt i = 0; i < CountComponentControls(); i++ )
        {
        if ( ComponentControl(i)->OfferKeyEventL( aKeyEvent, aType ) == 
            EKeyWasConsumed )
            {
            return EKeyWasConsumed;
            }
        }
    
    return EKeyWasNotConsumed;    
    }

// ----------------------------------------------------
// CBlidBaseContainer::HandleResourceChange
// Handles the change in app resource
// ----------------------------------------------------
//
void CBlidBaseContainer::HandleResourceChange(TInt aType)
    {
    CCoeControl::HandleResourceChange(aType);
    if( aType == KAknsMessageSkinChange )
        {        
        static_cast<CBlidAppUi*>(iCoeEnv->AppUi())->SkinChange();
        }
    }

// ----------------------------------------------------
// CBlidBaseContainer::CreateNaviPaneL
// Creates the navigation pane
// ----------------------------------------------------
//
CAknNavigationDecorator* CBlidBaseContainer::CreateNaviPaneL( 
                                             TBool aDefaultNaviPane )
    {    
    CEikStatusPane* sp = StatusPane();
    if( !sp )
    	{
    	User::Leave( KErrGeneral );
    	}
    // Fetch pointer to the default navi pane control
    CAknNavigationControlContainer* naviPane = 
        static_cast<CAknNavigationControlContainer*>(sp->ControlL( 
        TUid::Uid(EEikStatusPaneUidNavi)));
    if( !naviPane )
    	{
    	User::Leave( KErrGeneral );
    	}
    CAknNavigationDecorator* decorator = NULL;
    if ( aDefaultNaviPane ) // dummy tab
        {        
        naviPane->PushDefaultL(EFalse);
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
// CBlidBaseContainer::AddIconL
// Add icon
// ----------------------------------------------------
//
void CBlidBaseContainer::AddIconL
        ( CAknIconArray* aIcons,
          const TAknsItemID aSkinID,
          const TDesC& aIconFileWithPath,
          TInt aIconGraphicsIndex,
          TInt aIconGraphicsMaskIndex )
    {
    CFbsBitmap *bitmap = NULL;    
    CFbsBitmap* mask = NULL; 
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();
    AknsUtils::CreateIconLC( skin,
                            aSkinID,
                            bitmap,
                            mask,
                            aIconFileWithPath,
                            aIconGraphicsIndex,
                            aIconGraphicsMaskIndex );
    
    // Append it to icons array
	CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::Pop( 2 ); // mask, bitmap. icon has now ownership
	CleanupStack::PushL( icon );
	aIcons->AppendL( icon );
    CleanupStack::Pop( ); // icon

    }

// ----------------------------------------------------
// CBlidBaseContainer::MakeTitleL
// Sets the title to text specified by a resource
// ----------------------------------------------------
//
void CBlidBaseContainer::MakeTitleL( TInt aResourceText )
    {
    HBufC* buf = StringLoader::LoadLC(aResourceText);       
    CAknTitlePane* title = static_cast<CAknTitlePane*>( StatusPane()->
                            ControlL( TUid::Uid( EEikStatusPaneUidTitle ) ) );
    CleanupStack::Pop(); //buf
    title->SetText(buf); // Takes ownership of buf
    }

// ----------------------------------------------------
// CBlidBaseContainer::MakeEmptyTextListBoxL
// 
// ----------------------------------------------------
//
void CBlidBaseContainer::MakeEmptyTextListBoxL( 
                CEikListBox* aListBox,
                TInt aResourceText )
    {
    HBufC* textFromResourceFile;
    textFromResourceFile = 
        StringLoader::LoadLC( aResourceText, iCoeEnv );        
    aListBox->View()->SetListEmptyTextL( *textFromResourceFile );
    CleanupStack::PopAndDestroy();  // textFromResourceFile.
    }

// ----------------------------------------------------
// CBlidBaseContainer::StatusPane
// Returns the status pane
// ----------------------------------------------------
//
CEikStatusPane* CBlidBaseContainer::StatusPane()
    {
    return iEikonEnv->AppUiFactory()->StatusPane();
    }

// ----------------------------------------------------
// CBlidBaseContainer::AddTabsL
// Adds the tabs
// ----------------------------------------------------
//
void CBlidBaseContainer::AddTabsL(CAknTabGroup& aTabGroup)
    {
    aTabGroup.SetTabFixedWidthL(EAknTabWidthWithTwoTabs);
	MAknsSkinInstance* skin = AknsUtils::SkinInstance(); 
    HBufC* filename = BlidUtils::GetBitmapFilenameLC();        
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* bitmapMask = NULL;
        
    //Add tab icon 1
    AknsUtils::CreateIconLC( skin,
                            KAknsIIDQgnPropBlidNavigationTab3,
                            bitmap,
                            bitmapMask,
                            *filename,
                            EMbmBlidQgn_prop_blid_navigation_tab3,
                            EMbmBlidQgn_prop_blid_navigation_tab3_mask );             
    aTabGroup.AddTabL( EBlidNavigationView, bitmap, bitmapMask );
    CleanupStack::Pop(2); //bitmap, bitmapmask

    //Add tab icon 2
    AknsUtils::CreateIconLC( skin,
                            KAknsIIDQgnPropBlidTripTab3,
                            bitmap,
                            bitmapMask,
                            *filename,
                            EMbmBlidQgn_prop_blid_trip_tab3,
                            EMbmBlidQgn_prop_blid_trip_tab3_mask );             
    aTabGroup.AddTabL( EBlidTripMeterView, bitmap, bitmapMask );
    CleanupStack::Pop(2); //bitmap, bitmapmask

    
    
    CleanupStack::PopAndDestroy(); //filename
    }

// ---------------------------------------------------------
// CBlidBaseContainer::SetGPSAvailability
// Sets the GPS availability to ETrue or EFalse
// ---------------------------------------------------------
//
void CBlidBaseContainer::SetGPSAvailability(const TBool aAvailability)
    {
    iGPSAvailability = aAvailability;
    }

// ---------------------------------------------------------
// CBlidBaseContainer::IsGPSAvailable
// Checks if GPS is available
// ---------------------------------------------------------
//
TBool CBlidBaseContainer::IsGPSAvailable() 
    {
    return iGPSAvailability;
    }

// End of File  
