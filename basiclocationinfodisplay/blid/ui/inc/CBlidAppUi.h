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
* Description:  Blid application UI class definition.
*
*/



#ifndef BLIDAPPUI_H
#define BLIDAPPUI_H

// INCLUDES
#include <aknViewAppUi.h>
#include <akntabgrp.h>
#include <aknnavide.h>
#include <AknTabObserver.h>
// included for Huitk impl
#include <alf/alfenv.h>

// USER INCLUDE
#include "MKeyProcessor.h"
#include "BlidNotes.h"

// FORWARD DECLARATIONS
//class CBlidMainContainer;
class CBlidDocument;
class CSatelliteInfoUI;

// CLASS DECLARATION

/**
* Application UI class. 
*/
class CBlidAppUi : public CAknViewAppUi, public MAknTabObserver, 
                   public MKeyProcessor
    {
    public: // Constructors and destructor
        /**        
         * Second phase constructor        
         */      
        void ConstructL();

        /**
         * Destructor.
         * Frees reserved resources
         */      
        ~CBlidAppUi();
        
    public: // New functions
        /**
         * Returns the Blid document object.
         * @return CBlidDocument
         */
        CBlidDocument* BlidDocument() const;

        /**
         * Show BLID application's dialogs
         * @param aNoteId Id of Note
         */
        void ShowDialogL( const BlidNotes::TBlidNotes aNoteId );

        /**
         * Skin changed
         *
         */
        void SkinChange();

        /**
         * Checks if App is in foreground
         */
        TBool IsForeground();
        
        /**
         * Set satellite view active or inactive
         * @param aActive To set active or inactive
         */        
        void SetSatelliteViewActive( TBool aActive );
        
        /**
         * Check if Satellite view is active
         *
         */        
        TBool IsSatelliteViewActive( );
        
        /**
         * Resets the odometer value
         *
         */        
        void ResetOdoMeterValueL();
        
        /**
         * Launches Satellite info dialog
         *
         */        
        void LaunchSatelliteInfoDlgL();

    public: // From MAknTabObserver
        /**
         * Called when Tab is changed, activates appropriate view
         * @param aIndex
         */    
        void TabChangedL(TInt aIndex);

    public: // From  MKeyProcessor
        /**
         * Process key event
         * @param aKeyEvent
         * @param aType
         */    
        TBool ProcessKeyEventL( const TKeyEvent& aKeyEvent,TEventCode aType );
        
        /**
         * Retruns CBlidAppUi pointer, which is Tab observer
         *
         */        
        MAknTabObserver* TabObserver();
    
    public: // from MBlidEngObserver
        void NotifyL( const TBool aOnlineMode );
        void NotifyErrorL( TInt aErrorCode );

    protected: // from CCoeAppUi
        /**
         * Called whenever app goes to background or comes to foreground
         * @param aForeground
         */    
	    void HandleForegroundEventL(TBool aForeground);
	    
        /**
         * Handles a change to the resources
         * @param aType
         */	    
		void HandleResourceChangeL( TInt aType );
		
    public: // From CEikAppUi
        /**
         * From CEikAppUi, takes care of command handling.
         * @param aCommand command to be handled
         */
        void HandleCommandL(TInt aCommand);

    private: // new functions
        /**
         * Launch Landmarks
         */
        void LaunchLandmarksDialogL();
        
    private: //Data
        /// Ref: A pointer to CSatelliteInfoUI
        CSatelliteInfoUI* iSatelliteInfo;

        /// Own: TBool variable
        TBool iContextPane;
        
        /// Own: State to hole app state (foreground/background)
        TBool iForeground;
    
		/*! @var HUIToolkit environment. */
    	CAlfEnv* iAlfEnv;
    	
    	TBool iIsSatViewActive;
    };

#endif // BLIDAPPUI_H

// End of File
