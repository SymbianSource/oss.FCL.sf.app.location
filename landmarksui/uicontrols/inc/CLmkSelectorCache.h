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
* Description:    LandmarksUi Content File -    Selector's cache class
*
*/








#ifndef CLMKSELECTORCACHE_H
#define CLMKSELECTORCACHE_H

//  INCLUDES
#include <e32base.h>        // CBase
#include "MLmkListProviderObserver.h"

// FORWARD DECLARATIONS
class CLmkListProviderBase;
class CLmkUiItemBase;


// CLASS DECLARATION
/**
* CLmkSelectorCache class
*/
class CLmkSelectorCache : public CBase, public MLmkListProviderObserver
    {
    private:
        class CLinkedItem: public CBase
            {
            public:
                CLinkedItem();
                ~CLinkedItem();
            public:
                TInt iIndex;
                CLmkUiItemBase* iPtr;
            };

    public:  // Constructors and destructor

        /**
        *
        *
        * @return newly instantiated object
        */
        static CLmkSelectorCache* NewL( CLmkListProviderBase& aListProvider );

        /**
        * Destructor.
        */
        ~CLmkSelectorCache();

    public: // New functions

        CLmkUiItemBase& ItemAtL( TInt aIndex );

        TInt ItemCount() const;

    public: // From MLmkListProviderObserver

        void HandleListProviderEvent( TLmkListProviderEventType aEvent );

        void HandleListProviderError( TInt aError );

    protected:  // New functions

    protected:  // Functions from base classes

    private:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CLmkSelectorCache( CLmkListProviderBase& aListProvider );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private:    // Data

        CLmkListProviderBase& iListProvider;
        RPointerArray<CLinkedItem> iItemList;
    };

#endif      // CLMKSELECTORCACHE_H

// End of File
