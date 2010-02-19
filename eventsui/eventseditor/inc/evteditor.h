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
* Description:   Form class for creating events.
*
*/


#ifndef EVTEDITOR_H_
#define EVTEDITOR_H_

#include <AknForm.h>
#include <e32base.h>        // CBase
#include <ConeResLoader.h>

#include "evtmgmtuieditorcmdhandler.h"
#include "evtmgmtuisysofmeasurement.h"
#include "evtmgmtuiengine.h"

class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CEvtEvent;
class TCoordinate;
class CEvtMgmtUiEditorCmdHandler;
class TCoeHelpContext;
class CAknIconArray;
class TAknsItemID;
class CAknListQueryDialog;

// Derive the class from CAknForm 
class CEvtEditor : public CAknForm,
                   public MEvtMgmtUiEditorObserver,
                   public MEvtMgmtUiSOMObserver
    {   
    public:  // Constructor and destructor  
        /**
         * Static Two phase constructor
         *         
         */
        static CEvtEditor* NewL( TBool                              aIsEditable, 
                                 CEvtEvent&                         aEvent,
                                 CEvtMgmtUiEngine&                  aEventEngine,
                                 TBool                              aDisplayExitMenuItem = ETrue );
 
        /**
         * Virtual destructor
         */
        virtual ~CEvtEditor(); 
        
    public://new functions
        /*
        * It sets help context
        * 
        * @since Symbian v9.1
        * 
        * @param[in] aContext - help context object  
        */
       void SetHelpContext( TCoeHelpContext aContext );
      
        
    private: //new functions
        /**
         * Load intial data values
         */
        void LoadFormValuesFromDataL();
        
        /**
         * Displays confirmation query dialog
         * @param aResourceId Text to be displayed on query
         */
        TBool DisplayConfirmationQueryDlgL( TInt aResourceId);
      
        /**
         * Set navipane title
		 * @param aTitle title for navipane
         */
        void SetNavipaneTitleL(const TDesC& aTitle);
        
        /**
		 * Change RSK caption
         * 
         */		 
        void ChangeRSKCaptionL();

         /** Change MSK caption
         * @param aControlId of current focused line
         */
           void ChangeMSKCaptionL(TInt  aControlId);
        
        /**
         * Perform the required action when MSK is pressed
         */
        void HandleMSKL();
        
        /**
         * Sets default date and time for date-time components of editor
         */
        //void SetDefaultDateTime();
        
        /**
         * It deletes dialog line for audio-loop
         */
        void RemoveAudioLoopDlgLineL();
        
        /**
        * It inserts dialog line for audio-loop
        */
        void InsertAudioLoopDlgLinesL();
        
        /**
        * It inserts dialog line for radius
        */
        void InsertRadiusDlgLineL();
        
        /**
         * It removes dialog line for radius
         */
        void RemoveRadiusDlgLineL();
        
        /**
         * It inserts dialog line for descriptor
         */
        void InsertDesDlgLineL();
        
        /**
         * It removes dialog line for descriptor
         */
        void RemoveDesDlgLineL();
        
        /**
        * It sets text to descriptor edwin
        */
        void SetDescL();
        
        /**
        * It sets text to place edwin
        */
        void SetPlaceL();
        
        /**
        * It sets Status to popup
        */
        void SetStatusL();
        
        /**
        * It Checks for the status and dynamically sets the popup list
        */
        void CheckStatusL();
        
        /**
        * It sets text to radius editor
        */
        void SetRadiusL();
        
        /**
        * It sets text to ringtone editor
        */
        void SetRingtoneL();
        
       /**
       * It checks if all mandatory fields are filled
       *  @return ETrue if all mandatory fields are filled
       *          KFalse otherwise
       */
        TBool MandatoryFieldsFilled();
        

	    /** 
	     * Set the Title text
	     */ 
	    void MakeTitleL( TDesC& aText );
	    
	   
	           
	    /**
         * Opens the nearest localized resourcefile using aResourceLoader. 
         * aResourceLoader should be opened only once before closing it. 
         * Otherwise it will raise a panic. Therefore it is better to have 
         * multiple resource loaders.
         * 
         * @param aResourceFileName Drive and name of resource file in format 
         *                          <path>:<rsc_file_name>
         * @param aResourceLoader Resource loader for the resource. The loader 
         *                        must not be opened allready or it will raise 
         *                        a panic. It is caller's responsibility to 
         *                        close the resource loader after using the 
         *                        resource.
         */
        void OpenLocalizedResourceFileL( const TDesC& aResourceFileName, 
                                RConeResourceLoader&  aResourceLoader );
	   
    public:  // Functions from base class
        /*
         *  Inherited from CCoeControl
         */
        void SetMopParent( MObjectProvider* aParent );
       
        /*
         * Inherited from CEikDialog
         */
        TInt ExecuteLD();
            
        /*
         * Inherited from CCoeControl
         */
        void GetHelpContext( TCoeHelpContext& aContext ) const;
      
   private:  // Functions from base class
       
        /**
        * Inherited from CAknDialog
        */
        void LineChangedL( TInt  aControlId  ); 
        
        /**
         * Inherited from CAknForm
         */
        //void HandleControlStateChangeL();
        void HandleControlStateChangeL(TInt aControlId);
        
       
        /**
         * Inherited from CAknForm
         */
        void DynInitMenuPaneL(TInt aResourceId, CEikMenuPane* aMenuPane);
         
       /**
        * Inherited from CAknForm
        */
       void ProcessCommandL(TInt  aCommandId); 
       
        /**
         * Inherited from CEikDialog
         */
        TBool OkToExitL( TInt aButtonId );
         
        /**
         * Inherited from CEikDialog
         */
        void PreLayoutDynInitL();        
       
        /**
        * Inherited from CEikDialog
        */
        void PostLayoutDynInitL();  
        
       /**
         * Inherited from MEvtMgmtUiEditorObserver
         */
        void NotifyEditorL(TInt aCommand, TInt aErrorCode);   
         
        // Inherited from MEvtMgmtUiSOMObserver
        void NotifySOMChangeL( );
  
        void HandleDialogPageEventL( TInt aEventID );
        
        TKeyResponse OfferKeyEventL (const TKeyEvent &aKeyEvent, TEventCode aType); 
        
    private:  // Constructor
        /**
         * C++ Default constructor
         */       
         CEvtEditor( TBool                              aIsEditable, 
                     CEvtEvent&                         aEvent, 
                     CEvtMgmtUiEngine&                  aEventEngine,
                     TBool                              aDisplayExitMenuItem = ETrue);
  
        /**
         * Second phase of the two phase constructor
         */ 
         void ConstructL();
         
    private: //new functions
        
        /*
         * It checks if event is empty.
         * 
         * @since Symbian v9.1
         * 
         * @ret true if event is empty else false.
         */
        TBool IsEmptyEventL();
        
        TBool ProcessDoneCommandL();
        
       /**
        * it toggles value of popup field
        */        
        void TogglePopupFieldControlL( TInt aControlId );
        
    private:
        /**
        * owns: It handles all editor commands
        */
       CEvtMgmtUiEditorCmdHandler*      iCmdHandler;
       /*
        * Owns: It checks unit for radius
        */
       CEvtMgmtUiSysOfMeasurement*      iSysOfMeas;
       /*
        * Mode in which editor is opened. It can be either view or edit mode.
        */   
       TBool                            iIsEditMode;
       /*
        *  Ref: The event to be displayed on editor
        */
       CEvtEvent&                       iEvent;
       /*
        *  help context object
        */
       TCoeHelpContext                  iHelpContext;  
       
       /**
       * Resource loader handle
       * Owns
       */
      RConeResourceLoader             iResourceLoader;
      
      /**
       * Reference to the Events UI Engine
       */
      CEvtMgmtUiEngine&               iEventEngine;

      /*
       * Optimization:It is used in order to avoid repeated check in dynitmenupaneL
       */
      TBool                           iDisplayExitMenuItem;
      
      /*
       * It determines which attributes are modified
       */
      TEvtEventAttributeMask          iEventAttributeMask;
 };

#endif /*EVTEDITOR_H_*/
