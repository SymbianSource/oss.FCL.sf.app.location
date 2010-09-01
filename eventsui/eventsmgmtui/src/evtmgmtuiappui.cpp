/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Application class for Events Management UI.
*
*/


// SYSTEM FILES
#include <avkon.hrh>
#include <AknsConstants.h>
#include <aknnotewrappers.h>
#include <textresolver.h>
#include <evtmgmtui.rsg>
#include <eikspane.h> 		// Status pane
#include <akntitle.h>   	// CAknTitlePane
#include <StringLoader.h>
#include <eikserverapp.h>
#include <aknmessagequerydialog.h>
#include <aknnotewrappers.h> 
#include <evtmgmtui.rsg>

// USER INCLUDES
#include "evtmgmtuiappui.h"
#include "evtmgmtui.hrh"
#include "evtmgmtuistartupview.h"
#include "evtmgmtuilegalnote.h"
#include "evtdebug.h"

// ----------------- Member funtions for CEvtMgmtUiAppUi ------------------------------

// ---------------------------------------------------------------------------
// void CEvtMgmtUiAppUi::ConstructL
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiAppUi::ConstructL()
    {
    EVTUIDEBUG( "=================================== EventsUi is launched ===========================" );
    
    
    BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible );
    MakeTitleL( R_EVTUI_TITLE );
    
    // Create the View object and add it to the View stack
    CEvtMgmtUiStartupView* startupview = CEvtMgmtUiStartupView::NewL();
    CleanupStack::PushL( startupview );  	
    AddViewL( startupview );
    CleanupStack::Pop( startupview );
    
    iLegalNote = CEvtMgmtUiLegalNote::NewL(*this);
    iLegalNote->DisplayNoteL();
	
    EVTUIDEBUG( "CEvtMgmtUiAppUi::ConstructL() Ends" );
      
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiAppUi::~CEvtMgmtUiAppUi
// ---------------------------------------------------------------------------
//
CEvtMgmtUiAppUi::~CEvtMgmtUiAppUi()
    {
    delete iLegalNote;
    EVTUIDEBUG( "=================================== EventsUi is terminated ===========================" );
    }

// ---------------------------------------------------------------------------
// TKeyResponse CEvtMgmtUiAppUi::HandleKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CEvtMgmtUiAppUi::HandleKeyEventL( const TKeyEvent& /*aKeyEvent*/,
                                                     TEventCode /*aType*/ )
    {
    return EKeyWasNotConsumed;
    }

// ---------------------------------------------------------------------------
// void CEvtMgmtUiAppUi::HandleCommandL
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiAppUi::HandleCommandL( TInt aCommand )
    {
    switch ( aCommand )
        {        
        case EAknSoftkeyExit:
        case EAknSoftkeyBack:
        case EEikCmdExit:
        	{
        	Exit();
            break;
        	}
        case EAknCmdExit:
            {
			// The User has exited the application using the Exit option from
            //.Options menu.  But, thats possible only if we exit the application
            // with EAknCmdExit.
            // A bit of convoluted logic but needed because we are using the
            // same components for the Settings UI Server as well as LC.

           	CEikAppServer* server = iEikonEnv->AppServer();
			if ( server )
				{
				server->NotifyServerExit(EAknCmdExit);
				}            
            Exit();            
            break;
            }
       default: 
       	    { 	          	           	    
       	    break; 
       	    }
        }
    }

// ---------------------------------------------------------------------------
// void CEvtMgmtUiAppUi::MakeTitleL
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiAppUi::MakeTitleL( TInt aResourceText )
	{
	// Obtain the title from the Status Pane
	CAknTitlePane* title = static_cast<CAknTitlePane*>( StatusPane()->
        ControlL( TUid::Uid( EEikStatusPaneUidTitle )));
       
    // Set the Title's buffer    
    HBufC* buf = StringLoader::LoadL( aResourceText );
    title->SetText( buf ); // Takes ownership of buf
	}

// ---------------------------------------------------------------------------
// CEikStatusPane* CEvtMgmtUiAppUi::StatusPane
// ---------------------------------------------------------------------------
//
CEikStatusPane* CEvtMgmtUiAppUi::StatusPane()
	{
	return iEikonEnv->AppUiFactory()->StatusPane();	
	}
	
// End of File  
