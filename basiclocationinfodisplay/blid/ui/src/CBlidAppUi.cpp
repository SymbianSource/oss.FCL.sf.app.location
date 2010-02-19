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
* Description:  Provides Blid UI class methods.
*
*/


// INCLUDE FILES
#include <AknsUtils.h>
#include <Blid.rsg>
#include <akncontext.h> 
#include <hlplch.h>
#include <StringLoader.h>
#include <blid.mbg>
#include <CSatelliteInfoUI.h>

#include "CBlidAppUi.h"
#include "Blid.hrh"
#include "CBlidDocument.h"
#include "CBlidMainView.h"
#include "CBlidNavigationView.h"
#include "CBlidTripMeterView.h"
#include "CBlidSettingsView.h"
#include "CBlidSavedWayptsView.h"
#include "CBlidSatelliteView.h"
#include "CBlidEng.h"
#include "Blidutils.h"
#include "CHtkNavigationControl.h"
#include "CHtkTripMeterControl.h"
#include "FileSystemInfo.h"
#include "BlidUID.h"
#include "CBlidSatelliteControl.h"

#ifdef _DEBUG
_LIT( KPanicMsg, "CBlidAppUi::TabChangedL");
	
enum TPanicCode
    {
    KBlidNullPointer = 1
    };
#endif
    
// ================= MEMBER FUNCTIONS =======================
//
// ----------------------------------------------------------
// CBlidAppUi::ConstructL
// Second phase construction
// ----------------------------------------------------------
//
void CBlidAppUi::ConstructL()

    {
    RDebug::Print(_L("CBlidAppUi::ConstructL() called"));
    
    TLocale locale;    
    locale.SetUnitsGeneral(EUnitsMetric);    
    locale.Set();
    

    BaseConstructL( EAknEnableSkin | EAknEnableMSK | EAknSingleClickCompatible);
    
    //Code added for HTK tool kit support    
    // Create the environment
    iAlfEnv = CAlfEnv::NewL();
    
    // assign private path to the texture manager
    CFileSystemInfo* fileSystemInfo = CFileSystemInfo::NewLC(TUid::Uid(KBLIDUID3));
    TFileName path = fileSystemInfo->PrivatePath();    
    CleanupStack::PopAndDestroy(fileSystemInfo);
    
    iAlfEnv->TextureManager().SetImagePathL( path );
    
    //End of addition.
    iContextPane = ETrue;
    iIsSatViewActive = EFalse;

    CBlidBaseView* navigationView = CBlidNavigationView::NewLC( *this, *iAlfEnv ); 
    AddViewL( navigationView );      // transfer ownership to CAknViewAppUi
    CleanupStack::Pop( navigationView );    // navigationView 

    CBlidBaseView* tripMeterView = CBlidTripMeterView::NewLC( *this, *iAlfEnv );
    AddViewL( tripMeterView );
    CleanupStack::Pop( tripMeterView ); // tripMeterView

    CBlidBaseView* settingsView = CBlidSettingsView::NewLC(*this);
    AddViewL( settingsView );
    CleanupStack::Pop( settingsView ); // settingsView

    CBlidBaseView* savedwayptsView = CBlidSavedWayptsView::NewLC(*this);
    AddViewL( savedwayptsView );
    CleanupStack::Pop( savedwayptsView ); // savedwayptsView

    CBlidMainView* mainview = CBlidMainView::NewLC( *this, *iAlfEnv );
    AddViewL( mainview );
    CleanupStack::Pop( mainview ); // mainview

    CBlidSatelliteView* mainSatview = CBlidSatelliteView::NewLC( *this, *iAlfEnv );
    AddViewL( mainSatview );
    CleanupStack::Pop( mainSatview ); // mainSatview

    SetDefaultViewL( *mainview );
    
	//start requesting
    CBlidEng* engine = BlidDocument()->Engine();

	if( engine->LocationModel() )
		{
	    HBufC* srvName = StringLoader::LoadLC( 
	                      R_BLID_SERVICE_RULE_NAME, iCoeEnv );

	    engine->LocationModel()->InitialLocationRequestL( *srvName );
	    CleanupStack::PopAndDestroy(srvName); //srvName
	    engine->LocationModel()->StartRequesting();
		}
    }

// ----------------------------------------------------
// CBlidAppUi::~CBlidAppUi
// Destructor
// Frees reserved resources
// ----------------------------------------------------
//
CBlidAppUi::~CBlidAppUi()
    {
    delete iAlfEnv;
    }

// ----------------------------------------------------
// CBlidAppUi::LocDocument
// Returns the Location document object.
// ----------------------------------------------------
//
CBlidDocument* CBlidAppUi::BlidDocument() const
    {
    // Explicit cast: the document must always be of type CBlidDocument
    return static_cast<CBlidDocument*>(Document());
    }

// ----------------------------------------------------
// CBlidAppUi::ShowDialog
// Displayes Overwrite waypoint note
// ----------------------------------------------------
//
void CBlidAppUi::ShowDialogL( const BlidNotes::TBlidNotes /*aNoteId*/ )
    {
    BlidNotes::OverWriteWaypointNoteL();
    }



// ----------------------------------------------------
// CBlidAppUi::SkinChange
// Handle skin change
// ----------------------------------------------------
//
void CBlidAppUi::SkinChange()
    {
    iContextPane = !iContextPane;
    }

// ----------------------------------------------------
// CBlidAppUi::HandleCommandL
// Handle options menu commands
// ----------------------------------------------------
//
void CBlidAppUi::HandleCommandL(TInt aCommand)
    {
    switch ( aCommand )
        {
        case EAknCmdHelp:
            {
            HlpLauncher::LaunchHelpApplicationL(iCoeEnv->WsSession(), 
            AppHelpContextL());            
            break;
            }
        // these all are same: save & exit
        case EAknSoftkeyBack:
        case EAknSoftkeyExit:
        case EEikCmdExit:
            {
			CBlidTripMeterView* view = static_cast<CBlidTripMeterView*>( View( TUid::Uid( EBlidTripMeterView ) ) );
			if( view )
				{
				view->GetControl()->SaveOdometerValue();					
				}
			//////////////////////////////////////
            RWsSession& ws = iCoeEnv->WsSession();
            TApaTaskList taskList(ws);
            TUid appUid = { KBLIDUID3 };
            TApaTask task = taskList.FindApp( appUid );
            if ( task.Exists() )
                {
                // stop any task runing for this application.
                task.EndTask();
                }
			////////////////////////////////////////                
            Exit();
            break;
            }
        case EBlidCmdSettings:
            {
            ActivateLocalViewL( TUid::Uid(EBlidSettingsView) );
            break;
            }
        case EBlidCmdSatelliteStatus:
            {            
            TRAPD( error, LaunchSatelliteInfoDlgL() );
            if ( error != KErrNone )
                {
                // need to be NULL, before leave
                iSatelliteInfo = NULL;
                User::Leave( error );
                }
            break;
            }
        default:
            break;
        }
    }

// ----------------------------------------------------
// CBlidAppUi::TabChangedL
// Called when Tab is changed, activates appropriate view
// ----------------------------------------------------
//
void CBlidAppUi::TabChangedL(TInt aIndex)
    {
    // Gets a subpane control by ID
    CAknNavigationControlContainer* naviCtrlContainer =
            static_cast<CAknNavigationControlContainer*>
            ( StatusPane()->ControlL( TUid::Uid( EEikStatusPaneUidNavi ) ) );
    __ASSERT_DEBUG( naviCtrlContainer, 
        User::Panic(KPanicMsg, KBlidNullPointer) );

    // Return topmost object from navigation pane's object stack.
    CAknNavigationDecorator* decorator = naviCtrlContainer->Top();
    __ASSERT_DEBUG( decorator && decorator->ControlType() == 
        CAknNavigationDecorator::ETabGroup, 
        User::Panic( KPanicMsg, KBlidNullPointer ) );

    CAknTabGroup* tabGroup = 
        static_cast< CAknTabGroup* > ( decorator->DecoratedControl() );
    __ASSERT_DEBUG( tabGroup && aIndex >= 0 && aIndex < tabGroup->TabCount(),
        User::Panic(KPanicMsg, KBlidNullPointer));
    ActivateLocalViewL(TUid::Uid(tabGroup->TabIdFromIndex(aIndex)));
    }

// ----------------------------------------------------
// CBlidAppUi::HandleForegroundEventL
// Called whenever app goes to background or comes to foreground
// ----------------------------------------------------
//
void CBlidAppUi::HandleForegroundEventL(TBool aForeground)
    {
    CAknAppUi::HandleForegroundEventL( aForeground );  
    iForeground = aForeground;

    CBlidEng* engine = BlidDocument()->Engine();
    if (aForeground)
        {
        engine->SetAppMode(CBlidEng::EAppForeground);
        if (engine->LocationModel())
            {
            if (engine->LocationModel()->GetRegisteredViewId()
                    == EBlidNavigationView)
                {
                CBlidNavigationView* view =
                        static_cast<CBlidNavigationView*> (View(TUid::Uid(
                                EBlidNavigationView)));
                view->GetControl()->UpdateL();
                }
            if (engine->LocationModel()->GetRegisteredViewId()
                    == EBlidTripMeterView)
                {
                CBlidTripMeterView* view =
                        static_cast<CBlidTripMeterView*> (View(TUid::Uid(
                                EBlidTripMeterView)));
                view->GetControl()->UpdateL();
                }
            if (engine->LocationModel()->GetRegisteredViewId()
                    == EBlidMainSatelliteView)
                {
                CBlidSatelliteView* view = static_cast<CBlidSatelliteView*> (View(
                        TUid::Uid(EBlidMainSatelliteView)));
                view->GetControl()->UpdateL();
                }
            if (engine->LocationModel()->GetRegisteredViewId()
                    == EBlidMainView)
                {
                CBlidMainView* view = static_cast<CBlidMainView*> (View(
                        TUid::Uid(EBlidMainView)));
                view->GetControl()->UpdateL();
                }        
            }
        }
    else
        {
        engine->SetAppMode(CBlidEng::EAppBackground);
        engine->SetBackLightStateL(CBlidEng::EBlidBacklightNormal);
        if (engine->LocationModel()->GetRegisteredViewId()
                == EBlidNavigationView)
            {
            CBlidNavigationView* view =
                    static_cast<CBlidNavigationView*> (View(TUid::Uid(
                            EBlidNavigationView)));
            view->GetControl()->StopPlayerL();
            }
        if (engine->LocationModel()->GetRegisteredViewId()
                == EBlidNavigationView)
            {
            CBlidNavigationView* view =
                    static_cast<CBlidNavigationView*> (View(TUid::Uid(
                            EBlidNavigationView)));
            view->GetControl()->StopPlayerL();
            }
        }

    if (iSatelliteInfo)
        {
        iSatelliteInfo->HandleForegroundEventL( aForeground );
        }
    
    }

// ----------------------------------------------------
// CBlidAppUi::ProcessKeyEventL
// Process key event
// ----------------------------------------------------
//
TBool CBlidAppUi::ProcessKeyEventL( const TKeyEvent& /*aKeyEvent*/,
    TEventCode /*aType*/ )
    {    
    return EKeyWasNotConsumed; 
    }

// ----------------------------------------------------
// CBlidAppUi::TabObserver
// Retruns CBlidAppUi pointer, which is Tab observer
// ----------------------------------------------------
//
MAknTabObserver* CBlidAppUi::TabObserver()
    {
    return this;
    }

// ----------------------------------------------------
// CBlidAppUi::LaunchSatelliteInfoDlgL
// Launches Satellite info dialog
// ----------------------------------------------------
//
void CBlidAppUi::LaunchSatelliteInfoDlgL()
    {
    HBufC* srvName = StringLoader::LoadLC( 
                              R_BLID_SERVICE_RULE_NAME, 
                              iCoeEnv );                
    iSatelliteInfo = CSatelliteInfoUI::NewL();
    iSatelliteInfo->ExecuteLD( *srvName );
    CleanupStack::PopAndDestroy(srvName); //srvName
    iSatelliteInfo = NULL;
    }

// ----------------------------------------------------
// CBlidAppUi::IsForeground
// Checks if App is in foreground
// ----------------------------------------------------
//
TBool CBlidAppUi::IsForeground()
    {
    return iForeground; 
    }

// ----------------------------------------------------
// CBlidAppUi::HandleResourceChangeL
// Handles a change to the resources
// ----------------------------------------------------
//
void CBlidAppUi::HandleResourceChangeL( TInt aType )    
	{
	CAknAppUi::HandleResourceChangeL( aType );
    CBlidEng* engine = BlidDocument()->Engine();

	if( engine->LocationModel() )
		{
		CBlidNavigationView* NavView = static_cast<CBlidNavigationView*>(View(TUid::Uid(EBlidNavigationView)));
    	NavView->HandleResourceChangeL( aType );

		CBlidTripMeterView* TripView = static_cast<CBlidTripMeterView*>(View(TUid::Uid(EBlidTripMeterView)));
		TripView->HandleResourceChangeL( aType );

		CBlidMainView* MainView = static_cast<CBlidMainView*>(View(TUid::Uid(EBlidMainView)));
	    MainView->HandleResourceChangeL( aType );

		CBlidSatelliteView* SatView = static_cast<CBlidSatelliteView*>(View(TUid::Uid(EBlidMainSatelliteView)));
		SatView->HandleResourceChangeL( aType );
		}
	}

// ----------------------------------------------------
// CBlidAppUi::SetSatelliteViewActive
// Set satellite view active or inactive
// ----------------------------------------------------
//
void CBlidAppUi::SetSatelliteViewActive( TBool aActive )    
	{
	iIsSatViewActive = aActive;
	}

// ----------------------------------------------------
// CBlidAppUi::IsSatelliteViewActive
// Check if Satellite view is active
// ----------------------------------------------------
//
TBool CBlidAppUi::IsSatelliteViewActive( )
	{
	return iIsSatViewActive;	
	}

// ----------------------------------------------------
// CBlidAppUi::ResetOdoMeterValueL
// Resets the odometer value
// ----------------------------------------------------
//
void CBlidAppUi::ResetOdoMeterValueL()
	{
	CBlidTripMeterView* view = static_cast<CBlidTripMeterView*>(View(TUid::Uid(EBlidTripMeterView)));
	if( view )
		{
		view->GetControl()->ResetOdometerDistanceL();			
		}
	}
// End of File  
