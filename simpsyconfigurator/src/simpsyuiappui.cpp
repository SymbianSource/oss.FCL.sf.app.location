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


// SYSTEM INCLUDES
#include <eikenv.h>
#include <eikapp.h>
#include <aknenv.h>
#include <AknForm.h>
#include <AknWaitDialog.h>
#include <StringLoader.h> // StringLoader
#include <CAknFileSelectionDialog.h>
#include <centralrepository.h>
#include <lbspositioninfo.h>
#include <simpsyui.rsg>
#include <aknPopup.h>

// USER INCLUDES
#include "simpsyuiappui.h"
#include "simpsyui.hrh"
#include "simpsyuicontainer.h"
#include "simpsyuiao.h"
#include "simpsyfileselector.h"
#include "SimulationPSYInternalCRKeys.h"


// CONSTANT DEFINTIONS
_LIT(KSelectDialogTitle, "Select file");
	
// ---------------------------------------------------------
// CSimPsyUiAppUi::CSimPsyUiAppUi
// ---------------------------------------------------------
//
CSimPsyUiAppUi::CSimPsyUiAppUi()
	{
	}

// ---------------------------------------------------------
// CSimPsyUiAppUi::~CSimPsyUiAppUi
// ---------------------------------------------------------
//
CSimPsyUiAppUi::~CSimPsyUiAppUi()
	{				
    if (iAppContainer)
        {
        RemoveFromStack(iAppContainer);
        delete iAppContainer;
        }
    
    if ( iWaitDialog )
        {
        TRAPD( err, iWaitDialog->ProcessFinishedL() );
        if (err != KErrNone)
    		{
			delete iWaitDialog;
			iWaitDialog = NULL;
			}
	    }
	    	        
    delete iLocationRequestor;
    iLocationRequestor = NULL;        
	}

// ---------------------------------------------------------
// void CSimPsyUiAppUi::ConstructL
// ---------------------------------------------------------
//
void CSimPsyUiAppUi::ConstructL()
    {
    BaseConstructL( EAknEnableSkin | EAknEnableMSK );
    iAppContainer = new (ELeave) CSimPsyUiContainer ;
    iAppContainer->ConstructL( ClientRect() , this) ;
    AddToStackL( iAppContainer );
    
    iLocationRequestor = CSimPsyUiAO::NewL( *this );    
    }

// ---------------------------------------------------------
// void CSimPsyUiAppUi::DynInitMenuPaneL
// ---------------------------------------------------------
//	
void CSimPsyUiAppUi::DynInitMenuPaneL(
    TInt /*aResourceId*/,CEikMenuPane* /*aMenuPane*/)
    {
    }

// ---------------------------------------------------------
// TKeyResponse CSimPsyUiAppUi::HandleKeyEventL
// ---------------------------------------------------------
//
TKeyResponse CSimPsyUiAppUi::HandleKeyEventL(
    const TKeyEvent& /*aKeyEvent*/,TEventCode /*aType*/)
    {
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------
// void CSimPsyUiAppUi::HandleCommandL
// ---------------------------------------------------------
//
void CSimPsyUiAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {
        case EAknSoftkeyExit:
        case EAknSoftkeyBack:  
        case EEikCmdExit:
            {
            PrepareToExit();
            Exit();
            break;
            }
        case EAknSoftkeySelect:
        case ESimCmdConfigFile:
        	{                
			// Code for launching new SimPSY file selector			
    		CSimPsyFileSelector* fileSelector = new ( ELeave ) CSimPsyFileSelector();
			CleanupStack::PushL( fileSelector );
			    		
    		// Construct the popup with the list
    		CAknPopupList* popup = CAknPopupList::NewL( fileSelector, 
                                  						R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT,
                                  						AknPopupLayouts::EMenuGraphicWindow );
			CleanupStack::PushL( popup );                                  						                                  						  
    		fileSelector->ConstructL( popup );
    	    popup->SetTitleL( KSelectDialogTitle );
    	    CleanupStack::Pop( popup );
    	    
    		// Display the pop-up and let user selection.                             
    		TInt popupOk = popup->ExecuteLD();
            if ( popupOk )
        		{          
        		HBufC* fileName = NULL;
        		TRAPD( error, fileName = fileSelector->SimulationFileNameL().AllocL());
        		if ( !error )
        		    {
            		CleanupStack::PushL( fileName );
            		
                    // The full filename with path is stored in the cenrep file.
    				RDebug::Print(_L("SimPsyConfigurator: Setting Cenrep Key\n"));
    				CRepository* repository = CRepository::NewLC( KCRUidSimulationPSY );
    				repository->Set( KCRKeySimPSYSimulationFile, *fileName );
    				CleanupStack::PopAndDestroy( repository );
    				
    				// Update the SimPsyConfigurator listbox
    				iAppContainer->UpdateListBox();
    				    				        		
            		CleanupStack::PopAndDestroy( fileName );        		    
        		    }
        		}
			CleanupStack::PopAndDestroy( fileSelector );      			                
            break;       	
        	}
        case ESimCmdGetCurrentLocation:
        	{
			
	        // Obtain the Location information asynchrnously
	        TRAPD( err, iLocationRequestor->GetCurrentLocationL()); 	        
	        if( !err )
	            {
	            iWaitDialog = new(ELeave) CAknWaitDialog( NULL, ETrue );
	            iWaitDialog->SetCallback( this );
			    if ( !iWaitDialog->ExecuteLD( R_SIMPSYUI_REQUEST_LOCATION ) )
    				{
    				//Set iWaitDialog to NULL on pressing Cancel 
    				iWaitDialog = NULL;
    				iLocationRequestor->CancelLocationRequest();
    				} 
	            }	         
            break;
        	}
        default:
            break;      
        }
    }	

// ---------------------------------------------------------------------------
// void CSimPsyUiAppUi::NotifyLocationRequestCompletedL
// ---------------------------------------------------------------------------
//
void CSimPsyUiAppUi::NotifyLocationRequestCompletedL()
    {
    // Dismiss the wait note.	
    if ( iWaitDialog )
        {
        TRAPD( err, iWaitDialog->ProcessFinishedL() );
        if (err != KErrNone)
    		{
			delete iWaitDialog;
			iWaitDialog = NULL;
			}
	    }
    }

// ---------------------------------------------------------------------------
// void CSimPsyUiAppUi::DialogDismissedL
// ---------------------------------------------------------------------------
//
void CSimPsyUiAppUi::DialogDismissedL( TInt aButtonId )
    {
    if( EEikBidCancel != aButtonId )
        {
        // Display the current Location information to the user
        iLocationRequestor->DisplayCurrentLocationL();
        }
    }
    

