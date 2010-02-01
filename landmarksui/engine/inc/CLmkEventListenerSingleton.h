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
* Description:    LandmarksUi Content File -    Singleton class for listening events
*
*/








#ifndef CLMKEVENTLISTENERSINGLETON_H
#define CLMKEVENTLISTENERSINGLETON_H

//  INCLUDES
#include <coemain.h>        // CCoeStatic

// FORWARD DECLARATIONS
class CLmkDbEventListener;
class CPosLandmarkDatabase;
class MLmkDbObserver;

// CLASS DECLARATION

/**
* CLmkEventListenerSingleton class.
* It is mandatory that the user takes care of removing added observers
* before they or the database becomes invalid, since this is a singleton
* class and used with static methods.
*/
class CLmkEventListenerSingleton : public CCoeStatic
    {
    public: // New methods
        /**
        * Add observer
        * @param aObserver
        * @param aDb
        */
        static void AddObserverL( MLmkDbObserver& aObserver,
                                  CPosLandmarkDatabase& aDb );

        /**
         * Remove observer
         * @param aObserver
         */
        static void RemoveObserver( MLmkDbObserver& aObserver );

    protected:  // Constructors and destructor
        /**
        * C++ constructor.
        * @return newly instantiated object
        */
        CLmkEventListenerSingleton();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Destructor.
        */
        ~CLmkEventListenerSingleton();

    protected:    // Data
        ///Own:
        CLmkDbEventListener* iEventListener;
    };

#endif      // CLMKEVENTLISTENERSINGLETON_H

// End of File
