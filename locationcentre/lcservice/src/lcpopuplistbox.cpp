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
* Description:  List box used to display the Location Centre pop-up.
*
*/


// SYSTEM INCLUDES 
#include <akniconarray.h>		// Icon Array
#include <eikclbd.h>			// Column Listbox

// USER INCLUDES
#include "lcpopuplistbox.h"
#include "lcpopuplistboxmodel.h"
#include "lclocationappinfo.h"

// CONSTANT DEFINTION
const TInt KLcPopupListboxGranularity = 4;

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CLcPopupListBox::CLcPopupListBox
// ---------------------------------------------------------------------------
//
CLcPopupListBox::CLcPopupListBox( CLcLocationAppInfoArray*	aAppArray )
	:iAppArray( aAppArray )
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.
    }

// ---------------------------------------------------------------------------
// CLcPopupListBox::~CLcPopupListBox
// ---------------------------------------------------------------------------
//
CLcPopupListBox::~CLcPopupListBox()
    {
    // C++ Destructor. Free all resources associated with this class.
    
    // Delete the List box model.
    delete iListBoxModel;
    
    // Delete the Application Info array
    delete iAppArray;
    }

// ---------------------------------------------------------------------------
// void CLcPopupListBox::ConstructL
// ---------------------------------------------------------------------------
//
void CLcPopupListBox::ConstructL( CCoeControl*   aParent )
    {
    // Create the list box model
    iListBoxModel = CLcPopupListBoxModel::NewL( *iAppArray );
    
    CAknSingleGraphicPopupMenuStyleListBox::ConstructL( aParent, EAknListBoxMenuList );
    CreateScrollBarFrameL(ETrue);
    ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                               CEikScrollBarFrame::EAuto ); 
     
    UpdateIconsL();
                                                                                                                      
    // The ownership of the List box model is retained with the Container.
    // This is needed because the model is used to return the identifier of the
    // Location based Application or Content/Service when its selected.                                                     
    Model()->SetItemTextArray( iListBoxModel );  
    Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    
    ActivateL();
    }

// ---------------------------------------------------------------------------
// void CLcPopupListBox::UpdateModel
// ---------------------------------------------------------------------------
//
void CLcPopupListBox::UpdateListBoxL( CLcLocationAppInfoArray*	aAppArray )
	{
	// Delete the existing contents and set the new Application info array
	// to the listbox model.
	delete iAppArray;
	iAppArray = aAppArray;
	
	iListBoxModel->UpdateModel( *iAppArray );
	
	UpdateIconsL();
	
	HandleItemAdditionL();
	HandleItemRemovalL();
	
	DrawDeferred();
	}

// ---------------------------------------------------------------------------
// void CLcPopupListBox::UpdateIconsL
// ---------------------------------------------------------------------------
//
void CLcPopupListBox::UpdateIconsL()
	{
	TInt count = iAppArray->Count();
	// Update the Icons only if there are any elements the List box
	if ( count )
		{
		// Create a new Icons Array
	    CAknIconArray* icons = new( ELeave ) CAknIconArray( KLcPopupListboxGranularity );
	    CleanupStack::PushL( icons );

		// Add All the icons to the Icons array
		for ( TInt i = 0; i < count; i++ )
			{			
			icons->AppendL(( *iAppArray )[i].IconL() );
			}

	    // Get old icons and delete those. This is done because SetIconArray will
	    // not do it.
	    CAknIconArray* oldIcons = 
	    	static_cast<CAknIconArray*>( ItemDrawer()->ColumnData()->IconArray());
	    	    
	    delete oldIcons;

	    //Set New Icons to array
	    ItemDrawer()->ColumnData()->SetIconArray( icons );  
	    CleanupStack::Pop(icons);		
		}
    ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );   	
    
    DrawDeferred();
	}
	
// ---------------------------------------------------------------------------
// TInt CLcPopupListBox::AppInfoId
// ---------------------------------------------------------------------------
//
TPtrC CLcPopupListBox::AppInfoId() const
	{   	
	return ( *iAppArray )[ CurrentItemIndex() ].Id();
	}

// ---------------------------------------------------------------------------
// void CLcPopupListBox::HandleResourceChange
// ---------------------------------------------------------------------------
//
void CLcPopupListBox::HandleResourceChange( TInt    aType )
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
