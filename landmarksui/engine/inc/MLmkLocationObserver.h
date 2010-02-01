/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -    Provides an observer interface to Landmark application engine.
*
*/








#ifndef __MLMKLOCATIONOBSERVER_H__
#define __MLMKLOCATIONOBSERVER_H__

// CLASS DECLARATIONS
/**
*  Engine provides this API only for Lmk application.
*
*/
class MLmkLocationObserver
    {
    public: // New functions
        /**
        * Notifies the engine observer about occured event.
        * Engine observer has to update its data when this function is called.
        * @param aStatus
        */
        virtual void NotifyL( const TInt aStatus ) = 0;

        /**
        * Called if occured event causes a leave in previous NotifyL method.
        * @param aErrorCode Occured general errorcode.
        */
        virtual void NotifyErrorL( TInt aErrorCode ) = 0;
    };

#endif      // __MLMKLOCATIONOBSERVER_H__

// End of File
