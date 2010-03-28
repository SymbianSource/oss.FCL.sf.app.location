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
* Description:    LandmarksUi Content File -
*				Base class for application's (view-based) selectors
*
*/







#ifndef CLMKAPPLMITEMSELECTORIMPL_H
#define CLMKAPPLMITEMSELECTORIMPL_H

//  INCLUDES
#include "CLmkAppSelectorImplBase.h"
#include "MLmkAOOperationObserver.h"
#include "MLmkLocationObserver.h"
#include "lmkwaitdlglauncher.h"
#include <CLmkEditorDlg.h>

// FORWARD DECLARATIONS
class CLmkLmItemListProvider;
class CLmkDbUtils;
class CLmkLocationService;
class TPosition;
class CPosLandmark;
class CLmkSender;
class CLmkLocSettingsLauncher;


// CLASS DECLARATION

/**
*  Base class for application's (view-based) selectors that list items
*  having TPosLmItemId, i.e. using provider of type CLmkLmItemListProvider.
*/
class CLmkAppLmItemSelectorImpl
: public CLmkAppSelectorImplBase,
  public MLmkAOOperationObserver,
  public MLmkLocationObserver,
  public MLmkWaitDialogLauncherObserver
    {
    public:  // Constructors and destructor
        /**
        * Destructor.
        */
        ~CLmkAppLmItemSelectorImpl();

    public: // New functions
        /**
        * Method for changing an icon of a listbox item. Calls templated
        * method which should handle subclass-specific details.
        */
        void ChangeIconCmdL();
        // Added for Bug Fix
        void DiskSpaceBelowCriticalLevelL(TInt aBytesToWrite, TChar aDrive);

		TInt CalculateLandmarkDisksize(const CPosLandmark& aLandmark);

    public: // From CLmkAppSelectorImplBase
        MLmkListMemento* MementoL();
		/**
		* Method to handle change in screen size.
		* When the icon selection popup is shown screen size changes have to
		* be propagated to the popup.
		*/
		void HandleScreenSizeChange();

		TInt CurrentCategoryId();

    public: //from MLmkMapAndNavigationObserver
        void HandleLmCreated(TPosLmItemId aNewLmkItemId);

    public://from MLmkWaitDialogLauncherObserver
    	void HandleDialogDismissed( TInt aButtonId );

    protected: // From MEikCommandObserver
        void ProcessCommandL( TInt aCommandId );

    protected: // From MLmkLocationObserver
        void NotifyL( const TInt aStatus );
        void NotifyErrorL( TInt aErrorCode );

    protected: // New functions
        /**
        * Returns provider reference as CLmkLmItemListProvider type.
        * @return provider reference
        */
        CLmkLmItemListProvider& LmItemListProvider();

        /**
        * Currently selected item's id
        * @return TPosLmItemId
        */
        TPosLmItemId SelectedIdL();

        /**
        * Currently marked item ids
        * @return TPosLmItemId array reference
        */
        RArray<TPosLmItemId>& MarkedItemIdsL();

        /**
        * Template method called by ChangeIconCmdL.
        * @param aId
        * @param aIconIndex
        */
        virtual void ChangeIconL( TPosLmItemId aId, TInt aIconIndex, TInt aMaskIndex) = 0;

        /**
        * Template method called by ChangeIconCmdL.
        * @param aIdArray
        * @param aIconIndex
        */
        virtual void ChangeIconsL( RArray<TPosLmItemId>& aIdArray,
                                   TInt aIconIndex, TInt aMaskIndex) = 0;

		IMPORT_C void LaunchPosSettingsL();
        /**
        * Initial landmark
        * @param aLandmark
        */
        virtual void InitialLandmarkL( CPosLandmark* /*aLandmark*/ ){}

        /**
        * Method to change the mark index or indexes after adding
        * new Landmark.
        * @param None
        */
        void UpdateMarkIndexAfterAdditionL( TInt aIndexToBeSelected );
        //resolves id, to be focussed after deletion.
        void StoreIdTobeFocussedAfterDeletion( RArray<TPosLmItemId>& markedItemIds );
        //sets item to focus after deletion
        void SetFocusToItemAfterDeletionOfMarkedItems();

    protected: // Constructors and destructor
        /**
        * C++ default constructor.
        */
        CLmkAppLmItemSelectorImpl( CPosLandmarkDatabase& aDb,
                                   CLmkSender& aSender,
                                   TBool aFindBox );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void BaseConstructL();

    private: //new functions
        /**
        * Make new landmark
        */
        void NewLandmarkCmdL();

        /**
        * Executes landmark editor with correct parameters for
        * new landmark creation.
        * @param aLandmark empty or initialized landmark for editing
        */
        void ExecuteNewLandmarkEditorL( CPosLandmark& aLandmark );
        /**
        * Request current location
        */
        void RequestCurrenLocationL();

        /**
        * Create new landmark from current location
        * @param aPosition
        */
        void CreateLmkFromCurrentLocationL( const TPosition& aPosition );

        static TInt MessageQueryCallBack( TAny* aPtr );

    protected: // Data
        ///Ref:
        CLmkSender& iSender;

        ///Own:
        CLmkDbUtils* iDbUtils;

        ///Own:
        CLmkLocationService* iLocationService;

        RArray<TPosLmItemId> iMarkedItemIds;

        // If a new Item is added then the highlight will have to be
        // adjusted to point to the currently added Item.
        // For new Landmarks This flag will be set in this class
        // For new Categories this flag will be set in the derived class
        // And the flag is used in the respective derived classes.
        TBool iNewListBoxItemAdded;

        // Own: Pointer to Location Settings Launcher
        CLmkLocSettingsLauncher* iLauncher;

	    //this holds the landmark view dialog
	    CLmkEditorImpl* iDlg;

        /**
         * Wait dialog launcher
         * Owns
         */
        CLmkWaitDialogLauncher* iWaitDialogLauncher;
        //item to focus after deletion
        TPosLmItemId iFocusItemIdAfterDeletion;
    };

#endif      // CLMKAPPLMITEMSELECTORIMPL_H

// End of File
