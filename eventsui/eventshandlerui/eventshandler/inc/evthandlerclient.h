/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Client for Events Handler Server
*
*/


#ifndef C_EVTHANDLERCLIENT_H
#define C_EVTHANDLERCLIENT_H

#include <e32std.h>

/**
 *  Client class for the Event Handler Server.
 *
 *  @lib 
 *  @since S60 v9.1
 */
class REvtHandlerSession : public RSessionBase
	{
public:
        /**
        * Connect to the Event Handler Server
        */
		IMPORT_C TInt Connect();
	
	    /**
        * Sends the Event Id to the Event handler Server to perform the 
		* required action.
        * @param aMessage Message containg the Event Id.
        * @param aAccuracy Accuracy of the Event Id.
        */
		IMPORT_C TInt SendEventId( const TDesC& aMessage, const TInt aAccuracy );
	};

#endif  // C_EVTHANDLERCLIENT_H

// End of File
