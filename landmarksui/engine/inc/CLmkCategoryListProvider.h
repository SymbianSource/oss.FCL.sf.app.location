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
* Description:    LandmarksUi Content File -    Category list provider
*
*/








#ifndef CLMKCATEGORYLISTPROVIDER_H
#define CLMKCATEGORYLISTPROVIDER_H

//  INCLUDES
#include "CLmkLmItemListProvider.h"
#include "LmkListProviderFactory.h"

// FORWARD DECLARATIONS
class CPosLmCategoryManager;
class CPosLandmarkCategory;

/********************************/
class CPosLandmarkSearch;
class CPosLmOperation;
class CPosLmCategoryCriteria;
/********************************/

// CLASS DECLARATION



/**
* CLmkCategoryListProvider class
*/
class CLmkCategoryListProvider : public CLmkLmItemListProvider
    {
    public:  // Constructors and destructor
        /**
        * Creates a provider for all items in the database.
        * @param aDb landmark database reference
        * @param aShowType determines whether non-referenced
        *        categories are considered or not
        * @return newly instantiated object
        */
        static CLmkCategoryListProvider* NewL( CPosLandmarkDatabase& aDb,
                                               TLmkProviderType aShowType );

        /**
        * Destructor.
        */
        ~CLmkCategoryListProvider();

    public: // From CLmkListProviderBase
        CLmkUiItemBase* ItemAtL( TInt aIndex );

    public: // From MLmkDbObserver
        void HandleDatabaseEvent( TPosLmEvent& aEvent );

    protected: // From CLmkLmItemListProvider
        void PrepareListL();
        TBool PrepareListL(const TDesC& aSearchPattern,
    							TBool aSearchOnlyInPreviousMatches);
        void RemoveLandmarksL(const RArray<TPosLmItemId>& aSelectedItems ) ;

    private:  // Constructors and destructor
        /**
        * C++ constructor.
        * @param aDb
        * @param aShowType
        * @return newly instantiated object
        */
        CLmkCategoryListProvider( CPosLandmarkDatabase& aDb,
                                  TLmkProviderType aShowType );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: //new functions
        /**
        * Make category usage list
        */
        void PrepareCategoryUsageListL();

        /**
         * Check is category used or not
         * @param aCategory, list item category
         * @return TBool
         */
        TBool IsUsedL( CPosLandmarkCategory& aCategory );


/******************************************************/
     protected: // From MLmkAOOperationObserver
        void HandleOperationL( TOperationTypes aType,
                              TReal32 aProgress,
                              TInt aStatus );


	/**
        * Read items to array
        */
        void ReadItemsToArrayL();
/******************************************************/



    private:    // Data
        ///Own:
        CPosLmCategoryManager* iCategoryMgr;

        TLmkProviderType iShowType;

        // category usage id list
        RArray<TPosLmItemId> iUsageArray;

        // index
        TInt iUsageIndex;

		// are any uncategorised items in database
		TBool iAreUncategorisedItems;

/************************************************/
  ///Own:
        CPosLandmarkSearch* iSearch;

        ///Own:
        CLmkAOOperation* iSearchAO;

        CPosLmCategoryCriteria* iCriteria;

/************************************************/


    };

#endif      // CLMKCATEGORYLISTPROVIDER_H

// End of File
