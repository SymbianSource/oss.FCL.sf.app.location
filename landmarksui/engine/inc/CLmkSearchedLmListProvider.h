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
*
*/









#ifndef CLMKSEARCHEDLMLISTPROVIDER_H
#define CLMKSEARCHEDLMLISTPROVIDER_H

//  INCLUDES
#include "CLmkLmItemListProvider.h"

// FORWARD DECLARATIONS
class CPosLmPartialReadParameters;
class CPosLmSearchCriteria;
class CPosLandmarkSearch;
class CPosLmOperation;

// CLASS DECLARATION

/**
* CLmkSearchedLmListProvider class
*/
class CLmkSearchedLmListProvider : public CLmkLmItemListProvider
    {
    public:  // Constructors and destructor

        /**
        * Creates a provider that considers all items in a database.
        * @param aDb landmark database reference
        * @param aCriteria reference to used search criteria
        * @return newly instantiated object
        */
        static CLmkSearchedLmListProvider* NewL( CPosLandmarkDatabase& aDb,
                                            CPosLmSearchCriteria& aCriteria );

        /**
        * Destructor.
        */
        ~CLmkSearchedLmListProvider();

    public: // From CLmkListProviderBase
        CLmkUiItemBase* ItemAtL( TInt aIndex );

    protected: // From MLmkAOOperationObserver
        void HandleOperationL( TOperationTypes aType,
                              TReal32 aProgress,
                              TInt aStatus );

    protected: // From MLmkDbObserver
        void HandleDatabaseEvent( TPosLmEvent& aEvent );

    protected: // From CLmkLmItemListProvider
        void PrepareListL();
        TBool PrepareListL(const TDesC& aSearchPattern,
    							TBool aSearchOnlyInPreviousMatches);
        void RemoveLandmarksL(const RArray<TPosLmItemId>& aSelectedItems ) ;

    protected: // New methods
        /**
        * Read items to array
        */
        void ReadItemsToArrayL();

        /**
        * * Prepare items for deletion
        * iType must be set before using this method
        */
		void PrepareForDeleteL();

        /**
        * * Starts the second search to seach the data with owned category
        */
		void StartSecondSearchL();

    private:  // Constructors and destructor
        /**
        * C++ default constructor.
        * @return newly instantiated object
        */
        CLmkSearchedLmListProvider();

        /**
        * C++ constructor.
        * @param aDb
        * @param aCriteria
        * @return newly instantiated object
        */
        CLmkSearchedLmListProvider( CPosLandmarkDatabase& aDb,
                                    CPosLmSearchCriteria& aCriteria );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        ///Ref:
        CPosLmSearchCriteria& iCriteria;

        ///Own:
        CPosLmPartialReadParameters* iReadParams;

        ///Own:
        CPosLandmarkSearch* iSearch;

        ///Own:
        CLmkAOOperation* iSearchAO;
        TBool iCatDelete;
        TBool iCatUpdate;

        TBool iIsSecondSearchStarted;
    };

#endif      // CLMKSEARCHEDLMLISTPROVIDER_H

// End of File
