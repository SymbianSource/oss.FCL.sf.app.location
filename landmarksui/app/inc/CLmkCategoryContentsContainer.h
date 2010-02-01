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
* Description:    LandmarksUi Content File -    LM application category content view's container class.
*
*/


#ifndef CLMKCATEGORYCONTENTSCONTAINER_H
#define CLMKCATEGORYCONTENTSCONTAINER_H

// INCLUDES
#include <coecobs.h>
#include "CLmkLbWithFilterContainer.h"

// FORWARD DECLARATIONS

class CAknView; // For landscape support

class CEikTextListBox;
class CLmkAppLmSelectorImpl;
class CAknNavigationDecorator;
class CPosLandmark;

// CLASS DECLARATION
/**
*  CLmkCategoryContentsContainer  container control class.
*
*/
class CLmkCategoryContentsContainer : public CLmkLbWithFilterContainer, public MCoeControlObserver
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

		static CLmkCategoryContentsContainer* NewL(
                                        MLmkKeyProcessor& aKeyProcessor,
                                        MObjectProvider* aParent,
		                                const TRect& aRect, CAknView& aView);
        /**
        * Destructor.
        */
        ~CLmkCategoryContentsContainer();

    public: // from CCoeControl
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );
        // For Landscape Support
		void  HandleResourceChange( TInt aType );
		
	public: // New functions
		TInt GetSelectedLandmarksL(RPointerArray<CPosLandmark> &aArray);
		TBool IsLandmarkDataEmptyL(CPosLandmark* aLandmark);
	    TBool IsEditorOpened();	      
		void SetEditorOpenedBool(TBool aBoolvalue);

    public: // From MCoeControlObserver

        /**
        * HandleControlEventL handles an event from an observed control.
        *
        * @param aControl the control that reported the event
        * @param aEventType contains info about the event
        */
       void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);

   public://for MSK
   		/**
        * This function, removes opened category from selected landmarks.
        */
		void RemoveFromThisCategoryL();
		/**
        * This function checks, if the category opened is 'Uncategorized' .
        * @return ETrue if opened category is 'Uncategorized' else Efalse.
        */
		TBool IsUncategorizedCategory();

    protected: // From CLmkLbWithFilterContainer
        void SetupTitlePaneL();
        void SetupSelectorAndListL( CPosLandmarkDatabase& aDb,
                                    CLmkSender& aSender );
        void SetupNaviPaneL();
        
    public ://from CLmkBaseContainer
    	TLmkMskDispItem ResolveMskDisplayItem();
    	void UpdateMskContainerForFilterL();
    	
    private:
        /**
        * C++ default constructor.
        * @param aKeyProcessor
        * @return newly instantiated object
        */
		CLmkCategoryContentsContainer( MLmkKeyProcessor& aKeyProcessor , CAknView& aView);
		void FocusChanged(TDrawNow aDrawNow);

        /**
        * Symbian 2nd phase constructor
        * @param aRect
        */
        void ConstructL( const TRect& aRect );

	private://from MLmkMskObserver

    	void UpdateMskContainerL();

	private:	// Data
		CAknView& iView;
		TBool iIsEditorOpened;
		TBool iIsRemoveOperation;
    };

#endif // CLMKCATEGORYCONTENTSCONTAINER_H

// End of File
