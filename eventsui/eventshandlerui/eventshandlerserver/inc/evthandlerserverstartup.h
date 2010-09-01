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
* Description:  Events Handler Server start up.
*
*/


#ifndef C_EVTHANDLERSERVERSTARTUP_H
#define C_EVTHANDLERSERVERSTARTUP_H

// SYSTEM INCLUDES
#include <e32base.h>

/**
 *  Events Handler server start up routines.
 *
 *  It contains static funtions which start up the Events Handler server.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( EvtHandlerServerStartup )
    {
public:
    /**
     * Starts the Events Handler server.
     *
     * @since S60 v5.0
     * @return KErrNone, If the server has been started successfully.
     *         System wide error codes incase the startup fails.
     */
    static TInt StartEvtHandlerServer();

private:
    /**
     * Starts the Events Handler server.
     *
     * @since S60 v5.0
     * @leave System wide error codes incase the startup fails.
     */
    static void StartEvtHandlerServerL();     
    };

#endif // C_EVTHANDLERSERVERSTARTUP_H
