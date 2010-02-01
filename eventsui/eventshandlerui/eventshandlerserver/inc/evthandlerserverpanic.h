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
* Description:  panic definition for Events Handler Server
*
*/


#include <e32cmn.h>

enum TEvtHandlerServerPanic
	{
	EPanicBadDescriptor,
	EPanicIllegalFunction,
	EPanicAlreadyReceiving
	};

// ---------------------------------------------------------------------------
// PanicClient()
// ---------------------------------------------------------------------------
//
void PanicClient( const RMessagePtr2& aMessage, TEvtHandlerServerPanic aPanic )
/*
 * RMessage::Panic() also completes the message. This is:
 * (a) important for efficient cleanup within the kernel
 * (b) a problem if the message is completed a second time
 */
	{
	_LIT( KPanic,"evthandlerserver" );
	aMessage.Panic( KPanic, aPanic );
	}
	
// end of file
