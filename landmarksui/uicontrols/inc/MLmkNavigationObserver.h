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
* Description:    LandmarksUi Content File -    MLmkNavigationObserver class for observing tab navigation in
*                dialog.
*
*/








#ifndef MLMKNAVIGATIONOBSERVER_H
#define MLMKNAVIGATIONOBSERVER_H

//  INCLUDES
#include <coedef.h>
#include <w32std.h>

// CLASS DECLARATION

/**
* MLmkNavigationObserver class for observing tab navigation in
* dialog. Also allows observing dialog launching and closing.
*/
class MLmkNavigationObserver
    {
    public: // New functions
        /**
        * Handle navigation events
        * @param aKeyEvent
        * @return TKeyResponse
        */
        virtual TKeyResponse HandleNavigationEventL(
                                const TKeyEvent& aKeyEvent ) = 0;

        /**
        * Handle dialog launching. Called just before showing
        * the dialog, from dialog's PreLayoutDynInitL.
        */
        virtual void HandleLaunchingEventL() = 0;

        /**
        * Handle dialog closing. Called just before dialog will
        * be exited.
        */
        virtual void HandleClosingEventL() = 0;
    };

#endif      // MLMKNAVIGATIONOBSERVER_H

// End of File
