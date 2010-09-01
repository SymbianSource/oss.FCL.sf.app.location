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
* Description:    LandmarksUi Content File -    CLmkDoubleGraphicLBModel class
*
*/







#ifndef CLMKDOUBLEGRAPHICLBMODEL_H
#define CLMKDOUBLEGRAPHICLBMODEL_H

//  INCLUDES
#include "CLmkSelectorLBModelBase.h"

// FORWARD DECLARATIONS
class CLmkListProviderBase;

// CLASS DECLARATION

/**
* CLmkDoubleGraphicLBModel class
*/
class CLmkDoubleGraphicLBModel : public CLmkSelectorLBModelBase
    {
    public:  // Constructors and destructor

        /**
        * Create CLmkDoubleGraphicLBModel object
        * @param aListProvider
        * @return newly instantiated object
        */
        static CLmkDoubleGraphicLBModel* NewL(
                    CLmkListProviderBase& aListProvider );

        /**
        * Destructor.
        */
        ~CLmkDoubleGraphicLBModel();

    protected:  // Functions from base classes
        // leaving method
        virtual void MdcaPointL( TInt aIndex ) const;

    protected:  // Constructors and destructor
        /**
        * C++ default constructor.
        * @return newly instantiated object
        */
        CLmkDoubleGraphicLBModel();

        /**
        * Symbian 2nd phase constructor
        * @param aListProvider
        */
        void ConstructL( CLmkListProviderBase& aListProvider );

    private:    // Data
        ///Own:
        HBufC* iEmpty;
    };

#endif      // CLMKDOUBLEGRAPHICLBMODEL_H

// End of File
