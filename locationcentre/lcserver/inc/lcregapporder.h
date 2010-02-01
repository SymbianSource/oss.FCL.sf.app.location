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
* Description:  Location Centre Server object.
*
*/


#ifndef C_LCREGAPPORDER_H
#define C_LCREGAPPORDER_H


// SYSTEM INCLUDES
#include <e32base.h>
#include <centralrepository.h>
#include <s32strm.h>

// USER INCLUDDE
#include "lcipcparams.h"

// FORWARD DECLARATIONS
class CLcRegAppStore;

//DATA TYPE
	
/**
 *  Location Centre registered application content/service display order.
 *
 *  This class is inherited from CBase. Extracts the registration
 *  order from the central repository key 
 *  and externalize/internalize of those from/to TDesC key value.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CLcRegAppOrder ) : public CBase
	{
public:
    /**
     * Constructs a new instance of CLcRegAppOrder.
     *
     * @return The new instance of CLcRegAppOrder object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcRegAppOrder* NewL( CLcRegAppStore& aLcRegAppStore);

    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Centre
     * server registry handler
     */
    ~CLcRegAppOrder();
    
public:
    /**
     * Extracts the application registered order according to place 
     * like top,middle,and bottom in the list from central repository key
     * buffer output.
     *
     * @since S60 v5.0
     * @param [in]aUuid			UUID of the registered app/service
     * @param [out]aRowPos		returns the position of app/service
     *
     * @return TInt  If successfull the return KErrNone.  
     */
	TInt GetRegisteredAppOrder( const TDesC& aUuid,
							    TInt& aRowPos);    

    /**
     * Remove the deregistered application from the array 
     * depends upon the UUID's sent through the function.
     *
     * @since S60 v5.0
     * @param aUuid			UUID of the registered app/service
     *
     * @return TInt  If successfull the return KErrNone.  
     */
	void RemoveArrayL(const TDesC& aUuid);

    /**
     * Append the registered application to the array 
     * depends upon the UUID's sent through the function.
     *
     * @since S60 v5.0
     * @param aUuid			UUID of the registered app/service
     *
     * @return TInt  Index.  
     */
	TInt AppendArrayL(const TDesC& aUuid);
	
private: // new private method
    /**
     * 
     *
     * @since S60 v5.0
     * @param aArray[ out ]
     * @param aArray[ in ]
     *
     */
    void ParseCenRepKeyL( RPointerArray< HBufC >&	aArray,
    				     const TDesC&				aKey );
    				     
    /**
     *
     * @since S60 v5.0
     * @param aBuffer
     *
     * @return TUint32
     */
	TUint32	ParseInteger( const TDesC&	aBuffer ); 				     

private:
    /**
     * C++ Default constructor
     */
    CLcRegAppOrder( CLcRegAppStore& aLcRegAppStore );
    
    /**
     * Second phase of the two phase constructor
     */
    void ConstructL();
    
private: //data member
	//own: Central repository instance.
	CRepository*				iRepository;
	
	//own: array of top portion's UUID's 
    RPointerArray< HBufC >		iTopArray;
    
    //own: array of bottom portion's UUID's 
    RPointerArray< HBufC >		iBottomArray; 
    
    // array of middle portion's UUID's
    RLcIpcAppIdArray    		iMiddleArray;
	
	//reference to appstore object comming through lc registry
	CLcRegAppStore* 			iLcRegAppStore;
	};

#endif   // C_LCREGAPPORDER_H

