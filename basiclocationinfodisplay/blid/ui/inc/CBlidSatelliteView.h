/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declares main satellite view for Blid application.
*
*/

#ifndef CBLIDSATELLITEVIEW_H
#define CBLIDSATELLITEVIEW_H

// INCLUDES
#include "CBlidBaseView.h"
#include "CBlidLocSettingLauncher.h"

// FORWARD DECLARATIONS
class CBlidSatelliteControl;
class CAlfEnv;
class CAlfControlGroup;

// CLASS DECLARATION

/**
*  CBlidSatelliteView view class.
*
*/
class CBlidSatelliteView : public CBlidBaseView
    {
    public: // Constructors and destructor
        /**
         * C++ default constructor.
         * @param aKeyProcessor 
         * @return Created CBlidSatelliteView object
         */
        static CBlidSatelliteView* NewLC( MKeyProcessor& aKeyProcessor,CAlfEnv& aEnv );

        /**
         * Destructor.
         */
        ~CBlidSatelliteView();
        
        /**
         * Control Getter
         */
        CBlidSatelliteControl* GetControl();

    private: // constructor
        /**
         * C++ constructor
         * @param aKeyProcessor
         */
        CBlidSatelliteView( MKeyProcessor& aKeyProcessor,CAlfEnv& aEnv );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    public: // From MKeyProcessor
		TBool ProcessKeyEventL( const TKeyEvent& aKeyEvent,
                                TEventCode aType );

    public: // CAknView
        TUid Id() const;
        void HandleCommandL(TInt aCommand);
        void HandleResourceChangeL( TInt aType );

		#ifdef RD_SCALABLE_UI_V2
		void HandleListBoxSelectionL();
		#endif //RD_SCALABLE_UI_V2
		
    public: //From MBlidEngObserver
        void NotifyL( const TBool aOnlineMode );
        void NotifyErrorL( TInt aErrorCode );
		            

    private: //From CAknView
        void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
            const TDesC8& aCustomMessage);
        void DoDeactivate();
    
    private: // Data
		//own:navigationcontrol class
		CBlidSatelliteControl*  iMainSatelliteCtrl;
		
		CAlfEnv& iEnv;

		CAlfControlGroup* iMainSatelliteCtrlGroup;
    };

#endif // CBLIDSATELLITEVIEW_H

// End of File
