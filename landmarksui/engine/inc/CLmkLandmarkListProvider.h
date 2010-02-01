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
* Description:    LandmarksUi Content File -    Landmark list provider.
*
*/








#ifndef CLMKLANDMARKLISTPROVIDER_H
#define CLMKLANDMARKLISTPROVIDER_H

//  INCLUDES
#include "CLmkLmItemListProvider.h"

// FORWARD DECLARATIONS
class CPosLmPartialReadParameters;
class CPosLandmarkSearch;
class CPosLmOperation;

// CLASS DECLARATION

/**
* CLmkLandmarkListProvider class
*/
class CLmkLandmarkListProvider : public CLmkLmItemListProvider
    {
    public:  // Constructors and destructor
        /**
        * Creates a provider for all items in the database.
        * @param aDb landmark database reference
        * @return newly instantiated object
        */
        static CLmkLandmarkListProvider* NewL( CPosLandmarkDatabase& aDb );

        /**
        * Destructor.
        */
        ~CLmkLandmarkListProvider();

    public: // From CLmkListProviderBase
        CLmkUiItemBase* ItemAtL( TInt aIndex );

    public: // From MLmkDbObserver
        void HandleDatabaseEvent( TPosLmEvent& aEvent );

    protected: // From CLmkLmItemListProvider
        void PrepareListL();
        TBool PrepareListL(const TDesC& aSearchPattern,
    							TBool aSearchOnlyInPreviousMatches);
        void RemoveLandmarksL(const RArray<TPosLmItemId>& aSelectedItems ) ;

    protected: // From MLmkAOOperationObserver
        void HandleOperationL( TOperationTypes aType,
                              TReal32 aProgress,
                              TInt aStatus );

    protected: // New methods
        /**
        * Read items to array
        */
        void ReadItemsToArrayL();

        /**
        * Prepare items for deletion
        */
		void PrepareForDeleteL() ;

    private:  // Constructors and destructor
        /**
        * C++ constructor.
        * @param aDb
        * @return newly instantiated object
        */
        CLmkLandmarkListProvider( CPosLandmarkDatabase& aDb );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        ///Own:
        CPosLmPartialReadParameters* iReadParams;

        ///Own:
        CPosLandmarkSearch* iSearch;

        ///Own:
        CLmkAOOperation* iSearchAO;
    };

#endif      // CLMKLANDMARKLISTPROVIDER_H

// End of File
