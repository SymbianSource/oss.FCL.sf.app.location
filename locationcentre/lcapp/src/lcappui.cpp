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
* Description:  Application class for Location Centre Application UI.
*
*/


// SYSTEM FILES
#include <avkon.hrh>
#include <lcservice.h>
#include <lclocationappinfo.h>
#include <lclocationappfilter.h>
#include <AknsConstants.h>
#include <aknnotewrappers.h>
#include <textresolver.h>

// USER INCLUDES
#include "lcappui.h"
#include "lcapp.hrh"
#include "lcview.h"
#include "lclistboxmodel.h"

// ----------------- Member funtions for CLcApp ------------------------------

// ---------------------------------------------------------------------------
// void CLcAppUi::ConstructL
// ---------------------------------------------------------------------------
//
void CLcAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin | EAknEnableMSK |EAknSingleClickCompatible );
    
    // Create the Service class
    iLcService = CLcService::NewL();    
    iLcService->SetObserverL( *this );
    
    // Handle condition where the function leaves with KErrNotFound
    // Create the Listbox Model
    
    CLcLocationAppInfoArray* appInfoArray = NULL;
    TRAPD( error, appInfoArray = iLcService->GetLocationApplicationsL());
    if ( KErrNotFound == error || KErrNone == error )
    	{
    	CleanupStack::PushL( appInfoArray );
    	// The ownership of the array is transferred.
    	iListBoxModel = CLcListBoxModel::NewL( appInfoArray );    	
    	CleanupStack::Pop( appInfoArray );
    	}
	else
		{
		// Not handling other error conditions. So better let others handle it.
		User::Leave( error );
		}
    
    // Create the View object and add it to the View stack
    CLcView* lcView = CLcView::NewL( *this, *iListBoxModel );
    CleanupStack::PushL( lcView );  	
    AddViewL( lcView );
    CleanupStack::Pop( lcView );
    iLcView = lcView;
    }

// ---------------------------------------------------------------------------
// CLcAppUi::~CLcAppUi
// ---------------------------------------------------------------------------
//
CLcAppUi::~CLcAppUi()
    {        
    // Delete the Service class    
    delete iLcService;
    
    // Delete the Model
    delete iListBoxModel;
    }

// ---------------------------------------------------------------------------
// TKeyResponse CLcAppUi::HandleKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CLcAppUi::HandleKeyEventL( const TKeyEvent& /*aKeyEvent*/,
                                              TEventCode /*aType*/ )
    {
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------------------------
// void CLcAppUi::HandleCommandL
// ---------------------------------------------------------------------------
//
void CLcAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyExit: 
        case EEikCmdExit:
            {
            Exit();
            break;
            }
        case ELcOpen:
            {
            HandleEventL( ELcOpen );
            break;
            }
       default: 
       	    { 	          	           	    
       	    break; 
       	    }
        }
    }
	
// ---------------------------------------------------------------------------
// void CLcAppUi::LcStatusChangedL
// ---------------------------------------------------------------------------
//    
void CLcAppUi::LcStatusChangedL( TInt aErrorCode )
    {
    // Update the listbox only if its a valid update.		
    if ( KErrNone == aErrorCode )
    	{
    	// There has been a change in Location Centre's registry. We have to
    	// update the list of Applications
    	
	    CLcLocationAppInfoArray* appInfoArray = NULL;
	    TRAPD( error, appInfoArray = iLcService->GetLocationApplicationsL());
	    if ( KErrNotFound == error || KErrNone == error )
	    	{
	    	// The ownership of the new array is transferred.
	    	iListBoxModel->UpdateModel( appInfoArray );
	    	iLcView->UpdateL( *iListBoxModel );   	
	    	}
		else
			{
			// Not handling other error conditions. So better let others handle it.
			User::Leave( error );
			}    	
    	}
    }

// ---------------------------------------------------------------------------
// void CLcAppUi::HandleEventL
// ---------------------------------------------------------------------------
//    
void CLcAppUi::HandleEventL( TInt	aEvent )
	{
	switch( aEvent )
		{
		case KEikDynamicLayoutVariantSwitch:
			{
			// Request the Client container to change its rect to a new Rect
			iLcView->ScreenSizeChanged();
			break;
			}
		case KAknsMessageSkinChange:
			{
			iLcView->UpdateIconsL();
			break;
			}
		case ELcOpen:
			{
			// Obtain the Identifier corresponding to the Selected Item
			TInt selectedItem = iLcView->GetFocussedItem();
			if ( selectedItem >= 0 && selectedItem < iListBoxModel->MdcaCount())
				{
				TRAP_IGNORE( iLcService->
						LaunchLocationApplicationL( iListBoxModel->AppInfoId( selectedItem )));
				}
			break;
			}
		default:
			break;
		}
	}
	
// End of File  
