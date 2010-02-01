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
* Description:  View class for Events Management UI.
*
*/


#ifndef C_EVTMGMTUISTARTUPVIEWAPPVIEW_H
#define C_EVTMGMTUISTARTUPVIEWAPPVIEW_H

//  SYSTEM INCLUDES
#include <aknview.h>
#include <aknViewAppUi.h>
#include <AknWaitNoteWrapper.h> // MAknBackgroundProcess

#include "evtmgmtuiengine.h"
#include "evtmgmtuicmdhdlr.h"


// FORWARD DECLARATIONS
class CEvtMgmtUiStartupContainer;
class CEvtMgmtUiLocSettingsLauncher;
class CEvtMgmtUiModel;
/**
 * Events Management UI Start-up view class.
 *
 * @lib evtmgmtui.exe
 * @since S60 v5.0
 */
class CEvtMgmtUiStartupView : public CAknView,
                              public MEvtMgmtUiEngineObserver,
                              public MEvtMgmtUiCmdHdlr,
                              public MAknBackgroundProcess
    {
    
public:
	/**
	 * Static Two phase constructor
	 *
	 * @return CEvtMgmtUiStartupView*, Pointer to the newly created object.
	 */                                              
    static CEvtMgmtUiStartupView* NewL();
    
    /**
     * C++ Destructor.
     */
    virtual ~CEvtMgmtUiStartupView();
    
private://new functions
    /**
     * It will update view
     */ 
    void UpdateViewL();
    
    /** 
     * It will delete event from database
     */ 
    void DeleteEventL();
   
    /** 
     * It shows confirmation query dialog
     */    
    TBool DisplayConfirmationQueryDlgL( const TDesC& aMsg );
    /** 
     * Change status of the event
     * param[in] aStatus -  value to which event status should be changed
     */ 
    void ChangeEventStatusL(TEvtEventStatus aStatus);
    
    /** 
     * Show all events with given status
     * param[in] aStatusFilter -  status filter to display events
     */
    void ShowEventsL( CEvtMgmtUiEngine::TEvtEventStatusFilter aStatusFilter );
    
    /** 
     * It will handle 'show on map' command.
    */
    void HandleShowOnMapCommandL();
    
    /** 
     * It will handle 'navigate to place' command.
     */
    void HandleNavigateToPlaceCommandL();
    
public: // Inherited from Base classes    
  	/**
  	 * Inherited from CAknView
  	 */     
    void DoActivateL( const TVwsViewId& aPrevViewId,
                            TUid        aCustomMessageId,
                      const TDesC8&     aCustomMessage );

  	/**
  	 * Inherited from CAknView
  	 */ 
    void DoDeactivate();

  	/**
  	 * Inherited from CAknView
  	 */ 
    TUid Id() const;

  	/**
  	 * Inherited from CAknView
  	 */ 
    void HandleCommandL( TInt aCommand );
   
  	/**
  	 * Inherited from CAknView
  	 */    
    void DynInitMenuPaneL( TInt 			aResourceId, 
    					   CEikMenuPane* 	aMenuPane );
    					   
    /**
     * Inherited from MEvtMgmtUiEngineObserver
     */
    void NotifyEventChangeL(TInt aErrorCode, TInt aState );
    
    /**
     * Inherited from MEvtMgmtUiEngineObserver
     */
    void NotifyModelChangeL();
    
    /**
     * Inherited from MEvtMgmtUiCmdHdlr
     */
    void HandleCmdL( TEvtMgmtUiCmd aCommand );
     
private:
	/**
	 * Overloaded C++ Contructor.
	 */
    CEvtMgmtUiStartupView();

	/**
	 * Second phase of the two phase constructor.
	 */
    void ConstructL();
    
    /**
     * Handles the visbility of the S60 Help feature.
     *
     * @param aMenuPane, Menu pane object on which the Help feature must
     *                   be evaluated.
     */
    void HandleHelpFeature( CEikMenuPane& aMenuPane ) const;

	/**
	 * To change the CBA
	 */
    void SetCba( TInt aResourceId );  
    
    /**
     * It will return the event-id of the selected item. 
     */
    TEvtEventId EventIdOfCurrentItem();
    
    /**
     * It will return the event id of event.
     */
    TEvtEventId EventId( TInt aIndex );
    
    /*
     * It will return type of marked events
     */
    TInt  MarkedEventsStatus();
    
    /*
     * It will start wait dialog.
     */
    void StartWaitDialogL();
    
   /*
    * Check wether map functionality can be enabled.
    * ret ETrue if map functionality should be enabled else EFalse
    */
    TBool EnableMapFunctionalityL( );
    
private: // from MAknBackgroundProcess
    /**
    * Called by the wait note wrapper when the note is dismissed. 
    * This is as a result of either the user cancelling the note,
    * or the process finishing.
    */
    void DialogDismissedL(TInt /*aButtonId*/); 

    /**
    * Called by the wait note wrapper following StepL. Unless the user has cancelled the note, 
    * if this returns EFalse, it will call StepL again, otherwise, it will call ProcessFinished.
    * @return ETrue if processing is complete, EFalse otherwise
    */
    TBool IsProcessDone() const;

    /**
    * Called by the wait note wrapper if the process is complete or if the user cancelled the note.
    * Completes the game save and resets the steps completed counter.
    */
    void ProcessFinished();  

    /**
    * Called by the wait note wrapper if the processing is not yet complete. 
    * Completes a step in the processing, by saving part of the game to a file, and incrementing the
    * number of steps completed. This is a synchronous method.
    */
    void StepL();  

    /**
    * Handles error occurred in StepL
    * @param aError error code that occurred
    * @return translated error code. If this is != KErrNone process will be stopped.
    */
    TInt CycleError(TInt aError);
       
private: // Enumerations
    /**
     * Enumeration to Flag the Marked Event's Status.
     */
    enum TMarkedEventsStatus
        {
        EMarkedEventsStatusActive,
        EMarkedEventsStatusDraft,
        EMarkedEventsStatusCompleted,
        EMarkedEventsStatusInvalid,
        EMarkedEventsStatusMix
        };
    
    /**
     * Wait Dialog's States.
     */
    enum TWaitDlgState
        {
        EWaitDlgLaunchToDelete,
        EWaitDlgLaunchToChangeStatus,        
        EWaitDlgInitiateProcess,
        EWaitDlgDestroy       
        };
    
private: // Data members
	/**
	 * Events Management UI Start-up container.
	 * Owns.
	 */
    CEvtMgmtUiStartupContainer*   	iContainer;
    
    /**
     * Model to perform all events (data only) manipulation
     */
    CEvtMgmtUiEngine* iEngine;
    
    /*
     *  Position Settings Launcher
     *  Owns
     */
    CEvtMgmtUiLocSettingsLauncher* iPosSetLauncher;
    
    /*
     *  Boolean to maintain the application launch type
     */
    TBool iIsEmbeddedApp;
    
    /*
     *  Model
     *  Owns
     */
    CEvtMgmtUiModel*                iModel;
  
    /*
     * List of events id to be deleted/updated
     * Owns
     */
    RArray<TEvtEventId>* iEvtIdArray;
      
    /*
     * It is used as state-machine for wait dialog
     */
    TInt iWaitState;
    
    /*
     * It stores event status .... used for 'change event status'
     */
    TEvtEventStatus iEvtStatus;
    
    /*
     *  It will determine if error note should be displayed
     *  once 'Activate' operation is completed for multiple events
     *  An error note is required when event doesnt contain mandatory fields.
     */
    TBool iDisplayNote;
    };          

#endif // C_EVTMGMTUISTARTUPVIEWAPPVIEW_H

