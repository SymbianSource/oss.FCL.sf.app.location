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
* Description:  Shutdown Timer for Events Handler Server
*
*/


#include "evthandlerclientserver.h"
#include "evthandlershutdowntimer.h"
#include "evtdebug.h"

// ---------------------------------------------------------------------------
// CEvtHandlerShutdownTimer::RunL()
// ---------------------------------------------------------------------------
//
void CEvtHandlerShutdownTimer::RunL()
//
// Initiate server exit when the timer expires
//
    {
    EVTUIDEBUG( "+ CEvtHandlerShutdownTimer::RunL()" );
	CActiveScheduler::Stop();
    EVTUIDEBUG( "- CEvtHandlerShutdownTimer::RunL()" );
	}

// end of file
