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
* Description:  Location Centre Server start up.
*
*/


#ifndef C_LCSERVERSTARTUP_H
#define C_LCSERVERSTARTUP_H

// SYSTEM INCLUDES
#include <e32base.h>

/**
 *  Location Centre server start up routines.
 *
 *  It contains static funtions which start up the Location Centre server.
 *
 *  @since S60 v5.0
 */
NONSHARABLE_CLASS( LcServerStartup )
    {
public:
    /**
     * Starts the Location Centre server.
     *
     * @since S60 v5.0
     * @return KErrNone, If the server has been started successfully.
     *         System wide error codes incase the startup fails.
     */
    static TInt StartLocationCentreServer();

private:
    /**
     * Starts the Location Centre server.
     *
     * @since S60 v5.0
     * @leave System wide error codes incase the startup fails.
     */
    static void StartLocationCentreServerL();     
    };

#endif // C_LCSERVERSTARTUP_H
