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
* Description:  Observer class defintion to notify the Location Centre Server
*                to any updations in the Location Centre Engine.
*
*/


#ifndef M_LCSERVERENGINEOBSERVER_H
#define M_LCSERVERENGINEOBSERVER_H

// SYSTEM INCLUDES
#include <e32base.h>

/**
 *  @class MLcServerEngineObserver
 *  Observer class to notify updations to the Location Centre Server Engine.
 *
 *  This observer will provide the notification when there is an updation to
 *  the Location Centre Server Engine due to updations in the underlying
 *  Location Centre Registry. The Server classes is expected to communicate
 *  the information to all the Clients instances which have registered for
 *  such notifcations.
 *
 *  @since S60 v5.0
 */
class MLcServerEngineObserver
    {
public:
    /**
     * Notification to notify the updation of Location Centre Registry.
     *
     * @since S60 v5.0
     */
    virtual void LcRegistryUpdated() = 0;
    };

#endif // M_LCSERVERENGINEOBSERVER_H
