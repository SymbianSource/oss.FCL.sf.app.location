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


#ifndef M_LCREGISTRYOBSERVER_H
#define M_LCREGISTRYOBSERVER_H

/**
 *  @file mlcregistryobserver.h
 *
 *  This file provides the definitions for the observer class which enables
 *  the lcregistry engine class to listen to dynamic behaviour of lcregistry.
 *
 *  @since S60 v5.0 
 */


// SYSTEM INCLUDES
#include <e32base.h>

/**
 *  @class MLcRegistryObserver
 *  Observer class to notify to server engine wheather any dynamic changes happen to lcregistry
 *  due to applications/services installation/uninstallation/upgradation etc..
 *
 *  @since S60 v5.0
 */

class MLcRegistryObserver
	{
public:
   /**
     * This function notify LC server engine
	 * that registry is undergoing for dynamic updation
	 *
     * @since S60 v5.0
     * @param None.
     */
	virtual void RegistryUnderUpdation( ) = 0;		

   /**
     * This function notify LC server engine
     * that registry has completed it's dynamic updation process
     *
     * @since S60 v5.0
     * @param None.
     */
	virtual void RegistryUpdated() = 0;
	};

#endif // M_LCREGISTRYOBSERVER_H
