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
* Description:  session handling for Events Handler Server
*
*/


#include <AknGlobalNote.h> 	// CAknGlobalNote

#include "evthandlerserverconsts.h"
#include "evthandlerclientserver.h"
#include "evthandlerserverpanic.h"
#include "evtdebug.h"

#include "evthandlerserversession.h"

// ---------------------------------------------------------------------------
// CEvtHandlerServerSession::CreateL()
// ---------------------------------------------------------------------------
//
void CEvtHandlerServerSession::CreateL()
	{
	}

// ---------------------------------------------------------------------------
// CEvtHandlerServerSession::~CEvtHandlerServerSession()
// ---------------------------------------------------------------------------
//
CEvtHandlerServerSession::~CEvtHandlerServerSession()
	{
	}

// ---------------------------------------------------------------------------
// CEvtHandlerServerSession::ServiceL()
// ---------------------------------------------------------------------------
//
void CEvtHandlerServerSession::ServiceL( const RMessage2& aMessage )
	{
    EVTUIDEBUG( "+ CEvtHandlerServerSession::ServiceL()" );
    TInt function = aMessage.Function();
	switch ( function )
		{
		case ESendEventId:
			{
			EVTUIDEBUG( "Received Client Message" );
			// Read the Event ID from the message structure
            TBuf<KMaxEvtHandlerMessage> message;
            aMessage.ReadL( 0, message );
			
            TInt64 eventId = 0;	
            TLex lex( message );
            User::LeaveIfError( lex.Val( eventId ));	
			
			// Get the Accuracy Value
			TInt accuracy = aMessage.Int1();
				
			// Pass it along to the server to handle the message
			Server().HandleNewEventL( eventId, accuracy );
		    
		    // Successful handling complete. Complete the outstanding message
		    aMessage.Complete( KErrNone );	
			break;
			}
		default:
			{
			aMessage.Complete( KErrArgument );
			break;
			}
		}
    EVTUIDEBUG( "- CEvtHandlerServerSession::ServiceL()" );
	}

// ---------------------------------------------------------------------------
// CEvtHandlerServerSession::ServiceError()
// ---------------------------------------------------------------------------
//
void CEvtHandlerServerSession::ServiceError( 
                                    const RMessage2& aMessage, 
                                          TInt      aError )
	{
    EVTUIDEBUG( "+ CEvtHandlerServerSession::ServiceError()" );
	aMessage.Complete( aError );
    EVTUIDEBUG( "- CEvtHandlerServerSession::ServiceError()" );
	}

// End of File
