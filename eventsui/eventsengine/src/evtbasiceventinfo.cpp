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
* Description:  Basic Classes for the Location Events.
*
*/


// System Includes

// User Includes
#include "evtbasiceventinfo.h"

// ================ Member funtions for CEvtBasicEventInfo class =============

// ---------------------------------------------------------------------------
// CEvtBasicEventInfo::CEvtBasicEventInfo
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtBasicEventInfo::CEvtBasicEventInfo():
	iEventId( 0 )
    {    
    }
    
// ---------------------------------------------------------------------------
// CEvtBasicEventInfo::~CEvtBasicEventInfo
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtBasicEventInfo::~CEvtBasicEventInfo()
    {
    delete iSubject;
    delete iPlace;
    } 
    
// ---------------------------------------------------------------------------
// CEvtBasicEventInfo::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtBasicEventInfo* CEvtBasicEventInfo::NewL()
    {
	CEvtBasicEventInfo* self = NewLC( );
	CleanupStack::Pop( self );
	return self;
    }

// ---------------------------------------------------------------------------
// CEvtBasicEventInfo::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtBasicEventInfo* CEvtBasicEventInfo::NewLC()
    {
	CEvtBasicEventInfo* self = new ( ELeave )CEvtBasicEventInfo( );
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
    } 

// ---------------------------------------------------------------------------
// CEvtBasicEventInfo::ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtBasicEventInfo::ConstructL()
    {
    // Allocate empty string.
    iSubject = KNullDesC().AllocL();
    iPlace = KNullDesC().AllocL();
    } 
    
// ---------------------------------------------------------------------------
// CEvtBasicEventInfo::EventId
// ---------------------------------------------------------------------------
//
EXPORT_C TEvtEventId CEvtBasicEventInfo::EventId() const
    {
    return iEventId;
    } 
    
// ---------------------------------------------------------------------------
// CEvtBasicEventInfo::SetEventId
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtBasicEventInfo::SetEventId( const TEvtEventId aId )
    {
    iEventId = aId;
    } 
    
// ---------------------------------------------------------------------------
// CEvtBasicEventInfo::Subject
// ---------------------------------------------------------------------------
//
EXPORT_C TPtrC CEvtBasicEventInfo::Subject() const
    {
    return iSubject->Des();
    } 
    
// ---------------------------------------------------------------------------
// CEvtBasicEventInfo::SetSubjectL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtBasicEventInfo::SetSubjectL( const TDesC& aSubject )
    {
    delete iSubject;
    iSubject = NULL;
    iSubject = aSubject.AllocL();
    } 
    
// ---------------------------------------------------------------------------
// CEvtBasicEventInfo::Place
// ---------------------------------------------------------------------------
//
EXPORT_C TPtrC CEvtBasicEventInfo::Place() const
    {
    return iPlace->Des();
    } 
    
// ---------------------------------------------------------------------------
// CEvtBasicEventInfo::SetPlaceL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtBasicEventInfo::SetPlaceL( const TDesC& aPlace )
    {
    delete iPlace;
    iPlace = NULL;
    iPlace = aPlace.AllocL();
    } 
    
// ---------------------------------------------------------------------------
// CEvtEvent::Repeat
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CEvtBasicEventInfo::Repeat() const
    {
    return iRepeat;
    } 
    
// ---------------------------------------------------------------------------
// CEvtEvent::SetRepeat
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtBasicEventInfo::SetRepeat( const TBool aRepeat )
    {
    iRepeat = aRepeat;
    } 

// ---------------------------------------------------------------------------
// CEvtBasicEventInfo::EventStatus
// ---------------------------------------------------------------------------
//
EXPORT_C TEvtEventStatus CEvtBasicEventInfo::EventStatus() const
    {
    return iEventStatus;
    } 
    
// ---------------------------------------------------------------------------
// CEvtBasicEventInfo::SetEventStatus
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtBasicEventInfo::SetEventStatus( 
                            const TEvtEventStatus aEventStatus )
    {
    iEventStatus = aEventStatus;
    } 

// End of File
