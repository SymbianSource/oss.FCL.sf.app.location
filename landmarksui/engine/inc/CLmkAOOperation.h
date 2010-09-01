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
* Description:   	LandmarksUi Content File
*				Handle active object operations
*
*/








#ifndef CLMKAOOPERATION_H
#define CLMKAOOPERATION_H

//  INCLUDES
#include "MLmkAOOperationObserver.h"
#include <e32base.h>        // CBase


// FORWARD DECLARATIONS
class CPosLmOperation;
class MLmkAOOperationObserver;
class TOperationTypes;

// CLASS DECLARATION

/**
* CLmkAOOperation class
*/
class CLmkAOOperation : public CActive
    {
    public:  // Constructors and destructor
        /**
        * Constructor
        * @param aLmOperation
        * @param aObserver
        * @param aType
        * @param aProgressNote
        * @return newly instantiated object
        */
        IMPORT_C static CLmkAOOperation* NewL(
                            CPosLmOperation* aLmOperation,
                            MLmkAOOperationObserver& aObserver,
                            MLmkAOOperationObserver::TOperationTypes aType,
                            TBool aProgressNote );

        /**
        * Destructor.
        */
        IMPORT_C ~CLmkAOOperation();

    public: // New methods
        /**
        * Start AO operation
        *
        */
        IMPORT_C void StartOperation();

        /**
        * Stop AO operation
        *
        */
		IMPORT_C void StopOperation();

    private: // From CActive
        void RunL();
        void DoCancel();

    protected:  // Constructors and destructor
        /**
        * C++ constructor.
        * @param aLmOperation
        * @param aObserver
        * @param aType
        * @param aProgressNote
        * @return newly instantiated object
        */
        IMPORT_C CLmkAOOperation( CPosLmOperation* aLmOperation,
                         MLmkAOOperationObserver& aObserver,
                         MLmkAOOperationObserver::TOperationTypes aType,
                         TBool aProgressNote );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    protected:    // Data
        TReal32 iProgress;

        ///Own:
        CPosLmOperation* iLmOperation;

        MLmkAOOperationObserver& iObserver;

        MLmkAOOperationObserver::TOperationTypes iType;

        TBool iProgressNote;
        RTimer iTimer;
        TBool  iIsTimerSet;
    };

#endif      // CLMKAOOPERATION_H

// End of File
