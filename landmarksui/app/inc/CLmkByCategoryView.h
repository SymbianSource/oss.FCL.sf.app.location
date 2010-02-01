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
* Description:    LandmarksUi Content File -    LMK application ByCategory view class definition.
*
*/






#ifndef CLMKBYCATEGORYVIEW_H
#define CLMKBYCATEGORYVIEW_H

// INCLUDES
#include "CLmkBaseView.h"

// FORWARD DECLARATIONS
class CLmkByCategoryContainer;
class CLmkAppCategorySelectorImpl;
class MLmkListMemento;
class CLmkAppUi;
// CLASS DECLARATION

/**
*  CLmkByCategoryView view class.
*
*/
class CLmkByCategoryView : public CLmkBaseView
    {
    public: // Constructors and destructor
        /**
        * C++ default constructor.
        * @param aKeyProcessor
        * @return Created CLmkByCategoryView object
        */
        static CLmkByCategoryView* NewLC( MLmkKeyProcessor& aKeyProcessor );

        /**
        * Destructor.
        */
        ~CLmkByCategoryView();

    public: // New functions
        /**
        * Category contents view needs information about category
        * selector's last status.
        * @return memento reference for reading memorized status
        */
        const MLmkListMemento& BorrowMemento();

    protected: // From MLmkKeyProcessor
		TBool ProcessKeyEventL( const TKeyEvent& aKeyEvent,
                                TEventCode aType );

    public: // CAknView
        TUid Id() const;
        void HandleCommandL( TInt aCommand );

    protected:  // From MEikMenuObserver
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    private: // constructor
        /**
        * C++ constructor
        * @param aKeyProcessor
        * @return newly instantiated object
        */
        CLmkByCategoryView( MLmkKeyProcessor& aKeyProcessor );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: //From CAknView
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );
        void DoDeactivate();

	public: // New functions
		void Update(); //Added for landscape support
		void AttachAIWMenuInterestL();

	private://From CLmkBaseView
 		void EnableMskMenuL(TBool aEnable);
    	void EnableMskLabelL(TBool aEnable);

    #ifdef RD_SCALABLE_UI_V2
	protected://for touch support

		//from MEikListBoxObserver
    	void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
    #endif//RD_SCALABLE_UI_V2
    private: // Data
        /// Own: A pointer to container
        CLmkByCategoryContainer* iContainer;

        // Own: list status information
        MLmkListMemento* iListMemento;
    };

#endif // CLMKBYCATEGORYVIEW_H

// End of File
