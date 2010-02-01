/*
* Copyright (c)  Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  List box defintion for Location System UI
*
*/


// SYSTEM INCLUDES 
#include <eikclbd.h>
#include <gulicon.h>
#include <akniconarray.h>

// USER INCLUDES
#include "lclistbox.h"
#include "lclistboxmodel.h"
#include "lclocationappinfo.h"

// CONSTANT DEFINTION
const TInt KLcViewListboxGranularity = 2;


// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CLcListBox::CLcListBox
// ---------------------------------------------------------------------------
//
CLcListBox::CLcListBox()
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.
    }

// ---------------------------------------------------------------------------
// CLcListBox::~CLcListBox
// ---------------------------------------------------------------------------
//
CLcListBox::~CLcListBox()
    {
    // C++ Destructor. Free all resources associated with this class.
        
    // Delete the listbox model
    delete iListBoxModel;
    }

// ---------------------------------------------------------------------------
// void CLcListBox::ConstructL
// ---------------------------------------------------------------------------
//
void CLcListBox::ConstructL( 	   CLcLocationAppInfoArray*      aAppArray,
							 const CCoeControl* 		   		 aParent,
							 	   MEikListBoxObserver*			 aObserver )
    {
    // Create the List box model using the Application Info array
    iListBoxModel = CLcListBoxModel::NewL( aAppArray );    
    
    // Call the base class constructor
    CAknSingleLargeStyleListBox::ConstructL( aParent, EAknListBoxSelectionList );
    
    // If the parent window has been set, the set it as the Container window
    if ( aParent )
    	{
    	SetContainerWindowL( *aParent );
    	}
    // If the list box observer has been set then set ot to the list box
    if ( aObserver )
    	{
    	SetListBoxObserver( aObserver );
    	}
    CreateScrollBarFrameL( ETrue );
    
    ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                               CEikScrollBarFrame::EAuto );
                                                         
    // The ownership of the list box model is retained with the Container.
    // This is because the model contains the additional functionality of
    // changing the settings values in addition to retreiving it.                                                       
    Model()->SetItemTextArray( iListBoxModel );  
    Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );                                                          
    
    UpdateIconsL();    
   	// ItemDrawer()->ColumnData()->SetIconArray( NULL );
    ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
    
    }

// ---------------------------------------------------------------------------
// CLcListBox* CLcListBox::NewL
// ---------------------------------------------------------------------------
//
CLcListBox* CLcListBox::NewL( 		
									CLcLocationAppInfoArray*     aAppArray,
							  const CCoeControl* 		   		 aParent,
							  		MEikListBoxObserver*	     aObserver )
    {
    CLcListBox* self = new ( ELeave ) CLcListBox;
    CleanupStack::PushL( self );
    self->ConstructL( aAppArray, aParent, aObserver );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// void CLcListBox::UpdateListBoxContents
// ---------------------------------------------------------------------------
//
void CLcListBox::UpdateListBoxContentsL( 
							CLcLocationAppInfoArray*      aAppArray )
    {
    // This function transfers the ownership of the Application information
    // array. So first we have to delete the existing array and then we can
    iListBoxModel->UpdateListBoxContents( aAppArray );
    
    UpdateIconsL();
    }
    
// -----------------------------------------------------------------------------
// CLocPsySettingsContainer::UpdateIconsL
// -----------------------------------------------------------------------------
void CLcListBox::UpdateIconsL()
    {
    CLcLocationAppInfoArray& iconArray( iListBoxModel->AppInfoArray());
    
    CAknIconArray* icons = new( ELeave ) CAknIconArray( KLcViewListboxGranularity );
    CleanupStack::PushL( icons );

	// Append each of the Icon elements to this array.
	for ( TInt i = 0; i < iconArray.Count(); i++ )
		{
		TRAPD( error, icons->AppendL( iconArray[i].IconL()));
		if ( error )
			{
			User::Leave( error );
			}
		}

    // Destroy the old icons because SetIconArray() does not destroy them.
    CArrayPtr<CGulIcon>* oldIcons = ItemDrawer()->ColumnData()->IconArray();
    if( oldIcons )
        {
        oldIcons->ResetAndDestroy();
        delete oldIcons;
        }

    // Transfer ownership of icon array to the lbx.
    // SetIconArray() Does not delete the Old icon array
    ItemDrawer()->ColumnData()->SetIconArray( icons );
    CleanupStack::Pop( icons );

    // Draws listbox again
    HandleItemAdditionL();
    HandleItemRemovalL();
    }     

