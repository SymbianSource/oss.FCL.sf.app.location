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
* Description:  View class for Location Centre Application UI.
*
*/


// SYSTEM INCLUDES
#include <lcapp.rsg>
#include <hlplch.h>
#include <featmgr.h>
#include <eikmenup.h>
#include <eikbtgpc.h>

// USER INCLUDES
#include "lcview.h"
#include "lccontainer.h"
#include "lcappui.h"
#include "lclistboxmodel.h"
#include "lcapp.hrh"
  
// CONSTANT DEFINTIONS
const TUid	KLcViewId = TUid::Uid( 1 );

// ----------------- Member funtions for CLcView class -----------------------

// ---------------------------------------------------------------------------
// CLcView::CLcView
// ---------------------------------------------------------------------------
//  
CLcView::CLcView( MLcEventHandler&	aEventHandler,
				  CLcListBoxModel&	aListBoxModel  )
	:iEventHandler( aEventHandler ),
	iListBoxModel( &aListBoxModel )
    {
    }

// ---------------------------------------------------------------------------
// CLcView::~CLcView
// ---------------------------------------------------------------------------
//  
CLcView::~CLcView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;
    
    FeatureManager::UnInitializeLib();
    }

// ---------------------------------------------------------------------------
// CLcView* CLcView::NewL
// ---------------------------------------------------------------------------
//        
CLcView* CLcView::NewL( MLcEventHandler&	aEventHandler,
						CLcListBoxModel&	aListBoxModel )
    {   
    CLcView* self = CLcView::NewLC( aEventHandler, aListBoxModel );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CLcView* CLcView::NewLC
// ---------------------------------------------------------------------------
//   
CLcView* CLcView::NewLC( MLcEventHandler&	aEventHandler,
						 CLcListBoxModel&	aListBoxModel )
    {   
    CLcView* self = new ( ELeave ) CLcView( aEventHandler, aListBoxModel );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------------------------
// void CLcView::ConstructL
// ---------------------------------------------------------------------------
//  
void CLcView::ConstructL()
    {
    BaseConstructL( R_LC_VIEW );
    
    // Feature manager required for Help feature check
    FeatureManager::InitializeLibL();
            
    }

// ---------------------------------------------------------------------------
// void CLcView::ScreenSizeChanged
// ---------------------------------------------------------------------------
//  
void CLcView::ScreenSizeChanged()
	{
	// Request the Client container to change its rect to a new Rect
	if( iContainer )
        {
        iContainer->SetRect( ClientRect());     
        }	
	}

// ---------------------------------------------------------------------------
// void CLcView::UpdateL
// ---------------------------------------------------------------------------
//  
void CLcView::UpdateL( CLcListBoxModel&	aListBoxModel )
	{
    // Set the new List box model.
    iListBoxModel = &aListBoxModel;

    // Now update the contaier.
    if (iContainer)
        {
        iContainer->UpdateL(*iListBoxModel);
        }

    if (iListBoxModel->MdcaCount())
        {
        if (Cba())
            {
            Cba()->SetCommandSetL(R_LC_CBA);
            }
        }
    else
        {
        if (Cba())
            {
            Cba()->SetCommandSetL(R_LC_CBA_EMPTY);
            }
        }
    }

// ---------------------------------------------------------------------------
// void CLcView::UpdateIconsL
// ---------------------------------------------------------------------------
//  	
void CLcView::UpdateIconsL()
	{
	if ( iContainer )
		{
		iContainer->UpdateIconsL();
		}
	iContainer->DrawDeferred();
	}

// ---------------------------------------------------------------------------
// TInt CLcView::GetFocussedItem
// ---------------------------------------------------------------------------
//	
TInt CLcView::GetFocussedItem()
	{
	if ( iContainer )
		{
		return iContainer->GetFocussedItem();
		}
	else
		{
		return KErrNotFound;
		}
	}
			
// ---------------------------------------------------------------------------
// void CLcView::DoActivateL
// ---------------------------------------------------------------------------
//  	
void CLcView::DoActivateL( const TVwsViewId&  /* PrevViewId*/,
                                 TUid         /* aCustomMessageId*/,
                           const TDesC8&      /* aCustomMessage */)
    {
    // Destroy the existing container if it exists
    if( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer=NULL;
        }                       
    
    // Create new Container 
    iContainer = CLcContainer::NewL( ClientRect(), iEventHandler, *iListBoxModel );
    iContainer->SetMopParent( this );
    AppUi()->AddToViewStackL( *this, iContainer );
    
    if( iListBoxModel->MdcaCount())
    	{
    	Cba()->SetCommandSetL( R_LC_CBA );
    	}
	else
		{
    	Cba()->SetCommandSetL( R_LC_CBA_EMPTY );		
		}    	
    }
    
// ---------------------------------------------------------------------------
// void CLcView::DoDeactivate
// ---------------------------------------------------------------------------
//           
void CLcView::DoDeactivate()
    {
    // Destroy Container
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        delete iContainer;
        iContainer = NULL;
        }
    }
    
// ---------------------------------------------------------------------------
// TUid CLcView::Id
// ---------------------------------------------------------------------------
//   
TUid CLcView::Id() const
    {
    return KLcViewId;
    }

// ---------------------------------------------------------------------------
// void CLcView::HandleCommandL
// ---------------------------------------------------------------------------
//       
void CLcView::HandleCommandL( TInt aCommand )   
    {
    // This Class only handles the Help feature. All the other events are handled
    // by the App UI class.
    switch( aCommand )
    	{
    	case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL( iCoeEnv->WsSession(),
                                                 AppUi()->AppHelpContextL());
            break;  
            }
        default:
        	{
			AppUi()->HandleCommandL( aCommand );
			break;        	
        	}
    	}
    }

// ---------------------------------------------------------------------------
// void CLcView::DynInitMenuPaneL
// ---------------------------------------------------------------------------
//  
void CLcView::DynInitMenuPaneL( TInt          aResourceId, 
                                CEikMenuPane* aMenuPane )
    {
    if ( aResourceId == R_LC_MENU )
        {
        HandleHelpFeature( *aMenuPane );
        
	    if( iListBoxModel->MdcaCount())
	    	{
	    	aMenuPane->SetItemDimmed( ELcOpen, EFalse );					
			}
		else
	    	{
	    	aMenuPane->SetItemDimmed( ELcOpen, ETrue );					
			}		
        }
    }

// ---------------------------------------------------------------------------
// void CLcView::HandleHelpFeature
// ---------------------------------------------------------------------------
//      
void CLcView::HandleHelpFeature( CEikMenuPane& aMenuPane ) const
    {
    // The Help menu item must be displayed only if the Feature is supported.
    // If not, the Help item must be dimmed in the Options menu.
    if ( FeatureManager::FeatureSupported( KFeatureIdHelp ))
        {
        aMenuPane.SetItemDimmed( EAknCmdHelp, EFalse );
        }
    else
        {
        aMenuPane.SetItemDimmed( EAknCmdHelp, ETrue );
        }
    }
