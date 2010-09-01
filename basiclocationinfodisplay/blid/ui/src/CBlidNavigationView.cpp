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
* Description:  Provides blid compass view class methods.
*
*/


// SYSTEM INCLUDE FILES
#include <blid.rsg>
//Added for HTK toolkit support
#include <alf/alfenv.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfdisplay.h>
#include <alf/alftransformation.h>
#include <alf/alfroster.h>
//End of addition

#include <csxhelp/blid.hlp.hrh>
#include <hlplch.h>

// USER INCLUDE
#include "CBlidNavigationView.h"
#include "Blid.hrh"
#include "CBlidDocument.h"
#include "CBlidAppUi.h"
#include "CBlidEng.h"
#include "CHtkNavigationControl.h"
#include "CHtkTripMeterControl.h"
#include "MBlidSettings.h"
#include "CBlidLocSettingLauncher.h"
#include "bliduiconsts.h"
#include "BlidUID.h"


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CBlidNavigationView::NewLC
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBlidNavigationView* CBlidNavigationView::NewLC( MKeyProcessor& aKeyProcessor,
												 CAlfEnv& aEnv )
    {
    CBlidNavigationView* self = new (ELeave) CBlidNavigationView( aKeyProcessor,aEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
// ----------------------------------------------------------------------------
// CBlidNavigationView::CBlidNavigationView
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CBlidNavigationView::CBlidNavigationView( MKeyProcessor& aKeyProcessor,
										  CAlfEnv& aEnv ):
    CBlidBaseView( aKeyProcessor ),
    iEnv(aEnv)
    {
    }

// ----------------------------------------------------------------------------
// CBlidNavigationView::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CBlidNavigationView::ConstructL()
    {
    BaseConstructL( R_BLID_NAVIGATION_VIEW );

	// Create an instance of our list control.
	iNavigationCtrl = CHtkNavigationControl::NewL(iEnv,
												  this,
												  ClientRect(),
												  *iLocation,
												  *iRouter,
												  *iEngine,
												  *this );


	// Create a control group.
	iNavigationCtrlGroup = &( iEnv.NewControlGroupL( KPrimaryGroup ) );

	// pass ownership of the list control to the control group
	iNavigationCtrlGroup->AppendL(iNavigationCtrl);

    iCtrlDisplay = &( iEnv.NewDisplayL( ClientRect(), CAlfEnv::ENewDisplayAsCoeControl ) );
	// Use the Avkon skin background as the display background.
	iCtrlDisplay->SetClearBackgroundL(CAlfDisplay::EClearWithSkinBackground);

    }

// ----------------------------------------------------------------------------
// CBlidNavigationView::~CBlidNavigationView
// ----------------------------------------------------------------------------
//
CBlidNavigationView::~CBlidNavigationView()
    {
    delete iLauncher;
    }

// ----------------------------------------------------------------------------
// CBlidNavigationView::NotifyL
// ----------------------------------------------------------------------------
//
void CBlidNavigationView::NotifyL( const TBool aOnlineMode )
    {  
    iNavigationCtrl->UpdateL();
    CBlidBaseView::NotifyL( aOnlineMode );
    }

// ----------------------------------------------------------------------------
// CBlidNavigationView::NotifyError
// ----------------------------------------------------------------------------
//
void CBlidNavigationView::NotifyErrorL( TInt aErrorCode )
    {
    CBlidBaseView::NotifyErrorL( aErrorCode );    
    iNavigationCtrl->UpdateL();
    }

// ----------------------------------------------------------------------------
// CBlidNavigationView::ProcessKeyEventL
// ----------------------------------------------------------------------------
//
TBool CBlidNavigationView::ProcessKeyEventL( const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    return CBlidBaseView::ProcessKeyEventL( aKeyEvent,aType );
    }

// ----------------------------------------------------------------------------
// TUid CBlidNavigationView::Id
// ----------------------------------------------------------------------------
//
TUid CBlidNavigationView::Id() const
    {
    return TUid::Uid(EBlidNavigationView);
    }

// ----------------------------------------------------------------------------
// CBlidNavigationView::HandleCommandL
// ----------------------------------------------------------------------------
//
void CBlidNavigationView::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EBlidCmdWaypoint:
            {
            TInt index(0);

            if ( iRouter->Count() > 0 )
                {
                index = SelectWaypointL();
                if ( index != KErrCancel )
                    {
                    iRouter->SetWaypoint( index );
                    }
                }
            iNavigationCtrl->ClearNavigationInfoL();
			iNavigationCtrl->SetUpdateOptionL( ETrue );
			iLocation->SetNavigationStart( ETrue );
			iNavigationCtrl->SetFlagVisualL( );
			iNavigationCtrl->SetAccRacyVisualL();
			CHtkNavigationControl::iInArrivedZone = EFalse;
			if(iEngine->HasArrivedToDestination())
            	{
            	CHtkNavigationControl::iInArrivedZone = ETrue; 	
            	}
            iNavigationCtrl->UpdateL();
            break;
            }
        case EBlidCmdSavedWaypoints:
        	{
        	AppUi()->ActivateLocalViewL( TUid::Uid( EBlidSavedWayptsView ) );
        	break;
        	}
        case EBlidCmdStopNavigation:
            {
            iRouter->ClearSetPoints();
            iNavigationCtrl->ClearNavigationInfoL();
            iLocation->SetNavigationStart( EFalse );
            static_cast<CHtkTripMeterControl*>( &( iEnv.ControlGroup( KPrimaryTripGroup ).Control( 0 ) ) )->ChangeTripFocus( ETrue );
            iNavigationCtrl->UpdateL();            
            break;
            }
        case EBlidCmdLmk:
	        {	        
	        if(LaunchLandmarksDialogL())
	        	{
	        	iNavigationCtrl->ClearNavigationInfoL();
	        	iNavigationCtrl->SetFlagVisualL( );
	        	iNavigationCtrl->SetAccRacyVisualL();
	        	iNavigationCtrl->SetUpdateOptionL( ETrue );
	        	iLocation->SetNavigationStart( ETrue );
   	            static_cast<CHtkTripMeterControl*>( &( iEnv.ControlGroup( KPrimaryTripGroup ).Control( 0 ) ) )->ReChangeTripFocus();
   	            CHtkNavigationControl::iInArrivedZone = EFalse;
   	            if(iEngine->HasArrivedToDestination())
   	            	{
   	            	CHtkNavigationControl::iInArrivedZone = ETrue; 	
   	            	}
	        	iNavigationCtrl->UpdateL();	        		
	        	}	        
            break;
	        }
        case EBlidCmdManualWaypoint:
            {
            if(LaunchManualWaypointDialogL())
            	{
            	iNavigationCtrl->ClearNavigationInfoL();
            	iNavigationCtrl->SetFlagVisualL( );
            	iNavigationCtrl->SetAccRacyVisualL();
            	iNavigationCtrl->SetUpdateOptionL( ETrue );
            	iLocation->SetNavigationStart( ETrue );
            	static_cast<CHtkTripMeterControl*>( &( iEnv.ControlGroup( KPrimaryTripGroup ).Control( 0 ) ) )->ReChangeTripFocus();            	
            	CHtkNavigationControl::iInArrivedZone = EFalse;
            	if(iEngine->HasArrivedToDestination())
   	            	{
   	            	CHtkNavigationControl::iInArrivedZone = ETrue; 	
   	            	}
            	iNavigationCtrl->UpdateL();            	
            	}            
            break;
            }
        case EBlidCmdSaveLocation:
        	{
        	if ( !FeatureManager::FeatureSupported( KFeatureIdLandmarks ) )
        		{
        		if ( InsertWaypointL() )
	                {
	                static_cast<CBlidDocument*>(AppUi()->Document())->SaveL();
	                }
	            else
	            	{
	            	User::Leave( KErrGeneral );	
	            	}    
        		}
        	else
        		{
        		SaveCurrentPositionAsLandmarkL();
        		}
            break;		        		
        	}
        case EBlidCmdSatelliteStatus:
            {
            TBool lBacklightNormal = EFalse;
            iNavigationCtrl->StopPlayerL();
            MBlidSettings* setting = iEngine->SettingsModel();        
            if(setting->GetBacklightOption() == EBlidBackLightOnDuringNavigation)
                {
                setting->SetBacklightOption(EBlidBackLightNormal);
                iEngine->SetBackLightStateL(CBlidEng::EBlidBacklightNormal);
                lBacklightNormal = ETrue;    
                }
            (static_cast<CBlidAppUi*>(AppUi()))->HandleCommandL( aCommand );
            if(lBacklightNormal)
                {
                iEngine->SetBackLightStateL(CBlidEng::EBlidBacklightAlwaysOn); 
                setting->SetBacklightOption(EBlidBackLightOnDuringNavigation);               
                }
            break;
            }
        case EBlidCmdSettings:
            {
            iIsSettingsViewActive = ETrue;
            (static_cast<CBlidAppUi*>(AppUi()))->HandleCommandL( aCommand );
             break;
            }
       	case EBlidCmdShowDetails:
	       	{
	       	iNavigationCtrl->StopPlayerL();
	       	ShowCurrentPositionInfoL();
			break;	       		
	       	}
        case EBlidCmdLaunchPositionSettings:
        	{
        	iNavigationCtrl->StopPlayerL();
        	if(!iLauncher)
        		{
        		iLauncher = CBlidLocSettingsLauncher::NewL();        		
        		}
        	iLauncher->LaunchL();
        	break;
        	}
        case EBlidSoftKeyStop:
	        {
	        iNavigationCtrl->StopPlayerL();
			break;	        	
	        }
        case EBlidNavigationMskInfo:
	        {
	        iNavigationCtrl->DisplayInfoPopUpMsgL();
			break;	        	
	        }
        case EAknCmdHelp:
             {
             TCoeHelpContext context( TUid::Uid(KBLIDUID3), KHLP_BLID_NAVIGATION );
             CArrayFix<TCoeHelpContext>* navHelp = new CArrayFixFlat<TCoeHelpContext>(1);
             CleanupStack::PushL( navHelp );
             navHelp->AppendL(context);
             HlpLauncher::LaunchHelpApplicationL(iCoeEnv->WsSession(), navHelp );
             CleanupStack::Pop( navHelp );                        
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
// CBlidNavigationView::DoActivateL
// ----------------------------------------------------------------------------
//
void CBlidNavigationView::DoActivateL(
   const TVwsViewId& aPrevViewId,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    #ifdef _DEBUG
    RDebug::Print(_L("CBlidNavigationView::DoActivateL() called"));
    #endif
	
	iIsSettingsViewActive = EFalse;
	
    iLocation->SetObserver(*this, EBlidNavigationView); 
    iNavigationCtrl->MakeTitleL(R_BLID_TITLE_NAVIGATION);
    iNavigationCtrl->UpdateL();
    
    if( aPrevViewId.iViewUid.iUid != EBlidTripMeterView )
	    {
	    iNavigationCtrl->CreateNaviPaneL( EFalse );
	    iNavigationCtrl->ActivateTabL( *( iNavigationCtrl->GetNiviDecorator( ) ), EBlidNavigationView );	
	    
		// Hide the trip meter control
		iCtrlDisplay->Roster().Hide( iEnv.ControlGroup( KBlidMainSatelliteGroup ) );
		}
	
	// Hide the trip meter control
	iCtrlDisplay->Roster().Hide( iEnv.ControlGroup( KPrimaryTripGroup ) );
	
	// Show the control group on the display.
	iCtrlDisplay->Roster().ShowL( *iNavigationCtrlGroup );    

    MenuBar()->SetContextMenuTitleResourceId(R_BLID_VIEW_OK_MENUBAR);
    MBlidSettings* setting = iEngine->SettingsModel();        
    if(setting->GetBacklightOption() == EBlidBackLightOnDuringNavigation)
    	{
    	iEngine->SetBackLightStateL(CBlidEng::EBlidBacklightAlwaysOn);
    	}
   }

// ----------------------------------------------------------------------------
// CBlidNavigationView::DoDeactivate
// ----------------------------------------------------------------------------
//
void CBlidNavigationView::DoDeactivate()
    {
    TRAP_IGNORE(iEngine->SetBackLightStateL(CBlidEng::EBlidBacklightNormal));
    }

// ----------------------------------------------------------------------------
// CBlidNavigationView::SelectWaypointL
// ----------------------------------------------------------------------------
//
TInt CBlidNavigationView::SelectWaypointL()
    {
    TBool lBacklightNormal = EFalse;
    MBlidSettings* setting = iEngine->SettingsModel();
    iNavigationCtrl->StopPlayerL();        
    if(setting->GetBacklightOption() == EBlidBackLightOnDuringNavigation)
        {
        setting->SetBacklightOption(EBlidBackLightNormal);
        iEngine->SetBackLightStateL(CBlidEng::EBlidBacklightNormal);
        lBacklightNormal = ETrue;    
        }
    TInt index = CBlidBaseView::SelectWaypointL();
    if(lBacklightNormal)
        {
        iEngine->SetBackLightStateL(CBlidEng::EBlidBacklightAlwaysOn);                
        setting->SetBacklightOption(EBlidBackLightOnDuringNavigation);               
        }
    return index;        
    }

// ----------------------------------------------------------------------------
// CBlidNavigationView::LaunchLandmarksDialogL
// ----------------------------------------------------------------------------
//
TInt CBlidNavigationView::LaunchLandmarksDialogL()
    {
    TBool lBacklightNormal = EFalse;
    MBlidSettings* setting = iEngine->SettingsModel();
    iNavigationCtrl->StopPlayerL();        
    if(setting->GetBacklightOption() == EBlidBackLightOnDuringNavigation)
        {
        setting->SetBacklightOption(EBlidBackLightNormal);
        iEngine->SetBackLightStateL(CBlidEng::EBlidBacklightNormal);
        lBacklightNormal = ETrue;    
        }
    TInt lResult = CBlidBaseView::LaunchLandmarksDialogL();
    if(lBacklightNormal)
        {
        iEngine->SetBackLightStateL(CBlidEng::EBlidBacklightAlwaysOn);                
        setting->SetBacklightOption(EBlidBackLightOnDuringNavigation);               
        }
    return lResult;            
    }
    
// ----------------------------------------------------------------------------
// CBlidNavigationView::LaunchManualWaypointDialogL
// ----------------------------------------------------------------------------
//
TInt CBlidNavigationView::LaunchManualWaypointDialogL()
    {
    TBool lBacklightNormal = EFalse;
    MBlidSettings* setting = iEngine->SettingsModel();
    iNavigationCtrl->StopPlayerL();        
    if(setting->GetBacklightOption() == EBlidBackLightOnDuringNavigation)
        {
        setting->SetBacklightOption(EBlidBackLightNormal);
        iEngine->SetBackLightStateL(CBlidEng::EBlidBacklightNormal);
        lBacklightNormal = ETrue;    
        }
    TInt lResult = CBlidBaseView::LaunchManualWaypointDialogL();
    if(lBacklightNormal)
        {
        iEngine->SetBackLightStateL(CBlidEng::EBlidBacklightAlwaysOn);                
        setting->SetBacklightOption(EBlidBackLightOnDuringNavigation);               
        }
    return lResult;            
    } 
    
    
// ----------------------------------------------------------------------------
// CBlidNavigationView::InsertWaypointL
// ----------------------------------------------------------------------------
//
TBool CBlidNavigationView::InsertWaypointL()
    {
    TBool lBacklightNormal = EFalse;
    MBlidSettings* setting = iEngine->SettingsModel();
    iNavigationCtrl->StopPlayerL();   
    if(setting->GetBacklightOption() == EBlidBackLightOnDuringNavigation)
        {
        setting->SetBacklightOption(EBlidBackLightNormal);
        iEngine->SetBackLightStateL(CBlidEng::EBlidBacklightNormal);
        lBacklightNormal = ETrue;    
        }
    TBool lResult = CBlidBaseView::InsertWaypointL();
    if(lBacklightNormal)
        {
        iEngine->SetBackLightStateL(CBlidEng::EBlidBacklightAlwaysOn);                
        setting->SetBacklightOption(EBlidBackLightOnDuringNavigation);               
        }
    return lResult;            
    } 
  
  
// ----------------------------------------------------------------------------
// CBlidNavigationView::SaveCurrentPositionAsLandmarkL
// ----------------------------------------------------------------------------
//
void CBlidNavigationView::SaveCurrentPositionAsLandmarkL()
    {
    TBool lBacklightNormal = EFalse;
    MBlidSettings* setting = iEngine->SettingsModel();        
    iNavigationCtrl->StopPlayerL();
    if(setting->GetBacklightOption() == EBlidBackLightOnDuringNavigation)
        {
        setting->SetBacklightOption(EBlidBackLightNormal);
        iEngine->SetBackLightStateL(CBlidEng::EBlidBacklightNormal);
        lBacklightNormal = ETrue;    
        }
    CBlidBaseView::SaveCurrentPositionAsLandmarkL();
    if(lBacklightNormal)
        {
        iEngine->SetBackLightStateL(CBlidEng::EBlidBacklightAlwaysOn);                
        setting->SetBacklightOption(EBlidBackLightOnDuringNavigation);               
        }    
    } 

// ----------------------------------------------------------------------------
// CBlidNavigationView::GetControl
// ----------------------------------------------------------------------------
//
CHtkNavigationControl* CBlidNavigationView::GetControl()
    {
    return iNavigationCtrl;
    }

// ----------------------------------------------------------
// Handle Resource Change Events: Display orientation switch
// ----------------------------------------------------------
//
void CBlidNavigationView::HandleResourceChangeL( TInt aType )
    {
    if(aType == KEikDynamicLayoutVariantSwitch)
        {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
        
        // Notify skin of changed display size.
        iEnv.PrimaryDisplay().SetVisibleArea(rect);
        iNavigationCtrl->UpdateLayoutL();
        }
    }

#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------------------------
// CBlidNavigationView::HandleListBoxSelectionL
// ---------------------------------------------------------------------------
//
void CBlidNavigationView::HandleListBoxSelectionL()
    {
    //Do something if needed
    }
#endif //RD_SCALABLE_UI_V2
    
// End of File
