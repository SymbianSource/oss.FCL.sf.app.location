/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -    Interface for list provider observers.
*
*/







#ifndef MLMKLISTPROVIDEROBSERVER_H
#define MLMKLISTPROVIDEROBSERVER_H

//  INCLUDES
#include <e32base.h>

// DATA TYPES

enum TLmkListProviderEventType
    {
    ELmkEventNull,
    ELmkEventNewDefaultDatabaseLocation,
    ELmkEventMediaRemoved,
    ELmkEventListReady,
    ELmkEventCategoryDeleted,
    ELmkEventCategoryUpdated,
    ELmkEventItemAdditionComplete,
    ELmkEventFindListReady
    };

// CLASS DECLARATION

/**
* MLmkListProviderObserver class
*/
class MLmkListProviderObserver
    {
    public: // New functions
        /**
        * It is essential that this method is non-leaving, so that
        * all observers will be safely notified.
        * @param aEvent
        */
        virtual void HandleListProviderEvent(
                        TLmkListProviderEventType aEvent ) = 0;

        /**
        * It is essential that this method is non-leaving, so that
        * all observers will be safely notified.
        * @param aError
        */
        virtual void HandleListProviderError( TInt aError ) = 0;
    };

#endif      // MLMKLISTPROVIDEROBSERVER_H

// End of File
