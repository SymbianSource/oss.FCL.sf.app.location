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
* Description:   	LandmarksUi Content File
*				This class provides functionality for adding multiple
*               landmarks to multiple categories in one asynchronous
*               operation.
*
*/








#ifndef CLMKADDTOCATOPERATION_H
#define CLMKADDTOCATOPERATION_H

//  INCLUDES
#include "MLmkAOOperationObserver.h"
#include <EPos_CPosLmOperation.h>
#include <EPos_Landmarks.h>

// FORWARD DECLARATIONS
class CPosLmCategoryManager;

// CLASS DECLARATION

/**
* CLmkAddToCatOperation class.
* This class provides functionality for adding multiple landmarks to
* multiple categories in one asynchronous operation. It is done by chaining
* multiple CPosLmCategoryManager::AddCategoryToLandmarksL() operations.
*/
class CLmkAddToCatOperation
: public CPosLmOperation,
  public MLmkAOOperationObserver
    {
    public:  // Constructors and destructor
        /**
        * Creates CLmkAddToCatOperation
        * @param aCategoryMgr
        * @param aLandmarkIdArray
        * @param aCategoryIdArray
        * @return newly instantiated object
        */
        static CLmkAddToCatOperation* NewL(
                            CPosLmCategoryManager& aCategoryMgr,
                            const RArray<TPosLmItemId>& aLandmarkIdArray,
                            const RArray<TPosLmItemId>& aCategoryIdArray );

        /**
        * Creates CLmkAddToCatOperation
        * @param aCategoryMgr
        * @param aLandmarkId
        * @param aCategoryIdArray
        * @return newly instantiated object
        */
        static CLmkAddToCatOperation* NewL(
                            CPosLmCategoryManager& aCategoryMgr,
                            TPosLmItemId aLandmarkId,
                            const RArray<TPosLmItemId>& aCategoryIdArray );

        /**
        * Destructor.
        */
        ~CLmkAddToCatOperation();

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
        * @param aCategoryMgr
        * @return newly instantiated object
        */
        CLmkAddToCatOperation( CPosLmCategoryManager& aCategoryMgr );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aLandmarkIdArray
        * @param aCategoryIdArray
        */
        void ConstructL( const RArray<TPosLmItemId>& aLandmarkIdArray,
                         const RArray<TPosLmItemId>& aCategoryIdArray );

    private: //new functions
        /**
        * Internal helper method which performs one
        * CPosLmCategoryManager::AddCategoryToLandmarksL() operation.
        * @param aCategoryId
        */
        void DoAddToCatStepL( TPosLmItemId aCategoryId );

    private:    // Data
        RArray<TPosLmItemId> iLmIdArray;

        RArray<TPosLmItemId> iCatIdArray;

        CPosLmCategoryManager& iCategoryMgr;

        TInt iCurrentIndex;

        ///Ref:
        TRequestStatus* iStatus;

        ///Ref:
        TReal32* iProgress;

        ///Own:
        CLmkAOOperation* iAddOperation;
    };

#endif      // CLMKADDTOCATOPERATION_H

// End of File
