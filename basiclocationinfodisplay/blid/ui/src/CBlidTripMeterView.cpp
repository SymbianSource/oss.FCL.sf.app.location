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

// SYSTEM INCLUDES
#include <Blid.rsg>
//Added for HTK toolkit support
#include <alf/alfenv.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfdisplay.h>
#include <alf/alftransformation.h>
#include <alf/alfroster.h>
//End of addition

#include <csxhelp/blid.hlp.hrh>
#include <hlplch.h>

// USER INCLUDES
#include "cblidnotedialog.h"
#include "CBlidTripMeterView.h"
#include "Blid.hrh"
#include "CHtkTripMeterControl.h"
#include "MBlidLocation.h"
#include "MBlidRouter.h"
#include "MBlidSettings.h"
#include "CBlidEng.h"
#include "CBlidDocument.h"
#include "CBlidAppUi.h"
#include "bliduiconsts.h"
#include "CBlidLocSettingLauncher.h"
#include "BlidUID.h"

#include <locationvariationdomaincrkeys.h>
#include <centralrepository.h>


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CBlidTripMeterView::NewLC
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBlidTripMeterView* CBlidTripMeterView::NewLC( MKeyProcessor& aKeyProcessor,
											   CAlfEnv& aEnv )
    {
    CBlidTripMeterView* self = new (ELeave) CBlidTripMeterView(aKeyProcessor, aEnv );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }
// ----------------------------------------------------------------------------
// CBlidTripMeterView::CBlidTripMeterView
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CBlidTripMeterView::CBlidTripMeterView( MKeyProcessor& aKeyProcessor,
										CAlfEnv& aEnv ):
    CBlidBaseView( aKeyProcessor ),
    iEnv(aEnv)
    {
    }

// ----------------------------------------------------------------------------
// CBlidTripMeterView::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CBlidTripMeterView::ConstructL()
    {
    BaseConstructL( R_BLID_TRIPMETER_VIEW );
    
   	// Create an instance of our list control.
	iTripCtrl = CHtkTripMeterControl::NewL(iEnv,
										   *this,
										   this,
										   ClientRect(),
            							   iLocation,
            							   iRouter,
            							   *iEngine,
            							   *this);
            							   
	// Create a control group.
	iTripCtrlGroup = &( iEnv.NewControlGroupL( KPrimaryTripGroup ) );

	// pass ownership of the list control to the control group
	iTripCtrlGroup->AppendL(iTripCtrl);
	
    iCtrlDisplay = &( iEnv.NewDisplayL( ClientRect(), CAlfEnv::ENewDisplayAsCoeControl ) );
	// Use the Avkon skin background as the display background.
	iCtrlDisplay->SetClearBackgroundL(CAlfDisplay::EClearWithSkinBackground);

    iNoteDialog = CBlidNoteDialog::NewL();
    
    iLauncher = NULL;    
    }

// ----------------------------------------------------------------------------
// CBlidMainView::~CBlidMainView
// ----------------------------------------------------------------------------
//
CBlidTripMeterView::~CBlidTripMeterView()
    {
    delete iNoteDialog;
    delete iLauncher;
    }

// ---------------------------------------------------------
// CBlidTripMeterView::NotifyL
// ---------------------------------------------------------
//
void CBlidTripMeterView::NotifyL( const TBool aOnlineMode )
    {
    if(iTripCtrl->HasTripStarted())
        {
        if(aOnlineMode)
            {
            if(iTripCtrl->WasGPSDataLost())
                {
                if(static_cast<CBlidAppUi*>(AppUi())->IsForeground())
                    {
                    iTripCtrl->SetGPSDataAvailability(EFalse);                
                    iNoteDialog->ExecuteNoteDialog();                                    
                    }
				}
            }        
        }
    
    if( iTripCtrl )
        {
        iTripCtrl->UpdateL();
        }
    CBlidBaseView::NotifyL( aOnlineMode );     
    }

// ---------------------------------------------------------
// CBlidTripMeterView::NotifyErrorL
// ---------------------------------------------------------
//
void CBlidTripMeterView::NotifyErrorL( TInt aErrorCode )
    {
    CBlidBaseView::NotifyErrorL( aErrorCode );    
        
    if( iTripCtrl )
        {
        iTripCtrl->UpdateL();
        }
    }
    
// ----------------------------------------------------------------------------
// TUid CBlidTripMeterView::Id
// ----------------------------------------------------------------------------
//
TUid CBlidTripMeterView::Id() const
    {
    return TUid::Uid(EBlidTripMeterView);
    }

// ----------------------------------------------------------------------------
// CBlidTripMeterView::HandleCommandL
// ----------------------------------------------------------------------------
//
void CBlidTripMeterView::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EBlidCmdTripStart:
        case EBlidCmdTripStop:
        case EBlidCmdTripReset:
        case EBlidCmdTripClear:
        	{
         	iTripCtrl->ProcessCommandL(aCommand);
         	break;
        	}
        case EBlidCmdTripResume:
        	{        	
	        iTripCtrl->ProcessCommandL(aCommand);
         	break;
        	}
        case EBlidCmdSavedWaypoints:
        	{
        	AppUi()->ActivateLocalViewL( TUid::Uid( EBlidSavedWayptsView ) );
			break;  
  	      	}
  	    case EBlidCmdLmk:
	        {
	        LaunchLandmarksDialogL();
            break;
	        }
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
        		}
        	else
        		{
        		SaveCurrentPositionAsLandmarkL();
        		}
            break;		        		
        	}
       	case EBlidCmdShowDetails:
	       	{
	       	ShowCurrentPositionInfoL();
			break;	       		
	       	}
        case EBlidCmdLaunchPositionSettings:
        	{
        	if(!iLauncher)
        		{
        		iLauncher = CBlidLocSettingsLauncher::NewL();        		
        		}
        	iLauncher->LaunchL();
        	break;
        	}
        case EBlidNavigationMskInfo:
	        {
	        iTripCtrl->DisplayInfoPopUpMsgL();
			break;	        	
	        }
        case EBlidCmdSatelliteStatus:
            {
            TBool lBacklightNormal = EFalse;
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
       case EAknCmdHelp:
            {
            TCoeHelpContext context( TUid::Uid(KBLIDUID3), KHLP_BLID_TRIPMETER );
            CArrayFix<TCoeHelpContext>* tripHelp = new CArrayFixFlat<TCoeHelpContext>(1);
            CleanupStack::PushL( tripHelp );
            tripHelp->AppendL(context);
            HlpLauncher::LaunchHelpApplicationL(iCoeEnv->WsSession(), tripHelp );
            CleanupStack::Pop( tripHelp );                        
            break;
            }
        default:
            {
            static_cast<CBlidAppUi*>(AppUi())->HandleCommandL( aCommand );
            break;
            }
        } //switch
    }

// ----------------------------------------------------------------------------
// CBlidTripMeterView::DoActivateL
// ----------------------------------------------------------------------------
//
void CBlidTripMeterView::DoActivateL(
   const TVwsViewId& aPrevViewId,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    #ifdef _DEBUG
    RDebug::Print(_L("CBlidTripMeterView::DoActivateL() called"));
    #endif
    
	iIsSettingsViewActive = EFalse;
	    
    iLocation->SetObserver(*this, EBlidTripMeterView ); 
	
	iTripCtrl->MakeTitleL( R_BLID_TITLE_TRIPMETER );
    iTripCtrl->UpdateL();

    if( aPrevViewId.iViewUid.iUid != EBlidNavigationView )
	    {
	    iTripCtrl->CreateNaviPaneL( EFalse );
	    iTripCtrl->ActivateTabL( *( iTripCtrl->GetNiviDecorator( ) ), EBlidTripMeterView );	
	    }
	
	// Hide the navigation control group
	iCtrlDisplay->Roster().Hide( iEnv.ControlGroup( KPrimaryGroup ) );
	
	// Show the control group on the display.
	iCtrlDisplay->Roster().ShowL( *iTripCtrlGroup );    

    MenuBar()->SetContextMenuTitleResourceId(R_BLID_TRIP_METER_VIEW_OK_MENUBAR);
    MBlidSettings* setting = iEngine->SettingsModel();        
    if(setting->GetBacklightOption() == EBlidBackLightOnDuringNavigation)
    	{
    	iEngine->SetBackLightStateL(CBlidEng::EBlidBacklightAlwaysOn);
    	}
    		
   }

// ----------------------------------------------------------------------------
// CBlidTripMeterView::DoDeactivate
// ----------------------------------------------------------------------------
//
void CBlidTripMeterView::DoDeactivate()
    {
    TRAP_IGNORE(iEngine->SetBackLightStateL(CBlidEng::EBlidBacklightNormal));
    }

// ---------------------------------------------------------
// CBlidTripMeterView::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CBlidTripMeterView::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    switch( aResourceId )
        {
        case R_BLID_TRIP_METER_VIEW_OK_MENU:            
        case R_BLID_TRIPMETER_MENU:
            {
              if(iLocation->HasTripStarted())
            	{
            	aMenuPane->SetItemDimmed( EBlidCmdTripStart, ETrue );
            	aMenuPane->SetItemDimmed( EBlidCmdTripResume, ETrue );   
            	aMenuPane->SetItemDimmed( EBlidCmdTripStop, EFalse );
            	aMenuPane->SetItemDimmed( EBlidCmdTripReset, EFalse ); 
            	
            	if(aResourceId != R_BLID_TRIP_METER_VIEW_OK_MENU)
            	    {
                	aMenuPane->SetItemDimmed( EBlidCmdTripClear, ETrue ); 
            	    }            	            	
            	}else
              if(iLocation->HasTripNotStarted())  
                {
            	aMenuPane->SetItemDimmed( EBlidCmdTripStop, ETrue ); 
            	aMenuPane->SetItemDimmed( EBlidCmdTripResume, ETrue ); 
            	aMenuPane->SetItemDimmed( EBlidCmdTripStart, EFalse );
            	aMenuPane->SetItemDimmed( EBlidCmdTripReset, ETrue ); 
            	
            	if(aResourceId != R_BLID_TRIP_METER_VIEW_OK_MENU)
            	    {
                	aMenuPane->SetItemDimmed( EBlidCmdTripClear, ETrue ); 
            	    }            	            	
            	}else
               if(!iLocation->HasTripStarted())
            	{
            	if(iLocation->IsTripCleared())
            	    {
            	    aMenuPane->SetItemDimmed( EBlidCmdTripResume, ETrue );             	    
            	    aMenuPane->SetItemDimmed( EBlidCmdTripReset, ETrue );             	    
            	    }
            	else
            	    {
            	    aMenuPane->SetItemDimmed( EBlidCmdTripStart, ETrue );
            	    }
            	
            	if(iLocation->IsTripReset())
            	    {
            	    aMenuPane->SetItemDimmed( EBlidCmdTripReset, ETrue );
            	    }
            	
		        aMenuPane->SetItemDimmed( EBlidCmdTripStop, ETrue );		        
		        
            	if(aResourceId != R_BLID_TRIP_METER_VIEW_OK_MENU)
            	    {
            	    if(iLocation->IsTripCleared())
            	        {
            	        aMenuPane->SetItemDimmed( EBlidCmdTripClear, ETrue );            	        
            	        }
            	    }            	            	
            	}
            break;
            }
        case R_BLID_TRIP_VIEW_MENU:
            {
   			// Hide the satellite status menu option				
            if ( !(iLocation->IsGPSDeviceConnected() && 
                   iLocation->SatelliteCapability()) )
                {                
                aMenuPane->SetItemDimmed( EBlidCmdSatelliteStatus, ETrue );                
                }            
			// hide the save position.
            if(!iLocation->IsOnline())
                {
                aMenuPane->SetItemDimmed( EBlidCmdCurrentLocation, ETrue );
                }

			if ( !iRouter->Count() )
                {
                aMenuPane->SetItemDimmed( EBlidCmdSavedWaypoints, ETrue );
                }
                
			if ( FeatureManager::FeatureSupported( KFeatureIdLandmarks ) )
				{
				aMenuPane->SetItemDimmed( EBlidCmdSavedWaypoints, ETrue );
				}			
            break;
            }
        case R_BLID_MAIN_MENU:
            {
            CBlidBaseView::HandleHelpFeatureL( aMenuPane );
            break;
            }  
        case R_BLID_SAVE_CURRENT_LOCATION_MENU:
        	{
        		TInt settingsValue = 0;
	            CRepository* repository = CRepository::NewL( TUid::Uid( KCRUidLocLocalVariation) );
	            CleanupStack::PushL( repository );
	            User::LeaveIfError( repository->Get( KLocHideCoordinates,
	                                                      settingsValue ));  
	            CleanupStack::PopAndDestroy( repository );
	            
	            if ( ELocCoordinatesHidden == settingsValue )
	            {
	            	 aMenuPane->SetItemDimmed( EBlidCmdShowDetails, ETrue );
	            }
	            else
	            {
	             	 aMenuPane->SetItemDimmed( EBlidCmdShowDetails, EFalse );
	            }	
        	}
        default:
            {
            break;
            }
        }
    }

// ----------------------------------------------------------
// Handle Resource Change Events: Display orientation switch
// ----------------------------------------------------------
//
void CBlidTripMeterView::HandleResourceChangeL( TInt aType )
    {
    if(aType == KEikDynamicLayoutVariantSwitch)
        {
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, rect);
        
        // Notify skin of changed display size.
        iEnv.PrimaryDisplay().SetVisibleArea(rect);
        iTripCtrl->UpdateLayoutL();
        }
    }

// ----------------------------------------------------------------------------
// CBlidTripMeterView::GetControl
// ----------------------------------------------------------------------------
//
CHtkTripMeterControl* CBlidTripMeterView::GetControl()
    {
    return iTripCtrl;
    }


#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------------------------
// CBlidTripMeterView::HandleListBoxSelectionL
// ---------------------------------------------------------------------------
//
void CBlidTripMeterView::HandleListBoxSelectionL()
    {
    //Nothing to do here
    }
#endif // RD_SCALABLE_UI_V2
// End of File


