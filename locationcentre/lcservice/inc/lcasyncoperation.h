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
* Description:  Handles all the Asynchronous operations with the Location
*                Centre Client Session
*
*/


#ifndef C_LCASYNCOPERATION_H
#define C_LCASYNCOPERATION_H

// SYSTEM INCLUDES
#include <e32base.h>

// USER INCLUDES
#include "lclocationappfilter.h"

// FORWARD DECLARATIONS
class RLcClientSession;
class CLcBasicAppInfo;
class CLcLocationAppInfoArray;
class TLcLocationAppFilter;
class RLcClientSession;
class CBufFlat;

/**
 * Observer class to notify the completion of the Asynchronous operation
 * @lib lcservice.lib
 * @since v5.0
 */
class MLcAsynOperationObserver
	{
public:
	/**
	 * Notification for the Completion of the Asynchronous operation.
	 *
	 * @param[in] aError Error code for the completion of the Asynchronous
	 *                   Operation.
	 */		
	virtual void OperationComplete( TInt aError ) =0;
	};
	
/**
 *  Handles all the Asynchronous operations to the Location Centre Server.
 *
 *  @lib lcservice.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CLcAsyncOperation ) : public CActive
    {
public:
    /**
     * Constructs a new instance of Location Centre interface.
     *
     * @param[in] aSession  Reference to the Location Centre Client Session.
     * @param[in] aObserver Observer to notify the Completion of the Asynchronous
     *                      Operation.
     * @return The new instance of Location Centre interface object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcAsyncOperation* NewL( RLcClientSession&  			aSession,
    								MLcAsynOperationObserver&	aObserver );     

    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Centre
     * interface.
     */
    virtual ~CLcAsyncOperation();

    /**
     * Gets a filtered list of Location based Applications.
     *
     * @param[in] aLocationAppFilter	Filtering parameters.
     * @param[in] CLcLocationAppInfoArray* Array of Location based Applications and
     *                                  Contents/Services.
     */
    void GetLocationApplicationsL(
                     const TLcLocationAppFilter&        aLocationAppFilter,
                           CLcLocationAppInfoArray*&    aAppInfoArray );
    /**
     * Cancels an outstanding @ref GetLocationApplications request.
     */
    void CancelGetLocationApplications();
                           
private:
    /**
     * Default Constructor
     */                               
    CLcAsyncOperation( RLcClientSession&  		    aSession,
    				   MLcAsynOperationObserver&	aObserver );

	void ConstructL();
	
	/**
	 * Issues a self request to this Active Object.
	 */
	void GetLengthL();
	
    /**
     * Re-issues a request to the Location Centre Server. The request which will be re-issued
     * will be the last request which was issued.
     */
    void ReIssueRequestL();	
	    
private: // Inherited from Base classes
    /**
     * Inherited from CActive
     */
    void RunL();
    
    /**
     * Inherited from CActive
     */
    void DoCancel();
    
    /**
     * Inherited from CActive
     */
    TInt RunError( TInt aError );
    
private: // Member Variables
	/**
	 * Class declaration for holding the Location Application Information
	 * array.
	 */
	class TLcAppInfoContainer
		{
	public:
		/**
		 * Overloaded constructor which takes the reference of the
		 * Application Information array.
		 */
		TLcAppInfoContainer( CLcLocationAppInfoArray*&    aAppInfoArray );
		
	public:
		/**
		 * Reference to the Application Information array.
		 */
		CLcLocationAppInfoArray*&    iAppInfoArray;	
		};
		
    /**
     * Enumeration for the State of Asynchronous operation
     */
    enum TASyncOperation
        {
        ELcNoOperation,
        ELcGetAppLength,
        ELcGetApp
        };
    
    /**
     * Reference to the Client session to the Location Centre Server.
     */
    RLcClientSession&           	iClientSession;
    
    /**
     * Observer to notify the completion of the Asynchronous request.
     */
    MLcAsynOperationObserver&		iObserver;
        
    /**
     * Current State of operation
     */
    TASyncOperation					iOperation;
            
    /** 
     * Location Application filter for this Application.
     */
    TLcLocationAppFilter        	iAppFilter;
    
    /**
     * Buffer for exchanging information with the Location Centre Server.
     *
     * Owns 
     */
    CBufFlat* 						iBuffer;
    
    /**
     * Pointer to the above buffer
     */
    TPtr8							iBufferPtr;
    
    /**
     * Place holder for the Location Centre Application Information
     * Array.
     */
    TLcAppInfoContainer*			iAppInfoArrayContainer;
    
    /**
     * IPC arguments
     */
    TIpcArgs						iIpcArgs;
    
    /**
     * Package buffer containing the Application Filter options.
     */
    TPckg< TLcLocationAppFilter >*  iFilterBuffer;
    };

#endif // C_LCASYNCOPERATION_H
