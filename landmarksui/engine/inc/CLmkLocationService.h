/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -    Implements methods for interfacing with Location Acquisition API.
*
*/







#ifndef __CLMKLOCATIONSERVICE_H__
#define __CLMKLOCATIONSERVICE_H__

//  INCLUDES
#include <lbs.h>

// FORWARD DECLARATION
class MLmkLocationObserver;
class TPosition;
class RPositionServer;
class RPositioner;
class TPositionInfo;


// CLASS DECLARATION
/**
*  Implements entry point class to LmkEng.dll
*/
class CLmkLocationService : public CActive
    {
    public:
        /**
        * Two-phased constructor.
        * @param location service requester name
        *        (Landmarks Application's name)
        *        which is passed to Location framework as a parameter to Location Acquisition API
        *        It is maintained by Location Framework for privacy rules and logging purposes
        * @return CLmkLocationService object.
        */
        IMPORT_C static CLmkLocationService* NewL(const TDesC& aSrvName);

        /**
        * Destructor.
        */
        virtual ~CLmkLocationService();

    private:
        /**
        * By default Symbian 2nd phase constructor is private.
        * @param location service requester name
        *        (Landmarks Application's name)
        *        which is passed to Location framework as a parameter to Location Acquisition API
        *        It is maintained by Location Framework for privacy rules and logging purposes
        */
        void ConstructL(const TDesC& aSrvName);

        /**
        * C++ default constructor.
        * @param newly instantiated object
        */
        CLmkLocationService();

    public: //new functions
        /**
        * Set observer
        * The calling class sets itself as observer to CLmkLocationService class
        * @param the calling class which sets itself as observer to this class
        */
        IMPORT_C void SetObserver(MLmkLocationObserver& aObserver);

        /**
        * Remove observer
        */
        IMPORT_C void RemoveModelObserver();

        /**
        * Request current location
        */
        IMPORT_C void LocationRequestL();

        /**
        * Get current location
        * @return TPosition reference
        */
        IMPORT_C TPosition& CurrentPosition();

        /**
        * Cancel location request
        */
        IMPORT_C void CancelRequest();

    private: // from CActive
        void RunL();
        void DoCancel();
        TInt RunError(TInt aError);

    private:    // data
        /// Own: iServer
        RPositionServer iServer;

        ///Ref:
        MLmkLocationObserver* iObserver;

        ///  RPositioner
        RPositioner iPositioner;

        ///  TPosition
        TPosition iPosition;

        TPositionInfo iPositionInfo;

    };

#endif // __CLMKLOCATIONSERVICE_H__

// End of File
