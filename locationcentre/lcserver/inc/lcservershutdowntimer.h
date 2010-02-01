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
* Description:  Shutdown timer for Location Centre server.
*
*/


#ifndef C_LCSERVERSHUTDOWNTIMER_H
#define C_LCSERVERSHUTDOWNTIMER_H

// SYSTEM INCLUDES
#include <e32base.h>

/**
 *  Shutdown timer for Location Centre Server.
 *
 *  This class is used to ensure that Location Centre server does not run
 *  unnecessarily when there are no outstanding connections. The server object
 *  uses this class to initiate a timer when there are no connections. If the
 *  timer is not cancelled before it fires, then this class shuts down the
 *  Active scheduler hence termination the Location Centre server.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CLcServerShutDownTimer ): public CTimer
    {
public:    
    /**
     * Constructs a new instance of Location Centre server shut down timer.
     *
     * @return The new instance of Location Centre server shut down timer.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcServerShutDownTimer* NewL();
    
    /**
     * Constructs a new instance of Location Centre server shut down timer.
     * Leaves the created instance on the cleanup stack.
     *      
     * @return The new instance of Location Centre server shut down timer.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcServerShutDownTimer* NewLC();        

    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Centre
     * server shutdown timer.
     */
    virtual ~CLcServerShutDownTimer();
     
    /**
     * Starts the shutdown timer. The funtion doesn't do anything if the
     * timer is already running.
     */
    void StartTimer();
    
    /**
     * Stops an outstanding timer request.
     */
    void StopTimer();
    
public: // Inherited from base classes.
    
    /**
     * Inherited from CActive
     */
    void RunL();
    
    /**
     * Inherited from CActive
     */
    TInt RunError( TInt aError );
         
private:
    /**
     * C++ Default constructor.  
     */
    CLcServerShutDownTimer();
    
    /**
     * Second phase of the two phase constructor.
     */
    void ConstructL();  
    
private:
    /**
     * Location Centre Shutdown timer value in MicroSeconds
     */
    TInt            iShutDownTimerValue;
     
    };

#endif // C_LCSERVERSHUTDOWNTIMER_H
