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
* Description:    LandmarksUi Content File -    Database event listener.
*
*/








#ifndef CLMKDBEVENTLISTENER_H
#define CLMKDBEVENTLISTENER_H

//  INCLUDES
#include "EPos_Landmarks.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class CPosLandmarkDatabase;
class MLmkDbObserver;

// CLASS DECLARATION

/**
*  This class is not meant for direct usage, use CLmkEventListenerSingleton
*  instead.
*  This class is associated to a database since adding the first observer,
*  until removing last observer. It is user's responsibility to remove all
*  added observers.
*/
class CLmkDbEventListener : public CActive
    {
    public:  // Constructors and destructor
        /**
        * Create CLmkDbEventListener object
        * @return newly instantiated object
        */
        static CLmkDbEventListener* NewL();

        /**
        * Destructor.
        */
        ~CLmkDbEventListener();

    public: // New methods

        /**
        * Only one database currently supported, leaves with
        * KErrNotSupported if aDb differs from already used db.
        * @param aObserver
        * @param aDb
        */
        void AddObserverL( MLmkDbObserver& aObserver,
                           CPosLandmarkDatabase& aDb );

        /**
        * Remove observer
        * @param aObserver
        */
        void RemoveObserver( MLmkDbObserver& aObserver );

    protected: // New methods
        /**
        * Start listening
        */
        void StartListening();

    protected: // From CActive
        void RunL();
        void DoCancel();

    protected:  // Constructors and destructor
        /**
        * C++ default constructor.
        * @return newly instantiated object
        */
        CLmkDbEventListener();

        /**
        * Symbian 2nd phase constructor
        */
        void ConstructL();

    private:    // Data

        TPosLmEvent iEvent;

        ///Ref:
        CPosLandmarkDatabase* iDb;

        ///Own:
        HBufC* iDbURI;

        ///Ref:
        RPointerArray<MLmkDbObserver> iObservers;
    };

#endif      // CLMKDBEVENTLISTENER_H

// End of File
