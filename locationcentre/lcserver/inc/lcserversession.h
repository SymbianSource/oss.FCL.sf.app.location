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
* Description:  Location Centre Server side session object.
*
*/


#ifndef C_LCSERVERSESSION_H
#define C_LCSERVERSESSION_H

// SYSTEM INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CLcServer;
class CLcServerEngine;

/**
 *  Location Centre server side session object.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CLcServerSession ): public CSession2
    {
public:
    /**
     * Constructs a new instance of Location Centre server side session object.
     *
     * @param[in] aLcServer         Reference to the server side object.
     * @param[in] aLcServerEngine   Reference to the Location Centre Server Engine.
     * @return The new instance of Location Centre server side session object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcServerSession* NewL( CLcServer&       aLcServer,
                                   CLcServerEngine& aLcServerEngine );
    
    /**
     * Constructs a new instance of Location Centre server side session object.
     * Leaves the created instance on the cleanup stack.
     *
     * @param[in] aLcServer         Reference to the server side object.
     * @param[in] aLcServerEngine   Reference to the Location Centre Server Engine. 
     * @return The new instance of Location Centre server side session object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcServerSession* NewLC( CLcServer&          aLcServer,
                                    CLcServerEngine&    aLcServerEngine );        

    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Centre
     * server side session object.
     */
    virtual ~CLcServerSession();
    
    /**
     * Notifies the Server session class that the Location Centre Registry has
     * been updated. This enables the session to notify the corresponding
     * Location Centre Client which is observing for these changes.
     */
    void RegistryUpdated();

public: // Inherited from the base classes
    /**
     * Inherited from the CSession2 class
     *
     * Service handler for all messages that are directed to this session.
     */
    void ServiceL( const RMessage2& aMessage );
    
    /**
    * From CSession2
    *
    * Error function invoked when the ServiceL message leaves.
    */
    void ServiceError( const RMessage2& aMessage, 
                             TInt       aError );
            
private:
    /**
     * C++ Default constructor
     *
     * @param aLcServer Reference to the server side object.     
     */
    CLcServerSession( CLcServer&        aLcServer,
                      CLcServerEngine&  aLcServerEngine );
    
    /**
     * Second phase of the two phase constructor
     */
    void ConstructL();
    
private:
    /**
     * Reference to the Server side object.
     *
     * Required to notify the creation and destruction of a session
     * This is necessary to ensure that the server shut down when there
     * are no sessions outstanding.
     */
    CLcServer&                  iLcServer;
    
    /**
     * Boolean flag to indicate that this class has incremented the
     * number of sessions in the server. Hence, the need to decrement
     * the count on termination.
     */
    TBool                       iDecrementSession;
    
    /**
     * Reference to the Location Centre Server Engine.
     */
    CLcServerEngine&            iEngine;
   
    /**
     * Boolean value to indicate that a notification request is
     * outstanding.
     */
    TBool                       iNotificationOutStanding;
    
    TBool                       iNotifyPending;
     
    /**
     * RMessage2 object to notify the changes in Location Centre
     * registry.
     */
    RMessage2                  iObserverMessage;
   
    };

#endif // C_LCSERVERSESSION_H
