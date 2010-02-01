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
* Description:    LandmarksUi Content File -    Handle icon operations
*
*/








#ifndef CLMKICONOPERATION_H
#define CLMKICONOPERATION_H

//  INCLUDES
#include "landmarks.hrh"
#include <EPos_CPosLmOperation.h>
#include <EPos_Landmarks.h>

// FORWARD DECLARATIONS
class CPosLandmarkDatabase;
class CPosLmCategoryManager;

// CLASS DECLARATION

/**
* CLmkIconOperation class
*/
class CLmkIconOperation : public CPosLmOperation
    {
    public:  // Constructors and destructor
        /**
        * Create CLmkIconOpereation object
        * @param aDb
        * @param aIdArray
        * @param aItemsType
        * @param aMbmFileName
        * @param aIconIndex
        * @return newly instantiated object
        */
        static CLmkIconOperation* NewL( CPosLandmarkDatabase& aDb,
                            const RArray<TPosLmItemId>& aIdArray,
                            TLmkItemType aItemsType,
                            const TDesC& aMbmFileName,
                            TInt aIconIndex );

        /**
        * Destructor.
        */
        ~CLmkIconOperation();

    public: // New methods
        /**
        * Next step in AO operation
        * @param aStatus
        * @param aProgress
        */
        void NextStep( TRequestStatus& aStatus, TReal32& aProgress );

        /**
        * Execute AO operation
        */
        void ExecuteL();

    protected:  // Constructors and destructor
        /**
        * C++ constructor.
        * @param aDb
        * @param aItemsType
        * @param aIconIndex
        * @return newly instantiated object
        */
        CLmkIconOperation( CPosLandmarkDatabase& aDb,
                           TLmkItemType aItemsType,
                           TInt aIconIndex );

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aIdArray
        * @param aItemsType
        * @param aMbmFileName
        */
        void ConstructL( const RArray<TPosLmItemId>& aIdArray,
                         TLmkItemType aItemsType,
                         const TDesC& aMbmFileName );

    private: // From CPosLmOperation
        void DoLandmarkStepL( TPosLmItemId aId );
        void DoCategoryStepL( TPosLmItemId aId );

    private:    // Data
        ///Ref:
        CPosLandmarkDatabase& iDb;

        RArray<TPosLmItemId> iIdArray;

        TInt iCurrentIndex;

        // method pointer to DoLandmarkStepL or DoCategoryStepL
        void (CLmkIconOperation::*iStepMethodL)( TPosLmItemId aId );

        ///Own: icon file name
        HBufC* iMbmFileName;

        TInt iIconIndex ;

        ///Own:
        CPosLmCategoryManager* iCategoryMgr;
    };

#endif      // CLMKICONOPERATION_H

// End of File
