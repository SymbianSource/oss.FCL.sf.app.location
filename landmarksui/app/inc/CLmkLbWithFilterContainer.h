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
* Description:    LandmarksUi Content File -    LMK application's filter container class.
*
*/








#ifndef CLMKLBWITHFILTERCONTAINER_H
#define CLMKLBWITHFILTERCONTAINER_H

// INCLUDES
#include "CLmkBaseContainer.h"

// FORWARD DECLARATIONS
class MAknsSkinInstance;
class CAknsBasicBackgroundControlContext;

class CEikTextListBox;
class CAknSearchField;
class CLmkAppSelectorImplBase;
class CAknFilteredTextListBoxModel;
class CPosLandmarkDatabase;
class CAknNavigationDecorator;
class CLmkSender;

// CLASS DECLARATION
/**
*  CLmkLbWithFilterContainer  container control class.
*
*/
class CLmkLbWithFilterContainer : public CLmkBaseContainer
    {
    public: // Constructors and destructor
        ~CLmkLbWithFilterContainer();

    public: // New functions
        /**
        * Return reference to listbox
        * @return CEikTextListBox reference
        */
        CEikTextListBox& ListBox();

    public: // From CCoeControl
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

    protected: // New functions
        /**
        * Setup title pane
        * Template method.
        */
        virtual void SetupTitlePaneL() = 0;

        /**
        * Setup selector and list
        * Template method.
        * @param aDb database reference
        * @param aSender used to send landmarks
        */
        virtual void SetupSelectorAndListL( CPosLandmarkDatabase& aDb,
                                            CLmkSender& aSender ) = 0;

        /**
        * Setup navi panel
        * Template method.
        */
        virtual void SetupNaviPaneL() = 0;

    protected: // Constructors and destructor
        /**
        * C++ default constructor.
        * @param aKeyProcessor
        * @param aHelpContext
        * @return newly instantiated object
        */
        CLmkLbWithFilterContainer( MLmkKeyProcessor& aKeyProcessor,
                                   const TDesC& aHelpContext );

        /**
        * Symbian 2nd phase constructor
        * @param aRect
        */
        void ConstructL( const TRect& aRect );

    private: // New functions
        /**
        * Internal helper method for setting up find box.
        */
        void SetupFindBoxL();
	private://from MLmkMskObserver
        void UpdateMskContainerForFilterL();

    private: // From CCoeControl
        void SizeChanged();
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl(TInt aIndex) const;
        void Draw(const TRect& aRect) const;

    #ifdef RD_SCALABLE_UI_V2
    public://for touch support

    	void SetListBoxObserver( MEikListBoxObserver* aObserver );
    #endif//RD_SCALABLE_UI_V2

    protected: //data
        /// Ref.
        CEikTextListBox* iListBox;

        /// Own
        CAknSearchField* iFindBox;

        /// Own: Decorator class for navigation pane controls.
        CAknNavigationDecorator* iNaviDeco;
        
    private:
       /// Ref: A pointer MAknsSkinInstance
        MAknsSkinInstance* iSkinInstance;

        /// Own: A pointer CAknBasicBackgroundControlContext
		CAknsBasicBackgroundControlContext* iBackgroundSkinContext;

		mutable TBool iIsNeedToReDraw;
    };

#endif // CLMKLBWITHFILTERCONTAINER_H

// End of File
