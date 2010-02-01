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
*				Application's (view-based) category settings impl class.
*
*/







#ifndef CLMKAPPCATEGORYSETTINGSIMPL_H
#define CLMKAPPCATEGORYSETTINGSIMPL_H

//  INCLUDES
#include "CLmkAppLmItemSelectorImpl.h"
#include "TLmkDeletionHelper.h"

// FORWARD DECLARATIONS
class CAknWaitDialog;

// CLASS DECLARATION

/**
*  Application's (view-based) category settings impl class.
*
*/
class CLmkAppCategorySettingsImpl : public CLmkAppLmItemSelectorImpl
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aDb database reference
        * @param aSender landmark sender reference
        * @param aFindBox
        * @return newly instantiated object
        */
        IMPORT_C static CLmkAppCategorySettingsImpl* NewL(
                                    CPosLandmarkDatabase& aDb,
                                    CLmkSender& aSender,
                                    TBool aFindBox );


        /**
        * Destructor.
        */
        ~CLmkAppCategorySettingsImpl();

    public: // From CLmkSelectorImpl
        void SetupListProviderL();
        const CLmkListProviderBase& ListProvider() const;

    public: // From MEikCommandObserver
        void ProcessCommandL( TInt aCommandId );


    public:// New Member functions
    	IMPORT_C TBool IsPredefinedCategoryL();

    public: //from MLmkMapAndNavigationObserver
        void HandleLmCreated(TPosLmItemId aNewLmkItemId);

    protected: //from MLmkAOOperationObserver
        void HandleOperationL( TOperationTypes aType,
                              TReal32 aProgress,
                              TInt aStatus );

    protected: // From CLmkAppLmItemSelectorImpl
        void ChangeIconL( TPosLmItemId aId, TInt aIconIndex, TInt aMaskIndex);
        void ChangeIconsL( RArray<TPosLmItemId>& aIdArray, TInt aIconIndex, TInt aMaskIndex);
        TBool HandleEventListReadyL();

    protected: // Constructors and destructor
        /**
        * C++ default constructor.
        * @param aDb database reference
        * @param aSender landmark sender reference
        * @param aFindBox
        */
        CLmkAppCategorySettingsImpl( CPosLandmarkDatabase& aDb,
                                     CLmkSender& aSender,
                                     TBool aFindBox );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // New functions
        /**
        * Make new category
        */
        void NewCategoryCmdL();

        /**
        * Rename category
        */
        void RenameCategoryCmdL();

        /**
        * Delete category
        */
        void DeleteCategoryCmdL();

    protected: // Data
        /// helps updating the list after deletion
        TLmkDeletionHelper iDeletionHelper;

        /// Own: iBuffer
        HBufC* iBuffer;

        ///Own: iWaiNote
        CAknWaitDialog* iWaitNote;
    };

#endif      // CLMKAPPCATEGORYSETTINGSIMPL_H

// End of File
