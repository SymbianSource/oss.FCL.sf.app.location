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
* Description:  Tone class definition.
*
*/


// System Includes
#include <e32math.h>
#include <s32mem.h>

// User Includes
#include "evttoneaction.h"
#include "evtdebug.h"

// ================ Member funtions for CEvtToneAction class ======================

// ---------------------------------------------------------------------------
// CEvtToneAction::CEvtToneAction
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtToneAction::CEvtToneAction():
	iToneLoop( EFalse )
    {    
    }
    
// ---------------------------------------------------------------------------
// CEvtToneAction::~CEvtToneAction
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtToneAction::~CEvtToneAction()
    {
    delete iFileName;
    } 
    
// ---------------------------------------------------------------------------
// CEvtToneAction::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtToneAction* CEvtToneAction::NewL()
    {
	CEvtToneAction* self = NewLC( );
	CleanupStack::Pop( self );
	return self;
    }

// ---------------------------------------------------------------------------
// CEvtToneAction::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtToneAction* CEvtToneAction::NewLC()
    {
	CEvtToneAction* self = new ( ELeave )CEvtToneAction( );
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
    } 

// ---------------------------------------------------------------------------
// CEvtToneAction::ConstructL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtToneAction::ConstructL()
    {
    // Allocate the Null string by default
    iFileName = KNullDesC().AllocL();
    } 

// ---------------------------------------------------------------------------
// CEvtToneAction::Tone
// ---------------------------------------------------------------------------
//

EXPORT_C TPtrC CEvtToneAction::FileName() const
    {
    return iFileName->Des();
    } 
    
// ---------------------------------------------------------------------------
// CEvtToneAction::SetToneL
// ---------------------------------------------------------------------------
//

EXPORT_C void CEvtToneAction::SetFileNameL( const TDesC& aFileName )
    {
    delete iFileName;
	iFileName = NULL;
    iFileName = aFileName.AllocL();
    } 
    
// ---------------------------------------------------------------------------
// CEvtToneAction::ToneLoop
// ---------------------------------------------------------------------------
//

EXPORT_C TBool CEvtToneAction::ToneLoop() const
    {
    return iToneLoop;
    } 
    
// ---------------------------------------------------------------------------
// CEvtToneAction::SetToneLoop
// ---------------------------------------------------------------------------
//

EXPORT_C void CEvtToneAction::SetToneLoop( const TBool aToneLoop )
    {
    iToneLoop = aToneLoop;
    } 
    
// ---------------------------------------------------------------------------
// CEvtToneAction::ExternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtToneAction::ExternalizeL( CEvtAction&   aAction )
    {
    CBufFlat* buffer = CBufFlat::NewL( TotalSize() );
    CleanupStack::PushL( buffer );
    
    // Allocate a common buffer for both read and write streams
    RBufWriteStream writestream( *buffer, 0 );
    
    // Externalize from CEvtToneAction
    ExternalizeL( writestream );
    
    // Covert the 8 bit Stream to 16 bit descriptor.
    TPtr8 bufPtr = buffer->Ptr(0);
    TPtrC display;
    display.Set( (TUint16*)bufPtr.Ptr(), (bufPtr.Length()>>1) );

    // Internalize from CEvtAction
    aAction.SetActionL( display );
    
    CleanupStack::PopAndDestroy( buffer );
    } 
    
// ---------------------------------------------------------------------------
// CEvtToneAction::InternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtToneAction::InternalizeL( CEvtAction&   aAction )
    {
    // Internalize from CEvtAction
    TPtrC ptr = aAction.Action( );
    
    // Create a buffer of ptr's size granularity
    CBufFlat* buffer = CBufFlat::NewL( ptr.Size() );
    CleanupStack::PushL( buffer );
	
	// Insert the action String into buffer
    buffer->InsertL( 0, (TAny*)ptr.Ptr(), (ptr.Length()<<1) );
    
    // Allocate a common buffer for both read and write streams
    RBufReadStream readstream( *buffer, 0 );
        
    // Internalize from CEvtToneAction
    InternalizeL( readstream );
    
    CleanupStack::PopAndDestroy( buffer );
    } 

// ---------------------------------------------------------------------------
// CEvtToneAction::ExternalizeL
// ---------------------------------------------------------------------------
//
void CEvtToneAction::ExternalizeL( RWriteStream&   aWriteStream )
    {
    // Externalize will happen in the order of
    // - Action Type
    // - Tone String length
    // - Tone String
    // - Tone Loop
    TInt length = iFileName->Length();
    aWriteStream.WriteUint8L( EAlarm );
    aWriteStream.WriteUint32L( length );
    
    // Avoid WriteL, if the Tone String length is 0.
    if( 0 != length )
        aWriteStream.WriteL( iFileName->Des(), length );
    aWriteStream.WriteUint8L( iToneLoop );
    
    } 
    
// ---------------------------------------------------------------------------
// CEvtToneAction::InternalizeL
// ---------------------------------------------------------------------------
//
void CEvtToneAction::InternalizeL( RReadStream&    aReadStream )
    {
    // Internalize will happen in the order of
    // - Action Type
    // - Tone String length
    // - Tone String
    // - Tone Loop
    TEvtActionType type;    
    type = static_cast<TEvtActionType>(aReadStream.ReadUint8L());
    
    // Leave if the Action type is not EAlarm
    if( EAlarm != type )
        User::Leave( KErrArgument );
    
    // Get the length of tone String.
    TInt length = aReadStream.ReadUint32L();
    if( iFileName )
        delete iFileName;
    
    // Avoid ReadL, if the Tone String length is 0.
    if( 0 != length )
        {
        iFileName = HBufC::NewL( length );
        TPtr actn( iFileName->Des() );
    
	    // ReadL can leave if we try to read any intermediate 8 bit 
	    // descriptor. So we can ignore it.
        TRAP_IGNORE( aReadStream.ReadL(actn, length));
        }
    else
        {
        iFileName = KNullDesC().AllocL();
        }
    
    // Get the Toon Loop
    iToneLoop = static_cast<TBool>( aReadStream.ReadUint8L() );
    } 
    
// ---------------------------------------------------------------------------
// CEvtToneAction::TotalSize
// ---------------------------------------------------------------------------
//
TInt CEvtToneAction::TotalSize( )
    {
    TInt size=0;
    size += sizeof( TUint8 );                   // Type Size
    size += sizeof( TUint32 );                  // Tone Length Size
    size += iFileName->Des().MaxSize();       // Tone Size
    size += sizeof( TUint8 );                   // Loop Size
    
	return size;
    } 

// End of File
