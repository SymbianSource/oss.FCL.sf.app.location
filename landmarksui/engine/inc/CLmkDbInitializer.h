/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -    This class wraps functionality related to landmark database
*                initialization into a simpler interface.
*
*/







#ifndef CLMKDBINITIALIZER_H
#define CLMKDBINITIALIZER_H

//  INCLUDES
#include "MLmkAOOperationObserver.h"

// FORWARD DECLARATIONS
class CLmkDbUtils;
class CPosLandmarkDatabase;

// CLASS DECLARATION
/**
 * This class wraps landmark database initialization and compacting
 * into a simple helper class.
 * The database initialization and the database compaction operations
 * are performed in executing two methods of CPosLandmarkDatabase class
 * asynchronously
 */
class CLmkDbInitializer : public CBase, public MLmkAOOperationObserver
    {
	public:  // Constructors and destructor

		/**
		* Create CLmkDbInitializer object
        * @param aDb - landmark database which needs to be initialized
        * @param aObserver - caller class/object which sets itself as observer to
        *                  the CLmkDbInitializer class
        * @param aProgressNote - Note showing the progress of database initialise
        *					   operation
		* @return newly instantiated object
		*/
		static CLmkDbInitializer* NewL( CPosLandmarkDatabase& aDb,
                                        MLmkAOOperationObserver& aObserver,
                                        TBool aProgressNote );

		/**
		* Destructor. Cancels possibly ongoing operation.
		*/
		~CLmkDbInitializer();

    public: // New functions
        /**
        * Start the database initialize followed by the compact operation
        * This is done in two asynchronous calls, one for initialize and the other
        * for compacting
        */
        void StartInitAndCompactL();

    protected: //from MLmkAOOperationObserver
        void HandleOperationL( TOperationTypes aType,
                               TReal32 aProgress,
                               TInt aStatus );

    private:  // Constructors and destructor
        /**
        * C++ default constructor.
        * @param aObserver - caller class or object setting itself as observer
        *                    for this class
        * @param aProgressNote -- Note showing the progress of the database
        *                         initialize operation
        * @return newly instantiated object
        */
        CLmkDbInitializer( MLmkAOOperationObserver& aObserver,
                           TBool aProgressNote );

        /**
		* By default Symbian 2nd phase constructor is private.
        * @param aDb
		*/
        void ConstructL( CPosLandmarkDatabase& aDb );

    private:    // Data
        ///Ref:
        MLmkAOOperationObserver& iObserver;

        ///Own:
        CLmkDbUtils* iDbUtils;

        TBool iProgressNote;
    };

#endif      // CLMKDBINITIALIZER_H

// End of File
