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
* Description:  Shutdown Timer class for Events Handler Server
*
*/


#ifndef C_EVTHANDLERSERVERSHUTDOWNTIMER_H
#define C_EVTHANDLERSERVERSHUTDOWNTIMER_H

#include <e32base.h>

/**
 * Events Handler Server shutdown timer class. This class extents the CTimer
 * class for Events Handler Server.
 *
 * @lib evthandlerserver.exe
 * @since S60 v5.0
 */
class CEvtHandlerShutdownTimer : public CTimer
	{
	enum { KCEvtHandlerShutdownDelay = 0x500000 }; // approx 5s

public:
	inline CEvtHandlerShutdownTimer();
	inline void ConstructL();
	inline void Start();

private: // from CTimer
	void RunL();
	};

#include "evthandlershutdowntimer.inl"

#endif  // C_EVTHANDLERSERVERSHUTDOWNTIMER_H

// End of File
