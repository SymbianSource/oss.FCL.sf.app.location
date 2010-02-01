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
* Description:  Observer class defintion to notify the termination of
*                Chained Location Applications.
*
*/
#ifndef M_LCAPPEXITOBSERVER_H
#define M_LCAPPEXITOBSERVER_H

/**
 *  @file lcappexitobserver.h
 *
 *  This file provides the definitions for the observer class which is used
 *  by Location Centre to notify the termination of a Chained Location based
 *  Application.
 *
 *  @lib lcservice.lib
 *  @since S60 v5.0 
 */

// SYSTEM INCLUDES
#include <e32base.h>

/**
 *  @class MLcAppExitObserver
 *  Observer class to notify the termination of a launched Location based
 *  Application.
 *
 *  This observer will provide the notification only when the Location based
 *  Application is launched as a Chained application. This client
 *  application needs to pass an instance of this class when calling 
 *  @ref CLcService::LaunchLocationApplicationL if it wants to be notified
 *  of a Location Application exit event incase the Location based 
 *  Application is launched as a Chained application. 
 *
 *  @lib lcservice.lib
 *  @since S60 v5.0
 */
class MLcAppExitObserver
    {
public:
    /**
     * Notification for the termination of the chained Location based 
     * Application.
     *
     * @since S60 v5.0
     * @param[in] aReason The reason that the Location based Application exited.
     *                    This will either be an error code, or the command
     *                    id that caused the Location based Application to exit.
     */
    virtual void HandleChainedLocationAppExitL( TInt aReason ) = 0;
    };

#endif // M_LCAPPEXITOBSERVER_H
