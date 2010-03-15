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
* Description:  Container class for Location Centre Application UI.
*
*/


// SYSTEM INCLUDES
#include <avkon.hrh>
#include <barsread.h> 
#include <lclocationappinfo.h>
#include <aknlists.h>
#include <AknIconArray.h>		// Icon Array
#include <eikclbd.h>			// Column Listbox
#include <gulicon.h>			// Gul Icon
#include <barsread.h>   		// TResourceReader
#include <lcapp.rsg>
#include <csxhelp/loccentre.hlp.hrh>
#include <AknUtils.h>
#include <eikspane.h> 		    // Status pane
#include <akntitle.h>       	// CAknTitlePane
#include <StringLoader.h>

// USER INCLUDES
#include "lccontainer.h"
#include "lcappui.h"
#include "lclistboxmodel.h"
#include "lcappuid.hrh"
#include "lcapp.hrh"

// CONSTANT DEFINITIONS
const TInt KLcNoOfElements = 1;
const TInt KMinSelectedItemNumber = 0;
const TInt KViewListboxGranularity = 3; // The number of default Applications
 
// ----------------- Member funtions for CLcContainer class ------------------

// ---------------------------------------------------------------------------
// CLcContainer::CLcContainer
// ---------------------------------------------------------------------------
//
CLcContainer::CLcContainer( MLcEventHandler&	aEventHandler,
							CLcListBoxModel&	aListBoxModel )
	:iEventHandler( aEventHandler ),
	iListBoxModel( &aListBoxModel )
    {
    }


// ---------------------------------------------------------------------------
// CLcContainer::~CLcContainer
// ---------------------------------------------------------------------------
//
CLcContainer::~CLcContainer()
    {
    // Delete the List box
	delete iListBox;
    }
    
// ---------------------------------------------------------------------------
// CLcContainer::NewL
// ---------------------------------------------------------------------------
//
CLcContainer* CLcContainer::NewL( const TRect&				aRect,
										 MLcEventHandler&	aEventHandler,
										 CLcListBoxModel&	aListBoxModel )     
    {
    CLcContainer* self = new (ELeave) CLcContainer( aEventHandler, aListBoxModel );
    CleanupStack::PushL( self );
    self->ConstructL( aRect );
    CleanupStack::Pop();
    
    return self;
    }
    
// ---------------------------------------------------------------------------
// CLcContainer::CLcContainer
// ---------------------------------------------------------------------------
//
void CLcContainer::ConstructL( const TRect& 	aRect )
    {
    // This is the cheif control for this application. This has to be made a 
    // Window owning control
    CreateWindowL();
    
	// Create a New Title for the View
	MakeTitleL( R_LOC_TITLE_CENTRE );
	    
    // Create the List box
    CreateListboxL();
      
    // Get the Help context
    iContextName = KLOC_HLP_LOC_CENTRE();
            
    SetRect(aRect);
    ActivateL();
    }

// ---------------------------------------------------------------------------
// void CLcContainer::GetHelpContext
// ---------------------------------------------------------------------------
//
void CLcContainer::GetHelpContext( TCoeHelpContext& aContext ) const
	{
    aContext.iContext = iContextName;
    aContext.iMajor = TUid::Uid( KLcAppUid );	
	}

// ---------------------------------------------------------------------------
// TInt CLcContainer::GetFocussedItem
// ---------------------------------------------------------------------------
//	
TInt CLcContainer::GetFocussedItem()
	{
	// No need to check for the List box.
    return iListBox->CurrentItemIndex();
	}

// ---------------------------------------------------------------------------
// void CLcContainer::UpdateL
// ---------------------------------------------------------------------------
//
void CLcContainer::UpdateL( CLcListBoxModel&	aListBoxModel )
	{
	TBool itemRemoved = EFalse;
	
	if ( aListBoxModel.MdcaCount() < iListBoxModel->MdcaCount() )
	    {
	    itemRemoved = ETrue;
	    }
	                              
	// Assign the latest list box model
	iListBoxModel = &aListBoxModel;
	
	// If there is a list box update it.
	if ( iListBox )
		{
		// First we have to update the Icons Array
		UpdateIconsL();
		
		if ( itemRemoved )
		    {
		    iListBox->HandleItemRemovalL();
		    }
        else
            {
            iListBox->HandleItemAdditionL();
            }		    
		
		// Set the last element as the Selected element if the element has been
		// removed.
	    TInt currentItem = iListBox->CurrentItemIndex();   
	    MDesCArray* array = iListBox->Model()->ItemTextArray();
	    if( KErrNotFound == currentItem || currentItem >= array->MdcaCount() )
	        {
	        TInt newSelected = array->MdcaCount() - 1;
	        if( newSelected >= KMinSelectedItemNumber )
	            {
	            iListBox->SetCurrentItemIndex( newSelected );
	            }
	        }		
		}
		
	// Update the List box.
	DrawDeferred();
	}

// ---------------------------------------------------------------------------
// void CLcContainer::UpdateIconsL
// ---------------------------------------------------------------------------
//
void CLcContainer::UpdateIconsL()
	{
	// Update the Icons only if there are any elements the List box
	if ( iListBoxModel->MdcaCount())
		{
		// Create a new Icons Array
	    CAknIconArray* icons = new( ELeave ) CAknIconArray( KViewListboxGranularity );
	    CleanupStack::PushL( icons );

		CLcLocationAppInfoArray& appArray( iListBoxModel->AppArray());
		TInt count = appArray.Count();
		// Add All the icons to the Icons array
		for ( TInt i = 0; i < count; i++ )
			{			
			icons->AppendL( appArray[i].IconL() );
			}

	    // Get old icons and delete those. This is done because SetIconArray will
	    // not do it.
	    CAknIconArray* oldIcons = 
	    	static_cast<CAknIconArray*>( iListBox->ItemDrawer()->ColumnData()->IconArray());
	    	    
	    delete oldIcons;

	    //Set New Icons to array
	    iListBox->ItemDrawer()->ColumnData()->SetIconArray( icons );  
	    CleanupStack::Pop(icons);		
		}				
	}
		
// ---------------------------------------------------------------------------
// void CLcContainer::SizeChanged
// ---------------------------------------------------------------------------
//
void CLcContainer::SizeChanged()
    {
    if ( iListBox )
    	{
    	iListBox->SetRect( Rect());
    	}
    }

// ---------------------------------------------------------------------------
// TInt CLcContainer::CountComponentControls
// ---------------------------------------------------------------------------
//
TInt CLcContainer::CountComponentControls() const
    {
   	return KLcNoOfElements;
    }

// ---------------------------------------------------------------------------
// CCoeControl* CLcContainer::CLcContainer
// ---------------------------------------------------------------------------
//
CCoeControl* CLcContainer::ComponentControl(TInt /* aIndex*/ ) const
    {
    return iListBox;
    }
    
// ---------------------------------------------------------------------------
// TKeyResponse CLcContainer::OfferKeyEventL
// ---------------------------------------------------------------------------
//    
TKeyResponse CLcContainer::OfferKeyEventL( const TKeyEvent& aKeyEvent,
								  				 TEventCode aType )
    {
    switch ( aKeyEvent.iCode )
        {
        case EKeyLeftArrow:
        case EKeyRightArrow:
            {
            // No action in this application for the Right arrow and left
            // arrow.
            return EKeyWasNotConsumed;  
            }
        default:
            {
            break;  
            }
        }
    // Now it's List box's job to process the key event
    return iListBox->OfferKeyEventL( aKeyEvent, aType ); 
    }   

// ---------------------------------------------------------------------------
// void CLcContainer::HandleResourceChange
// ---------------------------------------------------------------------------
//
void CLcContainer::HandleResourceChange( TInt aType )
	{
    // Pass the event to the base class
    CCoeControl::HandleResourceChange( aType );
    
    switch( aType )
        {
        // Dynamic Layout switch and Skin Change
        case KEikDynamicLayoutVariantSwitch:
        case KAknsMessageSkinChange:
            {
            // The Command Handler needs to handle this event. TRAP_IGNORE
            // the error incase it occurs
            TRAP_IGNORE( iEventHandler.HandleEventL( aType ));
            break;
            }
        default:
            {
            break;
            }
        }	
	}

// ---------------------------------------------------------------------------
// void CLcContainer::HandleListBoxEventL
// ---------------------------------------------------------------------------
//	
void CLcContainer::HandleListBoxEventL( CEikListBox*      /* aListBox */, 
                          				TListBoxEvent     aEventType )
	{
    switch (aEventType)
        {
        // List box Item Selection
        case EEventEnterKeyPressed:
        case EEventItemSingleClicked:
            {
            // A Location based Application or Content/Service has been
            // selected. This element has to be returned back to the Command
            // Hanlder inorder to enable it to take appropriate action.
            iEventHandler.HandleEventL( ELcOpen );
            break;  
            }
        default:
           break;
        }	
	}

// ---------------------------------------------------------------------------
// void CLcContainer::FocusChanged
// ---------------------------------------------------------------------------
//	                     
void CLcContainer::FocusChanged( TDrawNow aDrawNow )
	{
    CCoeControl::FocusChanged( aDrawNow );
    
    // The focus event has to be explicitly handed over to all the compound
    // controls since CCoeControl does not do that implicitly
    iListBox->SetFocus( IsFocused(), aDrawNow );	
	}

// ---------------------------------------------------------------------------
// void CLcContainer::MakeTitleL
// Sets the Title text
//
// @param aResourceText Resource to create title
// ---------------------------------------------------------------------------
//
void CLcContainer::MakeTitleL( TInt aResourceText )
	{
	CEikStatusPane* statusPane = 
	    static_cast<CEikStatusPane*>( iEikonEnv->AppUiFactory()->StatusPane());
	// Obtain the title from the Status Pane
	CAknTitlePane* title = static_cast<CAknTitlePane*>( statusPane->
        ControlL( TUid::Uid( EEikStatusPaneUidTitle )));
       
    // Set the Title's buffer    
    HBufC* buf = StringLoader::LoadL( aResourceText );
    title->SetText( buf ); // Takes ownership of buf
	}
		
// ---------------------------------------------------------------------------
// void CLcContainer::CreateListboxL
// ---------------------------------------------------------------------------
// 	
void CLcContainer::CreateListboxL()
    {
    // Create the List box
    iListBox = new ( ELeave ) CAknSingleLargeStyleListBox;
    iListBox->ConstructL( this, EAknListBoxSelectionList );
    iListBox->SetContainerWindowL( *this ); 
    iListBox->SetListBoxObserver( this );
    iListBox->CreateScrollBarFrameL(ETrue);
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( CEikScrollBarFrame::EOff,
                                                         CEikScrollBarFrame::EAuto ); 
     
    UpdateIconsL();
                                                                                                                      
    // The ownership of the List box model is retained with the Container.
    // This is needed because the model is used to return the identifier of the
    // Location based Application or Content/Service when its selected.                                                     
    iListBox->Model()->SetItemTextArray( iListBoxModel );  
    iListBox->Model()->SetOwnershipType( ELbmDoesNotOwnItemArray );
    iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
                                     
    iListBox->ActivateL();
    }
    
// End of File
