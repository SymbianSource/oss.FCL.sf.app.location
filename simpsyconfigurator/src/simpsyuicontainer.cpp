/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Simulation PSY Configuration UI server application class.
*
*/

#include <aknlists.h>

#include "simpsyuiappui.h"
#include "simpsyuicontainer.h"
#include "simpsyuilbmodel.h"

// ---------------------------------------------------------
// void CSimPsyUiContainer::ConstructL
// ---------------------------------------------------------
//
void CSimPsyUiContainer::ConstructL( const TRect& 			aRect,
										   CSimPsyUiAppUi*	aAppUi )
    {
    if( !aAppUi )
    	{
    	User::Leave( KErrArgument );
   		}
   	iAppUi = aAppUi;
    CreateWindowL();
    CreateListboxL();
    SetRect(aRect);
   	ActivateL();
    }

// ---------------------------------------------------------
// CSimPsyUiContainer::~CSimPsyUiContainer
// ---------------------------------------------------------
//
CSimPsyUiContainer::~CSimPsyUiContainer()
    {
    delete iListBox;
    iListBox = NULL;
    }

// ---------------------------------------------------------
// TKeyResponse CSimPsyUiContainer::OfferKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CSimPsyUiContainer::OfferKeyEventL
						( const TKeyEvent& /*aKeyEvent*/,
				  				TEventCode /*aType*/ )
    {
    return TKeyResponse( EKeyWasConsumed ) ;
    }

// ---------------------------------------------------------
// void CSimPsyUiContainer::UpdateListBox
// ---------------------------------------------------------
//
void CSimPsyUiContainer::UpdateListBox()
	{
	if ( iListBox )
		{
		iListBox->DrawDeferred();	
		}
	}

// ---------------------------------------------------------
// void CSimPsyUiContainer::CreateListboxL
// ---------------------------------------------------------
//    
void CSimPsyUiContainer::CreateListboxL()
	{
	
	delete iListBox;
	iListBox = NULL;

	iListBox = new( ELeave ) CAknDoubleStyleListBox;
    iListBox->SetContainerWindowL( *this );
    iListBox->ConstructL( this, EAknGenericListBoxFlags);
    
	CSimPsyUiLBModel*	lLBModel = CSimPsyUiLBModel::NewLC();
	
	iListBox->Model()->SetItemTextArray( lLBModel ); 
    iListBox->Model()->SetOwnershipType( ELbmOwnsItemArray );
    CleanupStack::Pop(lLBModel); // model
    
    iListBox->DrawNow();
	}

// ---------------------------------------------------------
// CSimPsyUiContainer::SizeChanged()
// Called by framework when the view size is changed
// ---------------------------------------------------------
//
void CSimPsyUiContainer::SizeChanged()
    {
    if ( iListBox )
    	{
    	iListBox->SetRect( Rect() );
   	 	}
    }

// ---------------------------------------------------------
// CSimPsyUiContainer::CountComponentControls() const
// ---------------------------------------------------------
//
TInt CSimPsyUiContainer::CountComponentControls() const
    {
    if( iListBox )
    	{
    	return 1;
    	}
    else
    	{
    	return 0;		
    	}
    }

// ---------------------------------------------------------
// CSimPsyUiContainer::ComponentControl
// ---------------------------------------------------------
//
CCoeControl* CSimPsyUiContainer::ComponentControl( TInt /*aIndex*/ ) const
    {
 	if( iListBox )
	 	{
		return iListBox;	
	 	}
	 else
	 	{
	 	return NULL;
	 	}
    }
// ---------------------------------------------------------
// CSimPsyUiContainer::FocusChanged
// ---------------------------------------------------------
//
void CSimPsyUiContainer::FocusChanged(TDrawNow aDrawNow)
	{
	if( iListBox )
		{
		iListBox->SetFocus( IsFocused(), aDrawNow );
		}
	}

// -----------------------------------------------------------------------------
// CLocMainContainer::HandleResourceChange
// -----------------------------------------------------------------------------
//
void CSimPsyUiContainer::HandleResourceChange(TInt aType)
    {
    CCoeControl::HandleResourceChange(aType);
    switch( aType )
    	{
    	case KEikDynamicLayoutVariantSwitch:
    		{
    		if( iAppUi )
	    		{
	    		SetRect(iAppUi->ClientRect());
	    		DrawDeferred();	
	    		}
			break;
    		}
    	default:
    		{
    		break;
    		}
    	}
    }
