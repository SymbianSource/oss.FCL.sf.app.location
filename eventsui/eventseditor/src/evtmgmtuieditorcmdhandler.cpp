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
* Description:   It is handler class to handle editor commands
*
*/

#include <evtmgmteditorui.rsg>
#include <AknQueryDialog.h> 
#include <StringLoader.h>
#include <lbsposition.h>
#include <aknlistquerydialog.h>
#include <StringLoader.h>
#include <AknIconArray.h>       // Icon Array
#include <evteditor.mbg>
#include <AknsUtils.h>
#include <AknsSkinInstance.h>
#include <data_caging_path_literals.hrh>
#include <gulicon.h>            // Gul Icon
#include <eikapp.h>
#include <aknlistquerydialog.h>
#include <eikserverapp.h>
#include <aknappui.h>
#include <eikenv.h>

#include "evteditor.h"
#include "evtevent.h"
#include "evttoneaction.h"
#include "evtaction.h"
#include "evtmgmteditorui.hrh"
#include "evtmgmtuieditorcmdhandler.h"
#include "evtmgmtuiplacecomponent.h"
#include "evtmgmtuidesccomponent.h"
#include "evtmgmtuiringtonecomponent.h"
#include "evtmgmtuimapnavigationadapter.h"
#include "evtdebug.h"
#include "evteditorconsts.h"
#include "evtmgmtuiwaitdlglauncher.h"
#include "evtmgmtuiengine.h"
#include "evtmgmtuilbtadapter.h"

// ---------------------------------------------------------------------------
// CEvtEditor ::NewL()
// @param[in] aExitCmdHandler - handler to handle all commands which 
//                              may result into closing editor
// @param[in/out] aEvent - event object which contains current event information.
//                         event details may be changed while executing commands
// @param[in] aEditorObserver - observer to handle editor specific operations 
//                              after command is executed   
// ---------------------------------------------------------------------------
//
CEvtMgmtUiEditorCmdHandler* CEvtMgmtUiEditorCmdHandler::NewL( 
                                CEvtEvent&                      aEvent, 
                                MEvtMgmtUiEditorObserver&       aEditorObserver,
                                CEvtMgmtUiEngine&               aEventEngine )
    {
    CEvtMgmtUiEditorCmdHandler * self = 
                    new ( ELeave ) CEvtMgmtUiEditorCmdHandler( aEvent, 
                                                               aEditorObserver,
                                                               aEventEngine );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEditorCmdHandler ::~CEvtMgmtUiEditorCmdHandler()
// ---------------------------------------------------------------------------
//
CEvtMgmtUiEditorCmdHandler::~CEvtMgmtUiEditorCmdHandler()
    {
	EVTUIDEBUG("+ CEvtMgmtUiEditorCmdHandler::~CEvtMgmtUiEditorCmdHandler()");
    // Unregister the command handler as an observer
    iEventEngine.RemoveObserver( this );
        
    //cancel any asychronous operation
    Cancel();
    
    // Reset the Array
    iEvtIdArray.Reset(); 
    iEvtIdArray.Close();
    
    //Delete iMapAdapter 
    if( iMapAdapter )
        {
        delete iMapAdapter;
        iMapAdapter = NULL;
        }
    
    //Delete iRingtoneComponent
    if( iRingtoneComponent )
        {
        delete iRingtoneComponent;
        iRingtoneComponent = NULL;
        }
    
    //Delete iDescComponent
    if( iDescComponent )
        {
        delete iDescComponent;
        iDescComponent = NULL;
        }
    
    //Delete iPlaceComponent
    if( iPlaceComponent )
        {
        delete iPlaceComponent;
        iPlaceComponent = NULL;
        }
    
    //Delete iClientLibrary
    if( iClientLibrary )
        {
        delete iClientLibrary;
        iClientLibrary = NULL;
        }
    
    //Delete wait launcher
    if( iWaitDialogLauncher )
        {
        delete iWaitDialogLauncher;
        iWaitDialogLauncher = NULL;
        }
    
	EVTUIDEBUG("- CEvtMgmtUiEditorCmdHandler::~CEvtMgmtUiEditorCmdHandler()");
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEditorCmdHandler ::ConstructL()
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiEditorCmdHandler::ConstructL()
    {   
	EVTUIDEBUG("+ CEvtMgmtUiEditorCmdHandler::ConstructL()");
    CActiveScheduler::Add( this );
    
    iEvtIdArray.Append(iEvent.EventId());
    
    // Set the Command Handler as the observer
    iEventEngine.SetObserver( this );
	EVTUIDEBUG("- CEvtMgmtUiEditorCmdHandler::ConstructL()");
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEditorCmdHandler ::CEvtMgmtUiEditorCmdHandler()
// ---------------------------------------------------------------------------
//
CEvtMgmtUiEditorCmdHandler::CEvtMgmtUiEditorCmdHandler( 
                        CEvtEvent&                      aEvent, 
                        MEvtMgmtUiEditorObserver&       aEditorObserver,
                        CEvtMgmtUiEngine&               aEventEngine )
    :CActive( EPriorityStandard ), 
     iEvent( aEvent ), 
     iEditorObserver( aEditorObserver ),
     iEventEngine( aEventEngine )
    {
    iWaitDialogLauncher = NULL;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEditorCmdHandler ::HandleEditorCmdL()
// It will handle all editor commands
// @param[in] aCommand - command id of the command to be handled.
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiEditorCmdHandler::HandleEditorCmdL( TInt     aCommand )
    {
    EVTUIDEBUG("+ CEvtMgmtUiEditorCmdHandler::HandleEditorCmdL()" );
    
    // return if already Active
    if ( IsActive() || (iWaitDialogLauncher && iWaitDialogLauncher->IsActive()) )
        {
        return;
        }
    
    //set state of the state-machine.
    iState = aCommand;
    switch( aCommand )
        {     
        case EEvtEditorCmdNavigateToPlace:  
        case EEvtEditorCmdShowOnMap:  
        case EEvtEditorCmdAddDescNew:
        case EEvtEditorCmdEditDesc:
        case EEvtEditorCmdAddDescFromExisting:  
        case EEvtEditorCmdAssignTone:  
        case EEvtEditorCmdActivate:
        case EEvtEditorCmdSaveDraft:
        case EEvtEditorCmdDelete:  
            {           
            // Schedule a request onto yourself for completion
            TRequestStatus* status = &iStatus;
            User::RequestComplete( status, KErrNone );
            SetActive();                                  
            break;
            }
        case EEvtMgmtUiEventUiPosSettings:
            {
            if( !iClientLibrary )
                iClientLibrary  = CLocSettingsUiClient::NewL();  
            iClientLibrary->LaunchPosSettingsAsEmbeddedAppL( iStatus );
            SetActive();
            break;
            }
        case EEvtEditorCmdSetPlace: // set place
            {
            SetActive();
            iStatus = KRequestPending;
            
            if(!iPlaceComponent)    
                iPlaceComponent = CEvtMgmtUiPlaceComponent::NewL( iEvent.Place(), iEvent.Location() );
            iPlaceComponent->DisplayPlacePopupDlg( iStatus );                
            break;
            }
        default:
            {
            EVTUIDEBUG("Error CEvtMgmtUiEditorCmdHandler::HandleEditorCmdL()" );
            }
        }
    EVTUIDEBUG("- CEvtMgmtUiEditorCmdHandler::HandleEditorCmdL()" );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEditorCmdHandler ::StopWaitDialogL()
// Stop the Wait Loop
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiEditorCmdHandler::StopWaitDialogL()
    {
    // Handle the notification completion event. Stop the Wait Dialog
    if( iWaitDialogLauncher )
        {
        iWaitDialogLauncher->StopWaitDialogL();  
        }
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEditorCmdHandler ::StartWaitDialogL()
// Launch the Wait Loop
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiEditorCmdHandler::StartWaitDialogL(TInt aResourceId)
    {
    // Create the wait loop if it doesn't exit
    if( !iWaitDialogLauncher )
        {
        // Create the Wait loop
        iWaitDialogLauncher = CEvtMgmtUiWaitDialogLauncher::NewL(); 
        iWaitDialogLauncher->SetObserver( this ); 
        
        // No Cancel operation
        iWaitDialogLauncher->AllowUserToCancel( ETrue );                                          
        }

    if( iWaitDialogLauncher )
        {
        // Set the CBA accordingly
        iWaitDialogLauncher->SetTextL( aResourceId );

        // Finally start the Wait Dialog in Synchronous format
        iWaitDialogLauncher->StartWaitDialogL();                                           
        } 
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEditorCmdHandler ::HandleDialogDismissed()
// Derived from CActive
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiEditorCmdHandler::HandleDialogDismissed( TInt aButtonId )
    {
    EVTUIDEBUG("+ CEvtMgmtUiEditorCmdHandler::HandleDialogDismissed()");
    if( EEikBidCancel == aButtonId )
        {
        EVTUIDEBUG("wait dialog is dismissed manually" );
        
        //Cancel Outstanding Request
        iEventEngine.CancelRequest();  
        }
    EVTUIDEBUG("- CEvtMgmtUiEditorCmdHandler::HandleDialogDismissed()");
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEditorCmdHandler ::RunError()
// Derived from CActive
// ---------------------------------------------------------------------------
//
TInt CEvtMgmtUiEditorCmdHandler::RunError( TInt aError )
    {
    EVTUIDEBUG("+ CEvtMgmtUiEditorCmdHandler::RunError()" );
    if( aError != KErrNone )
        {
        if( iWaitDialogLauncher )
            {
            TRAP_IGNORE(iWaitDialogLauncher->StopWaitDialogL());
            }
        EVTUIDEBUG1("Error = %d", aError );   
        }
    EVTUIDEBUG("- CEvtMgmtUiEditorCmdHandler::RunError()" );
    return aError;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEditorCmdHandler ::RunL()
// Derived from CActive
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiEditorCmdHandler::RunL()
    {    
    TInt error = iStatus.Int();    
    EVTUIDEBUG1("+ CEvtMgmtUiEditorCmdHandler::RunL() Error = %d", error );    
    //if no error, handle the command
    switch( iState )
        {
        case EEvtEditorCmdActivate:           
        case EEvtEditorCmdSaveDraft:
            {
            StartWaitDialogL( R_LOCEV_NOTE_SAVING_EVENT );
            
            if( iEvent.EventId() ) //if present, modify event
                {
                iEventEngine.ModifyEventL(iEvent, iEventAttributeMask );                
                }
            else 
                {
                iEventEngine.AddEventL( iEvent );
                }
            return;
            }
        case EEvtEditorCmdDelete:
            {
            StartWaitDialogL( R_LOCEV_NOTE_SINGLEEVT_DELETE );
            
            iEventEngine.DeleteEventsL( &iEvtIdArray );
            return;    
            }
        case EEvtEditorCmdSetPlace:
            {  
            if( error == KErrCancel ) 
                {
                iEditorObserver.NotifyEditorL( iState,KErrCancel );
                if( iPlaceComponent )
                   {
                   delete iPlaceComponent;
                   iPlaceComponent = NULL;
                   }
                return;
                }
            //handle 'set place' command 
            HandleSetPlaceCommandL();
            break;
            }
        case EEvtMgmtUiEventUiPosSettings:
            {
            if(iClientLibrary)
                {
                delete iClientLibrary;
                iClientLibrary = NULL;
                }
            break;
            }
        case EEvtEditorCmdNavigateToPlace:
            {
            HandleNavigateToPlaceCommandL();
            break;
            }
        case EEvtEditorCmdShowOnMap:  
            {
            HandleShowOnMapCommandL();
            break;
            }
        case EEvtEditorCmdAddDescNew:
        case EEvtEditorCmdEditDesc:
        case EEvtEditorCmdAddDescFromExisting:  
            {
            error = HandleDescCommandsL( iState );
            break;
            }
        case EEvtEditorCmdAssignTone:
            {
            error = HandleAssignToneCommandL();
            break;
            }
        default:
            {
            EVTUIDEBUG("Error in CEvtMgmtUiEditorCmdHandler::HandleEditorCmdL()" );
            }  
        }
    
    //Notify editor status of the operation in order to do any UI specific updates
    iEditorObserver.NotifyEditorL(iState,error);
    
    EVTUIDEBUG("- CEvtMgmtUiEditorCmdHandler::RunL()" );
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiEditorCmdHandler ::DoCancel()
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiEditorCmdHandler::DoCancel()
    {    
    switch( iState )
        {
        case EEvtEditorCmdActivate:           
        case EEvtEditorCmdSaveDraft:
        case EEvtEditorCmdDelete:
            {
            iEventEngine.CancelRequest();  
            return; 
            }
        case EEvtEditorCmdSetPlace:
            {  
            if( iPlaceComponent )
               {
               delete iPlaceComponent;
               iPlaceComponent = NULL;
               }
            break;
            }
        case EEvtMgmtUiEventUiPosSettings:
            {
            if( iClientLibrary )
               {
               iClientLibrary->ClosePosSettings();
               delete iClientLibrary;
               iClientLibrary = NULL;
               }
            break;
            }
        case EEvtEditorCmdNavigateToPlace:
        case EEvtEditorCmdShowOnMap:  
            {
            if( iMapAdapter )
               {
               delete iMapAdapter;
               iMapAdapter = NULL;
               }
            break;
            }
        case EEvtEditorCmdAddDescNew:
        case EEvtEditorCmdEditDesc:
        case EEvtEditorCmdAddDescFromExisting:  
            {
            if( iDescComponent )
               {
               delete iDescComponent;
               iDescComponent = NULL;
               }
            break;
            }
        case EEvtEditorCmdAssignTone:
            {
            if( iRingtoneComponent )
               {
               delete iRingtoneComponent;
               iRingtoneComponent = NULL;
               }
            break;
            }
        default:
            {
            break;
            }  
        }
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiEditorCmdHandler::DisplayConfirmationQueryDlg()
// It shows confirmation query dialog
// -----------------------------------------------------------------------------
//
TBool CEvtMgmtUiEditorCmdHandler::DisplayConfirmationQueryDlgL( TInt aResourceId )
    {
    EVTUIDEBUG("+ CEvtMgmtUiEditorCmdHandler::DisplayConfirmationQueryDlgL()" );
    HBufC* msg = StringLoader::LoadLC( aResourceId );
    TBool response= EFalse;
    CAknQueryDialog* confirmationQuery = CAknQueryDialog::NewL();     
    response = confirmationQuery->ExecuteLD( R_EVTUI_CONFIRMATION_QUERY, *msg );         
    CleanupStack::PopAndDestroy(msg);
    EVTUIDEBUG("- CEvtMgmtUiEditorCmdHandler::DisplayConfirmationQueryDlgL()" );
    return response;
    }

// -----------------------------------------------------------------------------
//  CEvtMgmtUiEditorCmdHandler::HandleSetPlaceCommandL
//  It will handle'set place' operation.
// -----------------------------------------------------------------------------
//
void CEvtMgmtUiEditorCmdHandler::HandleSetPlaceCommandL()
    {
    EVTUIDEBUG("= Details recieved from Set Place component" ); 
    EVTUIDEBUG1("TriggerId = %d", iEvent.EventId() );
	TPtrC placePtr = iPlaceComponent->Place();
    EVTUIDEBUG1("Place = %S", &placePtr );
    EVTUIDEBUG1("Lat = %f", iPlaceComponent->TriggerLocation().Latitude() );
    EVTUIDEBUG1("Long = %f", iPlaceComponent->TriggerLocation().Longitude() );
    EVTUIDEBUG("= Finish Details recieved from Set Place component" );
   
    //if place is already set
    if( iEvent.Place().Compare(KNullDesC ) != 0 )
        {
        //display confirmation message to replace old details
        if( DisplayConfirmationQueryDlgL( R_EVTUI_EDITOR_CONFIRM_PLACESAVE ) )
           { 
           //if yes, then replace
           iEvent.SetPlaceL( iPlaceComponent->Place() );  
           iEventAttributeMask  = iEventAttributeMask | CEvtMgmtUiEngine::EEvtEventAttributePlace;
           }       
        }
    else //if place is not already set, then set it
        {
        iEvent.SetPlaceL( iPlaceComponent->Place() );   
        iEventAttributeMask  = iEventAttributeMask | CEvtMgmtUiEngine::EEvtEventAttributePlace;
        }
    }

// -----------------------------------------------------------------------------
//  CEvtMgmtUiEditorCmdHandler::HandleDescCommandsL
//  It will handle add/edit/remove description operation.
//  @param[in] aCommand - Command id of the operation to be performed.
// -----------------------------------------------------------------------------
//
TInt CEvtMgmtUiEditorCmdHandler::HandleDescCommandsL( TInt  aCommand )
    {
	EVTUIDEBUG("+ CEvtMgmtUiEditorCmdHandler::HandleDescCommandsL()");
	TInt result = KErrNone;
    switch( aCommand )
        {
        case EEvtEditorCmdAddDescNew:
        case EEvtEditorCmdEditDesc:     
            {
            iDescComponent = CEvtMgmtUiDescComponent::NewL(iEvent.Description());
            result = iDescComponent->DisplayNotepadEditorL();
            if( iDescComponent )
                {
                if(result == KErrNone || result == KErrCancel )
                        iEvent.SetDescriptionL( iDescComponent->Desc() );
                delete iDescComponent;
                iDescComponent = NULL;
                }
            EVTUIDEBUG("= add/edit description operation is completed" );
            break;
            }
        case EEvtEditorCmdAddDescFromExisting:
            {
            iDescComponent = CEvtMgmtUiDescComponent::NewL(iEvent.Description());
            result = iDescComponent->DescFromExistingNotesL();
            if( iDescComponent )
                {
                if(result == KErrNone )
                        iEvent.SetDescriptionL( iDescComponent->Desc() );
                delete iDescComponent;
                iDescComponent = NULL;
                }
            EVTUIDEBUG("= add desc from existing operation is completed" );
            break;
            }
        default:
            break;
        }
	EVTUIDEBUG("+ CEvtMgmtUiEditorCmdHandler::HandleDescCommandsL()");
	return result;
    }

// -----------------------------------------------------------------------------
//  CEvtMgmtUiEditorCmdHandler::HandleAssignToneCommandL
//  It will handle 'assign tone' command.
//  @ret returns KErrNone if successfully handled else error
// -----------------------------------------------------------------------------
//
TInt CEvtMgmtUiEditorCmdHandler::HandleAssignToneCommandL()
    {
    EVTUIDEBUG("+ CEvtMgmtUiEditorCmdHandler::HandleAssignToneCommandL()" );
    TInt retval = KErrNone; 
    CEvtToneAction* tone = CEvtToneAction::NewLC();
    if( iEvent.HasAction())
       {              
       tone->InternalizeL( iEvent.Action() );
       iRingtoneComponent = CEvtMgmtUiRingtoneComponent::NewL( tone->FileName() );
       }
    else
        iRingtoneComponent = CEvtMgmtUiRingtoneComponent::NewL( KNullDesC() );
   
    if( iRingtoneComponent->DisplayAudioTonePopupDlgL() == KErrCancel )
       {
       if( iRingtoneComponent )
           {
           delete iRingtoneComponent;
           iRingtoneComponent = NULL;
           }
       CleanupStack::PopAndDestroy(); //tone
       return KErrCancel;
       }
    if( iRingtoneComponent)
        {
        if( iRingtoneComponent->Ringtone().Compare( KNullDesC ) == 0 )
            {
            retval = KErrNotFound;
            iEvent.SetActionL( NULL );
            }
        else
            {
            tone->SetFileNameL( iRingtoneComponent->Ringtone() );
      
            CEvtAction* action = CEvtAction::NewLC();
            tone->ExternalizeL( *action );
            iEvent.SetActionL( action ); //ownership is transferred.
       
            CleanupStack::Pop( action );        
            }
        delete iRingtoneComponent;
        iRingtoneComponent = NULL;
        }
    CleanupStack::PopAndDestroy( ); //tone  
    EVTUIDEBUG("- CEvtMgmtUiEditorCmdHandler::HandleAssignToneCommandL()" );
    return retval;
    }

// -----------------------------------------------------------------------------
//  CEvtMgmtUiEditorCmdHandler::HandleShowOnMapCommandL
//  It will handle 'show on map' command.
// -----------------------------------------------------------------------------
//
void CEvtMgmtUiEditorCmdHandler::HandleShowOnMapCommandL()
    {
    EVTUIDEBUG("+ CEvtMgmtUiEditorCmdHandler::HandleShowOnMapCommandL()" );
    iMapAdapter = CEvtMgmtUiMapNavigationAdapter::NewL();
    iMapAdapter->SetLocation(iEvent.Location());
    iMapAdapter->ShowOnMapL();
    if(iMapAdapter)
        {
        delete iMapAdapter;
        iMapAdapter = NULL;
        }
    EVTUIDEBUG("- CEvtMgmtUiEditorCmdHandler::HandleShowOnMapCommandL()" );
    }

// -----------------------------------------------------------------------------
//  CEvtMgmtUiEditorCmdHandler::HandleNavigateToPlaceCommandL
//  It will handle 'navigate to place' command.
// -----------------------------------------------------------------------------
//
void CEvtMgmtUiEditorCmdHandler::HandleNavigateToPlaceCommandL()
    {
    EVTUIDEBUG("+ CEvtMgmtUiEditorCmdHandler::HandleNavigateToPlaceCommandL()" );
    iMapAdapter = CEvtMgmtUiMapNavigationAdapter::NewL();
    iMapAdapter->SetLocation(iEvent.Location());
    iMapAdapter->NavigateToPlaceL();
    if(iMapAdapter)
        {
        delete iMapAdapter;
        iMapAdapter = NULL;
        }
    EVTUIDEBUG("- CEvtMgmtUiEditorCmdHandler::HandleNavigateToPlaceCommandL()" );
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiEditorCmdHandler::SetEventAttributeMask
// It will set attribute mask which will be used to update event.
// -----------------------------------------------------------------------------
void CEvtMgmtUiEditorCmdHandler::SetEventAttributeMask( TEvtEventAttributeMask aEventAttributeMask )
    {
    iEventAttributeMask = iEventAttributeMask | aEventAttributeMask;
    }

// -----------------------------------------------------------------------------
// void CEvtMgmtUiEditorCmdHandler::NotifyEventChangeL
// -----------------------------------------------------------------------------
//    
void CEvtMgmtUiEditorCmdHandler::NotifyEventChangeL( TInt aErrorCode, TInt /*aState*/ )
    {
    EVTUIDEBUG("+ CEvtMgmtUiEditorCmdHandler::NotifyEventChangeL()" );
        
    // Stop the Wait Dialog
    StopWaitDialogL();
    
    //Notify editor status of the operation in order to do any UI specific updates
    iEditorObserver.NotifyEditorL(iState,iStatus.Int());
    
    if( aErrorCode == KErrServerTerminated || 
        aErrorCode == KErrServerBusy ||
        aErrorCode == KErrNotReady )
        {
        EVTUIDEBUG("Exiting application as server is not ready." );
        // The User has exited the application using the Exit option from
        //.Options menu.  But, thats possible only if we exit the application
        // with EAknCmdExit.
        // A bit of convoluted logic but needed because we are using the
        // same components for the Settings UI Server as well as LC.
        
        CEikonEnv* env = CEikonEnv::Static();
       	CEikAppServer* server = env->AppServer();
		if ( server )
			{
			server->NotifyServerExit( EAknCmdExit );
			}
	    CAknAppUi* appUi = static_cast< CAknAppUi* >( env->AppUi());
	    appUi->RunAppShutter();  
        return;
        }
    EVTUIDEBUG("- CEvtMgmtUiEditorCmdHandler::NotifyEventChangeL()" );
    }

// -----------------------------------------------------------------------------
// void CEvtMgmtUiEditorCmdHandler::NotifyEventChangeL
// -----------------------------------------------------------------------------
//
void CEvtMgmtUiEditorCmdHandler::NotifyModelChangeL()
    {
    // Do Nothing here
    }

//end of file
