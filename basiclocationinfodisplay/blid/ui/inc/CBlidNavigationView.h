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
* Description:  Declares compass view for Blid application.
*
*/

#ifndef BLIDNAVIGATIONVIEW_H
#define BLIDNAVIGATIONVIEW_H

// SYSTEM INCLUDES
#include "CBlidBaseView.h"
#include "MBlidLocation.h"
#include "MBlidRouter.h"


// FORWARD DECLARATIONS
class CHtkNavigationControl;
class CAlfEnv;
class CBlidLocSettingsLauncher;
class CAlfControlGroup;
class CAlfDisplay;

// CLASS DECLARATION

/**
*  CBlidNavigationView view class.
*
*/
class CBlidNavigationView : public CBlidBaseView
    {
    public: // Constructors and destructor
        /**
         * C++ default constructor.
         * @param aKeyProcessor
         * @return A pointer to CBlidNavigationView
         */
        static CBlidNavigationView* NewLC( MKeyProcessor& aKeyProcessor,CAlfEnv& aEnv );

        /**
         * Destructor.
         */
        ~CBlidNavigationView();

    private: // constructor
        /**
         * C++ constructor
         * @param aKeyProcessor
         */
        CBlidNavigationView( MKeyProcessor& aKeyProcessor,CAlfEnv& aEnv );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();
    
    public: //From MBlidEngObserver
    	/**
         * From MBlidEngObserver, called when a position update is received.
         */
        void NotifyL( const TBool aOnlineMode );
        /**
         * From MBlidEngObserver, called when there is an error in receiving position update
         */
        void NotifyErrorL( TInt aErrorCode );

    public: // From MKeyProcessor
    	/**
         * Process the key event
         */
		TBool ProcessKeyEventL( const TKeyEvent& aKeyEvent,
                                TEventCode aType );

    public: // From CAknView
    	/**
         * From CAknView, returns the view id
         */
        TUid Id() const;
        /**
         * From CAknView, takes care of command handling.
         */
        void HandleCommandL(TInt aCommand);
        /**
         * From CAknView, handles a change to the resources
         */
        void HandleResourceChangeL( TInt aType );
    public:
    	/**
         * Retruns the navigation control
         */
        CHtkNavigationControl* GetControl();        

    private: // From CAknView
    	/**
         * From CAknView, called when view is activated
         */
        void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
            const TDesC8& aCustomMessage);
        /**
         * From CAknView, called when view is deactivated
         */    
        void DoDeactivate();

    #ifdef RD_SCALABLE_UI_V2
    private:
       /**
        * From CBlidBaseView.
        */
        void HandleListBoxSelectionL();
    #endif // RD_SCALABLE_UI_V2
    
    private: 
    	/**
         * Selects a waypoint
         */
        TInt SelectWaypointL();
        /**
         * Launches landmarks selector dialogs
         */
        TInt LaunchLandmarksDialogL();
        /**
         * Launches waypoint dialog
         */
        TInt LaunchManualWaypointDialogL();
        /**
         * Inserts a waypoint
         */
        TBool InsertWaypointL();
        /**
         * Save current position, launches landmarks add/edit dialog
         */
        void SaveCurrentPositionAsLandmarkL();
    
    private: // Data
		
		CHtkNavigationControl*  iNavigationCtrl;
		
		CAlfEnv& iEnv;

		CAlfControlGroup* iNavigationCtrlGroup;
		
        // Own: Pointer to Location Settings Launcher
        CBlidLocSettingsLauncher* iLauncher;
        
        CAlfDisplay*		iCtrlDisplay;
    };

#endif

// End of File
