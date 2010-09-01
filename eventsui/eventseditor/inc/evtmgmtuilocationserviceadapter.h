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
* Description:  Implements methods for interfacing with Location Acquisition API.
*
*/


#ifndef __CEVTMGMTUILOCATIONSERVICEADAPTER_H__
#define __CEVTMGMTUILOCATIONSERVICEADAPTER_H__

//  INCLUDES
#include <lbs.h>

// FORWARD DECLARATION
class MEvtMgmtUiLocationObserver;
class TPosition;
class RPositionServer;
class RPositioner;
class TPositionInfo;

// CLASS DECLARATIONS
/**
 *  @class MEvtMgmtUiLocationServiceObserver
 *  Class that represents the observer to notify completion 
 *  of location acquisition operation
 * 
 *  It is used to notify completion of location acquisition operation.
 *  It is also used to notify error if it fails.
 *
 *  @since S60 v9.1
 */

class MEvtMgmtUiLocationServiceObserver
    {
    public: 
        /**
        * Notifies the observer about occured event.
        * Engine observer has to update its data when this function is called.
        * @param [in] aStatus - response code
        */
        virtual void HandleLocationServiceResponseL( const TInt aStatus ) = 0;

        /**
        * Called if occured event fails.
        * @param [in] aErrorCode - Occured general errorcode.
        *               KErrAccessDenied - if right capability is not added.         
        *               KPositionQualityLoss - if accuracy is not acceptable.
        *               KErrTimedOut - if operation takes a lot of time to complete.
        *               KErrNotFound - if no psy is enabled.           
        *               KErrCancel   - if operation is canceled.
        *               Otherwise system wide error-codes. 
        */
        virtual void HandleLocationServiceErrorL( TInt aErrorCode ) = 0;
    };


// CLASS DECLARATIONS
/**
 *  @class CEvtMgmtUiLocationServiceAdapter
 *  Class that represents the adapter for location acquisition engine
 *
 *  This class acts as an adapter for location acquisition engine. 
 *  It  is used to retrieve current location
 *
 *  @since S60 v9.1
 */

class CEvtMgmtUiLocationServiceAdapter : public CActive
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
         static CEvtMgmtUiLocationServiceAdapter* NewL( MEvtMgmtUiLocationServiceObserver& aObserver );

        /**
        * Destructor.
        */
        virtual ~CEvtMgmtUiLocationServiceAdapter();
   
    public: //new functions
 
        /**
        * It is used to request current location
        * 
        * @since S60 v9.1
        */
         void LocationRequestL();

        /**        
        * It is used to retrieve current location
        * 
        * @since S60 v9.1
        * 
        * @return Recently retrieved current location
        */
         TPosition& CurrentPosition();

        /**
        * It is used to cancel recently issued location request
        * 
        * @since S60 v9.1
        */
         void CancelRequest();

         /**
         * Check if all positioning methods are disabled.
         * 
         * @since S60 v9.1
         * 
         * @ret true if disabled else false
         */
         TBool AllPositionMethodsDisabledL();

    public: //Derived
        /*
         * Derived from CActive
         */
        void RunL();
        /*
         * Derived from CActive
         */
        void DoCancel();
        /*
         * Derived from CActive
         */
        TInt RunError(TInt aError);

    private:
           /**
           * By default Symbian 2nd phase constructor is private.
           * @param location service requester name 
           *        (Landmarks Application's name) 
           *        which is passed to Location framework as a parameter to Location Acquisition API
           *        It is maintained by Location Framework for privacy rules and logging purposes 
           */
           void ConstructL( );

           /**
           * C++ default constructor.
           * @param[in] aObserver - observer to notify completion of location acquisition operation
           */
           CEvtMgmtUiLocationServiceAdapter( MEvtMgmtUiLocationServiceObserver& aObserver );   
         
    private:    // data
        
        /// Own: iServer
        RPositionServer iServer;

        ///Ref: observer to notify completion of location acquisition operation
        MEvtMgmtUiLocationServiceObserver& iObserver;

        ///  RPositioner
        RPositioner iPositioner;

        ///  TPosition
        TPosition iPosition;

		///  TPositionInfo
        TPositionInfo iPositionInfo;       

    };

#endif // __CEVTMGMTUILOCATIONSERVICEADAPTER_H__

// End of File
