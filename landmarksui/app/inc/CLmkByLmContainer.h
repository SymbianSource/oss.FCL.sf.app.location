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
* Description:    LandmarksUi Content File -    LM application ByLm view's container.
*
*/








#ifndef CLMKBYLMCONTAINER_H
#define CLMKBYLMCONTAINER_H

// INCLUDES
#include <EPos_CPosLandmark.h>
#include <coecobs.h>
#include "CLmkLbWithFilterContainer.h"



// FORWARD DECLARATIONS
class CAknView;

// FORWARD DECLARATIONS
class CEikTextListBox;
class CLmkAppLmSelectorImpl;

// CLASS DECLARATION
/**
*  CLmkByLmContainer  container control class.
*
*/
class CLmkByLmContainer : public CLmkLbWithFilterContainer, public MCoeControlObserver
    {
    public: // Constructors and destructor
        /**
        * Two-phased constructor
		* @param aKeyProcessor
		* @param aParent
		* @param aRect A rectangle for drawing.
		* @return The newly created object.
		*/

              // For Landscape Support

		static CLmkByLmContainer* NewL(MLmkKeyProcessor& aKeyProcessor,
												MObjectProvider* aParent,
												const TRect& aRect ,
												CAknView& aView);

        /**
        * Destructor.
        */
        ~CLmkByLmContainer();

    public: // from CCoeControl
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType );

      // For Landscape Support

      void HandleResourceChange(TInt aType);

    public: // New functions
      TInt GetSelectedLandmarksL(RPointerArray<CPosLandmark> &aArray);

      TBool IsLandmarkDataEmptyL(CPosLandmark* aLandmark);
      
      TBool IsEditorOpened();
      
      void SetEditorOpenedBool(TBool aBoolvalue);

      void SetHandlePointerEventBool( TBool aBoolValue);

    protected: // From CLmkLbWithFilterContainer
        void SetupTitlePaneL();
        void SetupSelectorAndListL( CPosLandmarkDatabase& aDb,
                                    CLmkSender& aSender );
        void SetupNaviPaneL();

    public: // From MCoeControlObserver

        /**
        * HandleControlEventL handles an event from an observed control.
        *
        * @param aControl the control that reported the event
        * @param aEventType contains info about the event
        */
       void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);

    private:
        /**
        * C++ default constructor.
        * @param aKeyProcessor
        * @return newly instantiated object
        */
		// Changed for Landscape support
		CLmkByLmContainer( MLmkKeyProcessor& aKeyProcessor, CAknView& aView );

    	// from CCoeControl
    	void FocusChanged(TDrawNow aDrawNow);

        /**
        * Symbian 2nd phase constructor
        * @param aRect
        */
        void ConstructL( const TRect& aRect );

    private://from MLmkMskObserver
    	void UpdateMskContainerL();

	private: //from CLmkBaseContainer
        CEikMenuBar* MenuBar();
        TInt ContextSpecificMenuBar();

	#ifdef RD_SCALABLE_UI_V2
    public://for touch support
    	void HandlePointerEventL( const TPointerEvent& aPointerEvent );
    #endif//RD_SCALABLE_UI_V2

	private:	// Data
		CAknView &iView;
		TBool iIsEditorOpened;
		TBool iHandlePointerEvent;

    };

#endif // CLMKBYLMCONTAINER_H

// End of File
