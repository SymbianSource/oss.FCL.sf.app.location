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


// ---------------------------------------------------------------------------
// CEvtHandlerServerSession::CEvtHandlerServerSession()
// ---------------------------------------------------------------------------
//
inline CEvtHandlerServerSession::CEvtHandlerServerSession()
	{}

// ---------------------------------------------------------------------------
// CEvtHandlerServerSession::Server()
// ---------------------------------------------------------------------------
//
inline CEvtHandlerServer& CEvtHandlerServerSession::Server()
	{return *static_cast<CEvtHandlerServer*>(const_cast<CServer2*>(CSession2::Server()));}

// ---------------------------------------------------------------------------
// CEvtHandlerServerSession::ReceivePending()
// ---------------------------------------------------------------------------
//
inline TBool CEvtHandlerServerSession::ReceivePending() const
	{return !iReceiveMsg.IsNull();}


// end of file