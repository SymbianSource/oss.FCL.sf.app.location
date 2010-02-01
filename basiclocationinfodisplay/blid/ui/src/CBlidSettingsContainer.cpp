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
* Description:  Provides blid request container class methods.
*
*/



// INCLUDE FILES
#include <aknnavide.h>
#include <aknlists.h>
#include <Blid.rsg>
#include <AknDef.h>
#include <csxhelp/blid.hlp.hrh>

#include "CBlidSettingsContainer.h"
#include "CBlidSettingsLBModel.h"
#include "Blid.hrh"


// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CBlidSettingsContainer::NewL
// ---------------------------------------------------------
//
CBlidSettingsContainer* CBlidSettingsContainer::NewL(    
                            MKeyProcessor& aKeyProcessor,
                            MObjectProvider* aParent,
		                    const TRect& aRect,
                            MBlidSettings* aSettingsModel,
                            CBlidBaseView& aView )
	{
	CBlidSettingsContainer* view =
		new (ELeave) CBlidSettingsContainer( aKeyProcessor, aView );
	CleanupStack::PushL(view);
	view->SetMopParent( aParent );
	view->ConstructL(aRect, aSettingsModel );
    CleanupStack::Pop( view ); // view
	return view;
	}

// ---------------------------------------------------------
// CBlidSettingsContainer::CBlidSettingsContainer
// ---------------------------------------------------------
//
CBlidSettingsContainer::CBlidSettingsContainer( MKeyProcessor& aKeyProcessor,
                                                CBlidBaseView& aView )
    :CBlidBaseContainer( aKeyProcessor, KBLID_HLP_SETTINGS ), iView(aView)
    {
    }

// ---------------------------------------------------------
// CBlidSettingsContainer::ConstructL
// EPOC two phased constructor
// ---------------------------------------------------------
//
void CBlidSettingsContainer::ConstructL(const TRect& aRect, 
                                        MBlidSettings* aSettingsModel)
    {
    CreateWindowL();
    CreateNaviPaneL( ETrue );
    
    iListBox = new( ELeave ) CAknSettingStyleListBox;
    iListBox->SetContainerWindowL( *this );
    iListBox->ConstructL( this, EAknListBoxMenuList );
    
    // Make scroll arrows
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL(
    CEikScrollBarFrame::EOff,CEikScrollBarFrame::EOn );


    // create engine model for listbox, pass ownership
    MDesCArray* model = CBlidSettingsLBModel::NewLC( aSettingsModel );
    iListBox->Model()->SetItemTextArray( model );
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop(); // model

    MakeTitleL(R_BLID_TITLE_SETTINGS);
    SetRect(aRect);
    ActivateL();
    }

// ---------------------------------------------------------
// CBlidSettingsContainer::~CBlidSettingsContainer
// ---------------------------------------------------------
//
CBlidSettingsContainer::~CBlidSettingsContainer()
    {
    delete iDecoratedTabGroup;
    delete iListBox;
    }

// ---------------------------------------------------------
// CBlidSettingsContainer::ListBox
// ---------------------------------------------------------
//
CAknSettingStyleListBox* CBlidSettingsContainer::ListBox() const
    {
    return iListBox;
    }

// ---------------------------------------------------------
// CBlidSettingsContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CBlidSettingsContainer::SizeChanged()
    {
    iListBox->SetRect( Rect() );
    }

// ---------------------------------------------------------
// CBlidSettingsContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CBlidSettingsContainer::CountComponentControls() const
    {
    return 1; // return nbr of controls inside this container
    }

// ---------------------------------------------------------
// CBlidSettingsContainer::ComponentControl(TInt aIndex) const
// ---------------------------------------------------------
//
CCoeControl* CBlidSettingsContainer::ComponentControl(TInt /*aIndex*/ ) const
    {    
    return iListBox;
    }
    
// ---------------------------------------------------------
// CBlidSettingsContainer::HandleResourceChange
// ---------------------------------------------------------
//
void CBlidSettingsContainer::HandleResourceChange(TInt aType)
    {
    CBlidBaseContainer::HandleResourceChange( aType );
    switch(aType)
    	{
    	case KEikDynamicLayoutVariantSwitch:
	    	 {
             iView.Update(this);  
	    	 break;
	    	}
        default:
	        {
	        break;	
	        }
	   }   
    }

// ----------------------------------------------------------------------------
// CBlidSettingsContainer::FocuChanged()
// ----------------------------------------------------------------------------
//
void CBlidSettingsContainer::FocusChanged(TDrawNow aDrawNow)
	{
	if (iListBox)
		{
		iListBox->SetFocus(IsFocused(), aDrawNow);			
		}
	}

// ----------------------------------------------------------------------------
// CBlidSettingsContainer::OfferKeyEventL
// ----------------------------------------------------------------------------
//
TKeyResponse CBlidSettingsContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType)
    {    
    if ( aType == EEventKeyUp || aType == EEventKeyDown )
		{
		TInt lIndex = iListBox->CurrentItemIndex();
        switch( lIndex )
            {
            case EBlidSettingsResetOdometer:
                {
				iView.RemoveMiddleSoftKeyLabel( EBlidCmdMskChangeSetting );
				iView.SetMiddleSoftKeyLabelL( R_BLID_MSK_RESET,EBlidCmdMskResetSetting );				
            	break;
                }
            // Other values are not accepted.
            case EBlidSettingsBacklight:
            case EBlidSettingsArrivalTone:
            case EBlidSettingsCalibration:
                {
				iView.RemoveMiddleSoftKeyLabel( EBlidCmdMskResetSetting );
				iView.SetMiddleSoftKeyLabelL( R_BLID_MSK_CHANGE, EBlidCmdMskChangeSetting );				
				break;
                }
			default:
				{
				break;					
				}                
            }
		return EKeyWasConsumed;
		}
    return CBlidBaseContainer::OfferKeyEventL( aKeyEvent, aType );
    }
    	
#ifdef RD_SCALABLE_UI_V2 
// ---------------------------------------------------------------------------
// CBlidSettingsContainer::SetListBoxObserver
// ---------------------------------------------------------------------------
//
void CBlidSettingsContainer::SetListBoxObserver( MEikListBoxObserver* aObserver )
    {
    iListBox->SetListBoxObserver( aObserver );
    }
#endif // RD_SCALABLE_UI_V2

// End of File
