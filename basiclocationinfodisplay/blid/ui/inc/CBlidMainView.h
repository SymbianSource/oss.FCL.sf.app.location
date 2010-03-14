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
* Description:  Declares main view for Blid application.
*
*/

#ifndef CBLIDMAINVIEW_H
#define CBLIDMAINVIEW_H

// INCLUDES
#include "CBlidBaseView.h"
#include "CBlidLocSettingLauncher.h"
#include "CBlidMainControl.h"


// FORWARD DECLARATIONS
class CBlidMainControl;
class CAlfEnv;
class CAlfControlGroup;
class CAlfDisplay;

// CLASS DECLARATION

/**
*  CBlidMainView view class.
*
*/
class CBlidMainView : public CBlidBaseView
    {
    public: // Constructors and destructor
        /**
         * C++ default constructor.
         * @param aKeyProcessor 
         * @return Created CBlidMainView object
         */
        static CBlidMainView* NewLC( MKeyProcessor& aKeyProcessor,CAlfEnv& aEnv );

        /**
         * Destructor.
         */
        ~CBlidMainView();
        
        /**
         * View Getter
         */
        CBlidMainControl* GetControl();

    private: // constructor
        /**
         * C++ constructor
         * @param aKeyProcessor
         */
        CBlidMainView( MKeyProcessor& aKeyProcessor,CAlfEnv& aEnv );

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
		void CBlidMainView::HandleListBoxSelectionL();
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
		CBlidMainControl*  iMainCtrl;
		
		CAlfEnv& iEnv;

		CAlfControlGroup* iMainCtrlGroup;
		
		CAlfDisplay*		iCtrlDisplay;
    };

#endif  // CBLIDMAINVIEW_H

// End of File
