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
* Description:  Blid application settings view class definition.
*
*/


#ifndef BLIDSETTINGSVIEW_H
#define BLIDSETTINGSVIEW_H

// INCLUDES
#include "CBlidBaseView.h"

// FORWARD DECLARATIONS
class CBlidSettingsContainer;
class MBlidSettings;
class CEikMenuPane;
class CBlidEng;
// CLASS DECLARATION

/**
*  CBlidSettingsView view class.
*
*/
class CBlidSettingsView : public CBlidBaseView
    {
    public: // Constructors and destructor

        /**
         * Symbian default constructor.
         * @param aKeyProcessor A pointer to MKeyProcessor
         */
        static CBlidSettingsView* NewLC( MKeyProcessor& aKeyProcessor );

        /**
         * Destructor.
         */
        ~CBlidSettingsView();

    private:
        /**
         * C++ constructor
         * @param aKeyProcessor A pointer to MKeyProcessor
         */
        CBlidSettingsView( MKeyProcessor& aKeyProcessor );

        /**
        * Symbian default constructor.
        */
        void ConstructL();

    public: // from CAknView
        TUid Id() const;
        void HandleCommandL(TInt aCommand);

    public:  // From MEikMenuObserver
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    public: // From MLogsKeyProcessor
        TBool ProcessKeyEventL( const TKeyEvent& aKeyEvent,
                                TEventCode aType );

    private: // From CAknView
        void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
            const TDesC8& aCustomMessage);
        void DoDeactivate();

    #ifdef RD_SCALABLE_UI_V2
    private:
       /**
        * From CBlidBaseView.
        */
        void HandleListBoxSelectionL();
    #endif // RD_SCALABLE_UI_V2
    
    private: // new functions
        /** 
         * Change current altitude calibration
         */
        void ChangeAltitudeCalibrationL();

        /** 
         * Change Backlight settings Options
         * 1.Normal
         * 2.On During Navigation
         */
        void ChangeBacklightSettingsL();

        /** 
         * Change Backlight settings Options for Double tapping of stylus
         * on the item
         * 1.Normal
         * 2.On During Navigation
         */        
        void ChangeBacklightSettingsValueL(TInt aValue);
        
        void CheckResetOdometerOptionL( );
        
        void OpenArrivalToneDialogL( );

    private: // Data
        /// Own: A pointer to container
        CBlidSettingsContainer* iContainer;

        /// Ref: A pointer to Settings model
        MBlidSettings* iSettingsModel; 

        /// Own: previous view's id
        TInt iPrevViewId;

        /// Own: A pointer to TLocale
        TLocale* iLocale;
        
        // Own: Settings view backlight selection as Normal 
	    HBufC* iBlidBacklightNormal;
        
        // Own: Settings view backlight selection as On During Navigation
        HBufC* iBlidBacklightOn;
    };

#endif  // LOCSETTINGSVIEW_H

// End of File
