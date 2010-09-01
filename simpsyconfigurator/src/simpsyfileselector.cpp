/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Simulation PSY file selector dialog.
*
*/


// SYSTEM INCLUDES 
#include <AknIconArray.h>		// Icon Array
#include <eikclbd.h>			// Column Listbox
#include <aknlists.h>           // CAknPopupList
#include <aknconsts.h>
#include <AknIconArray.h>
#include <AknsUtils.h>
#include <AknsConstants.h>
#include <avkon.mbg>
#include <gulicon.h>
#include <fbs.h>

// USER INCLUDES
#include "simpsyfileselector.h"
#include "simpsyfileselectormodel.h"

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CSimPsyFileSelector::CSimPsyFileSelector
// ---------------------------------------------------------------------------
//
CSimPsyFileSelector::CSimPsyFileSelector()
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.
    }

// ---------------------------------------------------------------------------
// CSimPsyFileSelector::~CSimPsyFileSelector
// ---------------------------------------------------------------------------
//
CSimPsyFileSelector::~CSimPsyFileSelector()
    {
    // C++ Destructor. Free all resources associated with this class.
    
    // Delete the List box model.
    delete iListBoxModel;
    }

// ---------------------------------------------------------------------------
// void CSimPsyFileSelector::ConstructL
// ---------------------------------------------------------------------------
//
void CSimPsyFileSelector::ConstructL( CCoeControl*   aParent )
    {
    // Create the list box model
    iListBoxModel = CSimPsyFileSelectorModel::NewL();
    
    CAknSingleGraphicPopupMenuStyleListBox::ConstructL( aParent, EAknListBoxMenuList );
    
    CreateScrollBarFrameL( ETrue );
    ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                               CEikScrollBarFrame::EAuto ); 
     
    UpdateIconsL();

    Model()->SetItemTextArray( iListBoxModel );  
    Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    
    ActivateL();
    }

// ---------------------------------------------------------------------------
// void CSimPsyFileSelector::UpdateIconsL
// ---------------------------------------------------------------------------
//
void CSimPsyFileSelector::UpdateIconsL()
	{
    // Add icons to listbox
    CAknIconArray* iconArray = new( ELeave ) CAknIconArray( 1 );
    CleanupStack::PushL( iconArray );

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* mask = NULL;
    AknsUtils::CreateIconL( AknsUtils::SkinInstance(), 
                            KAknsIIDQgnPropNrtypNote,
                            bitmap,
                            mask,
                            KAvkonBitmapFile,
                            EMbmAvkonQgn_prop_nrtyp_note,
                            EMbmAvkonQgn_prop_nrtyp_note_mask );
    CleanupStack::PushL( bitmap );
    CleanupStack::PushL( mask );

    // Ownership is transferred to CGulIcon
    CGulIcon* icon = CGulIcon::NewL( bitmap, mask );
    CleanupStack::Pop( 2 ); // mask, bitmap
    CleanupStack::PushL( icon );
    iconArray->AppendL( icon ); // Ownership is transferred to CAknIconArray
    CleanupStack::Pop( icon ); // icon
	
    // Get old icons and delete those. This is done because SetIconArray will
    // not do it.
    CAknIconArray* oldIcons = 
    	static_cast<CAknIconArray*>( ItemDrawer()->ColumnData()->IconArray());
    	
	delete oldIcons;
	
    //Set New Icons to array
    ItemDrawer()->ColumnData()->SetIconArray( iconArray );  
    CleanupStack::Pop( iconArray );
    	 
	// Enable Marquee scrolling
	ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );	    	    	
    		
    DrawDeferred();
	}
	
// ---------------------------------------------------------------------------
//TPtrC CSimPsyFileSelector::SimulationFileNameL
// ---------------------------------------------------------------------------
//
TPtrC CSimPsyFileSelector::SimulationFileNameL()
	{   	
	return iListBoxModel->SimulationFileNameL( CurrentItemIndex());
	}

// ---------------------------------------------------------------------------
// void CSimPsyFileSelector::HandleResourceChange
// ---------------------------------------------------------------------------
//
void CSimPsyFileSelector::HandleResourceChange( TInt    aType )
    {
    // Pass the event to the base class
    CCoeControl::HandleResourceChange( aType );
    switch( aType )
        {
        // Dynamic Layout switch and Skin Change
        case KEikDynamicLayoutVariantSwitch:
            {
            SetRect( Rect());
            break;
            }
        case KAknsMessageSkinChange:
            {
            TRAP_IGNORE( UpdateIconsL());
            break;
            }
        default:
            {
            break;
            }
        } 
    DrawDeferred();
    }
