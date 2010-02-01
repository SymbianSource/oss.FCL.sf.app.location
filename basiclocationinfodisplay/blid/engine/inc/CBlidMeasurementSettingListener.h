/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides Update Measurement Settings Value from Central
*                repository
*
*/


#ifndef CBLIDMEASUREMENTSETTINGLISTENER_H
#define CBLIDMEASUREMENTSETTINGLISTENER_H

//  System Includes
#include <e32base.h>

// User Includes
#include "MBlidMeasurementSettings.h"

// Forward Declarations
class CRepository;

// Constant Declarations

// Class Definition
/**
 * Listener for getting the System of Measurement settings
 */
NONSHARABLE_CLASS( CBlidMeasurementSettingListener ) : public CActive
	{
	public:
		/**
         * Creates new System of Measurement Listener
         *
         * @param  aListenerObsrv                   Observer to the Settings Listener
		 * @return CBlidMeasurementSettingListener*	Pointer to the CBlidSettings
         */                                                 
        static CBlidMeasurementSettingListener* NewL( 
                            MBlidMeasurementSettings&	aListenerObsrv );


		/**
         * Creates new System of Measurement Listener
         * Leaves the object on the Clean up stack         
         *
         * @param  aListenerObsrv			            Observer to the Settings adapter
		 * @return CBlidMeasurementSettingListener*	    Pointer to the CBlidSettings
         */                                                 
        static CBlidMeasurementSettingListener* NewLC( 
                            MBlidMeasurementSettings&	aListenerObsrv );	

 
		/**
		 * Destructor
		 */
		~CBlidMeasurementSettingListener ();

		/**
		 * Returns the current value of the System of measurements settings
		 *
		 * @return TReal32  Current value of the System of measurements settings
		 */
		TReal32 SysofMeasurementL();
        
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
		CBlidMeasurementSettingListener( MBlidMeasurementSettings&	aListenerObsrv );

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
	     * Reference to the Settings observer
	     */
		MBlidMeasurementSettings&     iObserver;
	
		/**
		 * Reference to the Central repository object
		 *
		 * Owns
		 */
		CRepository*                    iRepository; 		
	};			


#endif //CBLIDMEASUREMENTSETTINGLISTENER_H