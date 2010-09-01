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
* Description:  Blid application main view's container class definition.
*
*/



#ifndef BLIDSAVEDWAYPTSCONTAINER_H
#define BLIDSAVEDWAYPTSCONTAINER_H

// INCLUDES
#include "CBlidBaseContainer.h"
#include "CBlidBaseView.h"
#include "coecobs.h"


// FORWARD DECLARATIONS
class CAknSingleGraphicStyleListBox;
class CEikColumnListBox;
class MBlidLocation;
class MBlidSettings;
class MBlidRouter;
// CLASS DECLARATION

/**
*  CBlidMainContainer  container control class.
*
*/
class CBlidSavedWayptsContainer : public CBlidBaseContainer
    {
    public: // Constructors and destructor
        /**
         * Two-phased constructor
		 * @param aKeyProcessor
		 * @param aParent
		 * @param aRect A rectangle for drawing.
         * @param aLocation A pointer to Location model
         * @param aSettings A pointer to Settings model
		 * @return The newly created object.
		 */
		static CBlidSavedWayptsContainer* NewL( MKeyProcessor& aKeyProcessor,
                                        MObjectProvider* aParent,
		                                const TRect& aRect,
                                        MBlidLocation* aLocation,
                                        MBlidSettings* aSettings,
                                        MBlidRouter* aRouter,
                                        CBlidBaseView& aView);
        /**
         * Destructor.
         */
        ~CBlidSavedWayptsContainer();

    private:
        /**
         * By default Symbian 2nd phase constructor is private.
         * @param aRect Frame rectangle for container.
         * @param aLocation A pointer to Location model
         * @param aSettings A pointer to Settings model
         */
        void ConstructL( const TRect& aRect, 
                        MBlidLocation* aLocation,
                        MBlidSettings* aSettings,MBlidRouter* aRouter );

        /**
         * C++ default constructor.
         * @param aKeyProcessor
         */
        CBlidSavedWayptsContainer( MKeyProcessor& aKeyProcessor, CBlidBaseView& aView );

    public: // New functions
        /**
         * Update listbox
         */ 
        void UpdateL();
        
        #ifdef RD_SCALABLE_UI_V2 
       /**
        * Sets listbox observer.
        * @param aObserver observer object.
        */
        void SetListBoxObserver( MEikListBoxObserver* aObserver );
        #endif // RD_SCALABLE_UI_V2         

    public: // from CCoeControl
        TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                     TEventCode aType);
        void HandleMarkCommandL(TInt aCommand);
        TInt SelectedRow() const;
        void FocusChanged(TDrawNow aDrawNow);

    private: // From CCoeControl
        void SizeChanged();
        TInt CountComponentControls() const;
        CCoeControl* ComponentControl(TInt aIndex) const;
        void HandleResourceChange(TInt aType);
        void UpdateIconsL();
        
        void SetSelectedRow(TInt aIndex) const;
        void MakeEmptyTextListBoxL( TInt aResourceText );
        
    private: // New functions
        /**
         * Creates owned list box.
         * @param aLocation A pointer to Location model
         * @param aSettings a pointer to Settings model
         */
        void CreateListboxL( MBlidLocation* aLocation, 
                             MBlidSettings* aSettings,MBlidRouter* aRouter );
        
        
    private: //data
    
        /// Own: Decorator class for navigation pane controls.
        CAknNavigationDecorator* iDecoratedTabGroup;
        
        CBlidBaseView& iView;

    public:
            /// Own: List box.
        CAknSingleGraphicStyleListBox* iListBox;

        
    };

#endif

// End of File
