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
* Description:  Test Class which has the list of test cases for CLcLocationAppInfo
*
*/


#ifndef CTESTLOCATIONAPPINFO_H
#define CTESTLOCATIONAPPINFO_H

// System Includes
#include <StifLogger.h>
#include <StifParser.h>
#include <StifTestModule.h>
#include <e32base.h>
#include <e32std.h>
#include <bautils.h>
#include <f32file.h>

//Forward Declarations
class CLcLocationAppInfo;

// Constants

// CLASS DECLARATION
/**
 *  Test Class For CLcLocationAppInfo
 *
 *  This has CLcLocationAppInfo object which invokes CLcLocationAppInfo methods 
 *
 *  @lib testlc.lib
 *  @since S60 v5.0
 */

class CTestLocationAppInfo : public CBase
    {
    public:  
    
    	// Constructors and destructor
        /**
        * C++ default constructor.
        */
        CTestLocationAppInfo(CStifLogger* aLog);

        /**
        * Destructor. 
        */
        ~CTestLocationAppInfo();

    public:         

        // New functions  
	    /**
	     * Method to Check the LocationApp Info NewL
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckNewL( );

	    /**
	     * Method to Check the LocationApp Info NewLC
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckNewLC( );

	    /**
	     * Method to Check the LocationApp Info NewLC
	     * Copy Constructor
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckCopyNewL( );

	    /**
	     * Method to Check the LocationApp Info NewLC
	     * Copy Constructor
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckCopyNewLC( );

	    /**
	     * Method to Check Id
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckId( );

	    /**
	     * Method to Check Name
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckName( );

	    /**
	     * Method to Check Launch Mode
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckLaunchMode( );

	    /**
	     * Method to Check System Characteristics
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckSystemCharacteristics( );

	    /**
	     * Method to Check Application Characteristics
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckApplicationCharacteristics( );

    private:  //data
	    /**
	     * CLcLocationAppInfo Instance
	     */     		 
    	CLcLocationAppInfo*         iLcLocationAppInfo;
    	  
	    /**
	     * Pointer to Stif Logger - Does not Own
	     */           
        CStifLogger* 			iLog;
	        
     };     

#endif      // CTESTLOCATIONAPPINFO_H

// End of File
