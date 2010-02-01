/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -    Declares category contents view for LMK application.
*
*/






#ifndef CLMKCATEGORYCONTENTSVIEW_H
#define CLMKCATEGORYCONTENTSVIEW_H

// INCLUDES
#include "CLmkBaseView.h"

// FORWARD DECLARATIONS
class CLmkCategoryContentsContainer;
class MLmkListMemento;
class CLmkSender;

// CLASS DECLARATION

/**
*  CLmkCategoryContentsView view class.
*
*/
class CLmkCategoryContentsView : public CLmkBaseView
    {
    public: // Constructors and destructor
        /**
        * C++ default constructor.
        * @param aKeyProcessor
        * @param aLmkSender
        * @return created CLmkCategoryContentsView object
        */
        static CLmkCategoryContentsView* NewLC( MLmkKeyProcessor& aKeyProcessor,
                                                CLmkSender& aLmkSender );

        /**
        * Destructor.
        */
        ~CLmkCategoryContentsView();

    private: // constructor

        /**
        * C++ constructor
        * @param aKeyProcessor forward key events
        * @param aLmkSender used to send landmarks
        * @return newly instantiated object
        */
        CLmkCategoryContentsView( MLmkKeyProcessor& aKeyProcessor,
                                  CLmkSender& aLmkSender );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public: // From MLmkKeyProcessor
		TBool ProcessKeyEventL( const TKeyEvent& aKeyEvent,
                                TEventCode aType );

    public: // CAknView
        TUid Id() const;
        void HandleCommandL( TInt aCommand );
    private: // New functions
    	void AttachAIWMenuInterestL();
    	TBool HandleAIWserviceCommandsL( TInt aServiceCommand ,TInt aMenuCommand);

    protected:  // From MEikMenuObserver
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    private: //From CAknView
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );
        void DoDeactivate();

	public: // For Landscape Support
		void Update ();
		TBool IsContainerPtrValid();

	 private://From CLmkBaseView

    	void EnableMskMenuL(TBool  aEnable);
    	void EnableMskLabelL(TBool aEnable);

    #ifdef RD_SCALABLE_UI_V2
    protected://For touch support

    	//from MEikListBoxObserver
    	void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
    #endif//RD_SCALABLE_UI_V2

    private: // Data
        /// Own: A pointer to container
        CLmkCategoryContentsContainer* iContainer;
        /// Own: list status information
        MLmkListMemento* iListMemento;
        /// Ref: landmark sender
        CLmkSender& iLmkSender;
    };

#endif // CLMKCATEGORYCONTENTSVIEW_H

// End of File
