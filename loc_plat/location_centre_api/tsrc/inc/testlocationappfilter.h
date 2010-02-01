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
* Description:  Test Class which has the list of test cases for TLcLocationAppFilter
*
*/


#ifndef CTESTLOCATIONAPPFILTER_H
#define CTESTLOCATIONAPPFILTER_H

// System Includes
#include <StifLogger.h>
#include <StifParser.h>
#include <StifTestModule.h>
#include <e32base.h>
#include <e32std.h>
#include <bautils.h>
#include <f32file.h>

// Constants

// CLASS DECLARATION
/**
 *  Test Class For TLcLocationAppFilter
 *
 *  This has TLcLocationAppFilter object which invokes TLcLocationAppFilter methods 
 *
 *  @lib testlc.lib
 *  @since S60 v5.0
 */

class CTestLocationAppFilter : public CBase
    {
    public:  
    
    	// Constructors and destructor
        /**
        * C++ default constructor.
        */
        CTestLocationAppFilter(CStifLogger* aLog);

        /**
        * Destructor. 
        */
        ~CTestLocationAppFilter();

    public:         

        // New functions  
	    /**
	     * Method to Check Creation of TLcLocationAppFilter
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckTLcLocationAppFilter( );
	
	    /**
	     * Method to Check set of FilterSystemCharacteristics
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckFilterSystemCharacteristics( TInt aChoice );
		 
	    /**
	     * Method to Check set of FilterApplicationCharacteristics
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckFilterApplicationCharacteristics( TInt aChoice );
		   		
	    /**
	     * Method to Check set of SysCharFilterConfiguration
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckSysCharFilterConfiguration( TInt aChoice );
		 
	    /**
	     * Method to Check set of AppCharFilterConfiguration
	     * @since S60 v5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckAppCharFilterConfiguration( TInt aChoice );
		  	  	   
    private:  //data
    	  
	    /**
	     * Pointer to Stif Logger - Does not Own
	     */           
        CStifLogger* 			iLog;
	        
     };     

#endif      // CTESTLOCATIONAPPFILTER_H

// End of File
