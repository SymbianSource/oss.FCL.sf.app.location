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
* Description:  Client side session to the Location Centre server.
*
*/


#ifndef R_LCCLIENTSESSION_H
#define R_LCCLIENTSESSION_H

// SYSTEM INCLUDES
#include <e32std.h>

// USER INCLUDES
#include "lcserverinterface.h"

// FORWARD DECLARATIONS

/**
 *  Client side session handle to the Location Centre server session.
 *
 *  @lib lcservice.lib
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( RLcClientSession ) : public RSessionBase
    {
public:
    /**
     * C++ Default Constructor
     *
     *  @lib lcservice.lib
     *  @since S60 v5.0     
     */
    RLcClientSession();      

    /**
     * C++ Destructor.
     *
     *  @lib lcservice.lib
     *  @since S60 v5.0     
     */
    virtual ~RLcClientSession();

    /**
     * Creates the Client side session handle to the Location Centre
     * server.
     *
     * This function starts the Location Centre server (if its not already
     * running) and then creates a session to it.
     *
     * @since S60 v5.0
     * @leave System wide error code if the operation fails.
     */
    TInt CreateSession();
    
    /**
     * Closes the Client side session handle to the Location Centre
     * server.
     *
     * @since S60 v5.0   
     */
    void Close();
   
    /**
     * Send an asynchronous request to the Location Centre server.
     *
     * @since S60 v5.0
     * @param aMsgType  Operation that is requested from the Location
     *                  Centre server.
     * @param aArgs     Message argument. 
     * @param aStatus   The request status object on which the completion
     *                  of the request is communicated.
     */
    void SendReceive(       TLcServerMsgType    aMsgType, 
                      const TIpcArgs&           aArgs,
                            TRequestStatus&     aStatus ) const;
    
    /**
     * Send an synchronous request to the Location Centre server.
     *
     * @since S60 v5.0
     * @param aMsgType  Operation that is requested from the Location
     *                  Centre server.
     * @param aArgs     Message argument. 
     * @return  KErrNone, If the request is completed successfully.
     *          System wide error codes in the request fails.
     */
    TInt SendReceive(       TLcServerMsgType    aMsgType, 
                      const TIpcArgs&           aArgs ) const;
 
    /**
     * Send an asynchronous request to the Location Centre server.
     *
     * @since S60 v5.0
     * @param aMsgType  Operation that is requested from the Location
     *                  Centre server.
     * @param aStatus   The request status object on which the completion
     *                  of the request is communicated.
     */
    void SendReceive(       TLcServerMsgType    aMsgType,
                            TRequestStatus&     aStatus ) const;
                             
    /**
     * Send an synchronous request to the Location Centre server.
     *
     * @since S60 v5.0
     * @param aMsgType  Operation that is requested from the Location
     *                  Centre server.
     * @return  KErrNone, If the request is completed successfully.
     *          System wide error codes in the request fails.
     */
    TInt SendReceive(       TLcServerMsgType    aMsgType ) const;
                              
private:
    /**
     * Starts a new server instance
     *
     * @return  KErrNone, If the server is successfully launched
     *          System wide error codes incase the Start fails
     */
    TInt StartServer();
    };

#endif // R_LCCLIENTSESSION_H
