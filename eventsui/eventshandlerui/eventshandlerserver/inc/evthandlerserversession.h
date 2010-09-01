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
* Description:  Session class for Events Handler Server
*
*/


#ifndef C_EVTHANDLERSERVERSESSION_H
#define C_EVTHANDLERSERVERSESSION_H


#include <e32base.h>
#include "evthandlerserver.h"

/**
 * Events Handler Server Session class. This class extents the CSession2
 * class for Events Handler Server.
 *
 * @lib evthandlerserver.exe
 * @since S60 v9.1
 */
class CEvtHandlerServerSession : public CSession2
	{
public:
	CEvtHandlerServerSession();
	void ResolveMessageL( const RMessage2& aMessage );
	void CreateL();

private:
	~CEvtHandlerServerSession();
	inline CEvtHandlerServer& Server();
	void ServiceL( const RMessage2& aMessage );
	void ServiceError( const RMessage2& aMessage, TInt aError );
	inline TBool ReceivePending() const;

private:
	RMessagePtr2 iReceiveMsg;
	TInt iReceiveLen;
	};

#include "evthandlerserversession.inl"

#endif  // C_EVTHANDLERSERVERSESSION_H

// End of File
