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
* Description:  Test Class which has the list of test cases
*
*/



#ifndef TESTLC_H
#define TESTLC_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>


// CONSTANTS
// Logging path
_LIT( KtestlcLogPath, "\\logs\\testframework\\testlc\\" ); 
// Log file
_LIT( KtestlcLogFile, "testlc.txt" ); 
_LIT( KtestlcLogFileWithTitle, "testlc_[%S].txt" );

// FORWARD DECLARATIONS
class CTestLc;

// CLASS DECLARATION

/**
*  CTestLc test class for STIF Test Framework TestScripter.
*  This is the testclass for Location Centre API
*
*  @lib testlbslocacquisition.lib
*  @since S60 v5.0
*/
NONSHARABLE_CLASS(CTestLc) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTestLc* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CTestLc();

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since S60 v5.0
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    private:

        /**
        * C++ default constructor.
        */
        CTestLc( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        /**
        * Frees all resources allocated from test methods.
        * @since S60 v5.0
        */
        void Delete();
		       
	    /**
	     * Test Case for Creating the new instance of CLcService
	     * This will test for Successful Creation on NewL
	     *
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckServiceNewL( CStifItemParser& aItem );
			       
	    /**
	     * Test Case for Creating the new instance of CLcService
	     * This will test for Successful Creation on NewLC
	     *
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckServiceNewLC( CStifItemParser& aItem );	 
		  		       
	    /**
	     * Test Case for Getting all the Location Applications 
	     * This will test for Successful Get.
	     *
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt TestGetLocationApplicationsL( CStifItemParser& aItem );
		       
	    /**
	     * Test Case for Getting all the Location Applications Asynchronously.
	     * This will test for Successful Get.
	     *
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt TestGetLocationApplicationsAsynL( CStifItemParser& aItem );
		  
	    /**
	     * Test Case for Getting all the Location Applications using Filter.
	     * This will test for Successful Get
	     *
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt TestGetLocationApplicationsFilterL( CStifItemParser& aItem );
		       
	    /**
	     * Test Case for Getting all the Location Applications using Filter.
	     * This will test for Successful Get
	     *
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt TestGetLocationApplicationsFilterAsynL( CStifItemParser& aItem );

	    /**
	     * Test Case for Setting and Removing the Observer.
	     * This will test for Successful Set and Remove.
	     *
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt TestSetRemoveObserverL( CStifItemParser& aItem );
		  
	    /**
	     * Test Case for Cancelling the GetLocationApplications request.
	     * This will test for Successful Cancel
	     *
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt TestCancelGetLocationApplications( CStifItemParser& aItem );
		  
	    /**
	     * Test Case for Creating new instance of CLcLaunchParam
	     * This will test for Successful Create.
	     *
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt TestCLcLaunchParamNewL( CStifItemParser& aItem );
	   
	    /**
	     * Test Case for Creating new instance of CLcLaunchParam
	     * This will test for Successful Create.
	     *
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt TestCLcLaunchParamNewLC( CStifItemParser& aItem );
//----------    appinfo    
	    /**
	     * Method to Check the LocationApp Info NewL
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckNewL( CStifItemParser& aItem );

	    /**
	     * Method to Check the LocationApp Info NewLC
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckNewLC( CStifItemParser& aItem );

	    /**
	     * Method to Check the LocationApp Info NewLC
	     * Copy Constructor
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckCopyNewL( CStifItemParser& aItem );

	    /**
	     * Method to Check the LocationApp Info NewLC
	     * Copy Constructor
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckCopyNewLC( CStifItemParser& aItem );

	    /**
	     * Method to Check Id
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckId( CStifItemParser& aItem );

	    /**
	     * Method to Check Name
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckName( CStifItemParser& aItem );

	    /**
	     * Method to Check Launch Mode
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckLaunchMode( CStifItemParser& aItem );

	    /**
	     * Method to Check System Characteristics
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckSystemCharacteristics( CStifItemParser& aItem );

	    /**
	     * Method to Check Application Characteristics
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckApplicationCharacteristics( CStifItemParser& aItem );

//-----------Array
	    /**
	     * Method to Check the Creation of AppInfo Array using NewL
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckArrayNewL( CStifItemParser& aItem );
	
	    /**
	     * Method to Check the Creation of AppInfo Array using NewLC
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckArrayNewLC( CStifItemParser& aItem );	
		 	
	    /**
	     * Method to Check the Count of AppInfo Array
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckCount( CStifItemParser& aItem );
		  	
	    /**
	     * Method to Check the append to AppInfo Array
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckAppendL( CStifItemParser& aItem );
		  	
	    /**
	     * Method to Check the Remove from AppInfo Array
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckRemoveL( CStifItemParser& aItem );
		  	
	    /**
	     * Method to Check the Reset of AppInfo Array
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckResetL( CStifItemParser& aItem );
		  	
	    /**
	     * Method to Check the [] Operator of AppInfo Array
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckOperatorL( CStifItemParser& aItem );
		  
// ----- Filter		  
	    /**
	     * Method to Check Creation of TLcLocationAppFilter
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckTLcLocationAppFilter( CStifItemParser& aItem );
	
	    /**
	     * Method to Check set of FilterSystemCharacteristics
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckFilterSystemCharacteristics( CStifItemParser& aItem );
		 
	    /**
	     * Method to Check set of FilterApplicationCharacteristics
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckFilterApplicationCharacteristics( CStifItemParser& aItem );

		   		
	    /**
	     * Method to Check set of SysCharFilterConfiguration
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckSysCharFilterConfiguration( CStifItemParser& aItem );
		 
	    /**
	     * Method to Check set of AppCharFilterConfiguration
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckAppCharFilterConfiguration( CStifItemParser& aItem );

    private:    // Data Members


    };

#endif      // TESTLC_H

// End of File
