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
* Description:    LandmarksUi Content File -    This class provides list items belonging to received
*                landmark package.
*
*/








#ifndef CLMKPARSEDLMLISTPROVIDER_H
#define CLMKPARSEDLMLISTPROVIDER_H

//  INCLUDES
#include "CLmkListProviderBase.h"
#include <EPos_Landmarks.h>

// FORWARD DECLARATIONS
class CLmkParser;

// CLASS DECLARATION

/**
*  This class provides list items belonging to received landmark package.
*  Landmark package parsing should be already completed when instantiating
*  the list provider.
*/

class CLmkParsedLmListProvider : public CLmkListProviderBase
    {
    public:  // Constructors and destructor
        /**
        * Creates a provider for items
        * @param aParser
        * @return newly instantiated object
        */
        static CLmkParsedLmListProvider* NewL( CLmkParser& aParser );

        /**
        * Destructor.
        */
        ~CLmkParsedLmListProvider();

    public: // From CLmkListProviderBase
        /**
        * In this class this method does nothing.
        */
        void InitializeL();
        TInt ItemCount() const;
        CLmkUiItemBase* ItemAtL( TInt aIndex );
        TBool StartSearchingLandmarksL(
								const TDesC& aSearchPattern,
    							TBool aSearchOnlyInPreviousMatches);

    private:  // Constructors and destructor
        /**
        * C++ constructor.
        * @param aParser
        * @return newly instantiated object
        */
        CLmkParsedLmListProvider( CLmkParser& aParser );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        ///Ref:
        CLmkParser& iParser;

    };

#endif      // CLMKPARSEDLMLISTPROVIDER_H

// End of File
