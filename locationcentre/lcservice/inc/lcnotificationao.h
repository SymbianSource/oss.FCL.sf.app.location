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
* Description:  Location Centre Notification Active Object.
*
*/


#ifndef C_LCNOTIFICATIONAO_H
#define C_LCNOTIFICATIONAO_H

// SYSTEM INCLUDES
#include <e32base.h>

// USER INCLUDES

// FORWARD DECLARATIONS
class MLcNotification;
class RLcClientSession;

/**
 *  Location Centre Notification Active Object.
 *
 *  This  class is used to handle Asynchronous Notification requests.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( CLcNotificationAO ): public CActive
    {
public:
    /**
     * Constructs a new instance of Location Centre Notification
     * Active Object.
     *
     * @param[in] aObserver Notification Observer.
     * @return The new instance of Location Centre Notification
     * Active Object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcNotificationAO* NewL( 
                    MLcNotification&    aObserver,
                    RLcClientSession&   aClientSession );    

    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Centre
     * Notification Active Object.
     */
    virtual ~CLcNotificationAO();
     
    /**
     * Issue a notification request to the Location Centre server. This
     * is a recurring request which keeps providing notifications
     * through the MLcNotification observer until a CancelRequest()
     * is called.
     */
    void IssueRequest();
    
    /**
     * Cancels the outstanding notification request issued through the
     * IssueRequest() call.
     */
    void CancelRequest();

protected: // Inherited from Base classes

    /**
     * Inherited from CActive
     */
    void RunL();
    
    /**
     * Inherited from CActive
     */    
    void DoCancel();
             
private:
    /**
     * C++ Default constructor
     */
    CLcNotificationAO( MLcNotification&    aObserver,
                       RLcClientSession&   aClientSession ); 
    
private:
    /**
     * Observer to communicate the notification requests.
     */
    MLcNotification&                iObserver;
    
    RLcClientSession&               iClientSession;
    };

#endif // C_LCNOTIFICATIONAO_H
