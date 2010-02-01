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
* Description:  Active object interface for obtaining Location updates
*
*/


#ifndef CSIMPSYUIAO_H_
#define CSIMPSYUIAO_H_


// System Includes
#include <e32base.h>
#include <lbs.h>
#include <lbspositioninfo.h>

// User Includes

// Forward Declarations

// Class Definition

/**
 * Observer to the SimPsyConfigurator Active Object
 */
class MSimPsyAOObserver
    {
    public:
        /**
         * Notifies that the previously issued Location request is complete
         * This implies that the TPositionInfo passed to the Location request
         * call is now populated with the current location information
         */
        virtual void NotifyLocationRequestCompletedL() = 0; 
    };
 
 
/**
 * Interface class thats used to asynchronously obtain Location information.
 * This class interacts with the Location Acquistion API and asynchronously
 * obtains the current location
 */
class CSimPsyUiAO: public CActive
    {
    public:
        /**
 		 * Static Two phase contructor that instantiates the CSimPsyUiAO
		 * 
		 * @param  aObserver    Observer to the Active object
		 * @return CSimPsyUiAO*	Reference to the object created
		 */
        static CSimPsyUiAO* NewL( MSimPsyAOObserver&    aObserver );    
    
        /**
         * Destructor
         */        
        ~CSimPsyUiAO();
        
        /**
         * Obtains the current Location information using the Location
         * Acquisition API. The function call is asynchronous and the
         * notification regarding the completion of the operation is 
         * provided through the MSimPsyAOObserver object
         *
         */
        void GetCurrentLocationL();
        
        /**
         * Cancels any outstanding Location request
         */
        void CancelLocationRequest();
        
        /**
         * Displays the current Location
         */
        void DisplayCurrentLocationL();
    
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
         * Private Constructor
         */
        CSimPsyUiAO( MSimPsyAOObserver&    aObserver );
        
        /**
         * Second phase of the two phase constructor
         */
        void ConstructL();
        
        /**
         * Displays error note
         *
         * @param aErrorText Error text
         */
        void ShowErrorL(const TDesC& aErrorText);
                      	
    private:           
        /**
         * Reference to the Active object observer
         */
        MSimPsyAOObserver&          iObserver;
        
        /**
         * Handle to the Location server session
         * Owns
         */
        RPositionServer             iPositionServer;
        
        /**
         * Handle to the Location server sub-session
         */
        RPositioner                 iPositioner;  
        
		/**
		 * Position Info structure
		 */
		TPositionInfo           iPositionInfo;                         	
    };

#endif // CSIMPSYUIAO_H_