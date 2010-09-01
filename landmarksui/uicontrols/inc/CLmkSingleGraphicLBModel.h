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
* Description:    LandmarksUi Content File -    CLmkSingleGraphicLBModel class
*
*/







#ifndef CLMKSINGLEGRAPHICLBMODEL_H
#define CLMKSINGLEGRAPHICLBMODEL_H

//  INCLUDES
#include "CLmkSelectorLBModelBase.h"

// FORWARD DECLARATIONS
class CLmkListProviderBase;

// CLASS DECLARATION

/**
* CLmkSingleGraphicLBModel class
*/
class CLmkSingleGraphicLBModel : public CLmkSelectorLBModelBase
    {
    public:  // Constructors and destructor
        /**
        * Create CLmkSingleGraphicLBModel object
        * @param aListProvider
        * @return newly instantiated object
        */
        static CLmkSingleGraphicLBModel* NewL(
                    CLmkListProviderBase& aListProvider );

        /**
        * Destructor.
        */
        ~CLmkSingleGraphicLBModel();

    protected:  // Functions from base classes
        // leaving method
        void MdcaPointL( TInt aIndex ) const;

    protected:  // Constructors and destructor
        /**
        * C++ default constructor.
        * @return newly instantiated object
        */
        CLmkSingleGraphicLBModel();

        /**
        * Symbian 2nd phase constructor
        * @param aListProvider
        */
        void ConstructL( CLmkListProviderBase& aListProvider );
    };

#endif      // CLMKSINGLEGRAPHICLBMODEL_H

// End of File
