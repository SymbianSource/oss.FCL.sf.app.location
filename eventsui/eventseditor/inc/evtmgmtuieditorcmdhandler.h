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


#ifndef EVTMGMTUIEDITORCMDHANDLER_H_
#define EVTMGMTUIEDITORCMDHANDLER_H_

#include <e32base.h>        // CBase
#include <locsettingsuiclient.h>
#include "evtdefs.h"
#include "evtmgmtuiengine.h"
#include "evtmgmtuiwaitdlglauncher.h"

class CEvtEditor;
class CEvtEvent;
class TCoordinate;
class CEvtMgmtUiPlaceComponent;
class CAknIconArray;
class CAknListQueryDialog;
class TAknsItemID;
class CEvtMgmtUiWaitDialogLauncher;
class CEvtMgmtUiDescComponent;
class CEvtMgmtUiMapNavigationAdapter;
class CEvtMgmtUiRingtoneComponent;

class MEvtMgmtUiEditorObserver
    {
public:
    /**
     * It will be used to notify editor that command has been handled
     * @param aCommandId Id of the command pressed
     * @param aErrorCode It will contain the status of the operation
     *                KErrNone if successful otherwise error code          
     */
    virtual void NotifyEditorL( TInt aCommand, 
                                TInt aErrorCode )=0;
    };

/**
 *  @class CEvtMgmtUiEditorCmdHandler
 *  Class that represents command handler for editor
 *
 *  This class acts as command handler of editor.
 *  It  is used to :
 *      To activate event
 *      To save event as draft
 *      To save event as completed
 *      To delete event
 *      To add/edit/remove description
 *      To add/change place
 *      To assign ringtone
 *      To show event on map
 *      To navigate to place
 *      To launch position settings
 *
 *  @since S60 v9.1
 */

class CEvtMgmtUiEditorCmdHandler : public CActive,
                                   public MEvtMgmtUiEngineObserver,
                                   public MEvtMgmtUiWaitDialogLauncherObserver
    {
    public:  // Constructor and destructor  
      /**
       * Static Two phase constructor
       * 
       * @since Symbian v9.1
       *
       * @param[in/out] aEvent - event object which contains current event information.
       *                         event details may be changed while executing commands
       * @param[in] aEditorObserver - observer to handle editor specific operations 
       *                              after command is executed
       */
      static CEvtMgmtUiEditorCmdHandler* NewL( 
                        CEvtEvent&                      aEvent, 
                        MEvtMgmtUiEditorObserver&       aEditorObserver,
                        CEvtMgmtUiEngine&               aEventEngine );

      /**
       * Virtual destructor
       */
      virtual ~CEvtMgmtUiEditorCmdHandler(); 
      
    public: // new functions
        /**
         * It will handle all editor commands.
         * 
         * @since Symbian v9.1
         * 
         * @param[in] aCommand - command id of the command to be handled
         * @param[out] aResult - Result of the command operation
         */
        void HandleEditorCmdL( TInt     aCommand );
        
         /*
          * It will handle add/edit/remove description operation.
          * 
          * @since Symbian v9.1
          * 
          * @param[in] aCommand - Command id of the operation to be performed.
          */
         TInt HandleDescCommandsL( TInt     aCommand );
         
         /*
          * It will handle assign tone operation.
          * 
          * @since Symbian v9.1
          * 
          * @ret returns KErrNone if successfully handled else error
          */
         TInt HandleAssignToneCommandL();
         
         /*
          * It will handle 'show on map' operation.
          * 
          * @since Symbian v9.1
          * 
          */
         void HandleShowOnMapCommandL();
         
         /*
          * It will handle 'navigate to place' operation.
          * 
          * @since Symbian v9.1
          * 
          */
         void HandleNavigateToPlaceCommandL();
         
         /*
          *  It will set attribute mask which will be used to update event.
          * 
          *  @since Symbian v9.1
          * 
          */
         void SetEventAttributeMask( TEvtEventAttributeMask aEventAttributeMask );
                 
   public: //derived
        /**
        * Derived from CActive
        */
        void RunL();
        
        /**
        * Derived from CActive
        */
        TInt RunError( TInt aError );
        
        /**
        * Derived from CActive
        */
        void DoCancel();
        
        /**
        * Inherited from MEvtMgmtUiEngineObserver
        */
        void NotifyEventChangeL(TInt aErrorCode, TInt aState );
        
        /**
        * Inherited from MEvtMgmtUiEngineObserver
        */
        void NotifyModelChangeL();
        
        /**
        * Inherited from MEvtMgmtUiWaitDialogLauncherObserver
        */
        void HandleDialogDismissed( TInt aButtonId );
      
    private:  // Constructor
        /**
         * C++ Default constructor
         */       
        CEvtMgmtUiEditorCmdHandler(
                    CEvtEvent&                          aEvent,
                    MEvtMgmtUiEditorObserver&           aEditorObserver,
                    CEvtMgmtUiEngine&                   aEventEngine );
  
        /**
         * Second phase of the two phase constructor
         */ 
         void ConstructL();
         
    private:         
         /**
         * It will display comfirmation query dialog
         * 
         * @since Symbian v9.1
         * 
         * @param[in] aResourceId - resource id of the resource to be displayed 
         *                           on confirmation query dialog.
         * @ret returns true if OK is pressed.
         *      returns false if Cancel is pressed. 
         */
         TBool DisplayConfirmationQueryDlgL( TInt aResourceId );

         /*
          * It will handle'set place' operation.
          * 
          * @since Symbian v9.1
          * 
          */
         void HandleSetPlaceCommandL();
                        
         /**
          * Start wait Dialog for asynchronous processing.
          */
         void StartWaitDialogL(TInt aResourceId);
                        
         /**
          * Stop outstanding wait Dialog.
          */
         void StopWaitDialogL();
                        
         /**
          * It is used to update icons
          */
         void UpdateIconsL( CAknListQueryDialog* aDialog );    
          
         /**
          * Append icon to icon array. CAknIconArray owns icons so it is 
          * easier to it leave safely.
          * @param aIcons icon array
          * @param aIconFileWithPath icon file name with path
          * @param aSkinID skin id of the icon
          * @param aIconGraphicsIndex picture index
          * @param aIconGraphicsMaskIndex mask index
          */
         void AddIconL( CAknIconArray&       aIcons,
                        const TDesC&         aIconFileWithPath,
                        TAknsItemID          aSkinID,
                        TInt                 aIconGraphicsIndex,
                        TInt                 aIconGraphicsMaskIndex );
                              
         /**
          * Gets Events Management UI icon file path and name. Ownership
          * is transferred, item is left in the cleanup stack.
          * @return TFileName* The full name of the Events UI icon file.
          */        
         TFileName* IconFileNameLC() const;
         
    private:
        /*
         *  state-machine's state
         */
        TInt                            iState;
		
        /*
         *  The event to be displayed on editor
         */
        CEvtEvent&                      iEvent;      
		
        /*
         *  It performs all editor specific operation when command is executed
         */
        MEvtMgmtUiEditorObserver&       iEditorObserver;
		
        /*
         * Owns: 'Set place' component object
         * It retrieves place and location information
         */
        CEvtMgmtUiPlaceComponent*       iPlaceComponent;
		
        /*
         * Owns: It launches position settings
         */
        CLocSettingsUiClient*                   iClientLibrary;
        
        /*
         * Description Component Instance
         * Owns
         */
        CEvtMgmtUiDescComponent* iDescComponent;
        
        /*
         * Map Component Instance
         * Owns
         */
        CEvtMgmtUiMapNavigationAdapter* iMapAdapter;
        
        /*
         * Ringtone Component Instance
         * Owns
         */
        CEvtMgmtUiRingtoneComponent* iRingtoneComponent; 
        
        /**
        * Wait dialog launcher
        * Owns
        */ 
        CEvtMgmtUiWaitDialogLauncher*   iWaitDialogLauncher;
        
        /**
         * Reference to the Events UI Engine
         */
        CEvtMgmtUiEngine&               iEventEngine;      
        
        /*
         * List of events id to be deleted/updated
         * Owns
         */
        RArray<TEvtEventId> iEvtIdArray;     
        
        /*
         * It indicates if any Dialog is outstanding
         */
        TBool   iIsDlgOutstanding;
        
        /*
         * It determines which attributes are modified
         */
        TEvtEventAttributeMask          iEventAttributeMask;
    };

#endif //EVTMGMTUIEDITORCMDHANDLER_H_
