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
* Description:    LandmarksUi Content File -    LMK application category view's definition.
*
*/






#ifndef __CLMKCATEGORYSETTINGSVIEW_H__
#define __CLMKCATEGORYSETTINGSVIEW_H__

// INCLUDES
#include "CLmkBaseView.h"

// FORWARD DECLARATIONS
class CLmkCategorySettingsContainer;

// CLASS DECLARATION

/**
*  CLmkCategorySettingsView view class.
*
*/
class CLmkCategorySettingsView : public CLmkBaseView
    {
    public: // Constructors and destructor
        /**
        * C++ default constructor.
        * @param aKeyProcessor
        * @return Created CLmkCategorySettingsView object
        */
        static CLmkCategorySettingsView* NewLC( MLmkKeyProcessor& aKeyProcessor );

        /**
        * Destructor
        */
        ~CLmkCategorySettingsView();

    private: // constructor
        /**
        * C++ constructor
        * @param aKeyProcessor
        * @return newly instantiated object
        */
        CLmkCategorySettingsView( MLmkKeyProcessor& aKeyProcessor );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public: // From MLmkKeyProcessor
		TBool ProcessKeyEventL( const TKeyEvent& aKeyEvent,
                                TEventCode aType );

    public: // From CAknView
        TUid Id() const;
        void HandleCommandL(TInt aCommand);

    protected:  // From MEikMenuObserver
        void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

    private: //From CAknView
        void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,
            const TDesC8& aCustomMessage);
        void DoDeactivate();

	public: // for landscape support
		void Update();

	private://From CLmkBaseView

    	void EnableMskMenuL(TBool aEnable);

    #ifdef RD_SCALABLE_UI_V2
	protected://for touch support

		//from MEikListBoxObserver
    	void HandleListBoxEventL( CEikListBox* aListBox, TListBoxEvent aEventType );
    #endif//RD_SCALABLE_UI_V2

    private: // Data
        /// Own: A pointer to container
        CLmkCategorySettingsContainer* iContainer;

        /// Own: previous view's id
        TInt iPrevViewId;

    };

#endif // __CLMKCATEGORYSETTINGSVIEW_H__

// End of File
