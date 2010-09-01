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
* Description:    LandmarksUi Content File -    Base class for selectors listbox model
*
*/







#ifndef CLMKSELECTORLBMODELBASE_H
#define CLMKSELECTORLBMODELBASE_H

//  INCLUDES
#include <e32base.h>        // CBase
#include <bamdesca.h>

// FORWARD DECLARATIONS
class CLmkListProviderBase;
class CLmkSelectorCache;

// CLASS DECLARATION

/**
* CLmkSelectorLBModelBase class
*/
class CLmkSelectorLBModelBase : public CBase, public MDesCArray
    {
    public:  // Constructors and destructor
        /**
        * Destructor.
        */
        ~CLmkSelectorLBModelBase();

    public: // From MDesCArray
        TInt MdcaCount() const;
        TPtrC16 MdcaPoint( TInt aIndex ) const;

    protected:  // New functions
        // leaving method
        virtual void MdcaPointL( TInt aIndex ) const = 0;

    protected:  // Constructors and destructor
        /**
        * C++ default constructor.
        * @return newly instantiated object
        */
        CLmkSelectorLBModelBase();

        /**
        * Symbian 2nd phase constructor
        * @param aListProvider
        */
        void BaseConstructL( CLmkListProviderBase& aListProvider );

    protected:    // Data
        ///Own:
        CLmkSelectorCache* iCache;

        ///Own:
        HBufC* iBuffer;
    };

#endif      // CLMKSELECTORLBMODELBASE_H

// End of File
