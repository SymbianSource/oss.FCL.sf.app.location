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
* Description:  Notifier class for any change in Storage API database.
*
*/


#ifndef C_EVTDBNOTIFIER_H
#define C_EVTDBNOTIFIER_H

// System Includes

//  System Includes
#include <e32base.h>

// User Includes

// User Includes
#include "evtdbobserver.h"
#include "evtstoragedbobserver.h"
#include "evtstoragedomaincrkeys.h"

// Forward Declarations
class CRepository;

// extern data types

// global Functions

// constants go here

/**
 *  Class that monitors Storage Database for any change.
 *  This class requests for any change in the Stroage database. 
 *
 *  @lib evtstorage.lib
 *  @since S60 v9.1
 */
class CEvtDbNotifier : public CActive
    {  
public:
    /**
     * Constructs a new instance of Event Notifier.
     *
     * @return The new instance of Event Notifier.
     * @leave System wide error code if the object creation fails.         
     */
    static CEvtDbNotifier* NewL( MEvtDbObserver&    aObserver );
    
    /**
     * Constructs a new instance of Event Notifier.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of Event Notifier object.
     * @leave System wide error code if the object creation fails.         
     */
    static CEvtDbNotifier* NewLC( MEvtDbObserver&    aObserver );  
    
    /**
     * Destructor
     */
    virtual ~CEvtDbNotifier ();
    
    /**
     * Observer for TDbChangeType changes in Storage Database.
     */
    void DbChangedL( );

	// Inherited from CActive 
    void RunL();
    
    void DoCancel();
    
    TInt RunError( TInt aError ); 

private: 
    /**
     * Handle any change to database.
     */   
    void HandleDbChangedL( );
    
    /**
     * Request to get database notifications on change.
     */   
    void StartNotification();
    
    /**
     * Default C++ Constructor.
     */
    CEvtDbNotifier( MEvtDbObserver&    aObserver ); 

    /**
     * Second phase of the two phase constructor.
     */
    void ConstructL();

private: // data
    
    /**
     *  Reference to Observer
     * 	Does Not own
     */
    MEvtDbObserver&					iDbObserver;
    
    /**
     *  CRepository Instance
     */
    CRepository*                    iRepository;

    };

#endif // C_EVTDBNOTIFIER_H
