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
* Description:  messages & constants for Events Handler Server
*
*/


#ifndef C_EVTHANDLERCLIENTSERVER_H
#define C_EVTHANDLERCLIENTSERVER_H

#include <e32std.h>
#include <e32base.h>

_LIT( KEvtHandlerServerName, "evthandlerserver" );
_LIT( KEvtHandlerServerImg, "evthandlerserver" );		// EXE name

const TUid KServerUid3 = {0x2001E669};

enum TEvtHandlerServerMessages
	{
	ESendEventId,
	EReceiveEventId,
	ECancelReceive
	};


#endif  // C_EVTHANDLERCLIENTSERVER_H

// End of File