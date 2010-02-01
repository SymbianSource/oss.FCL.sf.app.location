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
* Description:    LandmarksUi Content File -    LMK application ByCategory view's container
*
*/








#ifndef CLMKBYCATEGORYCONTAINER_H
#define CLMKBYCATEGORYCONTAINER_H

// INCLUDES
#include "CLmkLbWithFilterContainer.h"



// FORWARD DECLARATIONS
// For landscape Support

class CAknView;

class CEikTextListBox;
class CLmkAppCategorySelectorImpl;
class CAknFilteredTextListBoxModel;

// CLASS DECLARATION
/**
*  CLmkByCategoryContainer  container control class.
*
*/
class CLmkByCategoryContainer : public CLmkLbWithFilterContainer
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor
		* @param aKeyProcessor
		* @param aParent
		* @param aRect A rectangle for drawing.
		* @return The newly created object.
		*/

        // For landscape Support

		static CLmkByCategoryContainer* NewL( MLmkKeyProcessor& aKeyProcessor,
                                              MObjectProvider* aParent,
		                                      const TRect& aRect,
		                                      CAknView &aView);

        /**
        * Destructor.
        */
        ~CLmkByCategoryContainer();

		// From CoeControl and for landscape support

		void HandleResourceChange( TInt aType );

    protected: // From CLmkLbWithFilterContainer
        void SetupTitlePaneL();
        void SetupSelectorAndListL( CPosLandmarkDatabase& aDb,
                                    CLmkSender& aSender );
        void SetupNaviPaneL();

    public: // New function
    	TInt GetCurrentCategoryId();

    private:
        /**
        * C++ default constructor.
        * @param aKeyProcessor
        * @return newly instantiated object
        */
		CLmkByCategoryContainer( MLmkKeyProcessor& aKeyProcessor, CAknView& aView );

		void FocusChanged(TDrawNow aDrawNow);

	private://from MLmkMskObserver

    	void UpdateMskContainerL();

   	private:// Data
		CAknView &iView;
    };

#endif // CLMKBYCATEGORYCONTAINER_H

// End of File
