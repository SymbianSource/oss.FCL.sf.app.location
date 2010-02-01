/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Test Class which has the list of test cases for CLcService
*
*/


#ifndef CTESTLCSERVICE_H
#define CTESTLCSERVICE_H

// System Includes
#include <StifLogger.h>
#include <StifTestModule.h>
#include <e32base.h>
#include <e32std.h>
#include <lcnotification.h>
#include <lcappexitobserver.h>


//Forward Declarations
class CLcService;

// Constants

// CLASS DECLARATION
/**
 *  Test Class to invoke the CLcService methods
 *
 *  This class is a helper class. The status variable of this active obect is 
 *  used to provide the asynchronous method Calls.
 *
 *  @lib testlc.lib
 *  @since S60 v5.0
 */

class CTestLcService :public CActive, public MLcNotification, public MLcAppExitObserver
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        */
        CTestLcService(CStifLogger* aLog);

        /**
        * Destructor. 
        */
        ~CTestLcService();

    public:         
        
        //From CActive
          void RunL();
          void DoCancel();
		  TInt RunError(TInt aError);   
		
		// From MLcNotification     
		  void LcStatusChangedL( TInt aErrorCode );      
	
		// From MLcAppExitObserver	   
		  void HandleChainedLocationAppExitL( TInt aReason );
		       
	    /**
	     * Test Case for Creating the new instance of CLcService
	     * This will test for Successful Creation on NewL
	     *
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckServiceNewL();
			       
	    /**
	     * Test Case for Creating the new instance of CLcService
	     * This will test for Successful Creation on NewLC
	     *
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckServiceNewLC();	  	  
		       
	    /**
	     * Test Case for Getting all the Location Applications 
	     * This will test for Successful Get.
	     *
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt TestGetLocationApplicationsL();
		       
	    /**
	     * Test Case for Getting all the Location Applications Asynchronously.
	     * This will test for Successful Get.
	     *
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt TestGetLocationApplicationsAsynL();
		       
	    /**
	     * Test Case for Getting all the Location Applications using Filter.
	     * This will test for Successful Get
	     *
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt TestGetLocationApplicationsFilterL();
		       
	    /**
	     * Test Case for Getting all the Location Applications using Filter.
	     * This will test for Successful Get
	     *
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt TestGetLocationApplicationsFilterAsynL();
		  	
	    /**
	     * Test Case for Setting and Removing the Observer.
	     * This will test for Successful Set and Remove.
	     *
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt TestSetRemoveObserverL(TInt aObserverState);
	   
	    /**
	     * Test Case for Cancelling the GetLocationApplications request.
	     * This will test for Successful Cancel
	     *
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt TestCancelGetLocationApplications();

	    /**
	     * Test Case for Creating new instance of CLcLaunchParam
	     * This will test for Successful Create.
	     *
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt TestCLcLaunchParamNewL();
	   
	    /**
	     * Test Case for Creating new instance of CLcLaunchParam
	     * This will test for Successful Create.
	     *
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt TestCLcLaunchParamNewLC();

		  		  	   
    private:  //data
	    /**
	     * CLcService Instance
	     */     		 
    	 CLcService*         iLcService;

	    /**
	     * Pointer to Stif Logger - Does not Own
	     */           
       CStifLogger* 			iLog;
	        
     };     

#endif      // CTESTLCSERVICE_H

// End of File
