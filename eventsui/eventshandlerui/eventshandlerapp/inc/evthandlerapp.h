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
* Description:  Application class for Events Handler Application.
*
*/


#ifndef C_EVTHANDLERAPP_H
#define C_EVTHANDLERAPP_H

// SYSTEM INCLUDES
#include <aknapp.h>

/**
 *  Events Handler App's App class. This class extents the CAknApplication
 *  class for Events Handler Application.
 *
 *  @lib lcapp.exe
 *  @since S60 v5.0
 */	
class CEvtHandlerApp : public CAknApplication
	{
private: // Inherited from Base classes
	/**
	 * Inherited from CAknApplication
	 */ 
    CApaDocument* CreateDocumentL();
    
	/**
	 * Inherited from CAknApplication
	 */         
    TUid AppDllUid() const;
    };

#endif  // C_EVTHANDLERAPP_H

// End of File

