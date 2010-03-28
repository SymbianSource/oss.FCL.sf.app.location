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
* Description:    LandmarksUi Content File -    CLmkSaveLandmarkUtils contains methods in aiding saving landmarks
*              : from the received landmarks package via Landmarks messaging
*
*/







#ifndef CLMKSAVELANDMARKUTILS_H
#define CLMKSAVELANDMARKUTILS_H

#include <e32std.h>
#include <e32base.h>
#include <EPos_CPosLandmarkDatabase.h>
#include "CLmkLandMarkCategoriesName.h"
#include "CLmkAOOperation.h"

// FORWARD DECLARATIONS
class CLmkParser;
class CEikonEnv;


/**
*  CLmkSaveLandmarkUtils class.
* This class contains the methods for saving landmarks from
* received landmarks package via Landmarks messaging
* These methods are currently used by CLmkPackageEditor and
* CLmkPackageSelector
*/
class  CLmkSaveLandmarkUtils : public CBase, public MLmkAOOperationObserver
    {
    public:  // Constructors and destructor

        /**
        * Creates  CLmkSaveLandmarkUtils
        * @return newly instantiated object
        */
        IMPORT_C static  CLmkSaveLandmarkUtils* NewL(CPosLandmarkDatabase& aDb,CLmkParser& aParser);

        /**
        * Destructor.
        */
        virtual ~ CLmkSaveLandmarkUtils();

    protected:  // Constructors and destructor
        /**
        * C++ constructor.
        * @return newly instantiated object
        */
         CLmkSaveLandmarkUtils(CPosLandmarkDatabase& aDb,CLmkParser& aParser);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    public://from MLmkAOOperationObserver

        void HandleOperationL(TOperationTypes aType, TReal32 aProgress, TInt aStatus );

    public :

        /*
         * Check for new received categories
         */
        TInt CheckForNewCategoriesL(RArray<TUint> & aSelectedIndices,
                                    TDes& aNewFoundCategory,
                                    RArray<TCategoryName>& aNewcategories);
         /**
        * Checks the new received categories, this function is used by the
        * category search operation.
        * @param aNewFoundCategory
        * @param aNewcategories
        */
        TInt CheckForNewCategories2L(TDes& aNewFoundCategory,
                                     RArray<TCategoryName>& aNewcategories);
         /*
         * Display confirmation query
         */
        TInt CategoryAddConfirmationQueryL( TInt aAmount, const TDesC& aNewCategoryName );
         /*
         * Display confirmation note
         */
        void LmsSavedConfirmationNoteL(RArray<TUint> & aImportIndexes,
                                       TInt aEditedItemIndex = 0);

    public://New Functions
        /**
        * Sets the observer for searching new categories operations
        * from a landmarks pacakge.
        * @param aObserver
        */
        void SetObserver(MLmkAOOperationObserver* aObserver);
        /**
        * Cancels operations for searching new categories from the landmarks
        * pacakge
        */
        void CancelSearchNewCat();
        /**
        * Starts the search operations to find out new categories in landmarks
        * package
        * @param aNoOfNewCategories
        * @param aNewCategories
        * @param aNewCategoryFound
        */
        void StartSearchNewCatL(TInt& aNoOfNewCategories,
                                RArray<TCategoryName>& aNewCategories,
                                TCategoryName& aNewCategoryFound);
 	private :
 		void CheckCategoryInDatabaseL(RArray<TCategoryName>& aNewcategories,
                                      const TDesC& aCategoryTobeAdded);

        private:    // Data

        /// Reference to database
        CPosLandmarkDatabase& iDb;

        ///Ref: landmark parser
        CLmkParser& iParser;

        // Ref :Eikon Env
        CEikonEnv*      iEnv;
        //Ref
        MLmkAOOperationObserver* iObserver;

        //Owns
        CLmkAOOperation* iAOOperation;
    };

#endif      // CLMKSAVELANDMARKUTILS_H
