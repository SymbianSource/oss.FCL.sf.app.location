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
* Description:  Class for the Location Events Action.
*
*/


// System Includesc
#include <s32mem.h>

// User Includes
#include "evtaction.h"
#include "evtdebug.h"

// ================ Member funtions for CEvtAction class ======================

// ---------------------------------------------------------------------------
// CEvtAction::CEvtAction
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtAction::CEvtAction():iId(0),
	iType( EAlarm )
    {    
    }
    
// ---------------------------------------------------------------------------
// CEvtAction::~CEvtAction
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtAction::~CEvtAction()
    {
    delete iActionString;
    
    } 
    
// ---------------------------------------------------------------------------
// CEvtAction::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtAction* CEvtAction::NewL()
    {
	CEvtAction* self = NewLC( );
	CleanupStack::Pop( self );
	return self;
    }

// ---------------------------------------------------------------------------
// CEvtAction::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtAction* CEvtAction::NewLC()
    {
	CEvtAction* self = new ( ELeave )CEvtAction( );
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
    } 

// ---------------------------------------------------------------------------
// CEvtAction::ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtAction::ConstructL()
    {
    // Allocate the Null string by default
    iActionString = KNullDesC().AllocL();
    } 
    
// ---------------------------------------------------------------------------
// CEvtAction::Id
// ---------------------------------------------------------------------------
//
TInt64 CEvtAction::Id() const
    {
    return iId;
    } 
    
// ---------------------------------------------------------------------------
// CEvtAction::SetId
// ---------------------------------------------------------------------------
//
void CEvtAction::SetId( const TInt64 aId )
    {
    iId = aId;
    } 
    
// ---------------------------------------------------------------------------
// CEvtAction::EvtId
// ---------------------------------------------------------------------------
//
TEvtEventId CEvtAction::EvtId() const
    {
    return iEvtId;
    } 
    
// ---------------------------------------------------------------------------
// CEvtAction::SetEvtId
// ---------------------------------------------------------------------------
//
void CEvtAction::SetEvtId( const TEvtEventId aEvtId )
    {
    iEvtId = aEvtId;
    } 

// ---------------------------------------------------------------------------
// CEvtAction::Action
// ---------------------------------------------------------------------------
//
TPtrC CEvtAction::Action() const
    {
    return iActionString->Des();
    } 
    
// ---------------------------------------------------------------------------
// CEvtAction::SetActionL
// ---------------------------------------------------------------------------
//
void CEvtAction::SetActionL( const TDesC& aAction )
    {
    delete iActionString;
    iActionString = NULL;
    iActionString = aAction.AllocL();
    
    // return, if the String length is 0.
    // Since we can not retrieve type of Action
    if( 0 == iActionString->Length() )
        return;
    
	// Retrieve action type
    CBufFlat* lengthBuf = CBufFlat::NewL( iActionString->Length() );
    CleanupStack::PushL( lengthBuf );
    lengthBuf->InsertL( 0, iActionString->Ptr(), iActionString->Length() );
    
    // Create a stream to buffer
    RBufReadStream writeStream( *lengthBuf, 0 );
    CleanupClosePushL( writeStream ); 
    
    // Extract action type info
    iType = static_cast<TEvtActionType>( writeStream.ReadInt8L() );
    
    CleanupStack::PopAndDestroy(); // writeStream
    CleanupStack::PopAndDestroy( lengthBuf ); // lengthBuf
      
    } 
    
// ---------------------------------------------------------------------------
// CEvtAction::Type
// ---------------------------------------------------------------------------
//
EXPORT_C TEvtActionType CEvtAction::Type() const
    {
    return iType;
    } 
    
// ---------------------------------------------------------------------------
// CEvtAction::SetType
// ---------------------------------------------------------------------------
//
void CEvtAction::SetType( const TEvtActionType aType )
    {
    iType = aType;
    } 
    
// End of File
