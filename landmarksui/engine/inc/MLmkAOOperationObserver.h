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
* Description:    LandmarksUi Content File -    Interface for active object observers
*
*/







#ifndef MLMKAOOPERATIONOBSERVER_H
#define MLMKAOOPERATIONOBSERVER_H

//  INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CLmkAOOperation;

// CLASS DECLARATION

/**
* MLmkAOOperationObserver class
*/
class MLmkAOOperationObserver
    {
    public: //types
        enum TOperationTypes
            {
            EDeleteCategories,
            EDeleteLandmarks,
            EAddToCategory,
            EChangeIcons,
            ESearch,
            EParse,
            EImport,
            EEncode,
            EInitialize,
            ECompact,
            EInitAndCompact, // combined init and compact operation
            ENewPkgCategory,
            ERemoveCategory
            };


    public: // New functions
        /**
        * Virtual method for handling operation.
        * @param aType
        * @param aProgress
        * @param aStatus completion code
        */
        virtual void HandleOperationL( TOperationTypes aType,
                                       TReal32 aProgress,
                                       TInt aStatus ) = 0;
    };

#endif      // MLMKAOOPERATIONOBSERVER_H

// End of File
