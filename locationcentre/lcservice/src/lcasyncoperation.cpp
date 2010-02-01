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
* Description:  Handles all the Asynchronous operations with the Location
*                Centre Client Session
*
*/


// SYSTEM INCLUDES
#include <s32mem.h>

// USER INCLUDES
#include "lcasyncoperation.h"
#include "lcclientsession.h"
#include "lcipcparams.h"
#include "lcsyncoperation.h"

// CONSTANT DEFINTIONS
const TInt  KLcLengthofInteger  = 4;

// ----- Member functions for TLcAppInfoContainer ----------------------------

CLcAsyncOperation::TLcAppInfoContainer::TLcAppInfoContainer( 
						CLcLocationAppInfoArray*&    aAppInfoArray )
	:iAppInfoArray( aAppInfoArray )						
	{
	}
	
// ----- Member funtions for LcSyncOperation ---------------------------------

// ---------------------------------------------------------------------------
// CLcAsyncOperation::CLcAsyncOperation
// ---------------------------------------------------------------------------
//
CLcAsyncOperation::CLcAsyncOperation( RLcClientSession&  		aSession,
    								MLcAsynOperationObserver&	aObserver )
	:CActive( EPriorityStandard ),
	iClientSession( aSession ),
	iObserver( aObserver ),
	iBufferPtr( NULL, 0 )
	{
	CActiveScheduler::Add( this );	
	}

// ---------------------------------------------------------------------------
// CLcAsyncOperation::~CLcAsyncOperation
// ---------------------------------------------------------------------------
//	
CLcAsyncOperation::~CLcAsyncOperation()
	{
	// Cancel any outstanding request.
	Cancel();
	
	// Delete the filter
	delete iFilterBuffer;
	
	// Delete the Buffer pointer
	delete iBuffer;
	
	// Delete the App Info array container
	delete iAppInfoArrayContainer;
	}

// ---------------------------------------------------------------------------
// CLcAsyncOperation* CLcAsyncOperation::NewL
// ---------------------------------------------------------------------------
//
CLcAsyncOperation* CLcAsyncOperation::NewL( RLcClientSession&  		    aSession,
    										MLcAsynOperationObserver&	aObserver )
	{
	CLcAsyncOperation* self = new ( ELeave ) CLcAsyncOperation( aSession, aObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

// ---------------------------------------------------------------------------
// void CLcAsyncOperation::ConstructL
// ---------------------------------------------------------------------------
//
void CLcAsyncOperation::ConstructL()
	{
	// Construct the Buffer structure used for exchanging information with the
	// Location Centre Server.
    iBuffer = CBufFlat::NewL( KLcLengthofInteger );
	}

// ---------------------------------------------------------------------------
// void CLcAsyncOperation::GetLocationApplicationsL
// ---------------------------------------------------------------------------
//	
void CLcAsyncOperation::GetLocationApplicationsL(
                     const TLcLocationAppFilter&        aLocationAppFilter,
                           CLcLocationAppInfoArray*&    aAppInfoArray )
	{
	// Check if there was any request outstanding. Incase, there was any
	// request then the request will leave with KErrInUse. 
	if ( IsActive())
		{
		User::Leave( KErrInUse );
		}
	// Copy the Filter parameters to the Filter member variable.
	iAppFilter = aLocationAppFilter;
	
	iFilterBuffer = new ( ELeave ) TPckg< TLcLocationAppFilter >( iAppFilter );
	
	// Hold the Array pointer
	iAppInfoArrayContainer = new ( ELeave )TLcAppInfoContainer( aAppInfoArray );
	
	// Set the Operation to the Initial state and issue a new request.
	GetLengthL();				
	}

// ---------------------------------------------------------------------------
// void CLcAsyncOperation::CancelGetLocationApplications
// ---------------------------------------------------------------------------
//
void CLcAsyncOperation::CancelGetLocationApplications()
	{
	Cancel();
	}

// ---------------------------------------------------------------------------
// void CLcAsyncOperation::ReIssueRequestL
// ---------------------------------------------------------------------------
//	
void CLcAsyncOperation::ReIssueRequestL()
	{	
	// Check if there are any operation outstanding. This request will be valid
	// only when there are no requests outstanding.
	if ( ELcNoOperation != iOperation )
		{	
		// Rest the State to the initial state and Issue a new request
		GetLengthL();
		}			
	}

// ---------------------------------------------------------------------------
// void CLcAsyncOperation::GetLengthL
// ---------------------------------------------------------------------------
//
void CLcAsyncOperation::GetLengthL()
	{
	if ( !IsActive())
		{
		
		// Set the size of this buffer to 4. This is required because we dont
	    // actually fill this buffer but expect the server to fill it. In that case
	    // if we dont set the expected length, the server would fail with
	    // KrrBadDescriptor.
	    iBuffer->ResizeL( KLcLengthofInteger );
	    
	    // Fill the IPC argument structure with the Length buffer, the server
	    // will write the data onto this buffer.
	    
	    // By the IPC exchange parameter defintion, this must be the first
	    // argument to the IPC message.
	    iIpcArgs.Set( 0, iFilterBuffer );
	    
	    // Set the buffer pointer to the start of the Length buffer
	    iBufferPtr.Set( const_cast< TUint8 *>( iBuffer->Ptr( 0 ).Ptr()),
	                    KLcLengthofInteger,
	                    KLcLengthofInteger );
	    
	    // This will be the second argument passed to this IPC message.
	    iIpcArgs.Set( 1, &iBufferPtr);
	    
	    // Send an asynchronous message to the server to obtain the length. On return the
	    // server is expected to pack the length of the Application information
	    // arrays in the LengthBuffer pointer.
	    iClientSession.SendReceive( ELcFilteredAppsBufferLength, 
	    							iIpcArgs,
	    							iStatus );
		iOperation = ELcGetAppLength;
		SetActive();
		}
	}

// ---------------------------------------------------------------------------
// void CLcAsyncOperation::RunL
// ---------------------------------------------------------------------------
//	
void CLcAsyncOperation::RunL()
	{
	if( !iStatus.Int())
		{
		switch ( iOperation )
			{
			case ELcGetAppLength:
				{
				// The Server has passed the Length of the buffer that is needed to pack
				// the Location Applications.
				
			    // If the server has not set the buffer then leave with KErrNotFound
			    if ( !iBufferPtr.Length())
			        {
			        User::Leave( KErrNotFound );
			        }
			    
			    // Obtain the length from the Length buffer;
			    RBufReadStream  readStream( *iBuffer, 0 );
			    CleanupClosePushL( readStream );
			    TUint length = readStream.ReadInt32L();
			    CleanupStack::PopAndDestroy();   // readStream
			    
				// If the server has returned a length of 0, then there are no applications
				// registered with Location Centre.
				if ( !length )
				    {
				    User::Leave( KErrNotFound );
				    }
				    
			    // Set the actual size to 'length' obtained in the previous IPC. This is required
			    // because we dont actually fill this buffer but expect the server to fill it.
			    // In that case if we dont set the expected length, the server would fail with
			    // KrrBadDescriptor.
			    iBuffer->ResizeL( length );
			    
			    // Fill the IPC argument structure with the Application info buffer, the server
			    // will write the data onto this buffer.
			    TIpcArgs    args;
			    	    
			    // Pass the filter parameters to the Location Centre Server
			    // By the IPC exchange parameter defintion, this must be the first
			    // argument to the IPC message.
			    args.Set( 0, iFilterBuffer );
			    
			    // Set the buffer pointer to the start of the Length buffer
			    iBufferPtr.Set( const_cast< TUint8 *>( iBuffer->Ptr( 0 ).Ptr()),
			                    length,
			                    length );
			    
			    // This will be the second argument passed to this IPC message.
			    args.Set( 1, &iBufferPtr);
			    
			    // Send an Asynchrnous message to the server to obtain the array of Applications. 
			    iClientSession.SendReceive( ELcFilteredApps, 
			    					        args,
			    					  		iStatus );
				
				iOperation = ELcGetApp;
				SetActive();    
				    				
				break;
				}
			case ELcGetApp:
				{
				// The server has successfully returned the Location based Applications
				
			    // If the server has not set the buffer then leave with KErrNotFound
			    if ( !iBufferPtr.Length())
			        {
			        User::Leave( KErrNotFound );
			        }
			        				
			    // Parse the Application information array to obtain the array
			    RBufReadStream  appReadStream( *iBuffer, 0 );
			    CleanupClosePushL( appReadStream );
			    
			    iAppInfoArrayContainer->iAppInfoArray = 
			    			LcSyncOperation::ParseLocAppBufferL( appReadStream );
			    
			    // The App Info array container has been used. We can now free the
			    // memory for the same.
			    delete iAppInfoArrayContainer;
			    iAppInfoArrayContainer = NULL;
			    
			    CleanupStack::PopAndDestroy();   // appReadStream

				// The request has been successfully processed.
				iObserver.OperationComplete( KErrNone );
				
				break;
				}				
			default:
				{
				// This condition should never occur
				break;
				}
			}
		}
	else
		{
		// If the Retrieval function completes with KErrOverflow then there has been
		// an update to the registry since the time we obtained the length. We need 
		// to re-issue the request to obtain the Length from the application		
		if( iOperation == ELcGetApp && KErrOverflow == iStatus.Int())
			{
			ReIssueRequestL();
			}
		else
			{
			// All other condtions terminate
			
			// There is nothing to be done to the App Info buffer. Delete it	
			delete iAppInfoArrayContainer;
			iAppInfoArrayContainer = NULL;
				    		
			// There is an error from the Server side. Hence, complete the request
			// to the Client Side.
			iObserver.OperationComplete( iStatus.Int());			
			}			
		}
	}

// ---------------------------------------------------------------------------
// void CLcAsyncOperation::DoCancel
// ---------------------------------------------------------------------------
//	
void CLcAsyncOperation::DoCancel()
	{
    iClientSession.SendReceive( ELcCancelFilteredApps );
    
    // Clear the App Info array container
	delete iAppInfoArrayContainer;
	iAppInfoArrayContainer = NULL;    
	}

// ---------------------------------------------------------------------------
// TInt CLcAsyncOperation::RunError
// ---------------------------------------------------------------------------
// 
TInt CLcAsyncOperation::RunError( TInt aError )
	{
    // Clear the App Info array container
	delete iAppInfoArrayContainer;
	iAppInfoArrayContainer = NULL;
	
	// The RunL has left with an Error, Notify the Client of the same
	iObserver.OperationComplete( aError );
	return KErrNone;
	}
    	                                                                                                            
// End of File
