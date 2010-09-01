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
* Description:  Incoming Call Handler.
*
*/


#ifndef C_EVTCALLHANDLER_H
#define C_EVTCALLHANDLER_H

// System Includes
#include <e32base.h>
#include <etel3rdparty.h>

//Forward Declaration

// Class Definition
/**
 *  Interface class to observe for an incoming call.
 *
 *  @lib 
 *  @since S60 v9.1
 */
class MEvtCallObserver
    {
public:	    
    /**
     * This will notify when there is an incoming call.            
     */
    virtual void HandleIncomingCallL( )=0;

    };
    
/**
 *  Adapter class to observe for an incoming Call.
 *  This class listens for an incoming call and notifies
 *  the observer(MEvtCallObserver).
 *
 *  @lib 
 *  @since S60 v9.1
 */
NONSHARABLE_CLASS( CEvtCallHandler ): public CActive
{
public:
    /**
     * Constructs a new instance of Call Handler.
     *
     * @return The new instance of Call Handler object.
     * @leave System wide error code if the object creation fails.         
     */
    static CEvtCallHandler* NewL( MEvtCallObserver& aObserver );
    
    /**
     * Constructs a new instance of Call Handler.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of Call Handler object.
     * @leave System wide error code if the object creation fails.         
     */
    static CEvtCallHandler* NewLC( MEvtCallObserver& aObserver );  

    /**
    * Destructor.
    */
    virtual ~CEvtCallHandler();

public:  // Public Functions	

	    /**
	     * Starts listening for the incoming calls.
	     */
	    void StartListening();

        /**
        * Check if there is any outstanding call.
        * @param TBool Returns ETrue if a call is active.
        */
        TBool IsCallActive( );

        /**
        * Check if there is any incoming call.
        * @param TBool Returns ETrue if there is an incoming call.
        */
        TBool IsCallIncoming( );
        
private: 
    /**
     * From CActive
     */
    void RunL();
 
    /**
     * From CActive
     */
    TInt RunError(TInt anError);
 
    /**
     * From CActive
     */
    void DoCancel();
    
    /**
     * Default C++ Constructor.
     */
	CEvtCallHandler( MEvtCallObserver& aObserver );

    /**
     * Second phase of the two phase constructor.
     */
    void ConstructL();

private: // data

	/**
	 * Incoming Call Observer.
	 */
	 MEvtCallObserver& iObserver;

	/**
	 * CTelephony handle.
	 * Own
	 */
     CTelephony* iTelephony;

	/**
	 * Call status
	 */
     CTelephony::TCallStatusV1 iCallStatus;

	/**
	 * Package for Call status
	 */
     CTelephony::TCallStatusV1Pckg iCallStatusPkg;
	 
};

#endif C_EVTCALLHANDLER_H