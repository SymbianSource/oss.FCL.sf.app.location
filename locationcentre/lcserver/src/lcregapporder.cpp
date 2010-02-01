/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Location Centre Server object.
*
*/


// SYSTEM INCLUDES
#include<s32mem.h>

// USER INCLUDES
#include "lcregapporder.h"
#include "lcprivatecrkeys.h"
#include "lcregappstore.h"

// CONSTANT DEFINTIONS
const TInt KLcNoofDigitsInInteger = 4;
const TInt KLcMaxBufLength = 1024;

// ----- Member funtions for CLcRegAppOrder ---------------------------------

// ---------------------------------------------------------------------------
// CLcRegAppOrder::CLcRegAppOrder
// ---------------------------------------------------------------------------
//
CLcRegAppOrder::CLcRegAppOrder( CLcRegAppStore& aLcRegAppStore )
	:iLcRegAppStore( &aLcRegAppStore )
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.Initiallize all the variable here
    }
         
// ---------------------------------------------------------------------------
// CLcRegAppOrder::~CLcRegAppOrder
// ---------------------------------------------------------------------------
//
CLcRegAppOrder::~CLcRegAppOrder()
    {
    // C++ Destructor. Free all resources associated with this class.
	delete iRepository;
	iRepository = NULL;	
	// clear the top key value.
	iTopArray.ResetAndDestroy();
	iTopArray.Close();
	// clear the bottom key value
	iBottomArray.ResetAndDestroy();
	iBottomArray.Close();
	// clear middle uuid array
	iMiddleArray.ResetAndDestroy();
	iMiddleArray.Close();
    }
        
// ---------------------------------------------------------------------------
// CLcRegAppOrder* CLcRegAppOrder::NewL
// ---------------------------------------------------------------------------
//
CLcRegAppOrder* CLcRegAppOrder::NewL( CLcRegAppStore& aLcRegAppStore )
    {
    // Symbian Two phased constructor. Leaves the object on the Clean-up
    // stack.
    CLcRegAppOrder* self = new ( ELeave )CLcRegAppOrder( aLcRegAppStore );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;         
    }

// ---------------------------------------------------------------------------
// void CLcRegAppOrder::ConstructL
// ---------------------------------------------------------------------------
//
void CLcRegAppOrder::ConstructL()
    {
	// Create the Central repository object for manipulating Avkon Central
	// repository for application order key
	iRepository = CRepository::NewL( KCRUidLocationCentre );
    HBufC* buffer = HBufC::NewLC( NCentralRepositoryConstants::KMaxUnicodeStringLength );
    TPtr bufferPtr( buffer->Des() );
    // get the top order services
    User::LeaveIfError( iRepository->Get( ELcTop, bufferPtr ) );    
	ParseCenRepKeyL( iTopArray, bufferPtr );
	// get the middle order services
	User::LeaveIfError( iRepository->Get( ELcBottom, bufferPtr ) );
	ParseCenRepKeyL( iBottomArray, bufferPtr );
	// get the bottom order services if any
	TBool isMidleKeySet = EFalse;
	User::LeaveIfError( iRepository->Get( ELcMiddle, isMidleKeySet ) );
	
	if( isMidleKeySet )
		{
        // Allocate the id Array buffer    
        CBufFlat* idArrayBuf = CBufFlat::NewL( KLcMaxBufLength );
        CleanupStack::PushL( idArrayBuf );
        idArrayBuf->ResizeL( KLcMaxBufLength );
        
        TPtr8   idArrayPtr( idArrayBuf->Ptr(0));
        
        iLcRegAppStore->GetUuidValueL( idArrayPtr );        
        
        if( idArrayPtr.Length() > 0 )
	        {
	        // Create a Read buffer stream to read the App Ids    
	        RBufReadStream  appReadStream( *idArrayBuf, 0 );
	        CleanupClosePushL( appReadStream );
	        
	        iMiddleArray.ResetAndDestroy();  
	                        
	        // Internalize the structure to obtain the Actual list of Ids    
	        iMiddleArray.InternalizeL( appReadStream );  
	        CleanupStack::PopAndDestroy(); // appReadStream
	        }
        CleanupStack::PopAndDestroy( idArrayBuf );
		}
	else
		{
		// Deep RFS handling
	    iLcRegAppStore->AddDataToTableL( KNullDesC8 );	
		}
		
    CleanupStack::PopAndDestroy( buffer );   
    }  

// ---------------------------------------------------------------------------
// void CLcRegAppOrder::ParseCenRepKey
// ---------------------------------------------------------------------------
//
void CLcRegAppOrder::ParseCenRepKeyL( RPointerArray< HBufC >&	aArray,
    				     			 const TDesC&				aKey )
	{
	// If the length of the buffer is less than 4 digits, then terminate
	// the function.
	if( aKey.Length() < KLcNoofDigitsInInteger )
		{
		return;
		}
	TUint count = ParseInteger( aKey );
	TInt currentlength = KLcNoofDigitsInInteger;
		
	for ( TInt i = 0; i < count; i ++ )
		{
		// If the length of the buffer is less than 4 digits, then terminate
		// the function.		
		if( aKey.Right( aKey.Length() - currentlength ).Length() < KLcNoofDigitsInInteger )
			{
			return;
			}
			
		TUint32 len = ParseInteger( aKey.Right( aKey.Length() - currentlength ));
		currentlength += KLcNoofDigitsInInteger;
		
		TPtrC buffer( aKey.Right( aKey.Length() - currentlength ));
		
		// If the length of the buffer is less than the length obtained from the
		// previous operation then terminate the function.
		if( buffer.Length() < len )
			{
			return;
			}
		HBufC* identifier = HBufC::NewLC( currentlength );
		identifier->Des().Copy( buffer.Ptr(), len );
		
		TInt error = aArray.Append( identifier );
		if( error )
			{
			CleanupStack::PopAndDestroy( identifier );
			}
		else
			{
			CleanupStack::Pop( identifier );
			}
		currentlength += len;			
		}
	}
    				     
// ---------------------------------------------------------------------------
// void CLcRegAppOrder::ParseInteger
// ---------------------------------------------------------------------------
//
TUint32	CLcRegAppOrder::ParseInteger( const TDesC&	aBuffer )
	{
	TPtrC buffer( aBuffer.Ptr(), KLcNoofDigitsInInteger );
		
	TBuf8< KLcNoofDigitsInInteger > integerBuf;
	integerBuf.Copy( buffer );
	
	TUint8*	currentPtr = const_cast< TUint8 *>( integerBuf.Ptr());
	TUint32 integer = 0;
	for( TInt i = 0; i < KLcNoofDigitsInInteger; i++ )
		{
		integer += ( integer * 10 ) + ( currentPtr[i] - '0' );
		}
	return integer;
	}
	
// ---------------------------------------------------------------------------
// void CLcRegAppOrder::GetRegisteredAppOrder
// ---------------------------------------------------------------------------
//
TInt CLcRegAppOrder::GetRegisteredAppOrder(const TDesC& aUuid,
												 TInt& aRowPos )
    {
    // search the identifier in the top array.
    for(TInt i = 0; i <iTopArray.Count(); i++  )
	    {
	    HBufC* identifier = iTopArray[i];
	    if( !identifier->Des().Compare( aUuid ) )
		    {
			aRowPos = i;
			return KErrNone;	    			    	
		    }
	    }
	    
    // search the identifier in the middle array.
    for(TInt i = 0; i <iMiddleArray.Count(); i++  )
	    {
	    HBufC* identifier = iMiddleArray[i];
	    if( !identifier->Des().Compare( aUuid ) )
		    {
			aRowPos = iTopArray.Count() + i;
			return KErrNone;	    			    	
		    }
	    }
	    	
    // search the identifier in the top array.
    for(TInt i = 0; i <iBottomArray.Count(); i++  )
	    {
	    HBufC* identifier = iBottomArray[i];
	    if( !identifier->Des().Compare( aUuid ) )
		    {
			aRowPos = iTopArray.Count() + iMiddleArray.Count() + i;
			return KErrNone;	    			    	
		    }
	    }
	return KErrNotFound;	    
    }  

// ---------------------------------------------------------------------------
// void CLcRegAppOrder::RemoveArray
// ---------------------------------------------------------------------------
//
void CLcRegAppOrder::RemoveArrayL(const TDesC& aUuid )
    {
    // search the identifier in the middle array.
    for(TInt i = 0; i <iMiddleArray.Count(); i++  )
	    {
	    HBufC* identifier = iMiddleArray[i];
	    if( !identifier->Des().Compare( aUuid ) )
		    {
			iMiddleArray.Remove(i);
			// now delete the identifier
			delete identifier;
			identifier = NULL;
			break;	    			    	
		    }
	    }
	    
    // Create the buffer for packing the Application Information
    // structure and pack the contents into this buffer
    CBufFlat* buffer = CBufFlat::NewL( iMiddleArray.BufferLength());
    CleanupStack::PushL( buffer );
    
    RBufWriteStream writeStream( *buffer, 0 );
    CleanupClosePushL( writeStream );                
    iMiddleArray.ExternalizeL( writeStream );
    CleanupStack::PopAndDestroy(); // writeStream    	
    iLcRegAppStore->AddDataToTableL( buffer->Ptr(0));
	CleanupStack::PopAndDestroy( buffer );
	// Now get the id index.
	if( iMiddleArray.Count() == 0 )
		{
		User::LeaveIfError( iRepository->Set( ELcMiddle, 0 ));			
		}
    }  

// ---------------------------------------------------------------------------
// void CLcRegAppOrder::AppendArray
// ---------------------------------------------------------------------------
//
TInt CLcRegAppOrder::AppendArrayL(const TDesC& aUuid )
    {
    HBufC* identifier = HBufC::NewL( aUuid.Length() );
    identifier->Des().Copy( aUuid );	
	User::LeaveIfError( iMiddleArray.Append( identifier ));
    // Create the buffer for packing the Application Information
    // structure and pack the contents into this buffer
    CBufFlat* buffer = CBufFlat::NewL( iMiddleArray.BufferLength());
    CleanupStack::PushL( buffer );
    
    RBufWriteStream writeStream( *buffer, 0 );
    CleanupClosePushL( writeStream );                
    iMiddleArray.ExternalizeL( writeStream );
    CleanupStack::PopAndDestroy(); // writeStream
   	
	iLcRegAppStore->AddDataToTableL( buffer->Ptr(0));
	
	CleanupStack::PopAndDestroy( buffer );

	TInt index;
	if ( GetRegisteredAppOrder(aUuid, index ) == KErrNotFound )
		{
		User::Leave( KErrNotFound );
		}
	User::LeaveIfError( iRepository->Set( ELcMiddle, 1 ));	
	return index;			
    }
    
//End of file


