/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*				Application's (view-based) landmark selector impl class
*
*/







#ifndef CLMKAPPLMSELECTORIMPL_H
#define CLMKAPPLMSELECTORIMPL_H

//  INCLUDES
#include "CLmkAppLmItemSelectorImpl.h"
#include "TLmkDeletionHelper.h"
#include <CLmkEditorDlg.h>
#include "CLmkAppSelectorImplBase.h"
#include "mlmkdbsearchutilsobserver.h"
#include "clmkcallcmd.h"

// FORWARD DECLARATIONS
class CPosLmCategoryCriteria;
class MLmkListMemento;
class CLmkDbSearchUtils;
class CLmkGoToURLCmd;

// CLASS DECLARATION

/**
*  Application's (view-based) landmark selector impl class.
*
*/
class CLmkAppLmSelectorImpl : public CLmkAppLmItemSelectorImpl, public MLmkDbSearchUtilsObserver
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aDb database reference
        * @param aSender landmark sender reference
        * @param aFindBox tells whether selector contains find box or not
        * @return newly instantiated object
        */
        IMPORT_C static CLmkAppLmSelectorImpl* NewL(
                                    CPosLandmarkDatabase& aDb,
                                    CLmkSender& aSender,
                                    TBool aFindBox );

        /**
        * Two-phased constructor.
        * Creates lm selector based on memento of category view's
        * current selection.
        * @param aDb database reference
        * @param aCategoryMemento category view state
        * @param aSender landmark sender reference
        * @param aFindBox tells whether selector contains find box or not
        * @return newly instantiated object
        */
        IMPORT_C static CLmkAppLmSelectorImpl* NewL(
                                    CPosLandmarkDatabase& aDb,
                                    const MLmkListMemento& aCategoryMemento,
                                    CLmkSender& aSender,
                                    TBool aFindBox );

        /**
        * Destructor.
        */
        ~CLmkAppLmSelectorImpl();
        /**
        * @return ETrue if opened category is 'Uncategorized' else EFalse.
        */
        IMPORT_C TBool IsCriteriaUnCategorizedCat();

    public: // New functions
        /**
        * Return category name, used when viewing category contents.
        * If selector was not created with valid category memento parameter
        * this method leaves with KErrNotSupported.
        * @return category name
        */
        IMPORT_C TPtrC CategoryNameL();

    public: // From CLmkSelectorImpl
        void SetupListProviderL();
        const CLmkListProviderBase& ListProvider() const;
    public: // From MLmkDbSearchUtilsObserver
    	void HandleCategorySearchNotifyL(RArray<TPosLmItemId> aArray);
    	void HandleLmkSearchNotifyL();

    public: //from MLmkMapAndNavigationObserver
        void HandleLmCreated(TPosLmItemId aNewLmkItemId);

    public: //from MLmkAOOperationObserver
        void HandleOperationL( TOperationTypes aType,
                              TReal32 aProgress,
                              TInt aStatus );

    public: // From MEikCommandObserver
        void ProcessCommandL( TInt aCommandId );

    protected: // From CLmkAppLmItemSelectorImpl
        void ChangeIconL( TPosLmItemId aId, TInt aIconIndex, TInt aMaskIndex);
        void ChangeIconsL( RArray<TPosLmItemId>& aIdArray, TInt aIconIndex, TInt aMaskIndex);
        void InitialLandmarkL( CPosLandmark* aLandmark );
        TBool HandleEventListReadyL();

    public:  // From CLmkAppLmItemSelectorImpl
        /**
        * Following 3 methods are Wrapper methods for caller's menu and interest
        * resources to AIW menu service handler
        */
        void AttachToAIWMenuL( TInt aMenuResourceId,
        					   TInt aInterestResourceId);

        void InitializeMenuPaneL( CEikMenuPane& aMenuPane,
                                  TInt aMenuResourceId);

        void ExecuteAIWCallCmdL( TInt aCommandId );
        void AttachInterestL ( TInt aInterestResourceId );
        //from CLmkAppSelectorImplBase
        TInt ServiceCmdByMenuCmd( TInt aMenuCmd );

        /**
        * Checks if atleast one landmark with this field exist in database
        */

        TBool IsLmWithThisFieldExistL( TLmkAppCmdId  aFieldId);

        virtual TInt GetSelectedLandmarksL(RPointerArray<CPosLandmark> &aArray);

		virtual TBool IsLandmarkDataEmptyL(CPosLandmark* aLandmark);

		/**
        * Removes opened category from the selected landmarks.
        */
		IMPORT_C void RemoveLandmarksFromCategoryL();

		/**
        * From CLmkListProviderBase
        * Asynchronous starting of Landmarks Search to prepare the list.
        */
		IMPORT_C void StartSearchingLandmarksL(const TDesC& aSearchPattern,
    								  TBool aSearchOnlyInPreviousMatches);

    protected: // Constructors and destructor
        /**
        * C++ default constructor.
        * @param aDb database reference
        * @param aFindBox
        * @return newly object
        */
        CLmkAppLmSelectorImpl( CPosLandmarkDatabase& aDb,
                               CLmkSender& aSender,
                               TBool aFindBox );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aCategoryMemento
        */
        void ConstructL( const MLmkListMemento& aCategoryMemento );

    private: // New functions
        /**
        * Edit landmark
        */
        void EditLandmarkCmdL(TLmkEditorMode aEditMode);

        /**
        * Delete landmark(s)
        */
        void DeleteLandmarksCmdL();

        /**
        * Add landmark(s) to category(s)
        */
        void AddToCatCmdL();

        /**
        * Gets the position field value for a selected Landmark
        */
        TInt GetPositionFieldForSelectedLmL(
	                        TPositionFieldId  aFieldId,
	                        HBufC& aFieldValue);
		/**
		* Closes wait note
		*/
		void CloseWaitNoteL();


    protected: // Data
        /// helps updating the list after deletion
        TLmkDeletionHelper iDeletionHelper;

        ///Own:
        CPosLmCategoryCriteria* iCriteria;

        ///Own:
        HBufC* iCategoryName;

        ///Own: pointer to db serach utils
        CLmkDbSearchUtils* iDbSearchUtils;

        /// Own: Wait dialog displayed while searching landmarks
        CAknWaitDialog* iWaitNote;
    private:     
    	// own
        CLmkCallCmd *iLmkCallCmd;	          
        // own
        TInt iMarkedCatCountAdded;        
        // own
        CLmkGoToURLCmd* iLmkGoToURLCmd;        
        // own
        CLmkAOOperation* iOperation;
        // own
        TInt  iCatRemoveFlag;				
		// to check dlg open or not                
        TBool iIsDlgOpen; 
    };

#endif      // CLMKAPPLMSELECTORIMPL_H

// End of File
