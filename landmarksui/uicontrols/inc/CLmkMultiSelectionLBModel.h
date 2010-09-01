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
* Description:    LandmarksUi Content File -    Multi selection listbox model
*
*/







#ifndef CLMKMULTISELECTIONLBMODEL_H
#define CLMKMULTISELECTIONLBMODEL_H

//  INCLUDES
#include "CLmkSelectorLBModelBase.h"

// FORWARD DECLARATIONS
class CLmkListProviderBase;

// CLASS DECLARATION

/**
* CLmkMultiSelectionLBModel class
*/
class CLmkMultiSelectionLBModel : public CLmkSelectorLBModelBase
    {
    public:  // Constructors and destructor

        /**
        *
        *
        * @return newly instantiated object
        */
        static CLmkMultiSelectionLBModel* NewL(
                        CLmkListProviderBase& aListProvider );

        /**
        * Destructor.
        */
        ~CLmkMultiSelectionLBModel();

    public: // New functions

    protected:  // New functions

        // leaving method
        virtual void MdcaPointL( TInt aIndex ) const;

    protected:  // Functions from base classes

    protected:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CLmkMultiSelectionLBModel();

        /**
        * Symbian 2nd phase constructor
        */
        void ConstructL( CLmkListProviderBase& aListProvider );

    protected:    // Data
    };

#endif      // CLMKMULTISELECTIONLBMODEL_H

// End of File
