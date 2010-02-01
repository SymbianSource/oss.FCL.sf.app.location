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


#ifndef C_LCSERVER_H
#define C_LCSERVER_H

// SYSTEM INCLUDES
#include <e32base.h>

// USER INCLUDES
#include "lcserverengineobserver.h"

// FORWARD DECLARATIONS
class CLcServerShutDownTimer;
class CLcServerEngine;

/**
 *  Location Centre server.
 *
 *  This class is inherited from CServer2 and not CPolicyServer. This is
 *  because there is no capability checking required for accessing the
 *  services offered by this server.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CLcServer ): public CServer2,
                                public MLcServerEngineObserver
    {
public:
    /**
     * Constructs a new instance of Location Centre server.
     *
     * @return The new instance of Location Centre server object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcServer* NewL();
    
    /**
     * Constructs a new instance of Location Centre server.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of Location Centre server object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcServer* NewLC();        

    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Centre
     * server
     */
    virtual ~CLcServer();
    
    /**
     * Increments the number of Location centre sessions that are open.
     * 
     * This is generally done by the session objects once they have 
     * successfully completed their session initialization.
     */
    void IncrementSessions();
    
    /**
     * Decrements the number of Location centre sessions that are open.
     * 
     * This is generally done by the session objects in their
     * destrutors.
     */
    void DecrementSessions();
    

public: // Inherited from the base classes
    /**
     * Inherited from CServer2
     *
     * @param aVersion The version number for the Location Centre server.
     * @param aMessage The connect message from the client.
     * @return Reference to the new session object.
     */
    CSession2* NewSessionL( const TVersion&     aVersion,
                            const RMessage2&    aMessage ) const;
     
    /**
     * Inherited from MLcServerEngineObserver
     */
    void LcRegistryUpdated();
    
private:
    /**
     * C++ Default constructor
     */
    CLcServer();
    
    /**
     * Second phase of the two phase constructor
     */
    void ConstructL();
    
private:
    /**
     * Shutdown timer.
     *
     * Used to shutdown the server if there are no outstanding connections
     * for a specified period of time.
     * Owns.
     */
    CLcServerShutDownTimer*         iShutDownTimer;
    
    /**
     * Number of Location centre server sessions.
     */
    TInt                            iNumberofSessions;
    
    /**
     * Location Centre Server Engine.
     *
     * Owns
     */
    CLcServerEngine*                iEngine;
    };

#endif // C_LCSERVERSTARTUP_H
