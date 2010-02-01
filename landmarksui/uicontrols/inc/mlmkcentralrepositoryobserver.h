/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -    This interface provides, handler functions to handle
*                central repository changes
*
*/








#ifndef __MLMKCENTRALREPOSITORYOBSERVER__H
#define __MLMKCENTRALREPOSITORYOBSERVER__H

// CLASS DECLARATION

/**
*  This interface provides functions to handle changes in the central
*  repository.
*/
class MLmkCentralRepositoryObserver
    {
    public: // New functions

        /**
         * Callback function,called when, there is any change in the
         * central repository
         * @return None
         */
        virtual void HandleCentralRepositoryChangeL() = 0;
    };

#endif      // __MLMKCENTRALREPOSITORYOBSERVER__H

// End of File
