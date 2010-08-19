/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides blid main view class methods.
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
#include "CBlidMainView.h"
#include "Blid.hrh"
#include "CBlidMainControl.h"
#include "MBlidLocation.h"
#include "CBlidAppUi.h"
#include "bliduiconsts.h"
#include "Debug.h"

//CONSTANTS
enum TPanicCode
    {
    KBlidNullPointer = 1
    };

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CBlidMainView::NewLC( MKeyProcessor& aKeyProcessor )
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBlidMainView* CBlidMainView::NewLC( MKeyProcessor& aKeyProcessor,
									 CAlfEnv& aEnv  )
    {
    CBlidMainView* self = new (ELeave) CBlidMainView( aKeyProcessor,aEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
// ----------------------------------------------------------------------------
// CBlidMainView::CBlidMainView
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CBlidMainView::CBlidMainView( MKeyProcessor& aKeyProcessor ,
							  CAlfEnv& aEnv ):
    CBlidBaseView( aKeyProcessor ),
    iEnv(aEnv)
    {
    }

// ----------------------------------------------------------------------------
// CBlidMainView::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CBlidMainView::ConstructL()
    {
    // Modify the Main view option key
    BaseConstructL( R_BLID_MAIN_VIEW );
    
	// Create an instance of our list control.
	iMainCtrl = CBlidMainControl::NewL(iEnv, ClientRect(), *this );
	iMainCtrl->CreateNaviPaneL();

	// Create a control group.
	iMainCtrlGroup = &( iEnv.NewControlGroupL( KBlidMainGroup ) );

	// pass ownership of the list control to the control group
	iMainCtrlGroup->AppendL(iMainCtrl);

    iCtrlDisplay = &( iEnv.NewDisplayL( ClientRect(), CAlfEnv::ENewDisplayAsCoeControl ) );
	// Use the Avkon skin background as the display background.
	iCtrlDisplay->SetClearBackgroundL(CAlfDisplay::EClearWithSkinBackground);
    }

// ----------------------------------------------------------------------------
// CBlidMainView::~CBlidMainView
// ----------------------------------------------------------------------------
//
CBlidMainView::~CBlidMainView()
    {
    }

// ----------------------------------------------------------------------------
// CBlidMainView::ProcessKeyEventL
// ----------------------------------------------------------------------------
//
TBool CBlidMainView::ProcessKeyEventL( const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    return CBlidBaseView::ProcessKeyEventL( aKeyEvent,aType );
    }

// ----------------------------------------------------------------------------
// TUid CBlidMainView::Id
// ----------------------------------------------------------------------------
//
TUid CBlidMainView::Id() const
    {
    return TUid::Uid(EBlidMainView);
    }

// ----------------------------------------------------------------------------
// CBlidMainView::HandleCommandL
// ----------------------------------------------------------------------------
//
void CBlidMainView::HandleCommandL(TInt aCommand)
    {
    (static_cast<CBlidAppUi*>(AppUi()))->HandleCommandL( aCommand );
    }

// ----------------------------------------------------------------------------
// CBlidMainView::DoActivateL
// ----------------------------------------------------------------------------
//
void CBlidMainView::DoActivateL(
   const TVwsViewId& /*aPrevViewId*/,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    #ifdef _DEBUG
    RDebug::Print(_L("CBlidMainView::DoActivateL() called"));
    #endif

    iLocation->SetObserver(*this, EBlidMainView); 
    iMainCtrl->MakeTitleL( R_BLID_TITLE_MAIN );
	
	// Show the control group on the display.
	iCtrlDisplay->Roster().ShowL( *iMainCtrlGroup );    
   }

// ----------------------------------------------------------------------------
// CBlidMainView::DoDeactivate
// ----------------------------------------------------------------------------
//
void CBlidMainView::DoDeactivate()
    {
	// Nothing to do.
    }

// ----------------------------------------------------------------------------
// CBlidMainView::NotifyL
// ----------------------------------------------------------------------------
//
void CBlidMainView::NotifyL( const TBool aOnlineMode )
    {  
    if( iMainCtrl &&  aOnlineMode )
        {
        iMainCtrl->SetErrorCode( KErrNone );
        iMainCtrl->SetOnlineMode( aOnlineMode );
        iMainCtrl->UpdateL();
        }
	DEBUG1(" CBlidMainView::NotifyL called ::%d", aOnlineMode );        
    CBlidBaseView::NotifyL( aOnlineMode );
    }

// ----------------------------------------------------------------------------
// CBlidMainView::NotifyError
// ----------------------------------------------------------------------------
//
void CBlidMainView::NotifyErrorL( TInt aErrorCode )
    {
    CBlidBaseView::NotifyErrorL( aErrorCode );    
    DEBUG1(" CBlidMainView::NotifyErrorL errorcode ::%d", aErrorCode );
    if( iMainCtrl )
        {
        iMainCtrl->SetErrorCode( aErrorCode );
        iMainCtrl->UpdateL();
        }
    }

// ----------------------------------------------------------
// Handle Resource Change Events: Display orientation switch
// ----------------------------------------------------------
//
void CBlidMainView::HandleResourceChangeL( TInt aType )
    {
    if(aType == KEikDynamicLayoutVariantSwitch)
        {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
        
        // Notify skin of changed display size.
        iEnv.PrimaryDisplay().SetVisibleArea(rect);
        iMainCtrl->UpdateDisplayLayoutL();
        }
    }

CBlidMainControl* CBlidMainView::GetControl()
    {
    return iMainCtrl;
    }
    
#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------------------------
// CBlidNavigationView::HandleListBoxSelectionL
// ---------------------------------------------------------------------------
//
void CBlidMainView::HandleListBoxSelectionL()
    {
    //Do something if needed
    }
#endif //RD_SCALABLE_UI_V2    
// End of File


