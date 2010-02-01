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
* Description:  Location Centre Server Engine Object.
*
*/


// SYSTEM INCLUDES
#include <s32mem.h>
#include <e32std.h>

// USER INCLUDES
#include "lcserverengine.h"
#include "lcregistry.h"
#include "lcserverengineobserver.h"
#include "lcserverinterface.h"
#include "lcdebug.h"

// CONSTANT DEFINTIONS
const TUint32 KUidMaxValue = 0xFFFFFFFF;

// ----- Member funtions for CLcServerEngine ---------------------------------

// ---------------------------------------------------------------------------
// CLcServerEngine::CLcServerEngine
// ---------------------------------------------------------------------------
//
CLcServerEngine::CLcServerEngine( MLcServerEngineObserver&    aObserver )
    :iObserver( aObserver )
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.
    }
         
// ---------------------------------------------------------------------------
// CLcServerEngine::~CLcServerEngine
// ---------------------------------------------------------------------------
//
CLcServerEngine::~CLcServerEngine()
    {
    // C++ Destructor. Free all resources associated with this class.
    
    // Complete any outstanding messages if they exist and free all the
    // associated resources
    while ( iMessageArray.Count())
        {
        // Extract the IPC message handle from the Array
        RMessage2* message = iMessageArray[0];
        iMessageArray.Remove( 0 );
        
        // Service the Request.
        message->Complete( KErrServerTerminated );
        
        // Free the message structure
        delete message;
        }    
    iMessageArray.ResetAndDestroy();
    iMessageArray.Close();
    
    // Delete the Location Centre Registry
    delete iRegistry;
    }
        
// ---------------------------------------------------------------------------
// CLcServerEngine* CLcServerEngine::NewL
// ---------------------------------------------------------------------------
//
CLcServerEngine* CLcServerEngine::NewL( MLcServerEngineObserver&    aObserver )
    {
    CLcServerEngine* self = NewLC( aObserver );
    CleanupStack::Pop( self );
    return self;         
    }

// ---------------------------------------------------------------------------
// CLcServerEngine* CLcServerEngine::NewLC
// ---------------------------------------------------------------------------
//
CLcServerEngine* CLcServerEngine::NewLC( MLcServerEngineObserver&    aObserver )
    { 
    // Symbian Two phased constructor. Leaves the object on the Clean-up
    // stack.
    CLcServerEngine* self = new ( ELeave )CLcServerEngine( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;         
    }

// ---------------------------------------------------------------------------
// void CLcServerEngine::ConstructL
// ---------------------------------------------------------------------------
//
void CLcServerEngine::ConstructL()
    {
    // Create the Location Centre Registry
    iRegistry = CLcRegistry::NewL( *this );
      
    }

// ---------------------------------------------------------------------------
// void CLcServerEngine::ConstructL
// ---------------------------------------------------------------------------
//
void CLcServerEngine::DoServiceL( const RMessage2&      aMessage )
    {
    // We cannot store the reference to the RMessage2 object passed but
    // need to create a new instance and copy the contents for all
    // furture purposes
    RMessage2* message = new RMessage2;
    *message = aMessage;
    
    // Append the message to the Queue. All messages will be serviced later
    // in a generic manner.
    User::LeaveIfError( iMessageArray.Append( message ));
        
    if ( !iRegistryUnderUpdate )
        {
        // The registry is consistent. So the message can be handled immediately.
        ServiceRequests();
        }
    }

// ---------------------------------------------------------------------------
// void CLcServerEngine::RegistryUnderUpdation
// ---------------------------------------------------------------------------
//
void CLcServerEngine::RegistryUnderUpdation()
    {
    DEBUG( "+ CLcServerEngine::RegistryUnderUpdation")
    
    // Set the Registry update flag. This would prevent the Engine from
    // performing any operation until the updation completes.
    iRegistryUnderUpdate = ETrue;
    
    DEBUG( "- CLcServerEngine::RegistryUnderUpdation")
    }

// ---------------------------------------------------------------------------
// void CLcServerEngine::RegistryUpdated
// ---------------------------------------------------------------------------
//
void CLcServerEngine::RegistryUpdated()
    {
    DEBUG( "+ CLcServerEngine::RegistryUpdated")
        
    // Reset the Registry Update flag. Now the Registry is consistent and
    // the Engine can service any requestsy
    iRegistryUnderUpdate = EFalse;
    
    // Serve all the outstanding Location Requests.
    ServiceRequests();
    
    // Notify the Engine Observer about the changes to the Registry.
    iObserver.LcRegistryUpdated();
    
    DEBUG( "- CLcServerEngine::RegistryUpdated")    
    }
    
// ---------------------------------------------------------------------------
// void CLcServerEngine::ServiceRequests
// ---------------------------------------------------------------------------
//
void CLcServerEngine::ServiceRequests()
    {                       
    // Service all outstanding LC requests.
    while ( iMessageArray.Count())
        {
        // Extract the IPC message handle from the Array
        RMessage2* message = iMessageArray[0];
        iMessageArray.Remove( 0 );
        
        // Service the Request.
        TRAPD( error, ServiceRequestL(*message));
        
        DEBUG1( "ServiceRequest Error %d", error ) 
            
        // Complete the request with the error code.
        message->Complete( error );
                            
        // Free the message structure
        delete message;
        }   
    }

// ---------------------------------------------------------------------------
// void CLcServerEngine::ServiceRequestL
// ---------------------------------------------------------------------------
//
void CLcServerEngine::ServiceRequestL( RMessage2&      aMessage )
    {
    // Set the Current requestor's SID
    iCurrentRequestorSid = aMessage.SecureId();
       
    switch( aMessage.Function())
        {
        case ELcFilteredAppsBufferLength:
        case ELcFilteredApps:
            {
            // Getting the Application buffer length and Getting the list of applications
            // have similar functionality. Only the last portion of the code changes.
            // Hence, both the requests will be handled similarly with the necessary
            // differentiation at the end. 
           
            // Create the package buffer for obtaining the filter conditions
            TPckgBuf< TLcLocationAppFilter > filterBuf;
             
            // Read the filter contents
            User::LeaveIfError( aMessage.Read( 0, filterBuf ));
            
            RLcIpcAppInfoArray appArray;
            CleanupClosePushL( appArray );

            // Filter Location based Applications based on the Filter
            // conditions
            GetFilteredAppsL( filterBuf(), appArray );
            
            // Now for the differentiation
            if ( aMessage.Function() == ELcFilteredAppsBufferLength )
                {
                // Write the Length Value into a buffer
                CBufFlat* lengthBuffer = CBufFlat::NewL( sizeof( TUint32 ));
                CleanupStack::PushL( lengthBuffer );
                RBufWriteStream lengthWrite( *lengthBuffer, 0 );
                CleanupClosePushL( lengthWrite );
                lengthWrite.WriteInt32L( appArray.BufferLength());
                CleanupStack::PopAndDestroy(); // lengthWrite
                
                // Write the Buffer back to the IPC message                
                TPtr8    ptr( lengthBuffer->Ptr(0));
                User::LeaveIfError( aMessage.Write( 1, ptr ));
                
                // Pop and destroy the length buffer
                CleanupStack::PopAndDestroy( lengthBuffer );                    
                }
            else
                {
                // Create the buffer for packing the Application Information
                // structures and pack the contents into this buffer
                CBufFlat* buffer = CBufFlat::NewL( appArray.BufferLength() );
                CleanupStack::PushL( buffer );
                RBufWriteStream writeStream( *buffer, 0 );
                CleanupClosePushL( writeStream );                
                appArray.ExternalizeL( writeStream );
                CleanupStack::PopAndDestroy(); // writeStream
                
                // Write the buffer back to the IPC message.
                TPtr8    ptr( buffer->Ptr(0));
                User::LeaveIfError( aMessage.Write( 1, ptr ));
                
                // Pop and destroy the app buffer              
                CleanupStack::PopAndDestroy( buffer );                    
                }
            
            // Reset and Destroy the Contents of the Application info
            // array
            CleanupStack::PopAndDestroy( &appArray );
            break;
            }
        case ELcSpecifiedAppsBufferLength:
        case ELcSpecifiedApps:
            {
            // Getting the Application buffer length and Getting the list of applications
            // have similar functionality. Only the last portion of the code changes.
            // Hence, both the requets will be handled similarly with the necessary
            // differentiation at the end. 
                            
            // Obtain the array of Applications which need to be used for
            // filtering.              
            
            // Read the Descriptor from the IPC arguments
            TInt desLength = aMessage.GetDesLength( 0 );          
            // Allocate the id Array buffer    
            CBufFlat* idArrayBuf = CBufFlat::NewL( desLength );
            CleanupStack::PushL( idArrayBuf );
            idArrayBuf->ResizeL( desLength );
            
            TPtr8   idArrayPtr( idArrayBuf->Ptr(0));
            
            // Now read the descriptor finally
            User::LeaveIfError( aMessage.Read( 0, idArrayPtr ));
            
            // Create a Read buffer stream to read the App Ids    
            RBufReadStream  appReadStream( *idArrayBuf, 0 );
            CleanupClosePushL( appReadStream );
            
            RLcIpcAppIdArray    idArray;
            CleanupStack::PushL( TCleanupItem( RLcIpcAppIdArray::ResetAndDestroyIdArray, &idArray ));  
                            
            // Internalize the structure to obtain the Actual list of Ids    
            idArray.InternalizeL( appReadStream );  
             
             
            RLcIpcAppInfoArray appArray;
            CleanupClosePushL( appArray );
               
            // Read the second paramte to determine whether the list specified needs to be
            // included or excluded
            TBool   includeFlag = aMessage.Int1();
            if ( includeFlag )
                {
                GetSpecifiedAppsL( idArray, appArray );
                }
            else
                {
                GetWithoutSpecifiedAppsL( idArray, appArray );
                }
            
            if ( aMessage.Function() == ELcSpecifiedAppsBufferLength )
                {
                // Write the Length Value into a buffer
                CBufFlat* lengthBuffer = CBufFlat::NewL( sizeof( TUint32 ));
                CleanupStack::PushL( lengthBuffer );             
                
                RBufWriteStream lengthWrite( *lengthBuffer, 0 );
                CleanupClosePushL( lengthWrite );                
                lengthWrite.WriteInt32L( appArray.BufferLength());
                CleanupStack::PopAndDestroy(); // lengthWrite
                
                // Write the Buffer back to the IPC message                
                TPtr8    ptr( lengthBuffer->Ptr(0));
                User::Leave( aMessage.Write( 2, ptr ));
                
                // Pop and destroy the Length buffer              
                CleanupStack::PopAndDestroy( lengthBuffer );                    
                }
            else
                {
                // Create the buffer for packing the Application Information
                // structures and pack the contents into this buffer
                CBufFlat* buffer = CBufFlat::NewL( appArray.BufferLength() );
                CleanupStack::PushL( buffer );
                                
                RBufWriteStream writeStream( *buffer, 0 );
                CleanupClosePushL( writeStream );                
                appArray.ExternalizeL( writeStream );
                CleanupStack::PopAndDestroy(); // writeStream
                
                // Write the buffer back to the IPC message.
                TPtr8    ptr( buffer->Ptr(0));
                User::Leave( aMessage.Write( 2, ptr ));
                
                // Pop and destroy the app buffer                  
                CleanupStack::PopAndDestroy( buffer ); 
                }
            
            // We can also destroy the App id array buffer and Read streams
            CleanupStack::PopAndDestroy( 4, idArrayBuf );
            
            break;
            }
        case ELcAppInfoLength:
            {
            DEBUG( "+ CLcServerEngine::ServiceRequestL, ELcAppInfoLength ") 
               
            // Read the Descriptor from the IPC arguments
            TInt desLength = aMessage.GetDesLength( 0 );
                                 
            // Obtain the Application Identifier
            HBufC* identifier = HBufC::NewLC( desLength );
            TPtr des(identifier->Des());
            aMessage.Read( 0, des );
            
            // Obtain the Location based Application for this identifer
            CLcBasicAppInfo* appInfo = NULL;
            User::LeaveIfError( iRegistry->GetApplicationInfo( *identifier, appInfo ));
            
            // Pack the Length field into the Second argument
            CBufFlat* lengthBuffer = CBufFlat::NewL( sizeof( TUint32 ));
            CleanupStack::PushL( lengthBuffer );                                       
                                        
            RBufWriteStream lengthWrite( *lengthBuffer, 0 );
            CleanupClosePushL( lengthWrite );                
            lengthWrite.WriteInt32L(  appInfo->BufferLength());
            CleanupStack::PopAndDestroy(); // lengthWrite
            
            // Write the Buffer back to the IPC message                
            TPtr8    ptr( lengthBuffer->Ptr(0));
            User::LeaveIfError( aMessage.Write( 1, ptr ));
            
            // Pop and destroy the Length buffer             
            CleanupStack::PopAndDestroy( 2, identifier );
            
            DEBUG( "- CLcServerEngine::ServiceRequestL, ELcAppInfoLength ") 
                        
            break;               
            }
        case ELcAppInfo:
            {
            DEBUG( "+ CLcServerEngine::ServiceRequestL, ELcAppInfo ") 
            
            // Read the Descriptor from the IPC arguments
            TInt desLength = aMessage.GetDesLength( 0 );
                
            // Obtain the Application Identifier
            HBufC* identifier = HBufC::NewLC( desLength );
            TPtr des(identifier->Des());
            aMessage.Read(0, des);
                        
            // Obtain the Location based Application for this identifer
            CLcBasicAppInfo* appInfo = NULL;
            User::LeaveIfError( iRegistry->GetApplicationInfo( *identifier, appInfo ));
            
            // Create the buffer for packing the Application Information
            // structure and pack the contents into this buffer
            CBufFlat* buffer = CBufFlat::NewL( appInfo->BufferLength());
            CleanupStack::PushL( buffer );
            
            RBufWriteStream writeStream( *buffer, 0 );
            CleanupClosePushL( writeStream );                
            appInfo->ExternalizeL( writeStream );
            CleanupStack::PopAndDestroy(); // writeStream
            
            // Write the buffer back to the IPC message.
            TPtr8    ptr( buffer->Ptr(0));
            User::LeaveIfError( aMessage.Write( 1, ptr ));
            
            // Pop and destroy the App info buffer                  
            CleanupStack::PopAndDestroy( 2, identifier );
            
            DEBUG( "- CLcServerEngine::ServiceRequestL, ELcAppInfo ") 
                                      
            break;
            }
        default:
            {
            DEBUG( "CLcServerEngine::ServiceRequestL, KErrNotSupported ") 
            User::Leave( KErrNotSupported );
            break;
            }
        }       
    }
    
// ---------------------------------------------------------------------------
// void CLcServerEngine::GetFilteredApps
// ---------------------------------------------------------------------------
//
void CLcServerEngine::GetFilteredAppsL( 
        TLcLocationAppFilter&         aAppFilter,             
        RLcIpcAppInfoArray&           aFilteredAppArray )
    {
    DEBUG( "+ CLcServerEngine::GetFilteredAppsL")  
        
    // Obtain the current list of Location based Applications from the
    // Location Centre registry. Incase, the application satisfies the 
    // filter conditions, then the application is appended to the list.
    
    RPointerArray<CLcAppInfo> appInfoArray;
    User::LeaveIfError( iRegistry->GetAllRegisteredAppsList( appInfoArray ));
    
    TInt count = appInfoArray.Count();
    
    // Parse through the array list for each item
    for ( TInt i = 0; i < count; i++ )
        {
        // Obtain the element at the position 'i'
        CLcAppInfo* element = appInfoArray[i];
        
        if ( element->ApplicationType() == ELcNativeApplication &&
        	 IsSameSID( element->ApplicationData()))
        	{
        	continue;
        	}
        	
        // Check for the filtering conditions. Here we can check for the System
        // filters and Application filters simultaneously. The code seems
        // pretty lengthy owing to many accessor methods but retained it here
        // instead of using too many local variables.
        if ( Compare( aAppFilter.SystemCharacteristics(),
                      element->SystemCharacteristics(),
                      aAppFilter.SysCharFilterConfiguration()) &&
             Compare( aAppFilter.ApplicationCharacteristics(),
                      element->ApplicationCharacteristics(),
                      aAppFilter.AppCharFilterConfiguration()))
            {
            // The filter characteristics have matched. So append the element
            // to the Array
            
            // Ignoring the Append error. Incase there is an error then the
            // element would not be appened.                
            User::LeaveIfError( aFilteredAppArray.Append( element ));          
            }
        }
    appInfoArray.Reset();
    appInfoArray.Close();
    
    DEBUG1( "App Count : %d", aFilteredAppArray.Count())
    DEBUG( "- CLcServerEngine::GetFilteredAppsL")    
    }        

// ---------------------------------------------------------------------------
// void CLcServerEngine::GetSpecifiedAppsL
// ---------------------------------------------------------------------------
//
void CLcServerEngine::GetSpecifiedAppsL( 
        RLcIpcAppIdArray&             aSpecifedApps,             
        RLcIpcAppInfoArray&           aSpecifiedAppArray )
    {
    // Obtain the current list of Location based Applications from the
    // Location Centre registry. Incase, the application satisfies the 
    // filter conditions, then the application is appended to the list.
    RPointerArray<CLcAppInfo> appInfoArray;
	User::LeaveIfError( iRegistry->GetAllRegisteredAppsList( appInfoArray ));
    TInt count = appInfoArray.Count();
        
    // Parse through the array list for each item
    for ( TInt i = 0; i < count; i++ )
        {
        // Obtain the element at the position 'i'
        CLcAppInfo* element = appInfoArray[i];
        
        if ( element->ApplicationType() == ELcNativeApplication &&
        	 IsSameSID( element->ApplicationData()))
        	{
        	continue;
        	}
        	        
        // Check for the filtering conditions
        for ( TInt j = 0; j < aSpecifedApps.Count(); j++ )
            {
            // Obtain the current identifier
            HBufC*  identifier = aSpecifedApps[j];
            
            // Check if the current App info's id is the same as this identifer
            // If Yes, then append it and go to the next element.
            if ( !identifier->Des().Compare( element->Id()))
                {            
                // Ignoring the Append error. Incase there is an error then the
                // element would not be appened.                
                User::LeaveIfError( aSpecifiedAppArray.Append( element ));
                }
            }
        // Small optimization check to see if the number of elements that have
        // been added are equal to the the number of elements in the App id
        // array. If yes, we can stop the checking.
        if ( aSpecifiedAppArray.Count() == aSpecifedApps.Count() )
            {
            break;
            }
        }
    appInfoArray.Reset();
    appInfoArray.Close();           
    }        

// ---------------------------------------------------------------------------
// void CLcServerEngine::GetWithoutSpecifiedAppsL
// ---------------------------------------------------------------------------
//       
void CLcServerEngine::GetWithoutSpecifiedAppsL( 
        RLcIpcAppIdArray&             aSpecifedApps,            
        RLcIpcAppInfoArray&           aSpecifiedAppArray )
    {
    // Obtain the current list of Location based Applications from the
    // Location Centre registry. Incase, the application satisfies the 
    // filter conditions, then the application is appended to the list.
    
    RPointerArray<CLcAppInfo> appInfoArray;
	User::LeaveIfError( iRegistry->GetAllRegisteredAppsList( appInfoArray ));
    TInt count = appInfoArray.Count();
    
    // Parse through the array list for each item
    for ( TInt i = 0; i < appInfoArray.Count(); i++ )
        {
        // Obtain the element at the position 'i'
        CLcAppInfo* element = appInfoArray[i];
   
        if ( element->ApplicationType() == ELcNativeApplication &&
        	 IsSameSID( element->ApplicationData()))
        	{
        	continue;
        	}
        	   
        // Boolean flag to check whether an element exists or not.
        TBool append = ETrue;
                    
        // Check for the filtering conditions
        for ( TInt j = 0; j < aSpecifedApps.Count(); j++ )
            {
            // Obtain the current identifier
            HBufC*  identifier = aSpecifedApps[j];
            
            // Check if the current App info's id is the same as this identifer
            // If Yes, then append it and go to the next element.
            if ( !identifier->Des().Compare( element->Id()))
                {            
                // Incase, the element being compared in the list is requested
                // then set the flag and break the switch
                append = EFalse;
                break;
                }
            }
           
        if ( append )
            {
            // Ignoring the Append error. Incase there is an error then the
            // element would not be appened.                
            User::LeaveIfError( aSpecifiedAppArray.Append( element ));            
            }
        }
    appInfoArray.Reset();
    appInfoArray.Close();            
    }                     

// ---------------------------------------------------------------------------
// void CLcServerEngine::Compare
// ---------------------------------------------------------------------------
// 
TBool  CLcServerEngine::Compare( TUint32         aFilter,
                                 TUint32         aFiltertoCompare,   
                                 TInt            aFilterConfig )
    {
    // Here,
    // aFilter          - Filter passed by the Client Application
    // aFiltertoCompare - Filter of the Location based Application
    
    // If no filter configurations are set then there will be no need to
    // proceed with checking. The Locaiton based Application must always be
    // included. Hence, return True.
    if ( !aFilter )
        {
        return ETrue;
        }
    
    // Proceed for the Actual checking.
    
    // Set the default value to EFalse to enable pessimistic match.
    TBool ret = EFalse;
        
    switch( aFilterConfig )
        {
        case TLcLocationAppFilter::EFilterStrict:
            {
            // Strict Filtering needs to be applied. So here we just have to
            // compare the values and then return True only if there is an
            // exact match
            if ( aFilter == aFiltertoCompare )
                {
                ret = ETrue;
                }
            break;
            }
        case TLcLocationAppFilter::EFilterInclusive:
            {
            // The Location based Application must have all the characteristics
            // passed by the Client application. It can have any additonal
            // characteristic too.
            if (( aFilter & aFiltertoCompare ) == aFilter )
                {
                ret = ETrue;
                }
            break;
            }
        case TLcLocationAppFilter::EFilterExclusive:
            {
            // The filtering should be such that there should be no match
            // between the requested character set and the one specified for the
            // Location based Application.
            if ( !( aFilter & aFiltertoCompare ))
                {
                ret = ETrue;
                }
            break;
            }
        default:
            {
            break;
            }      
        }
    return ret;
    }

// ---------------------------------------------------------------------------
// TBool CLcServerEngine::IsSameSID
// ---------------------------------------------------------------------------
// 
TBool CLcServerEngine::IsSameSID( const TDesC&		aAppSid )
	{
    // Since the request is for a Native S60 application, the Identifer
    // contains an UID. Obtain it using Lexer
    TLex lexer( aAppSid );
    TUint32 uidValue;
    
    if ( lexer.BoundedVal( uidValue, EHex, KUidMaxValue ) || 
    	 uidValue != iCurrentRequestorSid )
    	{
    	return EFalse;
    	}
	else
		{
		return ETrue;
		}
	}
	
// End of File
