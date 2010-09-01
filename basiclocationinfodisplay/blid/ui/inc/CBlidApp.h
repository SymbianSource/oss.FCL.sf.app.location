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
* Description:  Blid application class definition.
*
*/


#ifndef BLIDAPP_H
#define BLIDAPP_H

// INCLUDES
#include <aknapp.h>

// CLASS DECLARATION

/**
* CBlidApp application class.
* Provides factory to create concrete document object.
* 
*/
class CBlidApp : public CAknApplication
    {
    private:

        /**
         * From CApaApplication, creates CBlidDocument document object.
         * @return A pointer to the created document object.
         */
        CApaDocument* CreateDocumentL();
        
        /**
         * From CApaApplication, returns application's UID (KBLIDUID3).
         * @return The value of KBLIDUID3.
         */
        TUid AppDllUid() const;
    };

#endif // BLIDAPP_H

// End of File
