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
* Description:  Handles all the Synchronous operations with the Location
*                Centre Client Session
*
*/


// SYSTEM INCLUDES
#include <s32mem.h>

// USER INCLUDES
#include "lcsyncoperation.h"
#include "lcclientsession.h"
#include "lcipcparams.h"
#include "lcdebug.h"

// CONSTANT DEFINTIONS
const TInt  KLcLengthofInteger  = 4;

// ----- Member funtions for LcSyncOperation ---------------------------------

// ---------------------------------------------------------------------------
// CLcLocationAppInfoArray* CLcServiceImpl::GetLocationApplicationsL
// ---------------------------------------------------------------------------
//    
CLcLocationAppInfoArray* LcSyncOperation::GetLocationApplicationsL(
											  RLcClientSession&   	aSession,
							            const TLcLocationAppFilter& aLocationAppFilter )
    {
    DEBUG("+ LcSyncOperation::GetLocationApplicationsL")
    DEBUG("Filtering")
    
    // This function needs to obtain a list of all applications. This would
    // first require to obtain the length of the buffer that needs to be
    // passed from the client to the server to pack the array.
    CBufFlat* lengthBuf = CBufFlat::NewL( KLcLengthofInteger );
    CleanupStack::PushL( lengthBuf );
    
    // Set the size of this buffer to 4. This is required because we dont
    // actually fill this buffer but expect the server to fill it. In that case
    // if we dont set the expected length, the server would fail with
    // KrrBadDescriptor.
    lengthBuf->ResizeL( KLcLengthofInteger );
    
    // Fill the IPC argument structure with the Length buffer, the server
    // will write the data onto this buffer.
    TIpcArgs    args;
    
    // Pass the filter parameters to the Location Centre Server
    TPckg< TLcLocationAppFilter >    filterBuffer( aLocationAppFilter );
    
    // By the IPC exchange parameter defintion, this must be the first
    // argument to the IPC message.
    args.Set( 0, &filterBuffer );
    
    // Set the buffer pointer to the start of the Length buffer
    TPtr8 bufPtr( const_cast< TUint8 *>( lengthBuf->Ptr( 0 ).Ptr()),
                  0,
                  KLcLengthofInteger );
    
    // This will be the second argument passed to this IPC message.
    args.Set( 1, &bufPtr);
    
    // Send a synchrnous message to the server to obtain the length. On return the
    // server is expected to pack the length of the Application information
    // arrays in the LengthBuffer pointer.
    User::LeaveIfError( aSession.SendReceive( ELcFilteredAppsBufferLength, args ));

    // If the server has not set the buffer then leave with KErrNotFound
    if ( !bufPtr.Length())
        {
        User::Leave( KErrNotFound );
        }
    
    // Obtain the length from the Length buffer;
    RBufReadStream  readStream( *lengthBuf, 0 );
    CleanupClosePushL( readStream );
    TUint length = readStream.ReadInt32L();
    CleanupStack::PopAndDestroy( 2, lengthBuf );   // readStream
    
    // If the server has returned a length of 0, then there are no applications
    // registered with Location Centre.
    if ( !length )
        {
        User::Leave( KErrNotFound );
        }
    
    // Now that the length has been obtained. Allocate the descriptor
    // of suitable length and send it across to the server.
    CBufFlat* appInfoBuf = CBufFlat::NewL( length );
    CleanupStack::PushL( appInfoBuf );
    
    // Set the actual size to 'length' obtained in the previous IPC. This is required
    // because we dont actually fill this buffer but expect the server to fill it.
    // In that case if we dont set the expected length, the server would fail with
    // KrrBadDescriptor.
    appInfoBuf->ResizeL( length );
    
    // Fill the IPC argument structure with the Application info buffer, the server
    // will write the data onto this buffer.
    
    // Pass the filter parameters to the Location Centre Server
    // By the IPC exchange parameter defintion, this must be the first
    // argument to the IPC message.
    args.Set( 0, &filterBuffer );
    
    // Set the buffer pointer to the start of the Length buffer
    bufPtr.Set( const_cast< TUint8 *>( appInfoBuf->Ptr( 0 ).Ptr()),
                0,
                length );
    
    // This will be the second argument passed to this IPC message.
    args.Set( 1, &bufPtr);
    
    // Send a synchrnous message to the server to obtain the length. On return the
    // server is expected to pack the length of the Application information
    // arrays in the LengthBuffer pointer.
    TInt error =  aSession.SendReceive( ELcFilteredApps, args );
    
	// If the Retrieval function completes with KErrOverflow then there has been
	// an update to the registry since the time we obtained the length.
	if ( KErrOverflow == error )
		{
		// Cleanup the current state and Re-issue the same request again
		CleanupStack::PopAndDestroy( appInfoBuf );
		return GetLocationApplicationsL( aSession, aLocationAppFilter );
		}
	else if( error )
		{
		User::Leave( error );
		}

    // If the server has not set the buffer then leave with KErrNotFound
    if ( !bufPtr.Length())
        {
        User::Leave( KErrNotFound );
        }
        
    // Parse the Application information array to obtain the array
    RBufReadStream  appReadStream( *appInfoBuf, 0 );
    CleanupClosePushL( appReadStream );
    
    CLcLocationAppInfoArray* array = ParseLocAppBufferL( appReadStream );
    
    CleanupStack::PopAndDestroy( 2, appInfoBuf );   // appReadStream

    DEBUG("- LcSyncOperation::GetLocationApplicationsL")
    
    return array;

    }

// ---------------------------------------------------------------------------
// CLcLocationAppInfoArray* LcSyncOperation::GetLocationApplicationsL
// ---------------------------------------------------------------------------
//     
CLcLocationAppInfoArray* LcSyncOperation::GetLocationApplicationsL(
                                      RLcClientSession&   		aSession,
                                      const RArray<TPtrC>&      aAppArray,
                                            TBool               aIncludeFlag )
    {
        
    // Obtain the array of Application Identifiers in a buffer
    RLcIpcAppIdArray        idArray;
    CleanupStack::PushL( TCleanupItem( RLcIpcAppIdArray::ResetAndDestroyIdArray, &idArray ));
    
    // Append all the identifiers to the Array    
    for ( TInt i = 0; i < aAppArray.Count(); i++ )
        {
        // Create a new element
        HBufC* newElement = HBufC::NewLC( aAppArray[i].Length());
        
        // Copy the contents to the buffer and append it to the array
        newElement->Des().Copy( aAppArray[i] );
        User::LeaveIfError( idArray.Append( newElement ));
        
        // Now that the ownership is transferred, the content can be removed from
        // the Cleanup stack
        CleanupStack::Pop();
        }
    
    // Create the buffer for packing the Application Information
    // structure and pack the contents into this buffer
    CBufFlat* buffer = CBufFlat::NewL( idArray.BufferLength());
    CleanupStack::PushL( buffer );
    
    RBufWriteStream writeStream( *buffer, 0 );
    CleanupClosePushL( writeStream );                
    idArray.ExternalizeL( writeStream );
    CleanupStack::PopAndDestroy(); // writeStream
                   
    // This needs to be popped inorder to Close the idArray;
    CleanupStack::Pop( 2 ); // buffer, idArray
    idArray.ResetAndDestroy();
    idArray.Close();
    
    // Now reinsert the App buffer back into the Cleanupstack
    CleanupStack::PushL( buffer );
       
    // This function needs to obtain a list of all applications. This would
    // first require to obtain the length of the buffer that needs to be
    // passed from the client to the server to pack the array.
    
    TIpcArgs    args;
        
    // First pack the application id array to the IPC args
    // By the IPC exchange parameter defintion, this must be the first
    // argument to the IPC message.
    TPtr8 appArrayPtr( buffer->Ptr(0));
      
    args.Set( 0, &appArrayPtr );
     
    // The second argument is the Include flag
    args.Set( 1, aIncludeFlag );
               
    CBufFlat* lengthBuf = CBufFlat::NewL( KLcLengthofInteger );
    CleanupStack::PushL( lengthBuf );
    
    // Set the size of this buffer to 4. This is required because we dont
    // actually fill this buffer but expect the server to fill it. In that case
    // if we dont set the expected length, the server would fail with
    // KrrBadDescriptor.
    lengthBuf->ResizeL( KLcLengthofInteger );
 
    // Set the buffer pointer to the start of the Length buffer
    TPtr8 bufPtr( const_cast< TUint8 *>( lengthBuf->Ptr( 0 ).Ptr()),
                  0,
                  KLcLengthofInteger );
    
    // This will be the third argument passed to this IPC message.
    args.Set( 2, &bufPtr);
    
    // Send a synchrnous message to the server to obtain the length. On return the
    // server is expected to pack the length of the Application information
    // arrays in the LengthBuffer pointer.
    User::LeaveIfError( aSession.SendReceive( ELcSpecifiedAppsBufferLength, args ));

    // If the server has not set the buffer then leave with KErrNotFound
    if ( !bufPtr.Length())
        {
        User::Leave( KErrNotFound );
        }
    
    // Obtain the length from the Length buffer;
    RBufReadStream  readStream( *lengthBuf, 0 );
    CleanupClosePushL( readStream );
    TUint length = readStream.ReadInt32L();
    CleanupStack::PopAndDestroy( 2, lengthBuf );   // readStream
    
    // If the server has returned a length of 0, then there are no applications
    // registered with Location Centre.
    if ( !length )
        {
        User::Leave( KErrNotFound );
        }
    
    // Now that the length has been obtained. Allocate the descriptor
    // of suitable length and send it across to the server.
    CBufFlat* appInfoBuf = CBufFlat::NewL( length );
    CleanupStack::PushL( appInfoBuf );
    
    // Set the actual size to 'length' obtained in the previous IPC. This is required
    // because we dont actually fill this buffer but expect the server to fill it.
    // In that case if we dont set the expected length, the server would fail with
    // KrrBadDescriptor.
    appInfoBuf->ResizeL( length );
    
    // Fill the IPC argument structure with the Application info buffer, the server
    // will write the data onto this buffer.
    
    // Pass the Application ids to the Location Centre Server
    // By the IPC exchange parameter defintion, this must be the first
    // argument to the IPC message.
    args.Set( 0, &appArrayPtr );
    
    // The second argument is the Include flag
    args.Set( 1, aIncludeFlag );
    
    // Set the buffer pointer to the start of the Length buffer
    bufPtr.Set( const_cast< TUint8 *>( appInfoBuf->Ptr( 0 ).Ptr()),
                0,
                length );
    
    // This will be the third argument passed to this IPC message.
    args.Set( 2, &bufPtr);
    
    // Send a synchrnous message to the server to obtain the length. On return the
    // server is expected to pack the length of the Application information
    // arrays in the LengthBuffer pointer.
    TInt error = aSession.SendReceive( ELcSpecifiedApps, args );

	// If the Retrieval function completes with KErrOverflow then there has been
	// an update to the registry since the time we obtained the length.
	if ( KErrOverflow == error )
		{
		// Cleanup the current state and Re-issue the same request again
		CleanupStack::PopAndDestroy( 2, buffer );
		return GetLocationApplicationsL( aSession, aAppArray, aIncludeFlag );
		}
	else if( error )
		{
		User::Leave( error );
		}
		
		
    // If the server has not set the buffer then leave with KErrNotFound
    if ( !bufPtr.Length())
        {
        User::Leave( KErrNotFound );
        }
        
    // Parse the Application information array to obtain the array
    RBufReadStream  appReadStream( *appInfoBuf, 0 );
    CleanupClosePushL( appReadStream );
    
    CLcLocationAppInfoArray* array = LcSyncOperation::ParseLocAppBufferL( appReadStream );
    
    CleanupStack::PopAndDestroy( 3, buffer );   // idArray, appInfoBuf and buffer

    return array;  
    }
    
// ---------------------------------------------------------------------------
// CLcBasicAppInfo* LcSyncOperation::GetLocationAppInfoL
// ---------------------------------------------------------------------------
// 
CLcBasicAppInfo* LcSyncOperation::GetLocationAppInfoL( 
									RLcClientSession&   aSession,
							  const TDesC&				aIdentifier )
	{
    // This function needs to obtain the Length of the buffer which needs to be
    // passed to the Server to pack the Location Application Information.
    CBufFlat* lengthBuf = CBufFlat::NewL( KLcLengthofInteger );
    CleanupStack::PushL( lengthBuf );
    
    // Set the size of this buffer to 4. This is required because we dont
    // actually fill this buffer but expect the server to fill it. In that case
    // if we dont set the expected length, the server would fail with
    // KrrBadDescriptor.
    lengthBuf->ResizeL( KLcLengthofInteger );
    
    // Fill the IPC argument structure with the Length buffer, the server
    // will write the data onto this buffer.
    TIpcArgs    args;
    
    // By the IPC exchange parameter defintion, the first argument must the
    // Identifier name
    args.Set( 0, &aIdentifier );
    
    // Set the buffer pointer to the start of the Length buffer
    TPtr8 bufPtr( const_cast< TUint8 *>( lengthBuf->Ptr( 0 ).Ptr()),
                  0,
                  KLcLengthofInteger );
    
    // This will be the second argument passed to this IPC message.
    args.Set( 1, &bufPtr);
    
    // Send a synchrnous message to the server to obtain the length. On return the
    // server is expected to pack the length of the Application information
    // arrays in the LengthBuffer pointer.
    User::LeaveIfError( aSession.SendReceive( ELcAppInfoLength, args ));

    // If the server has not set the buffer then leave with KErrNotFound
    if ( !bufPtr.Length())
        {
        User::Leave( KErrNotFound );
        }
    
    // Obtain the length from the Length buffer;
    RBufReadStream  readStream( *lengthBuf, 0 );
    CleanupClosePushL( readStream );
    TUint length = readStream.ReadInt32L();
    CleanupStack::PopAndDestroy( 2, lengthBuf );   // readStream
    
    // If the server has returned a length of 0, then there are no applications
    // registered with Location Centre.
    if ( !length )
        {
        User::Leave( KErrNotFound );
        }
    
    // Now that the length has been obtained. Allocate the descriptor
    // of suitable length and send it across to the server.
    CBufFlat* appInfoBuf = CBufFlat::NewL( length );
    CleanupStack::PushL( appInfoBuf );
    
    // Set the actual size to 'length' obtained in the previous IPC. This is required
    // because we dont actually fill this buffer but expect the server to fill it.
    // In that case if we dont set the expected length, the server would fail with
    // KrrBadDescriptor.
    appInfoBuf->ResizeL( length );
    
    // Fill the IPC argument structure with the Application info buffer, the server
    // will write the data onto this buffer.
    
    // Pass the Application Identifier to the Location Centre Server
    // By the IPC exchange parameter defintion, this must be the first
    // argument to the IPC message.
    args.Set( 0, &aIdentifier );
    
    // Set the buffer pointer to the start of the Length buffer
    bufPtr.Set( const_cast< TUint8 *>( appInfoBuf->Ptr( 0 ).Ptr()),
                0,
                length );
    
    // This will be the second argument passed to this IPC message.
    args.Set( 1, &bufPtr);
    
    // Send a synchrnous message to the server to obtain the length. On return the
    // server is expected to pack the length of the Application information
    // in the buffer pointer.
    TInt error = aSession.SendReceive( ELcAppInfo, args );

	// If the Retrieval function completes with KErrOverflow then there has been
	// an update to the registry since the time we obtained the length.
	if ( KErrOverflow == error )
		{
		// Cleanup the current state and Re-issue the same request again
		CleanupStack::PopAndDestroy( appInfoBuf );
		return GetLocationAppInfoL( aSession, aIdentifier );
		}
	else if( error )
		{
		User::Leave( error );
		}
		
    // If the server has not set the buffer then leave with KErrNotFound
    if ( !bufPtr.Length())
        {
        User::Leave( KErrNotFound );
        }
        
    // Parse the Application information structure to obtain the Application
    // information structure
    RBufReadStream  appReadStream( *appInfoBuf, 0 );
    CleanupClosePushL( appReadStream );
    
    CLcBasicAppInfo* appInfo = CLcBasicAppInfo::NewLC();
    appInfo->InternalizeL( appReadStream );
    CleanupStack::Pop( appInfo );
    
    CleanupStack::PopAndDestroy( 2, appInfoBuf );   // appReadStream

    return appInfo;	
	}
	
// ---------------------------------------------------------------------------
// CLcLocationAppInfoArray* LcSyncOperation::ParseLocAppBufferL
// ---------------------------------------------------------------------------
// 
CLcLocationAppInfoArray* LcSyncOperation::ParseLocAppBufferL( 
                                                RReadStream&   aReadStream )
    {
    DEBUG("+ LcSyncOperation::ParseLocAppBufferL")
        
    CLcLocationAppInfoArray* appArray = CLcLocationAppInfoArray::NewLC();
    
    // Parse the contents into RLcIpcAppInfoArray type.
    RLcIpcAppInfoArray  ipcArray;
    CleanupStack::PushL( TCleanupItem( RLcIpcAppInfoArray::ResetAndDestroyAppArray, &ipcArray ));
    
    // Internalize the array onto the ipcArray structure.
    ipcArray.InternalizeL( aReadStream );

    // Pack the Application info array
    for ( TInt i = 0; i < ipcArray.Count(); i++ )
        {
        
        // Form the Application information object corresponding to
        // this ipcAppInfo.
        
        // Create a new Application Information object. This object will
        // be populated and the appended to the appArray.
        CLcLocationAppInfo* appInfo = CLcLocationAppInfo::NewLC();
        
        // Set the Unique identifer for the application
        appInfo->SetIdL( ipcArray[i]->Id());
        
        // Set the display name of the application
        appInfo->SetNameL( ipcArray[i]->Name());
        
        // Set the Launch Mode for the application
        // There is no need to check the launch mode and compare it with
        // the values of TLcLaunchMode. Internally its ensured that the 
        // server would only set the parameter based in TLcLaunchMode.
        appInfo->SetLaunchMode( static_cast<CLcLocationAppInfo::
                            TLcLaunchMode>( ipcArray[i]->LaunchMode()));
        
        // Set the Application characteristics
        appInfo->SetApplicationCharacteristics( 
                            ipcArray[i]->ApplicationCharacteristics());
        
        // Set the system characteristics
        appInfo->SetSystemCharacteristics( ipcArray[i]->SystemCharacteristics());
         
        // Load the application Icon Related Data
        if ( ipcArray[i]->IconFile().Compare( KNullDesC ))
        	{
        	// The Icon file name is specified. So Load the icons from the 
        	// Icon file. So we can set these values for the Icon file.
        	appInfo->SetIconL( ipcArray[i]->IconFileType(),
        					   ipcArray[i]->IconFile(),
        					   ipcArray[i]->FrameNo());
        	}
        else if ( ipcArray[i]->ApplicationType() == ELcNativeApplication )
        	{
        	// Only native applications are handled here.
        	// Here we have to pass the Application UID as the Icon data since
        	// the Icon will be loaded from the App shell
        	appInfo->SetIconL( ipcArray[i]->IconFileType(),
        					   ipcArray[i]->ApplicationData());        	
        	}
        		   
        appArray->AppendL( appInfo );
        CleanupStack::Pop( appInfo );
        }
    CleanupStack::PopAndDestroy(); // ipcArray
    
    // If there are no elements in the array then that means that there
    // are no applications registered with LC. So leave with KErrNotFound
    if ( !appArray->Count())
        {
        User::Leave( KErrNotFound );
        }
    
    CleanupStack::Pop( appArray ); 
    
    DEBUG("- LcSyncOperation::ParseLocAppBufferL")
           
    return appArray;
    }	                                                                                                            
// End of File
