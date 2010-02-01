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


#ifndef M_LCREGISTRATIONUPDATENOTIFIER_H
#define M_LCREGISTRATIONUPDATENOTIFIER_H

/**
 *  @file lcregistrationupdatenotifier.h
 *
 *  This file provides the definitions for the observer class which enables
 *  the lcregistryhandler to listen to notifications from file server.
 *
 *  @since S60 v5.0 
 */


// SYSTEM INCLUDES
#include <e32base.h>

/**
 *  @class MLcRegistryUpdateNotifier
 *  Observer class to notify to server wheather any changes happen to file server
 *  due to applications/services registered with Location Centre.
 *
 *  @since S60 v5.0
 */

class MLcRegistryUpdateNotifier
	{
public:
   /**
     * This function notify lc registry
     * for any dunamic changes happen to the Location
     * centre's private directory.
     *
     * @since S60 v5.0
     * @param None.
     */
	virtual void HandleDynamicRegistrationL() = 0;		
	};

#endif // M_LCREGISTRATIONUPDATENOTIFIER_H

