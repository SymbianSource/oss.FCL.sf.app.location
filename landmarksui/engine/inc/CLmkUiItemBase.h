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
* Description:    LandmarksUi Content File -    Base class for landmark ui item
*
*/








#ifndef CLMKUIITEMBASE_H
#define CLMKUIITEMBASE_H

//  INCLUDES
#include "landmarks.hrh"    // TLmkItemType
#include <e32base.h>        // CBase

// CLASS DECLARATION

/**
* CLmkUiItemBase class
*/
class CLmkUiItemBase : public CBase
    {
    public:  // Constructors and destructor
        virtual ~CLmkUiItemBase();

    public: // New functions
        /**
        * Get Landmark's item type
        * @return TLmkItemType
        */
        virtual TLmkItemType Type() = 0;

        /**
        * Get text
        * @return TPtrC
        */
        virtual TPtrC Text() = 0;

        /**
        * Get icon id
        * @return TInt
        */
        virtual TInt GetIconId() = 0;

        /**
        * Check if extension int exists
        * @return TBool
        */
        IMPORT_C TBool ExtensionIntExists();

        /**
        * Set Extension index
        * @param aInt
        */
        IMPORT_C void SetExtensionInt( TInt aInt );

        /**
        * Get extension int
        * @return TInt
        */
        IMPORT_C TInt ExtensionInt();

    protected:  // Constructors and destructor
        /**
        * Default constructor
        * @return newly instantiated object
        */
        CLmkUiItemBase();

    protected: // Data
        TBool iExtExists;
        TInt iExtInt;

    };

#endif      // CLMKUIITEMBASE_H

// End of File
