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
* Description:    LandmarksUi Content File -    Landmarks ui item.
*
*/








#ifndef CLMKLANDMARKUIITEM_H
#define CLMKLANDMARKUIITEM_H

//  INCLUDES
#include "CLmkUiItemBase.h"

// FORWARD DECLARATIONS
class CPosLandmark;

// CLASS DECLARATION

/**
* CLmkLandmarkUiItem class
*/
class CLmkLandmarkUiItem : public CLmkUiItemBase
    {
    public:  // Constructors and destructor
        /**
        * Create CLmkLandmarkUiItem object
        * @param aLandmark
        * @param aIconListIndex
        * @return newly instantiated object
        */
        static CLmkLandmarkUiItem* NewL( CPosLandmark* aLandmark,
                                         TInt aIconListIndex );

        /**
        * Destructor.
        */
        ~CLmkLandmarkUiItem();

    public: // From CLmkUiItemBase
        TLmkItemType Type();
        TPtrC Text();
        TInt GetIconId();

    private:  // Constructors and destructor
        /**
        * C++ default constructor.
        * @param aLandmark
        * @param aIconListIndex
        */
        CLmkLandmarkUiItem( CPosLandmark* aLandmark, TInt aIconListIndex );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data
        ///Own:
        CPosLandmark* iLandmark;

        TInt iIconListIndex;

        TPtrC iText;
    };

#endif      // CLMKLANDMARKUIITEM_H

// End of File
