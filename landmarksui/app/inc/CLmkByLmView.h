/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -    This class has implementation for 'By Landmarks name' view
*
*/






#ifndef CLMKBYLMVIEW_H
#define CLMKBYLMVIEW_H

// INCLUDES
#include "CLmkBaseView.h"

// FORWARD DECLARATIONS
class CLmkByLmContainer;
class MLmkListMemento;
class CLmkSender;

// CLASS DECLARATION

/**
*  CLmkByLmView view class.
*
*/
class CLmkByLmView : public CLmkBaseView
    {
    public: // Constructors and destructor
        /**
        * C++ default constructor.
        * @param aKeyProcessor
        * @param aLmkSender
        * @return created CLmkByLmView object
        */
        static CLmkByLmView* NewLC( MLmkKeyProcessor& aKeyProcessor,
                                    CLmkSender& aLmkSender );

        /**
        * Destructor.
        */
        ~CLmkByLmView();

    private: // constructor
        /**
        * C++ constructor
        * @param aKeyProcessor
        * @param aLmkSender
        * @return newly instantiated object
        */
        CLmkByLmView( MLmkKeyProcessor& aKeyProcessor,
                      CLmkSender& aLmkSender );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public: // From MLmkKeyProcessor
		TBool ProcessKeyEventL( const TKeyEvent& aKeyEvent,
                                TEventCode aType );

    public: // From CAknView
        TUid Id() const;
        void HandleCommandL( TInt aCommand );

    protected:  // From MEikMenuObserver
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    private: //From CAknView
        void DoActivateL( const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );
        void DoDeactivate();
    private://From CLmkBaseView

    	void EnableMskMenuL(TBool  aEnable);
    	void EnableMskLabelL(TBool aEnable);

    private: // New functions
    	void AttachAIWMenuInterestL();
    	TBool HandleAIWserviceCommandsL( TInt aServiceCommand ,
														  TInt aMenuCommand);

    public : // Added for Landscape

		void Update();

	#ifdef RD_SCALABLE_UI_V2
	protected://for touch support

		//from MEikListBoxObserver
    	void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
    #endif//RD_SCALABLE_UI_V2
    private: // Data
        /// Own: A pointer to container
        CLmkByLmContainer* iContainer;
        // Own: list status information
        MLmkListMemento* iListMemento;
        /// Ref: landmark sender
        CLmkSender& iLmkSender;
    };

#endif // CLMKBYLMVIEW_H

// End of File
