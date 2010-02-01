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
* Description:    LandmarksUi Content File -    CLmkParseAllWrapper class parses all landmarks and
*                maintains an index.
*
*/








#ifndef CLMKPARSEALLWRAPPER_H
#define CLMKPARSEALLWRAPPER_H

//  INCLUDES
#include <e32std.h>
#include "MLmkAOOperationObserver.h"
#include "CLmkLandMarkCategoriesName.h"
#include "CLmkLandmark.h"

// FORWARD DECLARATIONS
class CLmkAOOperation;
class CPosLandmarkParser;
class CPosLandmark;

class CPosLandmarkDatabase;
// CLASS DECLARATION

/**
* CLmkParseAllWrapper class.
* This class parses all landmarks and maintains an index.
* It becomes obsolete once CPosLandmarkParser supports indexed parsing.
*/
class CLmkParseAllWrapper
: public CBase, public MLmkAOOperationObserver
    {
    public:  // Constructors and destructor
        /**
        * Creates CLmkParseAllWrapper
        * @param aParser
        * @param aObserver
        * @return newly instantiated object
        */

        static CLmkParseAllWrapper* NewL( CPosLandmarkParser& aParser,
                                          MLmkAOOperationObserver& aObserver,
                                          CPosLandmarkDatabase& aDb );

        /**
        * Destructor.
        */
        ~CLmkParseAllWrapper();

    public: // New functions

        /**
        * Asynchronous method for parsing all landmarks.
        */
        void ParseAllL();

        /**
        * Returns parsed landmark, ownership is transferred.
        * @param aLandmarkIndex
        * @return landmark
        */
        CPosLandmark* LandmarkLC( TUint aLandmarkIndex );

		TInt NumOfLandmarks();
        CPosLandmark* Landmark( TUint aLandmarkIndex );
        CLmkLandMarkCategoriesName* CategoryNameLC( TUint aLandmarkIndex );

        /**
        * Gets the CLmkLandmark object
        * @param aLandmarkIndex
        */
        CLmkLandmark* LmkLandmarkLC( TInt aLandmarkIndex );
    protected: // From MLmkAOOperationObserver
        void HandleOperationL( TOperationTypes aType,
                              TReal32 aProgress,
                              TInt aStatus );

    protected:  // Constructors and destructor
        /**
        * C++ constructor.
        * @param aParser
        * @param aObserver
        * @return newly instantiated object
        */
        CLmkParseAllWrapper( CPosLandmarkParser& aParser,
                             MLmkAOOperationObserver& aObserver,
                             CPosLandmarkDatabase& aDb);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
    private:    // Data

        // Parsed landmarks
        RPointerArray<CLmkLandmark> iLmkLandmarks;
        RPointerArray<CLmkLandMarkCategoriesName> iCategoriesNames;

        ///Ref:
        CPosLandmarkParser& iParser;

        MLmkAOOperationObserver& iObserver;

        ///Own:
        CLmkAOOperation* iAOOperation;
        //Ref
         CPosLandmarkDatabase& iDb;
    };

#endif      // CLMKPARSEALLWRAPPER_H

// End of File
