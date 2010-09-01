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
* Description:    LandmarksUi Content File -    Category ui item
*
*/








#ifndef CLMKCATEGORYUIITEM_H
#define CLMKCATEGORYUIITEM_H

//  INCLUDES
#include "CLmkUiItemBase.h"

// FORWARD DECLARATIONS
class CPosLandmarkCategory;

// CLASS DECLARATION

/**
* CLmkCategoryUiItem class
*/
class CLmkCategoryUiItem : public CLmkUiItemBase
    {
    public:  // Constructors and destructor

        /**
        * Create CLmkCategoryUiItem object
        * @param aCategory
        * @param aIconListIndex
        * @return newly instantiated object
        */
        static CLmkCategoryUiItem* NewL( CPosLandmarkCategory* aCategory,
                                         TInt aIconListIndex );

        /**
        * Destructor.
        */
        ~CLmkCategoryUiItem();

    public: // From CLmkUiItemBase
        TLmkItemType Type();
        TPtrC Text();
        TInt GetIconId();

    private:  // Constructors and destructor
        /**
        * C++ default constructor.
        * @param aCategory
        * @param aIconListIndex
        * @return newly instantiated object
        */
        CLmkCategoryUiItem( CPosLandmarkCategory* aCategory,
                            TInt aIconListIndex );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        ///Own:
        CPosLandmarkCategory* iCategory;

        TInt iIconListIndex;

        TPtrC iText;
    };

#endif      // CLMKCATEGORYUIITEM_H

// End of File
