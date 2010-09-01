/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -    Common UI's (dialog-based) landmark selector impl class.
*
*/







#ifndef CLMKDLGCOMBISELECTORIMPL_H
#define CLMKDLGCOMBISELECTORIMPL_H

//  INCLUDES
#include "CLmkDlgSelectorImplBase.h"
#include "MLmkListProviderObserver.h"
#include "MLmkListMemento.h"
#include <eikcmobs.h>       // MEikCommandObserver
#include <AknTabObserver.h>
// FORWARD DECLARATIONS
class CLmkSelectorDialog;
class CAknNavigationControlContainer;
class CAknTabGroup;
class CAknNavigationDecorator;

// CLASS DECLARATION

/**
*  Common UI's (dialog-based) landmark selector impl class.
*  This selector contains both landmark by name & by category tabs
*  and it utilizes landmark and category selectors to do that.
*
*/
class CLmkDlgCombiSelectorImpl : public CLmkDlgSelectorImplBase, public MAknTabObserver
    {
    private:    // Type definitions

        enum TLmkSelectorState
            {
            ELmkLandmarkSelector = 0,   // used as index in iSelectors
            ELmkCategorySelector,       // used as index in iSelectors
            ELmkFilteredLmSelector,     // used as index in iSelectors
            ELmkInitialState,
            ELmkFinalState,
            };

        enum TLmkSelectorStateEvent
            {
            ELmkSelectorStart = 0,
            ELmkSelectorStop,
            ELmkSelectorNavigating,
            ELmkSelectorAbort,
            ELmkSelectorNoEvent
            };

    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aDb database reference
        * @return newly instantiated object
        */
        IMPORT_C static CLmkDlgCombiSelectorImpl* NewL(
                                    CPosLandmarkDatabase& aDb );

        /**
        * Destructor.
        */
        ~CLmkDlgCombiSelectorImpl();

    public: // From CLmkSelectorImplBase
        void StopExecutionL();
        TInt ExecuteL( TPosLmItemId& aOpenedItem,
                       TBool aAlwaysModifyOpened = EFalse );
        TInt ExecuteL( RArray<TPosLmItemId>& aSelectedItems,
                       TBool aAlwaysModifySelected = EFalse );

        void SetupListProviderL();
        const CLmkListProviderBase& ListProvider() const;
        void GetDlgResources( TBool aIsSingleSelector,
                              TInt& aTitlePaneResource,
                              TInt& aMenuBarResource,
                              TInt& aDialogResource ) const;
        
        void SetDialogTitleL(const TDesC& aTitle);

    protected: // From CLmkDlgSelectorImplBase
        void StoreMemento();
        void RestoreMemento();

    protected: // From MEikCommandObserver
        void ProcessCommandL( TInt aCommandId );

    protected: // From MLmkNavigationObserver
        TKeyResponse HandleNavigationEventL( const TKeyEvent& aKeyEvent );
        void HandleLaunchingEventL();
        void HandleClosingEventL();

	protected: // From MAknTabObserver
		void TabChangedL( TInt aIndex );

    protected:  // New functions
        /**
        * Handle any state event.
        * @param aEvent
        * @return TLmkSelectorStateEvent
        */
        TLmkSelectorStateEvent HandleAnyStateEventL(
                                    TLmkSelectorStateEvent aEvent );

        /**
        * Handle initial state event.
        * @param aEvent
        * @return TLmkSelectorStateEvent
        */
        TLmkSelectorStateEvent HandleInitialStateEventL(
                                    TLmkSelectorStateEvent aEvent );

        /**
        * Handle landmark selector state event.
        * @param aEvent
        * @return TLmkSelectorStateEvent
        */
        TLmkSelectorStateEvent HandleLandmarkSelectorStateEventL(
                                    TLmkSelectorStateEvent aEvent );

        /**
        * Handle category selector state event.
        * @param aEvent
        * @return TLmkSelectorStateEvent
        */
        TLmkSelectorStateEvent HandleCategorySelectorStateEventL(
                                    TLmkSelectorStateEvent aEvent );

        /**
        * Handle filtered selector state event.
        * @param aEvent
        * @return TLmkSelectorStateEvent
        */
        TLmkSelectorStateEvent HandleFilteredLmSelectorStateEventL(
                                    TLmkSelectorStateEvent aEvent );

        /**
        * Handle default state state evelt.
        * @param aEvent
        * @return TLmkSelectorStateEvent
        */
        TLmkSelectorStateEvent HandleDefaultStateStateEventL(
                                    TLmkSelectorStateEvent aEvent );

        /**
        * Helper method for getting selector corresponding to the
        * current state. If it doesn't exist yet it is created.
        * @param aCategoryId is only used when creating filtered lm
        *        selector which requires a category id for criteria
        */
        CLmkDlgSelectorImplBase& CurrentStateSelectorL(
                            TPosLmItemId aCategoryId = KPosLmNullItemId );

    protected:  // Constructors and destructor
        /**
        * C++ default constructor.
        * @param aDb
        * @return newly instantiated object
        */
        CLmkDlgCombiSelectorImpl( CPosLandmarkDatabase& aDb );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    private:

		/**
        * Add the individual tabs to tabgroup of NaviPane
        * @param aaTabGroup
        * @
        */
		void AddTabsL(CAknTabGroup& aTabGroup);

		/**
        * Fetch the name of the Bitmap file
        *
        * @return pointer to name of bitmap file containing tab icons
        */
		HBufC* GetBitmapFilenameLC();

    private:    // Data
        /// Current state of this class
        TLmkSelectorState iSelectorState;

        /// used when opening a category contents view
        TInt iCategoryIndex;

        /// dialog return value
        TInt iDlgRetVal;

        RPointerArray<CLmkDlgSelectorImplBase> iSelectors;

        RPointerArray<MLmkListMemento> iMementos;

        /// Is user currently navigating between tabs?
        TBool iNavigating;

        /// Is user interested in return values even if dialog was cancelled?
        TBool iAlwaysModify;

        ///Ref: used in multi selector mode
        RArray<TPosLmItemId>* iSelectedItems;

        ///Ref: used in single selector mode
        TPosLmItemId* iOpenedItem;

        ///Ref: navi pane reference
        CAknNavigationControlContainer* iNaviPane;

        ///Ref: tab group
        CAknTabGroup* iTabGroup;

        ///Own: navi decorator
        CAknNavigationDecorator* iNaviDeco;
        
        /// Own: custom title for the landmark selection dialog
        HBufC* iCustomLandmarksTitle;
    };

#endif      // CLMKDLGCOMBISELECTORIMPL_H

// End of File
