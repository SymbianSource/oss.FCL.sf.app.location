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
* Description:  Parameters for the IPC message handling between the 
*                Location Centre API and Location Centre Server.
*
*/


// SYSTEM INCLUDES
#include <e32std.h>

// USER INCLUDES
#include "lcipcparams.h"

// CONSTANT DEFINITIONS
const TInt KSizeofTUint32 = sizeof( TUint32 );
    
// ----- Member funtions for RLcIpcAppIdArray --------------------------------

// ---------------------------------------------------------------------------
// void RLcIpcAppIdArray::ExternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void RLcIpcAppIdArray::ExternalizeL( RWriteStream&   aWriteStream )
    {
    // The format for packing the buffer would be as follows
    // Count | Length1 | ID1 | Length2 | ID2 | ...
    // where
    // Count   - The number of strings which would be packed
    // Length1 - The Length of the first Identifier string
    // ID1     - First Identifier
    // Length2 - The Length of the second Identifier string
    // ID2     - Second Identifier.
    // ...
    
    const TInt  count = Count();
    // First pack the number of elements in the array. Even if there is no
    // elements pack a 0
    
    aWriteStream.WriteUint32L( Count());
    
    for ( TInt i = 0; i < count; i++ )
        {
        // Obtain the element at position i
        HBufC*  element = ( *this )[i];
        
        // As per the grammar first pack the length and then pack the string
        TInt length = element->Des().Length();
        aWriteStream.WriteUint32L( length );
        
        // Now pack the identifier
        aWriteStream.WriteL( element->Des(), length ); 
        }
        
    aWriteStream.CommitL();
    }

// ---------------------------------------------------------------------------
// void RLcIpcAppIdArray::InternalizeL
// ---------------------------------------------------------------------------
//    
EXPORT_C void RLcIpcAppIdArray::InternalizeL( RReadStream&    aReadStream )
    {
    // Clean up all the elements in the array so that the previous internalized
    // structure would be removed.
    ResetAndDestroy();
        
    // The first element in the array is the Count variable which indicates
    // the number of elements in this array.
    const TUint32 count = aReadStream.ReadUint32L();
   
    // Even a string containing no elements is acceptable.
    for ( TInt i = 0; i < count; i++ )
        {
        // First read the length of the Identifer
        TUint32 length = aReadStream.ReadUint32L();        
        if ( length >= KMaxTInt/2 )
            {
            User::Leave( KErrArgument );
            }
                
        // Now create a buffer to read the actual identifier name. The length
        // of the buffer would be defined by the length value. Use this buffer
        // read the identifier.
        HBufC*  buffer = HBufC::NewLC( length );
        TPtr bufferPtr( buffer->Des());
        
        aReadStream.ReadL( bufferPtr, length );
        
        // Transfer the ownership of the buffer to this array.
        User::LeaveIfError( Append( buffer ));        
        CleanupStack::Pop( buffer );
        }
    }

// ---------------------------------------------------------------------------
// void RLcIpcAppIdArray::BufferLength
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RLcIpcAppIdArray::BufferLength()
    {
    TInt length = 0;
    
    // Pack the Length for including the length of Count buffer
    length += KSizeofTUint32;
   
    // Even a string containing no elements is acceptable.
    for ( TInt i = 0; i < Count(); i++ )
        {
        // First 4 bytes for the Length of the string
        length += KSizeofTUint32;
        
        // Now pack the length for the buffer
        length += ( *this )[i]->Des().Size();
        }
    return length;
    }
    
// ---------------------------------------------------------------------------
// void RLcIpcAppIdArray::ResetAndDestroyIdArray
// ---------------------------------------------------------------------------
//
EXPORT_C void RLcIpcAppIdArray::ResetAndDestroyIdArray( TAny* aIdArray )
    {
    RLcIpcAppIdArray* array = reinterpret_cast< RLcIpcAppIdArray* >( aIdArray );
    if ( array )
        {
        array->ResetAndDestroy();
        }
        
    }
    
    
// ----- Member funtions for RLcIpcAppIdArray --------------------------------

// ---------------------------------------------------------------------------
// void RLcIpcAppInfoArray::ExternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void RLcIpcAppInfoArray::ExternalizeL( RWriteStream&   aWriteStream )
    {
    // The format for packing the buffer would be as follows
    // Count | IDLength | ID | NameLength | Name | Launch mode | Sys Characteristics |
    // Application Characteristics
    // where
    // Count            - The number of structures which would be packed
    // IDLength         - The Length of the Identifier string
    // ID               - Application Identifier
    // NameLength       - The Length of the name string
    // Name             - Application Name.
    // IconFile Length  - Length of the Icon file name.
    // Icon File        - Full path of the icon file with file name    
    // IconFile Type    - The Icon file type
    // Frame No         - The Frame no of the icon if the icon file is a MIF
    //                    file
    // Launch Mode      - Launching mode of the application
    // Sys Char         - System characteristics
    // App Char         - Application characteristics
    // These elements are repeated for all Application information structures.
    
    const TInt  count = Count();
    // First pack the number of elements in the array. Even if there is no
    // elements pack a 0
    
    aWriteStream.WriteUint32L( Count());
    
    for ( TInt i = 0; i < count; i++ )
        {
        // Obtain the element at position i
        CLcAppInfo*  element = ( *this )[i];
        
        // Externalize the parent class first and then externalize the child
        element->ExternalizeL( aWriteStream );
        }
        
    aWriteStream.CommitL();
    }

// ---------------------------------------------------------------------------
// void RLcIpcAppIdArray::InternalizeL
// ---------------------------------------------------------------------------
//    
EXPORT_C void RLcIpcAppInfoArray::InternalizeL( RReadStream&    aReadStream )
    {
    // Clean up all the elements in the array so that the previous internalized
    // structure would be removed.
    ResetAndDestroy();
        
    // The first element in the array is the Count variable which indicates
    // the number of elements in this array.
    const TUint32 count = aReadStream.ReadUint32L();
     
    // Even a string containing no elements is acceptable.
    for ( TInt i = 0; i < count; i++ )
        {
             
        // Create a new CLcAppInfo element
        CLcAppInfo* element = CLcAppInfo::NewLC();
               
        // Internalize the Parent class first and then proceed to the child
        element->InternalizeL( aReadStream );
               
        // Transfer the ownership of the buffer to this array.
        User::LeaveIfError( Append( element ));        
        CleanupStack::Pop( element );
               
        }
    }

// ---------------------------------------------------------------------------
// TInt RLcIpcAppInfoArray::BufferLength
// ---------------------------------------------------------------------------
//
EXPORT_C TInt RLcIpcAppInfoArray::BufferLength()
    {
    TInt length = 0;
    
    // Pack the length for the Count field.
    length += KSizeofTUint32;
    
    // Pack the length required for the individual app structures

    TInt count = Count();
    for ( TInt i = 0; i < count; i++ )
        {
        // Obtain the element at position i
        CLcAppInfo*  element = ( *this )[i];
        
        // Pack the length required for this element
        length += element->BufferLength();
        }
        
    return length;
    }
    
// ---------------------------------------------------------------------------
// void RLcIpcAppIdArray::ResetAndDestroyIdArray
// ---------------------------------------------------------------------------
//
EXPORT_C void RLcIpcAppInfoArray::ResetAndDestroyAppArray( TAny* aIdArray )
    {
    RLcIpcAppInfoArray* array = reinterpret_cast< RLcIpcAppInfoArray* >( aIdArray );
    if ( array )
        {
        array->ResetAndDestroy();
        }
    }
        
// ----- Member funtions for CLcBasicAppInfo ---------------------------------

// ---------------------------------------------------------------------------
// void CLcBasicAppInfo::ExternalizeL
// ---------------------------------------------------------------------------
//
EXPORT_C void CLcBasicAppInfo::ExternalizeL( RWriteStream&   aWriteStream )
    {
    // The format for packing the buffer would be as follows
    // Application Type | DataLength | Data | CmdLength | Cmd
    // where
    // Application Type  - The type of the Application which is stored in the
    //                     structure.
    // DataLength        - The Length of the Application data string
    // Data              - Application Data
    // CmdLength         - The Length of the command line arguments
    // Cmd               - Command line arguments
    
    // First pack the Application type
    
    aWriteStream.WriteUint32L( iAppType );
    
    // Pack the Launching mode of the application
    aWriteStream.WriteUint32L( iLaunchMode );
        
    // Pack the length of the Application Data. If there is no application
    // data then pack zero.
    TUint length = 0;
    if ( iAppData )
        {
        length = iAppData->Length();
        }
    aWriteStream.WriteUint32L( length );
    
    // If there is a valid data field pack it onto the buffer.
    if ( length )
        {
        aWriteStream.WriteL( iAppData->Des(), length );
        }
    
    // Pack the command line arguments. If there is no command line
    // parameters then pack a length of 0   
    length = 0;
    if ( iCmdLineParams )
        {
        length = iCmdLineParams->Length();
        }
    aWriteStream.WriteUint32L( length );
    
    // If there is a valid command line parameters field pack it onto the buffer.
    if ( length )
        {
        aWriteStream.WriteL( iCmdLineParams->Des(), length );
        }
                
    aWriteStream.CommitL();
    }

// ---------------------------------------------------------------------------
// void CLcBasicAppInfo::InternalizeL
// ---------------------------------------------------------------------------
//    
EXPORT_C void CLcBasicAppInfo::InternalizeL( RReadStream&    aReadStream )
    {
    // Clear the application data
    delete iAppData;
    iAppData = NULL;
    
    // Clear the command line params
    delete iCmdLineParams;
    iCmdLineParams = NULL;
    
    // The first element in the array if the Application type variable.
    iAppType = aReadStream.ReadUint32L();
    
    // The second element is the Application Launch mode
    iLaunchMode = aReadStream.ReadUint32L();
        
    // Now get the length of the data field.
    TUint32 length = aReadStream.ReadUint32L();
    if ( length >= KMaxTInt/2 )
        {
        User::Leave( KErrArgument );
        }        
    // Set the application data only if the length is a +ve value.
    if ( length )
        {
        iAppData = HBufC::NewL( length );
        TPtr appData( iAppData->Des());
        aReadStream.ReadL( appData, length );
        }
        
    // Now read the length of the command line parameters    
    length = aReadStream.ReadUint32L();
    if ( length >= KMaxTInt/2 )
        {
        User::Leave( KErrArgument );
        }                
    // Set the command line params only if the length is a +ve value.
    if ( length )
        {
        iCmdLineParams = HBufC::NewL( length );
        TPtr cmdParam( iCmdLineParams->Des());
        aReadStream.ReadL( cmdParam, length );
        }
    }
    
// ---------------------------------------------------------------------------
// TInt CLcBasicAppInfo::BufferLength
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CLcBasicAppInfo::BufferLength()
    {    
    TInt length = 0;
    
    // Include length for Application type
    length += KSizeofTUint32;
    
    // Add the length for Launch mode
    length += KSizeofTUint32;
        
    // Add the length of the Application Data and the length for Application
    // data if it exists
    length += KSizeofTUint32;
    
    if ( iAppData )
        {
        length += iAppData->Des().MaxSize();
        }
    
    // Add the length of the Command line args and the length for Cmd line
    // args if it exists
    length += KSizeofTUint32;
    
    if ( iCmdLineParams )
        {
        length += iCmdLineParams->Des().MaxSize();
        }
    return length;   
    }
        

// ----- Member funtions for CLcBasicAppInfo ---------------------------------
        
// ---------------------------------------------------------------------------
// void CLcBasicAppInfo::ExternalizeL
// ---------------------------------------------------------------------------
// 
void CLcAppInfo::ExternalizeL( RWriteStream&   aWriteStream )
    {
    CLcBasicAppInfo::ExternalizeL( aWriteStream );
    
    // As per the grammar first pack the length and then pack the string
    TInt length = Id().Length();
    aWriteStream.WriteUint32L( length );
    
    // Pack the Identifier. No need to check for the Length since
    // this is a mandatory field and the Application would not have been
    // accepted if it were not +ve        
    aWriteStream.WriteL( Id(), length );
    
    // Pack the name. If there is no name then pack a length of 0 for the
    // name field.
    length = Name().Length();
    aWriteStream.WriteUint32L( length );
    
    // Pack the name field only if the length is +ve
    if ( length )
        {
        aWriteStream.WriteL( Name(), length );
        }
    
    // Pack the Icon file and Icon file type for the file
    length = IconFile().Length();
    aWriteStream.WriteUint32L( length );
    
    if ( length )
        {
        aWriteStream.WriteL( IconFile(), length );
        aWriteStream.WriteUint32L( iIconFileType);
        
        // If the Icon file type is a MIF, pack the Frame no
        if ( EMifFile == iIconFileType )
            {
            aWriteStream.WriteUint32L( iFrameNo );
            }
        }
    
    // Pack the System characteristics
    aWriteStream.WriteUint32L( iSystemCharacteristics );
    
    // Pack the application characteristics
    aWriteStream.WriteUint32L( iAppCharacteristics );
    
    aWriteStream.CommitL();
    }
   
// ---------------------------------------------------------------------------
// void CLcBasicAppInfo::InternalizeL
// ---------------------------------------------------------------------------
//
void CLcAppInfo::InternalizeL( RReadStream&    aReadStream )
    {
    CLcBasicAppInfo::InternalizeL( aReadStream );
    
    // First read the length of the Identifer
    TUint32 length = aReadStream.ReadUint32L();
    if ( length >= KMaxTInt/2 )
        {
        User::Leave( KErrArgument );
        }
                
    // No need to check for the length for this is a mandatory field.
    // Now create a buffer to read the actual identifier. The length
    // of the buffer would be defined by the length value. Use this buffer
    // read the identifier.
    iId = HBufC::NewL( length );
    TPtr bufferPtr( iId->Des());
    aReadStream.ReadL( bufferPtr, length );
    
    // Read the length of the name buffer
    length = aReadStream.ReadUint32L();
    if ( length >= KMaxTInt/2 )
        {
        User::Leave( KErrArgument );
        }
                
    if ( length )
        {
        iApplicationName = HBufC::NewL( length );
        bufferPtr.Set( iApplicationName->Des());
        
        // Read the name field
        aReadStream.ReadL( bufferPtr, length );
        }
    
    // Read the Icon file if its exists
    length = aReadStream.ReadUint32L();
    if ( length >= KMaxTInt/2 )
        {
        User::Leave( KErrArgument );
        }
                
    if ( length )
        {
        iIconFile = HBufC::NewL( length );
        bufferPtr.Set( iIconFile->Des());
        
        // Read the Icon file field
        aReadStream.ReadL( bufferPtr, length );

        // Set the Icon file type
        iIconFileType =
            static_cast< CLcAppInfo::TLcIconFileType >( aReadStream.ReadUint32L());
            
        // If the Icon file type is a MIF file then set the Frame no
        if ( EMifFile == iIconFileType )
            {
            iFrameNo = aReadStream.ReadUint32L();
            }        
        }
    
    // Set the System characteristics
    iSystemCharacteristics = aReadStream.ReadUint32L();
    
    // Set the Application characteristics
    iAppCharacteristics = aReadStream.ReadUint32L();        
    }

// ---------------------------------------------------------------------------
// TInt CLcAppInfo::BufferLength
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CLcAppInfo::BufferLength()
    {    
    TInt length = 0;
    
    // First pack the length for the Basic App Info structure
    length += CLcBasicAppInfo::BufferLength();
    
    // Include length for Application Identifier
    length += KSizeofTUint32;
    length += iId->Des().MaxSize();
    
    // Pack the name field and the Length of the name field if it exists 
    // Add the length of the Command line args and the length for Cmd line
    // args if it exists
    length += KSizeofTUint32;
    
    if ( iApplicationName )
        {
        length += iApplicationName->Des().MaxSize();
        }
    
    // Pack the Icon file field and the name of icon file if it exists    
    // Read the Icon file if its exists
    length += KSizeofTUint32;
    if ( iIconFile )
        {
        // Pack the Icon file name length
        length += iIconFile->Des().MaxSize();
        
        // Pack the Icon file type
        length += KSizeofTUint32;
            
        // If the Icon file type is a MIF file then add length for the
        // frame number
        if ( EMifFile == iIconFileType )
            {
            length += KSizeofTUint32;
            }        
        }
    
    // Add the length for System characteristics
    length += KSizeofTUint32;
    
    // Add the length for Application characteristics
    length += KSizeofTUint32;
            
    return length;   
    }    
// End of File
