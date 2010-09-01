/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  An observer for change in System of Measurement
*
*/

#ifndef C_EVTMGMTUISYSOFMEASUREMENT_H_
#define C_EVTMGMTUISYSOFMEASUREMENT_H_

//  System Includes
#include <e32base.h>

// User Includes
#include "locnotprefplugindomaincrkeys.h"

// Forward Declarations
class CRepository;

// Constant Declarations

// Class Definition
class MEvtMgmtUiSOMObserver
    {
public:
    /**
     * It will be used to notify observer that lbt operation
     * has be completed
     * @param aTriggerId trigger id of newly created trigger
     * @param aState It will contain the the operation type              
     */
    virtual void NotifySOMChangeL( )=0;

    };
    

/**
 * Adapter to the System of Measurement settings storage
 */
class CEvtMgmtUiSysOfMeasurement : public CActive
    {
    public:
        /**
         * Creates new System of Measurement adapter
         *
         * @param  aAdapterObsrv                        Observer to the Settings adapter
         * @return CEvtMgmtUiSysOfMeasurement&  Reference to the application view
         */                                                 
        static CEvtMgmtUiSysOfMeasurement* NewL( 
                            MEvtMgmtUiSOMObserver& aAdapterObsrv );


        /**
         * Creates new System of Measurement adapter
         * Leaves the object on the Clean up stack         
         *
         * @param  aAdapterObsrv                        Observer to the Settings adapter
         * @return CEvtMgmtUiSysOfMeasurement&  Reference to the application view
         */                                                 
        static CEvtMgmtUiSysOfMeasurement* NewLC( 
                            MEvtMgmtUiSOMObserver& aAdapterObsrv );    

        
        /**
         * Destructor
         */
        ~CEvtMgmtUiSysOfMeasurement ();

        /**
         * Returns the current value of the System of measurements settings
         *
         * @return TLocSystemofMeasurementValues  Current value of the System of measurements settings
         */
        TLocSystemofMeasurementValues GetCurrentSysofMeasL();
        
    protected:
        /**
         * Inherited from CActive
         */
        void RunL();
        
        /**
         * Inherited from CActive
         */
        void DoCancel();
                        
    private:
        /**
         * Constructor
         */
        CEvtMgmtUiSysOfMeasurement( MEvtMgmtUiSOMObserver& aAdapterObsrv );

        /**
         * Second Phase Constructor
         */
        void ConstructL();
    
        /**
         * Start notification from the Central repository server for any changes in the
         * System of measurements key value
         */
        void StartNotification();
                
    private:
        /**
         * Reference to the Adapter observer
         */
        MEvtMgmtUiSOMObserver&     iObserver;

        /**
         * Reference to the Central repository object
         *
         * Owns
         */
        CRepository*                    iRepository;        
        
    };          

#endif // C_EVTMGMTUISYSOFMEASUREMENT_H_