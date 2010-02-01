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
* Description:  Interface class for showing Events Global Query.
*
*/



#ifndef EVTGLOBALQUERY_H
#define EVTGLOBALQUERY_H
 
// System Includes
#include <e32base.h>  //CActive

// User Includes
#include "evttoneplayer.h"

//Forward Declaration
class CAknGlobalMsgQuery;
class CEvtInfoNoteInputParam;
class CEvtTonePlayer;
class CEvtKeyLockHandler;

// CLASS DECLARATION

/**
*  Global Query class which handles the pop up of the global query. 
*
*  @since Series 60 9.1
*/
class CEvtGlobalQuery : public CActive,
						public MEvtTonePlayObserver
    {
	public:
        /**
        * Destructor
        */
	    IMPORT_C virtual ~CEvtGlobalQuery();
		
		/**
		 * Constructs a new instance of Global Query.
		 *
		 * @return The new instance of Global Query object.
		 * @leave System wide error code if the object creation fails.         
		 */
	    IMPORT_C static CEvtGlobalQuery* NewL( CEvtInfoNoteInputParam&      aInputParam );
		
	    /**
	     * Constructs a new instance of Global Query.
	     * Leaves the created instance on the cleanup stack.
	     *
	     * @return The new instance of Global Query object.
	     * @leave System wide error code if the object creation fails.         
	     */
	    IMPORT_C static CEvtGlobalQuery* NewLC( CEvtInfoNoteInputParam&      aInputParam );   
		
	public: 
	    /**
	     * Starts the Global Query.
	     *
	     * @since S60 v9.1  
	     * @param[in] aStatus Asynchronous request status
	     * @param[in] aRetBuffer Buffer which holds the return value
	     */
	    IMPORT_C void StartGlobalQueryL(TRequestStatus& aStatus, TDes8& aRetBuffer);   
		
	    /**
	     * Cancels the currently outstanding Global Query.
	     *
	     * @since S60 v9.1  
	     */
	    IMPORT_C void CancelDialog();   
    
	    // Inherited from MEvtTonePlayObserver 
	    void TonePlayCompleteL( TBool aTimeout ); 
		
	private:    
		/**
		 * Gets the Icon details from the Icon File.
		 */
	    void GetIconDetailsL( TFileName& aFileName, TInt& aBmpID, TInt& aBmpMaskID );
	
	private: 
		/**
		 * Default C++ Constructor.
		 */
	    CEvtGlobalQuery( CEvtInfoNoteInputParam&      aInputParam );

	    /**
	     * Second phase of the two phase constructor.
	     */
	    void ConstructL();  
		
	private: // from CActive    
	    void RunL();
	    void DoCancel();
	    TInt RunError( TInt aError ); 
		
	private: // data
	    /**
		 * Asynchronous return status
		 */
	    TRequestStatus* iReturnStatus;
		
	    /**
		 * Buffer value which holds the return value
		 * NOT Own:
		 */
	    TDes8* iReturnBuffer;
		
	    /**
		 * Avkon's Global Message Query
		 * Owns:
		 */
	    CAknGlobalMsgQuery* iGlobalMsgQuery;
		
	    /**
		 * Event Info Note parameters
		 */
	    CEvtInfoNoteInputParam&      iInputParam ;
    
	    /**
	     * Tone player
	     * Owns
	     */
	    CEvtTonePlayer*                 iTonePlayer;
    
	    /**
	     * Key Lock Handler
	     * Owns
	     */
	    CEvtKeyLockHandler* 			iKeyLockHandler;
    };
 
#endif  // EVTGLOBALQUERY_H
