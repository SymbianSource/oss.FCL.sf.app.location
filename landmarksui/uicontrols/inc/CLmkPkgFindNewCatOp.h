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
* Description:    LandmarksUi Content File -    This class provides functionality of finding out new categories
*                that are received from a landmarks package, which do not exists in
*                the landmarks database.
*
*/








#ifndef CLMKPKGFINDNEWCATOP_H
#define CLMKPKGFINDNEWCATOP_H

//  INCLUDES
#include "MLmkAOOperationObserver.h"
#include <EPos_CPosLmOperation.h>
#include <EPos_Landmarks.h>
#include "CLmkParser.h"
#include "CLmkSaveLandmarkUtils.h"

// CLASS DECLARATION

/**
* CLmkPkgFindNewCatOp class.
* This class provides functionality of finding out new categories
* that are received from a landmarks package, which do not exists in
* the landmarks database.
*/
class CLmkPkgFindNewCatOp
: public CPosLmOperation
    {
    public:  // Constructors and destructor
        /**
        * Creates CLmkPkgFindNewCatOp
        * @param aNoOfNewCategories
        * @param aNewCategories
        * @param aNewCategoryFound
        * @param aSaveLmUtils
        * @return newly instantiated object
        */
        static CLmkPkgFindNewCatOp* NewL(
                            TInt& aNoOfNewCategories,
                            RArray<TCategoryName>& aNewCategories,
                            TCategoryName& aNewCategoryFound,
                            CLmkSaveLandmarkUtils*aSaveLmUtils ) ;

        /**
        * Destructor.
        */
        ~CLmkPkgFindNewCatOp();

    public: // From CPosLmOperation
        void NextStep( TRequestStatus& aStatus, TReal32& aProgress );
        void ExecuteL();

    protected: // From MLmkAOOperationObserver
        void HandleOperationL( TOperationTypes aType,
                              TReal32 aProgress,
                              TInt aStatus );

    protected:  // Constructors and destructor
        /**
        * C++ constructor.
        * @param aNoOfNewCategories
        * @param aNewCategories
        * @param aNewCategoryFound
        * @param aSaveLmUtils
        * @return newly instantiated object
        */
        CLmkPkgFindNewCatOp(TInt& aNoOfNewCategories,
                            RArray<TCategoryName>& aNewCategories,
                            TCategoryName& aNewCategoryFound,
                            CLmkSaveLandmarkUtils*aSaveLmUtils ) ;

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aLandmarkIdArray
        * @param aCategoryIdArray
        */
        void ConstructL();

    private:    // Data

        ///Ref:
        TRequestStatus* iStatus;

        ///Ref:
        TReal32* iProgress;

        //Ref
        CLmkSaveLandmarkUtils* iSaveLmUtils;


        TBool iIsFirstStep;

        //Ref
        TCategoryName& iNewCategoryFound;

        //Ref
        RArray<TCategoryName>& iNewCategories;

        //Ref
        TInt& iNoOfNewCategories;
    };

#endif      // CLmkPkgFindNewCatOp_H

// End of File
