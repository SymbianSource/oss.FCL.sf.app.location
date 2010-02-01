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
* Description:  Interface to store and access the Location Events.
*
*/


// System Includes

// User Includes
#include "evteventmanager.h"
#include "evtdatabase.h"
#include "evtdebug.h"

// ================ Member funtions for CEvtEventManager class ===============

// ---------------------------------------------------------------------------
// CEvtEventManager::CEvtEventManager
// ---------------------------------------------------------------------------
//
CEvtEventManager::CEvtEventManager()
    {
    }
    
// ---------------------------------------------------------------------------
// CEvtEventManager::~CEvtEventManager
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtEventManager::~CEvtEventManager()
    {
    delete iEvtDb;
    } 
    
// ---------------------------------------------------------------------------
// CEvtEventManager::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtEventManager* CEvtEventManager::NewL()
    {
	CEvtEventManager* self = NewLC( );
	CleanupStack::Pop( self );
	return self;
    }

// ---------------------------------------------------------------------------
// CEvtEventManager::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtEventManager* CEvtEventManager::NewLC()
    {
	CEvtEventManager* self = new ( ELeave )CEvtEventManager( );
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
    } 

// ---------------------------------------------------------------------------
// CEvtEventManager::ConstructL
// ---------------------------------------------------------------------------
//
void CEvtEventManager::ConstructL()
    {
    iEvtDb = CEvtDatabase::NewL( );
    } 

// ---------------------------------------------------------------------------
// CEvtEventManager::GetEventsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtEventManager::GetEventsL( 
                            TEvtEventStatus    aEvtStatus,
                            RPointerArray<CEvtBasicEventInfo>&     aEvtEventArray )
    {
	EVTUIDEBUG( "+ CEvtEventManager::GetEventsL()" );
    // redirect the call to CEvtDatabase
	iEvtDb->GetEventsL( aEvtStatus, aEvtEventArray );
	EVTUIDEBUG( "- CEvtEventManager::GetEventsL()" );
    } 

// ---------------------------------------------------------------------------
// CEvtEventManager::GetEventsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtEventManager::GetEventsL( 
                            RPointerArray<CEvtBasicEventInfo>&     aEvtEventArray )
    {
	EVTUIDEBUG( "+ CEvtEventManager::GetEventsL()" );
    // redirect the call to CEvtDatabase
	iEvtDb->GetEventsL( aEvtEventArray );
	EVTUIDEBUG( "- CEvtEventManager::GetEventsL()" );
    } 
    
// ---------------------------------------------------------------------------
// CEvtEventManager::GetEventL
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtEvent* CEvtEventManager::GetEventL( TEvtEventId    aEvtId )
    {
	EVTUIDEBUG( "+ CEvtEventManager::GetEventL()" );
    // redirect the call to CEvtDatabase
	return iEvtDb->GetEventL( aEvtId );
    } 
    
// ---------------------------------------------------------------------------
// CEvtEventManager::RemoveEventsL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtEventManager::RemoveEventsL( 
                            const RArray<TEvtEventId>&     aEvtIdArray )
    {
	EVTUIDEBUG( "+ CEvtEventManager::RemoveEventsL()" );
    // redirect the call to CEvtDatabase
	iEvtDb->RemoveEventsL( aEvtIdArray );
	EVTUIDEBUG( "- CEvtEventManager::RemoveEventsL()" );
    } 

// ---------------------------------------------------------------------------
// CEvtEventManager::UpdateEventsStatusL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtEventManager::UpdateEventsStatusL( 
                            const RArray<TEvtEventId>&     aEvtIdArray,
                            TEvtEventStatus    aEvtStatus)
    {
    // redirect the call to CEvtDatabase
    iEvtDb->UpdateEventsStatusL( aEvtIdArray, aEvtStatus );
    } 
    
// ---------------------------------------------------------------------------
// CEvtEventManager::UpdateEventL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtEventManager::UpdateEventL( 
                            CEvtEvent&          aEvtEvent )
    {
	EVTUIDEBUG( "+ CEvtEventManager::UpdateEventL()" );
    // redirect the call to CEvtDatabase
	iEvtDb->UpdateEventL( aEvtEvent );
	EVTUIDEBUG( "- CEvtEventManager::UpdateEventL()" );
    } 
    
// ---------------------------------------------------------------------------
// CEvtEventManager::AddEventL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtEventManager::AddEventL( CEvtEvent&     aEvtEvent )
    {
	EVTUIDEBUG( "+ CEvtEventManager::AddEventL()" );
    // redirect the call to CEvtDatabase
	iEvtDb->AddEventL( aEvtEvent );
	EVTUIDEBUG( "- CEvtEventManager::AddEventL()" );
    } 
 
// ---------------------------------------------------------------------------
// CEvtEventManager::UpdateEventStatus
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtEventManager::UpdateEventStatusL( 
                            TEvtEventId      aEvtId,
                            TEvtEventStatus    aEvtStatus)
    {
	EVTUIDEBUG( "+ CEvtEventManager::UpdateEventStatusL()" );
    // redirect the call to CEvtDatabase
	iEvtDb->UpdateEventStatusL( aEvtId, aEvtStatus );
	EVTUIDEBUG( "- CEvtEventManager::UpdateEventStatusL()" );
    } 
    
// ---------------------------------------------------------------------------
// CEvtEventManager::RemoveEvent
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtEventManager::RemoveEventL( TEvtEventId      aEvtId )
    {
	EVTUIDEBUG( "+ CEvtEventManager::RemoveEventL()" );
    // redirect the call to CEvtDatabase
	iEvtDb->RemoveEventL( aEvtId );
	EVTUIDEBUG( "- CEvtEventManager::RemoveEventL()" );
    } 
    
// ---------------------------------------------------------------------------
// CEvtEventManager::SetObserver
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtEventManager::SetObserver( MEvtStorageDbObserver*      
																aDbObserver )
    {
	EVTUIDEBUG( "+ CEvtEventManager::SetObserver()" );
    // redirect the call to CEvtDatabase
	iEvtDb->SetObserver( aDbObserver );
	EVTUIDEBUG( "- CEvtEventManager::SetObserver()" );
    } 

// End of File
