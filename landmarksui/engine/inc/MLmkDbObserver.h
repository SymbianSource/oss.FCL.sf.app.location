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
* Description:    LandmarksUi Content File -    Interface for database observers
*
*/







#ifndef MLMKDBOBSERVER_H
#define MLMKDBOBSERVER_H

//  INCLUDES
#include "EPos_Landmarks.h"

// CLASS DECLARATION

/**
* MLmkDbObserver class
*/
class MLmkDbObserver
    {
    public: // New functions
        /**
        * Virtual method for handling database event.
        * @param aEvent
        */
        virtual void HandleDatabaseEvent( TPosLmEvent& aEvent ) = 0;
    };

#endif      // MLMKDBOBSERVER_H

// End of File
