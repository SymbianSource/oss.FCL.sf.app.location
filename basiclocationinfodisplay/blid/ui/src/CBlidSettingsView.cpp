/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides Blid request view class methods.
*
*/


// INCLUDE FILES
#include <aknlists.h>
#include <aknViewAppUi.h>
#include <akntextsettingpage.h> 
#include <Blid.rsg>
#include <aknradiobuttonsettingpage.h>
#include <sysutil.h>
#include <driveinfo.h>
#include <CFLDFileListContainer.h>
#include <aknnotedialog.h>

#include "CBlidSettingsView.h"
#include "Blid.hrh"
#include "CBlidSettingsContainer.h"
#include "CBlidDocument.h" 
#include "CBlidEng.h" 
#include "MBlidSettings.h"
#include "bliduiconsts.h"
#include "Blidutils.h"
#include "CBlidAppUi.h"

// CONSTANTS
enum TPanicCode
    {
    KBlidNullPointer = 1
    };


// ================= MEMBER FUNCTIONS =======================
// ---------------------------------------------------------
// CBlidSettingsView::NewLC
// ---------------------------------------------------------
//
CBlidSettingsView* CBlidSettingsView::NewLC( MKeyProcessor& aKeyProcessor )
    {
    CBlidSettingsView* self = new (ELeave) CBlidSettingsView( aKeyProcessor );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// ---------------------------------------------------------
// CBlidSettingsView::CBlidSettingsView
// ---------------------------------------------------------
//
CBlidSettingsView::CBlidSettingsView( MKeyProcessor& aKeyProcessor ):
    CBlidBaseView( aKeyProcessor )
    {
    }

// ---------------------------------------------------------
// CBlidSettingsView::ConstructL
// Symbian two-phased constructor
// ---------------------------------------------------------
//
void CBlidSettingsView::ConstructL()
    {
    CBlidEng* eng = static_cast<CBlidDocument*>(AppUi()->Document())->Engine();
    BaseConstructL( R_BLID_SETTINGS_VIEW );
    iLocale = new(ELeave)TLocale();

    eng->CreateModelL( CBlidEng::EBlidSettings );
    iSettingsModel = eng->SettingsModel();
    __ASSERT_DEBUG(iSettingsModel,User::Panic(_L("CBlidMainView"),
                   KBlidNullPointer));
    iBlidBacklightNormal = iCoeEnv->AllocReadResourceL( 
		         R_BLID_BACKLIGHT_NORMAL);
    iBlidBacklightOn = iCoeEnv->AllocReadResourceL( 
		         R_BLID_BACKLIGHT_ON);		         
    }

// ---------------------------------------------------------
// CBlidSettingsView::~CBlidSettingsView
// ---------------------------------------------------------
//
CBlidSettingsView::~CBlidSettingsView()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;
    delete iLocale;
    delete iBlidBacklightNormal;
    delete iBlidBacklightOn;
    }

// ---------------------------------------------------------
// CBlidSettingsView::Id
// ---------------------------------------------------------
//
TUid CBlidSettingsView::Id() const
    {
    return TUid::Uid(EBlidSettingsView);
    }

// ---------------------------------------------------------
// CBlidSettingsView::HandleCommandL
// ---------------------------------------------------------
//
void CBlidSettingsView::HandleCommandL(TInt aCommand)
    {
    switch (aCommand)
        {
        case EBlidCmdChangeSetting:
        case EBlidCmdResetSetting:
            {
            // Get the index in location functionality settings view
            TInt indexLocationSetting=iContainer->ListBox()->CurrentItemIndex();
            switch( indexLocationSetting )
                {
		        case EBlidSettingsCalibration:
		            {
                    ChangeAltitudeCalibrationL();
                    break;
                    }
                case EBlidSettingsBacklight:
                    {
                    ChangeBacklightSettingsL();
                    break;
                    }
                case EBlidSettingsArrivalTone:
                    {
                    OpenArrivalToneDialogL();
                    break;
                    }
                case EBlidSettingsResetOdometer:
                    {
                    CheckResetOdometerOptionL();
                    break;
                    }                    
		        default:
		            {
               	    User::Leave( KErrBadHandle );
               	    break;
                    }
	            }
	            break;
	    	}
        case EBlidCmdMskChangeSetting:
        case EBlidCmdMskResetSetting:
            {
            // Get the index in location functionality settings view
            TInt indexLocationSetting=iContainer->ListBox()->CurrentItemIndex();
            switch( indexLocationSetting )
                {
		        case EBlidSettingsCalibration:
		            {
                    ChangeAltitudeCalibrationL();
                    break;
                    }                
                case EBlidSettingsBacklight:
                    {
                    if ( iSettingsModel->GetBacklightOption() == EBlidBackLightNormal )
                        {
                        ChangeBacklightSettingsValueL(EBlidBackLightOnDuringNavigation);
                        }
                    else
                        {
                        ChangeBacklightSettingsValueL(EBlidBackLightNormal);
                        } 
                    break;                        
                    }
                case EBlidSettingsArrivalTone:
                    {
                    OpenArrivalToneDialogL();
                    break;
                    }
                case EBlidSettingsResetOdometer:
                    {
                    CheckResetOdometerOptionL();
                    break;
                    }                    
                default:
                    {
               	    User::Leave( KErrBadHandle );
               	    break;                    
                    }                    
                }
            break;            
            }
        case EBlidSettingsArrivalTone:
            {
            OpenArrivalToneDialogL();
            break;
            }
        case EBlidSettingsResetOdometer:
            {
            CheckResetOdometerOptionL();
            break;
            }            
        case EBlidSettingsCalibration:
		    {
            ChangeAltitudeCalibrationL();
            break;
            }
        case EAknSoftkeyBack:
            {
            AppUi()->ActivateLocalViewL(TUid::Uid(iPrevViewId) );
            break;
            }
        default:
            {
            static_cast<CBlidAppUi*>(AppUi())->HandleCommandL( aCommand );
            break;
            }
        }// switch
    }

// ---------------------------------------------------------
// CBlidSettingsView::DynInitMenuPaneL
// ---------------------------------------------------------
//
void CBlidSettingsView::DynInitMenuPaneL( TInt aResourceId,
										  CEikMenuPane* aMenuPane )
    {
    switch( aResourceId )
        { 
        case R_BLID_MAIN_MENU:
            {
            CBlidBaseView::HandleHelpFeature( aMenuPane );
            break;
            }
        case R_BLID_SETTINGS_MENU:
	        {
            // Get the index in location functionality settings view
            TInt indexLocationSetting=
                iContainer->ListBox()->CurrentItemIndex();
            switch( indexLocationSetting )
                {
                case EBlidSettingsResetOdometer:
                    {
                    aMenuPane->SetItemDimmed( EBlidCmdChangeSetting, ETrue );
                	break;
                    }
                // Other values are not accepted.
                default:
                    {
                    aMenuPane->SetItemDimmed( EBlidCmdResetSetting, ETrue );
					break;
                    }
                }
			break;	        	
	        }
        default:
	        {
	        break;	        	
	        }
        }
    }

// ---------------------------------------------------------
// CBlidSettingsView::ProcessKeyEventL
// ---------------------------------------------------------
//
TBool CBlidSettingsView::ProcessKeyEventL( const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    if( (aType == EEventKey) )
        {
        if ( aKeyEvent.iCode == EKeyOK )
            {
            // Get the index in location functionality settings view
            TInt indexLocationSetting=
                iContainer->ListBox()->CurrentItemIndex();
            switch( indexLocationSetting )
                {
                case EBlidSettingsCalibration:
                    {
                    HandleCommandL( EBlidSettingsCalibration );
                    break;
                    }
                // Other values are not accepted.
                default:
                    {
                    break;
                    }
                }
            return ETrue;
            }
        }

    return CBlidBaseView::ProcessKeyEventL( aKeyEvent,aType );
    }

// ---------------------------------------------------------
// CBlidSettingsView::DoActivateL
// ---------------------------------------------------------
//
void CBlidSettingsView::DoActivateL(
   const TVwsViewId& aPrevViewId,TUid /*aCustomMessageId*/,
   const TDesC8& /*aCustomMessage*/)
    {
    iPrevViewId = aPrevViewId.iViewUid.iUid;
    if (!iContainer)
        {
        iContainer = CBlidSettingsContainer::NewL(*this, this, ClientRect(),
            iSettingsModel, *this);
        AppUi()->AddToStackL( *this, iContainer );
        SetMiddleSoftKeyLabelL(R_BLID_MSK_CHANGE,EBlidCmdMskChangeSetting); 

        #ifdef RD_SCALABLE_UI_V2
        iContainer->SetListBoxObserver(this);               
        #endif //RD_SCALABLE_UI_V2        
        }
   }

// ---------------------------------------------------------
// CBlidSettingsView::DoDeactivate
// ---------------------------------------------------------
//
void CBlidSettingsView::DoDeactivate()
    {
    if ( iContainer )
        {
        AppUi()->RemoveFromViewStack( *this, iContainer );
        }

    delete iContainer;
    iContainer = NULL;
    }

// ---------------------------------------------------------
// CBlidSettingsView::ChangeAltitudeCalibrationL
// ---------------------------------------------------------
//
void CBlidSettingsView::ChangeAltitudeCalibrationL()
    {     
    TInt AltCalibration( 0 );
    TInt error(0);
    TReal altitude( iSettingsModel->AltCalibration() );
    if ( iSettingsModel->UnitOfMeasurement() != EUnitsMetric )
        {
        altitude *= KMetreToFoot;        
        }

    error = BlidUtils::TRealToTInt( altitude, AltCalibration );
    User::LeaveIfError( error );

    /*CBlidSettingsPage* dlg = new ( ELeave )
            CBlidSettingsPage( *(AppUi()->EventMonitor()),
             R_BLID_ALTITUDE_CALIBRATION_SETTINGS_PAGE, AltCalibration );*/

    TInt resourceId;
    if(iSettingsModel->UnitOfMeasurement() == EUnitsMetric)
    	{
    	resourceId = R_BLID_ALTITUDE_CALIBRATION_METRIC_SETTINGS_PAGE;
    	}
    else
    	{
    	resourceId = R_BLID_ALTITUDE_CALIBRATION_IMPERIAL_SETTINGS_PAGE;
    	}
    
    CAknIntegerSettingPage* dlg = new (ELeave)
    						CAknIntegerSettingPage(resourceId,
    											   AltCalibration,
    											   0);
    
    if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
        {
        RFs fs;
	    User::LeaveIfError(fs.Connect()); 
	    CleanupClosePushL( fs ); 

	    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &fs, 0, DriveInfo::EDefaultPhoneMemory ) )
	        {
	        BlidNotes::OutOfMemoryNoteL();
			}
		else
			{
			TReal32 newCalibration = static_cast<TReal32>(AltCalibration);

	        if ( iSettingsModel->UnitOfMeasurement()  != EUnitsMetric )
	            {
	            newCalibration /= static_cast<TReal32>(KMetreToFoot);
	            }
	        iSettingsModel->SetAltCalibration(newCalibration);
	        iContainer->DrawNow();
			}
		CleanupStack::PopAndDestroy(); //fs
        }    
    }

// -----------------------------------------------------------------------------
// CBlidSettingsView::ChangeBacklightSettingsL
// Change the backlight option in between Normal/On During Navigation
// -----------------------------------------------------------------------------
//
void CBlidSettingsView::ChangeBacklightSettingsL()
    {            
     // number of options in query
     CDesCArrayFlat* items = 
            new( ELeave ) CDesCArrayFlat( 2 );
     CleanupStack::PushL( items );
     items->AppendL( *(iBlidBacklightNormal) );
     items->AppendL( *(iBlidBacklightOn ));
                       
     TInt indexForNewValue;
     if ( iSettingsModel->GetBacklightOption() == EBlidBackLightNormal )
        {
        indexForNewValue = 0;
        }
     else
        {
        indexForNewValue = 1;
        }
     CAknRadioButtonSettingPage* dlg = new ( ELeave )
            CAknRadioButtonSettingPage(  R_BLID_SETTINGS_BACKLIGHT_PAGE,
                                         indexForNewValue, 
                                         items );
     if ( dlg->ExecuteLD( CAknSettingPage::EUpdateWhenChanged ) )
     	{
     	RFs fs;
	    User::LeaveIfError(fs.Connect()); 
	    CleanupClosePushL( fs ); 

	    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &fs, 0, DriveInfo::EDefaultPhoneMemory ) )
	        {
	        BlidNotes::OutOfMemoryNoteL();
			}
		else
			{			
		    // Change value if a new value given in radio button page.
		    iSettingsModel->SetBacklightOption(indexForNewValue);
		    iContainer->DrawNow();
			}
		 CleanupStack::PopAndDestroy();// fs
        }    
    CleanupStack::PopAndDestroy(); // items
    }


// ---------------------------------------------------------------------------
// CBlidSettingsView::ChangeBacklightSettingsValue
// ---------------------------------------------------------------------------
//
void CBlidSettingsView::ChangeBacklightSettingsValueL(TInt aValue)
    {
 	RFs fs;
    User::LeaveIfError(fs.Connect()); 
    CleanupClosePushL( fs ); 

    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &fs, 0, DriveInfo::EDefaultPhoneMemory ) )
        {
        BlidNotes::OutOfMemoryNoteL();
		}
	else
		{
	    // Change value if a new value given in radio button page.
	    iSettingsModel->SetBacklightOption(aValue);
	    iContainer->DrawNow();
		}
	CleanupStack::PopAndDestroy();// fs        
    }
    
// ---------------------------------------------------------------------------
// CBlidSettingsView::OpenArrivalToneDialogL
// ---------------------------------------------------------------------------
//
void CBlidSettingsView::OpenArrivalToneDialogL( )
	{
	TFileName iToneFile;
    // Create instance of FileList using new FileList API
    CFLDFileListContainer* filelist = CFLDFileListContainer::NewLC();

    filelist->InsertEndNullItemL( _L( "Off" ) );
    // Do not show videos, use new FileList API
    filelist->AddExclusiveMediaTypeL( ECLFMediaTypeVideo );
    
    if( iSettingsModel->GetArrivedToneName().Length() > 0 )
	    {
		iToneFile.Copy( iSettingsModel->GetArrivedToneName() );
	    }

    TBool ok( filelist->LaunchL( iToneFile ) );
    
    if( ok )
        {
        HBufC8* lFileName = HBufC8::NewLC( iToneFile.Length() + 1 );
        lFileName->Des().Copy( iToneFile );
	    // Change value if a new value given in radio button page.
	    iSettingsModel->SetArrivedToneName( *lFileName );
	    iContainer->DrawNow();
	    CleanupStack::PopAndDestroy( lFileName );
        }

    CleanupStack::PopAndDestroy( filelist ); // filelist 		
	}

// ---------------------------------------------------------------------------
// CBlidSettingsView::CheckResetOdometerOptionL
// ---------------------------------------------------------------------------
//
void CBlidSettingsView::CheckResetOdometerOptionL( )
	{
    CAknNoteDialog* dlg;
    dlg = new (ELeave) CAknNoteDialog(CAknNoteDialog::EConfirmationTone,CAknNoteDialog::ENoTimeout);
    TBool result = dlg->ExecuteLD( R_SETTINGS_ODOMETER_RESET_CONFIRMATION_NOTE );
    if( result )
	    {
	    // Change value if a new value given in radio button page.
	    iSettingsModel->SetResetOdometerOption( result );
	    static_cast<CBlidAppUi*>(AppUi())->ResetOdoMeterValueL( );
	    iContainer->DrawNow();
	    }
	dlg = NULL;	    
	}
    
#ifdef RD_SCALABLE_UI_V2
// ---------------------------------------------------------------------------
// CBlidSettingsView::HandleListBoxSelectionL
// ---------------------------------------------------------------------------
//
void CBlidSettingsView::HandleListBoxSelectionL()
    {
    // Get the index in location functionality settings view
    TInt indexLocationSetting=iContainer->ListBox()->CurrentItemIndex();
    switch( indexLocationSetting )
        {
        case EBlidSettingsCalibration:
            {
            ChangeAltitudeCalibrationL();
            break;
            }
        case EBlidSettingsBacklight:
            {
            if ( iSettingsModel->GetBacklightOption() == EBlidBackLightNormal )
                {
                ChangeBacklightSettingsValueL(EBlidBackLightOnDuringNavigation);
                }
            else
                {
                ChangeBacklightSettingsValueL(EBlidBackLightNormal);
                }                
            break;
            }
        case EBlidSettingsArrivalTone:
            {
            OpenArrivalToneDialogL();
            break;
            }
        case EBlidSettingsResetOdometer:
            {
            CheckResetOdometerOptionL();
            break;
            }            
        default:
            {
            User::Leave( KErrBadHandle );
            break;
            }
        }
    }
    
#endif //RD_SCALABLE_UI_V2
    
// End of File
