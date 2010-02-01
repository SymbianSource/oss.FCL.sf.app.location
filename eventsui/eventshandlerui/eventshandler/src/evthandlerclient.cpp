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


#include "evthandlerclientserver.h"
#include "evthandlerclient.h"
#include "evthandlerserverconsts.h"
#include "evtdebug.h"

// ----------------------------------------------------------------------------
// StartServer()
// ----------------------------------------------------------------------------
//
static TInt StartServer()
/*
 * Start the server process. Simultaneous launching
 * of two such processes should be detected when the second one attempts to
 * create the server object, failing with KErrAlreadyExists.
 */
	{
	EVTUIDEBUG("+ StartServer()");
	const TUidType serverUid( KNullUid, KNullUid, KServerUid3);
	RProcess server;
	TInt r = server.Create( KEvtHandlerServerImg, KNullDesC, serverUid );
	if (r != KErrNone)
		return r;
	TRequestStatus stat;
	server.Rendezvous(stat);
	if (stat != KRequestPending)
		server.Kill( 0 );		// abort startup
	else
		server.Resume();	// logon OK - start the server
	User::WaitForRequest( stat );		// wait for start or death
	// we can't use the 'exit reason' if the server panicked as this
	// is the panic 'reason' and may be '0' which cannot be distinguished
	// from KErrNone
	r = ( server.ExitType() == EExitPanic ) ? KErrGeneral : stat.Int();
	server.Close();
	EVTUIDEBUG("- StartServer()");
	return r;
	}

// ----------------------------------------------------------------------------
// REvtHandlerSession::Connect()
// ----------------------------------------------------------------------------
//
EXPORT_C TInt REvtHandlerSession::Connect()
/*
 * Connect to the server, attempting to start it if necessary
 */
	{
	EVTUIDEBUG("+ REvtHandlerSession::Connect()");
	TInt retry = 2;
	for (;;)
		{
		TInt r = CreateSession( KEvtHandlerServerName, TVersion( 0, 0, 0), 1 );
		if ( r != KErrNotFound && r != KErrServerTerminated )
			return r;

		if ( --retry == 0 )
			return r;

		r = StartServer();
		if ( r !=KErrNone && r != KErrAlreadyExists )
			return r;
		}
	}

// ----------------------------------------------------------------------------
// REvtHandlerSession::SendEventId()
// ----------------------------------------------------------------------------
//
EXPORT_C TInt REvtHandlerSession::SendEventId( const TDesC& aMessage, const TInt aAccuracy )
	{
	EVTUIDEBUG("+ REvtHandlerSession::SendEventId()");

	if ( aMessage.Length() > KMaxEvtHandlerMessage )
		return KErrArgument;
	return SendReceive( ESendEventId, TIpcArgs( &aMessage, aAccuracy )  );
	}
