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
* Description:  Declares main view for Blid application.
*
*/

#ifndef BLIDTRIPMETERVIEW_H
#define BLIDTRIPMETERVIEW_H

// INCLUDES
#include "CBlidBaseView.h"
#include <eikmenub.h> // CEikMenuBar

// FORWARD DECLARATIONS
class CHtkTripMeterControl;
class CBlidNoteDialog;
class CAlfEnv;
class CAlfControlGroup;
class CBlidLocSettingsLauncher;
class CAlfDisplay;

// CLASS DECLARATION

/**
*  CBlidTripMeterView view class.
*
*/
class CBlidTripMeterView : public CBlidBaseView
    {
    public: // Constructors and destructor
        /**
         * C++ default constructor.
         * @param aKeyProcessor
         * @return Created view object
         */
        static CBlidTripMeterView* NewLC( MKeyProcessor& aKeyProcessor,CAlfEnv& aEnv );

        /**
         * Destructor.
         */
        ~CBlidTripMeterView();

    private: // constructor
        /**
         * C++ constructor
         * @param aKeyProcessor
         */
        CBlidTripMeterView( MKeyProcessor& aKeyProcessor,CAlfEnv& aEnv );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    public: //from MBlidEngObserver
    	/**
         * From MBlidEngObserver, called when a position update is received.
         * @param aOnlineMode
         */
        void NotifyL( const TBool aOnlineMode );
        /**
         * From MBlidEngObserver, called when there is an error in receiving position update
         * @param aErrorCode 
         */
        void NotifyErrorL( TInt aErrorCode );

    public: // from CAknView
    	/**
         * From CAknView, returns the view id
         * @param None
         */
        TUid Id() const;
        /**
         * From CAknView, takes care of command handling.
         * @param aCommand command to be handled
         */
        void HandleCommandL(TInt aCommand);
        /**
         * From CAknView, handles a change to the resources
         * @param aType 
         */
        void HandleResourceChangeL( TInt aType );
        
    #ifdef RD_SCALABLE_UI_V2
    private:
       /**
        * From CBlidBaseView.
        */
        void HandleListBoxSelectionL();
    #endif // RD_SCALABLE_UI_V2
    
    private: // From CAknView
    	/**
         * From CAknView, called when view is activated
         * @param aPrevViewId
         * @param aCustomMessageId
         * @param aCustomMessage
         */
        void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
            const TDesC8& aCustomMessage);
        /**
         * From CAknView, called when view is deactivated
         * 
         */    
        void DoDeactivate();

    public:  // From MEikMenuObserver
    	/**
         * From MEikMenuObserver, dynamically intializes the menu
         * @param aResourceId
         * @param aMenuPane
         */
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );
        
	public:
		/**
         * Retruns the tripmeter control
         * 
         */
		CHtkTripMeterControl* GetControl();        

    private: // Data
		//own:tripmetercontrol class
		CHtkTripMeterControl*  iTripCtrl;
		
		CAlfEnv& iEnv;

        CBlidNoteDialog* iNoteDialog;
        
        CAlfControlGroup* iTripCtrlGroup;
        // Own: Pointer to Location Settings Launcher
        CBlidLocSettingsLauncher* iLauncher;
        
        CAlfDisplay*		iCtrlDisplay;

    };

#endif

// End of File
