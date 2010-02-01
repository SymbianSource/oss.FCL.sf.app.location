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
* Description:    LandmarksUi Content File -    Map And navigation event notifier.
*
*/







#ifndef MLMKANDNAVIGATIONOBSERVER_H
#define MLMKANDNAVIGATIONOBSERVER_H


/**
* MLmkMapAndNavigationObserver class
*/
class MLmkMapAndNavigationObserver
    {
    public: // New functions
        /**
        * Virtual method for handling map & navigation notifications.
        * This function is called when CLmkMapNavigationInterface receives
        * a notification regarding the landmark creation, it passes
        * the landmark id back to the observer.
        * @param aNewLmkItemId
        */
        virtual void HandleLmCreated(TPosLmItemId aNewLmkItemId) = 0;
    };

#endif      // MLMKANDNAVIGATIONOBSERVER_H

// End of File
