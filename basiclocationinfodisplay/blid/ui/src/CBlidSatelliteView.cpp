/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides blid main satellite view class methods.
*
*/


// SYSTEM INCLUDE FILES
#include <aknViewAppUi.h>
#include <blid.rsg>
#include <alf/alfenv.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfdisplay.h>
#include <alf/alftransformation.h>
#include <alf/alfroster.h>
// USER INCLUDE FILES
#include "CBlidSatelliteView.h"
#include "Blid.hrh"
#include "CBlidSatelliteControl.h"
#include "MBlidLocation.h"
#include "CBlidAppUi.h"
#include "bliduiconsts.h"
#include "BlidNotes.h"
#include "Debug.h"

//CONSTANTS
enum TPanicCode
    {
    KBlidNullPointer = 1
    };

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CBlidSatelliteView::NewLC( MKeyProcessor& aKeyProcessor )
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBlidSatelliteView* CBlidSatelliteView::NewLC( MKeyProcessor& aKeyProcessor,
									 		   CAlfEnv& aEnv  )
    {
    CBlidSatelliteView* self = new (ELeave) CBlidSatelliteView( aKeyProcessor,aEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
// ----------------------------------------------------------------------------
// CBlidSatelliteView::CBlidSatelliteView
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CBlidSatelliteView::CBlidSatelliteView( MKeyProcessor& aKeyProcessor ,
							  CAlfEnv& aEnv ):
    CBlidBaseView( aKeyProcessor ),
    iEnv(aEnv)
    {
    }

// ----------------------------------------------------------------------------
// CBlidSatelliteView::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CBlidSatelliteView::ConstructL()
    {
    // Modify the Main view option key
    BaseConstructL( R_BLID_MAINSAT_VIEW );

	// Create an instance of our list control.
	iMainSatelliteCtrl = CBlidSatelliteControl::NewL(iEnv, ClientRect(), *this );


	// Create a control group.
	iMainSatelliteCtrlGroup = &( iEnv.NewControlGroupL( KBlidMainSatelliteGroup ) );

	// pass ownership of the list control to the control group
	iMainSatelliteCtrlGroup->AppendL( iMainSatelliteCtrl );
    }

// ----------------------------------------------------------------------------
// CBlidSatelliteView::~CBlidSatelliteView
// Destructor, frees allocated resources
// ----------------------------------------------------------------------------
//
CBlidSatelliteView::~CBlidSatelliteView()
    {
    }

// ----------------------------------------------------------------------------
// CBlidSatelliteView::ProcessKeyEventL
// Processes key event
// ----------------------------------------------------------------------------
//
TBool CBlidSatelliteView::ProcessKeyEventL( const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    return CBlidBaseView::ProcessKeyEventL( aKeyEvent,aType );
    }

// ----------------------------------------------------------------------------
// TUid CBlidSatelliteView::Id
// Returns the id of satellite view
// ----------------------------------------------------------------------------
//
TUid CBlidSatelliteView::Id() const
    {
    return TUid::Uid( EBlidMainSatelliteView );
    }

// ----------------------------------------------------------------------------
// CBlidSatelliteView::HandleCommandL
// Handles the option menu commands in satellite view
// ----------------------------------------------------------------------------
//
void CBlidSatelliteView::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EBlidSoftkeyInfo:
	        {
	        BlidNotes::PoorGPSDataNoteL();
	        break;	        	
	        }
        default:
            {
            (static_cast<CBlidAppUi*>(AppUi()))->HandleCommandL( aCommand );
            break;
            }
        }
    }

// ----------------------------------------------------------------------------
// CBlidSatelliteView::DoActivateL
// Called when Satellite view is activated
// ----------------------------------------------------------------------------
//
void CBlidSatelliteView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    #ifdef _DEBUG
    RDebug::Print(_L("CBlidSatelliteView::DoActivateL() called"));
    #endif
    CAlfDisplay& ctrlDisplay = iEnv.NewDisplayL( ClientRect(), CAlfEnv::ENewDisplayAsCoeControl );
	// Use the Avkon skin background as the display background.
	ctrlDisplay.SetClearBackgroundL(CAlfDisplay::EClearWithSkinBackground);

    iLocation->SetObserver( *this, EBlidMainSatelliteView ); 
    
    (static_cast<CBlidAppUi*>(AppUi()))->SetSatelliteViewActive( ETrue );
    
    iMainSatelliteCtrl->MakeTitleL( R_BLID_TITLE_MAIN );
	
	ctrlDisplay.Roster().Hide( iEnv.ControlGroup( KBlidMainGroup ) );
	
	// Show the control group on the display.
	ctrlDisplay.Roster().ShowL( *iMainSatelliteCtrlGroup );    
   }

// ----------------------------------------------------------------------------
// CBlidSatelliteView::DoDeactivate
// Called when satellite view is deactivated
// ----------------------------------------------------------------------------
//
void CBlidSatelliteView::DoDeactivate()
    {
	(static_cast<CBlidAppUi*>(AppUi()))->SetSatelliteViewActive( EFalse );
    }

// ----------------------------------------------------------------------------
// CBlidSatelliteView::NotifyL
// Called by location model to notify about changes in current location
// ----------------------------------------------------------------------------
//
void CBlidSatelliteView::NotifyL( const TBool aOnlineMode )
    {
    DEBUG1(" CBlidSatelliteView::NotifyL called ::%d", aOnlineMode );          
    if( iMainSatelliteCtrl &&  aOnlineMode )
        {
        iMainSatelliteCtrl->UpdateL();
        iMainSatelliteCtrl->SetOnlineMode( aOnlineMode );
        }
    CBlidBaseView::NotifyL( aOnlineMode );
    }

// ----------------------------------------------------------------------------
// CBlidSatelliteView::NotifyError
// Called by location model to notify about error in current location
// ----------------------------------------------------------------------------
//
void CBlidSatelliteView::NotifyErrorL( TInt aErrorCode )
    {
    CBlidBaseView::NotifyErrorL( aErrorCode );    
    DEBUG1(" CBlidSatelliteView::NotifyErrorL errorcode ::%d", aErrorCode );
    if( iMainSatelliteCtrl )
        {
        iMainSatelliteCtrl->UpdateL();
        iMainSatelliteCtrl->SetErrorCode( aErrorCode );
        }
    }

// ----------------------------------------------------------
// CBlidSatelliteView::HandleResourceChangeL
// Handle Resource Change Events: Display orientation switch
// ----------------------------------------------------------
//
void CBlidSatelliteView::HandleResourceChangeL( TInt aType )
    {
    if(aType == KEikDynamicLayoutVariantSwitch)
        {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
        
        iMainSatelliteCtrl->UpdateDisplayLayoutL();
        // Notify skin of changed display size.
        iEnv.PrimaryDisplay().SetVisibleArea(rect);
        }
    }

/**
 * 
 */
CBlidSatelliteControl* CBlidSatelliteView::GetControl()
    {
    return iMainSatelliteCtrl;
    }
        
#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------------------------
// CBlidNavigationView::HandleListBoxSelectionL
// Handles the listbox selection
// ---------------------------------------------------------------------------
//
void CBlidSatelliteView::HandleListBoxSelectionL()
    {
    //Do something if needed
    }
#endif //RD_SCALABLE_UI_V2    
// End of File


