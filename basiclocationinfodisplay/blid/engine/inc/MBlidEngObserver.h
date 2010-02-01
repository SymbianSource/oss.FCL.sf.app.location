/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides an observer interface to blid application engine.
*
*/



#ifndef __MBLIDENGOBSERVER_H__
#define __MBLIDENGOBSERVER_H__

// CLASS DECLARATIONS
/**
*  Engine provides this API only for Blid application.
*
*/
class MBlidEngObserver
    {
    public: // New functions
        /**
         * Notifies the engine observer about occured event.
         * Engine observer has to update its data when this function is called.
         * @param aOnlineMode if ETrue application is online and if
         * EFalse application is offline.
         */
        virtual void NotifyL( const TBool aOnlineMode ) = 0;

        /**
         * Called if occured event causes a leave in previous NotifyL method.
         * @param aErrorCode Occured general errorcode.
         */
        virtual void NotifyErrorL( TInt aErrorCode ) = 0;
    };

#endif      // __MBLIDENGOBSERVER_H__

// End of File
