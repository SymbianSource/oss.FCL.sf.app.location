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
* Description:    LandmarksUi Content File -    This class provides functionality of encoding landmarks
*                both saved and unsaved landmarks (In Landmarks DB).
*
*/








#ifndef CLMKENCODEUNSAVEDLANDMARKSOP_H
#define CLMKENCODEUNSAVEDLANDMARKSOP_H

//  INCLUDES
#include "MLmkAOOperationObserver.h"
#include <EPos_CPosLmOperation.h>
#include <EPos_Landmarks.h>
#include "CLmkParser.h"
#include <EPos_CPosLandmarkEncoder.h>
#include <EPos_CPosLandmarkDatabase.h>
#include "CLmkLandmark.h"


// CLASS DECLARATION

/**
* CLmkEncodeUnsavedLandmarksOp class.
* This class does landmarks encoding operations.
* Though there is already one operation's class which does this in the
* landmarks engine framework, but that class does the encoding for only those
* landmarks which existis in landmarks database, but this class is does not
* requires any landmarks database assistance, it independently encodes the
* landmarks to xml format.
*/
class CLmkEncodeUnsavedLandmarksOp
: public CPosLmOperation
    {
    public:  // Constructors and destructor
        /**
        * Creates CLmkEncodeUnsavedLandmarksOp
        * @param aLandmarkEncoder
        * @param aLandmarks
        * @param aTransferOptions
        * @return newly instantiated object
        */
        static CLmkEncodeUnsavedLandmarksOp* NewL(
                     CPosLandmarkEncoder&  aLandmarkEncoder,
                     const RPointerArray<CLmkLandmark>&  aLandmarks,
                     CPosLandmarkDatabase::TTransferOptions  aTransferOptions);
        /**
        * Destructor.
        */
        ~CLmkEncodeUnsavedLandmarksOp();

    public: // From CPosLmOperation
        void NextStep( TRequestStatus& aStatus, TReal32& aProgress );
        void ExecuteL();

    protected:  // Constructors and destructor
        /**
        * C++ constructor.
        */
        CLmkEncodeUnsavedLandmarksOp(
                      CPosLandmarkEncoder&  aLandmarkEncoder,
                      CPosLandmarkDatabase::TTransferOptions  aTransferOptions);

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aLandmarks
        */
        void ConstructL( const RPointerArray<CLmkLandmark>&  aLandmarks );

    private:    // Data

        ///Ref:
        TRequestStatus* iStatus;

        ///Ref:
        TReal32* iProgress;

        CPosLandmarkEncoder&  iEncoder;

        RPointerArray<CLmkLandmark> iLandmarks;

        CPosLandmarkDatabase::TTransferOptions iTransferOptions;

        TBool iCurrentIsLm;

        // Item list with categories to export.
        RArray<TPosLmItemId> iCategoryIdArray;

        // Item list with categories to export.
        RPointerArray<CPosLandmarkCategory> iCategories;

        // The current category handled in the id array.
        TInt iCurrentCategory;

        TInt iNrOfSubOperations;

        TInt iStatusFlag;

        TInt iCurrentLm;
    };

#endif      // CLMKENCODEUNSAVEDLANDMARKSOP_H

// End of File
