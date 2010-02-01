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
* Description:    LandmarksUi Content File -    LMK application category settings view's container.
*
*/








#ifndef __CLMKCATEGORYSETTINGSCONTAINER_H__
#define __CLMKCATEGORYSETTINGSCONTAINER_H__

// INCLUDES
#include "CLmkBaseContainer.h"

// FORWARD DECLARATIONS


class CAknView; // For landscape support

class CLmkAppSelectorImplBase;
class CEikMenuBar;
class CLmkAppCategorySettingsImpl;
class CEikTextListBox;

// CLASS DECLARATION
/**
*  CLmkCategorySettingsContainer  container control class.
*
*/
class CLmkCategorySettingsContainer : public CLmkBaseContainer
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor
		* @param aKeyProcessor forward key events
		* @param aParent parent pointer
		* @param aRect A rectangle for drawing.
		* @return The newly created object.
		*/

		// For landscape support

		static CLmkCategorySettingsContainer* NewL(
                                        MLmkKeyProcessor& aKeyProcessor,
                                        MObjectProvider* aParent,
		                                const TRect& aRect, CAknView& aView );


        /**
        * Destructor.
        */
        ~CLmkCategorySettingsContainer();

    private:
        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aRect Frame rectangle for container.
        */
        void ConstructL( const TRect& aRect );

        /**
        * C++ default constructor.
        * @param aKeyProcessor forward key events
        * @return newly instantiated object
        */
        //CLmkCategorySettingsContainer( MLmkKeyProcessor& aKeyProcessor );

	// For landscape SUpport

	 CLmkCategorySettingsContainer( MLmkKeyProcessor& aKeyProcessor , CAknView& aView);


    public: // new functions
        /**
        * Return reference
        * @return CLmkAppCategorySettingsImpl reference
        */
        CLmkAppCategorySettingsImpl& SelectorImpl();

        /**
        * Return listbox reference
        * @return CEikTextListBox reference
        */
        CEikTextListBox& ListBox();

    public: // From CCoeControl
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

    #ifdef RD_SCALABLE_UI_V2
    	void HandlePointerEventL( const TPointerEvent& aPointerEvent );
    #endif // RD_SCALABLE_UI_V2

    private: // From CCoeControl
        void SizeChanged();
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl( TInt aIndex ) const;
        void FocusChanged(TDrawNow aDrawNow);

    private:

    	//from MLmkMskObserver
    	void UpdateMskContainerL();

        //from CLmkBaseContainer
        TLmkMskDispItem ResolveMskDisplayItem();

	private:
	//For Landscape support

	void HandleResourceChange( TInt aType );



    private: //from CLmkBaseContainer
        CEikMenuBar* MenuBar();
        TInt ContextSpecificMenuBar();
        TInt OptionsMenuBar();

    #ifdef RD_SCALABLE_UI_V2
	protected://for touch support

    	void SetListBoxObserver( MEikListBoxObserver* aObserver );
    #endif//#ifdef RD_SCALABLE_UI_V2

    private: //data
        /// Ref.
        CEikTextListBox* iListBox;
        // For landscape support
        CAknView &iView;
        
        // To Store current Index
        TInt iCurrentIndex;
        
    };

#endif // __CLMKCATEGORYSETTINGSCONTAINER_H__

// End of File
