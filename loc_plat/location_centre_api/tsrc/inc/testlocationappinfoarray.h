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


#ifndef CTestLocationAppInfoArray_H
#define CTestLocationAppInfoArray_H

// System Includes
#include <StifLogger.h>
#include <StifParser.h>
#include <StifTestModule.h>
#include <e32base.h>
#include <e32std.h>
#include <bautils.h>
#include <f32file.h>

//Forward Declarations
class CLcLocationAppInfoArray;

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

class CTestLocationAppInfoArray : public CBase
    {
    public:  
    
    	// Constructors and destructor
        /**
        * C++ default constructor.
        */
        CTestLocationAppInfoArray(CStifLogger* aLog);

        /**
        * Destructor. 
        */
        ~CTestLocationAppInfoArray();

    public:         

        // New functions  
	    /**
	     * Method to Check the Creation of AppInfo Array using NewL
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckArrayNewL( );
	
	    /**
	     * Method to Check the Creation of AppInfo Array using NewLC
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckArrayNewLC( );	
		 	
	    /**
	     * Method to Check the Count of AppInfo Array
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckCount( );
		  	
	    /**
	     * Method to Check the append to AppInfo Array
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckAppendL( );
		  	
	    /**
	     * Method to Check the Remove from AppInfo Array
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckRemoveL( );
		  	
	    /**
	     * Method to Check the Reset of AppInfo Array
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckResetL( );
		  	
	    /**
	     * Method to Check the [] Operator of AppInfo Array
	     * @since S60 5.0
	     * Returns KErrNone if successful. 
	     *
	     */        
		  TInt CheckOperatorL( );
		  		  	   
    private:  //data
	    /**
	     * CLcLocationAppInfo Instance
	     */     		 
    	CLcLocationAppInfoArray*         iLcLocationAppInfoArray;
    	  
	    /**
	     * Pointer to Stif Logger - Does not Own
	     */           
        CStifLogger* 			iLog;
	        
     };     

#endif      // CTestLocationAppInfoArray_H

// End of File
