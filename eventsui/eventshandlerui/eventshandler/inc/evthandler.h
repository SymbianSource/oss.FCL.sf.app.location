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
* Description:  Client Provider for Events Handler Server
*
*/


#ifndef __EVTHANDLER_H
#define __EVTHANDLER_H

// SYSTEM INCLUDES
#include <e32base.h>

/**
 *  Events Handler server start up routines.
 *
 *  It contains static funtions which start up the Events Handler server.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( EvtHandlerStartup )
    {
public:
    /**
     * Starts the Events Handler.
     *
     * @since S60 v5.0
     * @return KErrNone, If the process has been started successfully.
     *         System wide error codes incase the startup fails.
     */
    static TInt StartEvtHandler();

private:
    /**
     * Handles the Event Fired
	 * Logs the event id into log file and handles the fired event.     
	 *
     * @since S60 v5.0
     * @leave System wide error codes incase the startup fails.
     */
    static void HandleEventL();     
    };

#endif // __EVTHANDLER_H