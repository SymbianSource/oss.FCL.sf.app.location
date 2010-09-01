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
* Description:  Blid application settings view's container
*                class definition.
*
*/

#ifndef BLIDSETTINGSCONTAINER_H
#define BLIDSETTINGSCONTAINER_H

// INCLUDES
#include "CBlidBaseContainer.h"
#include "CBlidBaseView.h"

// FORWARD DECLARATIONS
class CAknNavigationDecorator;
class CAknSettingStyleListBox;
class CBlidSettingsView;
class MBlidSettings;
class CBlidEng;
// CLASS DECLARATION

/**
*  CBlidSettingsContainer  container control class.
*
*/
class CBlidSettingsContainer : public CBlidBaseContainer
    {
    public: // Constructors and destructor

        /**
         * Two-phased constructor
         * @param aKeyProcessor A pointer to MKeyProcessor
         * @param aParent A pointer to MObjectProvider
         * @param aRect A rectangle for drawing.
         * @param aSettingsModel A pointer to Settings model
         * @return The newly created object.
         */
        static CBlidSettingsContainer* NewL( MKeyProcessor& aKeyProcessor,
                                        MObjectProvider* aParent,
                                        const TRect& aRect,
                                        MBlidSettings* aSettingsModel,
                                        CBlidBaseView& aView);

        /**
         * Destructor.
         */
        ~CBlidSettingsContainer();

    private:
        /**
         * By default Symbian 2nd phase constructor is private.
         * @param aRect Frame rectangle for container.
         * @param aLocSettingsModel A pointer to settings model
         */
        void ConstructL(const TRect& aRect, MBlidSettings* aSettingsModel);

        /**
         * C++ default constructor.
         * @param aKeyProcessor A pointer to MKeyProcessor
         */
        CBlidSettingsContainer( MKeyProcessor& aKeyProcessor, CBlidBaseView& aView );
    
    public: // from CoeControl
        void FocusChanged(TDrawNow aDrawNow);
		TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
		                             TEventCode aType);        

    public: // new functions
       /**
        * Returns the iListBox.
        * @return A pointer to settings listbox
        */
        CAknSettingStyleListBox* ListBox() const;
         
        #ifdef RD_SCALABLE_UI_V2 
       /**
        * Sets listbox observer.
        * @param aObserver observer object.
        */
        void SetListBoxObserver( MEikListBoxObserver* aObserver );
        #endif // RD_SCALABLE_UI_V2          

    private: // From CCoeControl
        void SizeChanged();
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl(TInt aIndex) const;
        void HandleResourceChange(TInt aType);

    private: //data
        /// Own: Decorator class for navigation pane controls.
        CAknNavigationDecorator* iDecoratedTabGroup;

        /// Own: listbox
        CAknSettingStyleListBox* iListBox;
        
        CBlidBaseView& iView;
    };

#endif //BLIDSETTINGSCONTAINER_H

// End of File
