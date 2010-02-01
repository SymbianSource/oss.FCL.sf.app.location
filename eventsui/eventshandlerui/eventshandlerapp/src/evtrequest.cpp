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
* Description:  Container for Request and Type.
*
*/


// System Includes

// User Includes
#include "evtrequest.h"

// ================ Member funtions for CEvtRequest class ======================

// ---------------------------------------------------------------------------
// CEvtRequest::CEvtRequest
// ---------------------------------------------------------------------------
//
CEvtRequest::CEvtRequest():
	iEvent( NULL )
    {    
    }
    
// ---------------------------------------------------------------------------
// CEvtRequest::~CEvtRequest
// ---------------------------------------------------------------------------
//
CEvtRequest::~CEvtRequest()
    {
    delete iEvent;
    } 
    
// ---------------------------------------------------------------------------
// CEvtRequest::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtRequest* CEvtRequest::NewL()
    {
	CEvtRequest* self = NewLC( );
	CleanupStack::Pop( self );
	return self;
    }

// ---------------------------------------------------------------------------
// CEvtRequest::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtRequest* CEvtRequest::NewLC()
    {
	CEvtRequest* self = new ( ELeave )CEvtRequest( );
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
    } 

// ---------------------------------------------------------------------------
// CEvtRequest::ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtRequest::ConstructL()
    {
    }
    
// ---------------------------------------------------------------------------
// CEvtRequest::Action
// ---------------------------------------------------------------------------
//
CEvtEvent& CEvtRequest::Event()
    {
    return *iEvent;
    } 
    
// ---------------------------------------------------------------------------
// CEvtRequest::SetActionL
// ---------------------------------------------------------------------------
//
void CEvtRequest::SetEventL( CEvtEvent* aEvent )
    {
    iEvent = aEvent;
    } 
    
// ---------------------------------------------------------------------------
// CEvtRequest::EventId
// ---------------------------------------------------------------------------
//
EXPORT_C TEvtRequestType CEvtRequest::RequestType() const
    {
    return iRequestType;
    } 
    
// ---------------------------------------------------------------------------
// CEvtRequest::SetEventId
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtRequest::SetRequestType( const TEvtRequestType aRequestType )
    {
    iRequestType = aRequestType;
    } 

// End of File
