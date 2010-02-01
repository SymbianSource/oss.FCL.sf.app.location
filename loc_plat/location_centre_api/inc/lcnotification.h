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
* Description:  Observer class defintion to notify any changes to the
*                state of Location Applications.
*
*/

#ifndef M_LCNOTIFICATION_H
#define M_LCNOTIFICATION_H

/**
 *  @file lcnotification.h
 *
 *  This file provides the definitions for the observer class which enables
 *  the client application to listen to notifications from Location Centre.
 *
 *  @lib lcservice.lib
 *  @since S60 v5.0 
 */

// SYSTEM INCLUDES
#include <e32base.h>

/**
 *  @class MLcNotification
 *  Observer class to notify the changes to the applications registered with
 *  Location Centre.
 *
 *  This client application needs to pass an instance of this class when calling 
 *  @ref CLcService::StartNotificationL
 *
 *  @lib lcservice.lib
 *  @since S60 v5.0
 */
class MLcNotification
    {
public:
    /**
     * Notification for the termination of the chained Location based Application.
     *
     * The notification can occur under the following conditions
     * - When a new Location based Application registers into Location
     * Centre. In this case, the Location based Application would be 
     * immediately available for all Location Centre operations.
     * - When an already registered Location based Application de-registers.
     * In this case, the Location based Application will be removed from
     * Location Centre. Hence, it would not be available for any further Location
     * Centre operations.
     * - When a Location based Application is present on a removable media (like
     * Memory card)and the media is removed. In this case, for all practical
     * purposes the Location based Application behaves as in the previous context.
     * The difference being that the application does not get removed from
     * Location Centre but is marked as an Absent application. When the
     * removable media is re-inserted, the application immediately becomes 
     * available for all Location Centre operations.
     * - When an removable media containing an Absent Location based Application
     * is re-inserted. In this case, the Location based Application would be
     * immediately available for all Location Centre operations.
     * - When the Location Centre server terminates pre-maturely. In this case, no
     * further notifications would be given to the Client application and all 
     * further requests to the Location Centre server would fail with 
     * @p KErrServerTerminated.     
     *         
     * @since S60 v5.0
     * @param[in] aErrorCode Error code for the notification completion.
     *                       The following are the values which the variable
     *                       can assume on completion of the request.
     *                       - KErrNone If there is an new Location based
     *                       Application registered to Location Centre, when
     *                       an existing Location based Application is removed
     *                       from Location Centre or when an Location based 
     *                       Application becomes absent or present.
     *                       - KErrCancel If an outstanding request is Cancelled.
     *                       - KErrServerTerminated If the Location Centre server
     *                       is pre-maturely terminated.
     *                       - System wide error codes in other error conditions. 
     */
    virtual void LcStatusChangedL( TInt aErrorCode ) = 0;
    };

#endif // M_LCNOTIFICATION_H
