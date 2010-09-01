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
* Description:   	LandmarksUi Content File
*				LMK application class definition.
*
*/







#ifndef LMAPP_H
#define LMAPP_H

// INCLUDES
#include <aknapp.h>

// CLASS DECLARATION

/**
* CLmkApp application class.
* Provides factory to create concrete document object.
*
*/
class CLmkApp : public CAknApplication
    {
    private:
        /**
        * From CApaApplication, creates CLmkDocument document object.
        * @return A pointer to the created document object.
        */
        CApaDocument* CreateDocumentL();

        /**
        * From CApaApplication, returns application's UID (KLMUID3).
        * @return The value of KLMUID3.
        */
        TUid AppDllUid() const;
    };

#endif // LMAPP_H

// End of File
