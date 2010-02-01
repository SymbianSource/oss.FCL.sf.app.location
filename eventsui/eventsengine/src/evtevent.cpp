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
* Description:  Concrete Class for Location Events.
*
*/


// System Includes

// User Includes
#include "evtevent.h"
#include "evtaction.h"

// ================ Member funtions for CEvtEvent class ======================

// ---------------------------------------------------------------------------
// CEvtEvent::CEvtEvent
// ---------------------------------------------------------------------------
//
CEvtEvent::CEvtEvent():
	iEventId( 0 ),
	iEventStatus( EDraft ),
	iRadius( 0.1 ),
	iRepeat( EFalse )
    {    
    }
    
// ---------------------------------------------------------------------------
// CEvtEvent::~CEvtEvent
// ---------------------------------------------------------------------------
//
CEvtEvent::~CEvtEvent()
    {
    delete iSubject;
    delete iPlace;
    delete iDescription;
    if( iAction )
    	delete iAction;
    delete iLocation;
    } 
    
// ---------------------------------------------------------------------------
// CEvtEvent::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtEvent* CEvtEvent::NewL()
    {
	CEvtEvent* self = NewLC( );
	CleanupStack::Pop( self );
	return self;
    }

// ---------------------------------------------------------------------------
// CEvtEvent::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtEvent* CEvtEvent::NewLC()
    {
	CEvtEvent* self = new ( ELeave )CEvtEvent( );
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
    } 

// ---------------------------------------------------------------------------
// CEvtEvent::ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtEvent::ConstructL()
    {
    // Allocate the Null string by default
    iSubject = KNullDesC().AllocL();
    iPlace = KNullDesC().AllocL();
    iDescription = KNullDesC().AllocL();
    iLocation = new TPosition();
    }
    
// ---------------------------------------------------------------------------
// CEvtEvent::EventId
// ---------------------------------------------------------------------------
//
EXPORT_C TEvtEventId CEvtEvent::EventId() const
    {
    return iEventId;
    } 
    
// ---------------------------------------------------------------------------
// CEvtEvent::SetEventId
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtEvent::SetEventId( const TEvtEventId aId )
    {
    iEventId = aId;
    } 
    
// ---------------------------------------------------------------------------
// CEvtEvent::Subject
// ---------------------------------------------------------------------------
//
EXPORT_C TPtrC CEvtEvent::Subject() const
    {
    return iSubject->Des();
    } 
    
// ---------------------------------------------------------------------------
// CEvtEvent::SetSubjectL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtEvent::SetSubjectL( const TDesC& aSubject )
    {
    delete iSubject;
    iSubject = NULL;
    iSubject = aSubject.AllocL();
    } 
    
// ---------------------------------------------------------------------------
// CEvtEvent::Place
// ---------------------------------------------------------------------------
//
EXPORT_C TPtrC CEvtEvent::Place() const
    {
    return *iPlace;
    } 
    
// ---------------------------------------------------------------------------
// CEvtEvent::SetPlaceL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtEvent::SetPlaceL( const TDesC& aPlace )
    {
    delete iPlace;
    iPlace = NULL;
    iPlace = aPlace.AllocL();
    } 
  
// ---------------------------------------------------------------------------
// CEvtEvent::EventStatus
// ---------------------------------------------------------------------------
//
EXPORT_C TEvtEventStatus CEvtEvent::EventStatus() const
    {
    return iEventStatus;
    } 
    
// ---------------------------------------------------------------------------
// CEvtEvent::SetEventStatus
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtEvent::SetEventStatus( 
                            const TEvtEventStatus aEventStatus )
    {
    iEventStatus = aEventStatus;
    } 
    
// ---------------------------------------------------------------------------
// CEvtEvent::Radius
// ---------------------------------------------------------------------------
//
EXPORT_C TReal CEvtEvent::Radius() const
    {
    return iRadius;
    }
    
// ---------------------------------------------------------------------------
// CEvtEvent::SetRadius
// radius will always be in km.
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtEvent::SetRadius( const TReal aRadius )
    {
    iRadius = aRadius;
    } 
    
// ---------------------------------------------------------------------------
// CEvtEvent::Repeat
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CEvtEvent::Repeat() const
    {
    return iRepeat;
    } 
    
// ---------------------------------------------------------------------------
// CEvtEvent::SetRepeat
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtEvent::SetRepeat( const TBool aRepeat )
    {
    iRepeat = aRepeat;
    } 
    
// ---------------------------------------------------------------------------
// CEvtEvent::Description
// ---------------------------------------------------------------------------
//
EXPORT_C TPtrC CEvtEvent::Description() const
    {
    return iDescription->Des();
    } 
    
// ---------------------------------------------------------------------------
// CEvtEvent::SetDescriptionL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtEvent::SetDescriptionL( const TDesC& aDescription )
    {
    delete iDescription;
    iDescription = NULL;
    iDescription = aDescription.AllocL();
    } 
    
// ---------------------------------------------------------------------------
// CEvtEvent::Action
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtAction& CEvtEvent::Action() const
    {
    return *iAction;
    } 
    
// ---------------------------------------------------------------------------
// CEvtEvent::SetActionL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtEvent::SetActionL( CEvtAction* aAction )
    {
	// Clear Action , if the aAction is NULL
    if( !aAction  ) 
        {
        if( iAction )
            {
            delete iAction;
            iAction = NULL;
            }
        return;
        }
		
	// Set the Action Id and String if aAction is present.	
    TInt64 id = aAction->Id();
    if(iAction)
        {
        id = iAction->Id();
        delete iAction;
    	iAction = NULL;
        }
    iAction = aAction;
    iAction->SetId( id );
    iAction->SetEvtId(EventId());
    } 
    
// ---------------------------------------------------------------------------
// CEvtEvent::HasAction
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CEvtEvent::HasAction( )
	{
	return (iAction)?ETrue:EFalse;
	}

// ---------------------------------------------------------------------------
// CEvtEvent::Location
// ---------------------------------------------------------------------------
//
EXPORT_C TPosition& CEvtEvent::Location() const
    {
    return *iLocation;
    }

// ---------------------------------------------------------------------------
// CEvtEvent::HasAction
// ---------------------------------------------------------------------------
//
void CEvtEvent::SetLocationL(TPosition& aLocation)
    {
    *iLocation = aLocation;
    }
// End of File
