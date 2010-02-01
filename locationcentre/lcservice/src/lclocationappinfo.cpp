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
* Description:  Location Application Information structures.
*
*/


// SYSTEM INCLUDES
#include <gulicon.h>					// Gul Icon
#include <fbs.h>                        // Bitmap

// USER INCLUDES
#include "lclocationappinfo.h"
#include "lcnativeapps.hrh"
#include "lcerrors.h"
#include "lciconloader.h"
#include "lcipcparams.h"

// CONSTANT DEFINTIONS
// Maximum value that the UID field can take in S60
const TUint32   KUidMaxValue = 0xFFFFFFFF;

// ----- Member funtions for CLcLocationAppInfo ------------------

// ---------------------------------------------------------------------------
// CLcLocationAppInfo::CLcLocationAppInfo
// ---------------------------------------------------------------------------
//
CLcLocationAppInfo::CLcLocationAppInfo( 
                        const CLcLocationAppInfo&     aLCAppInfo )
    {
    // C++ Copy constructor. No allocations or functions which can Leave
    // should be called from here.
    
    // Set only those parameters which dont require memory allocation.
    iLaunchMode = aLCAppInfo.iLaunchMode;
    iSystemCharacteristics = aLCAppInfo.iSystemCharacteristics;
    iAppCharacteristics = aLCAppInfo.iAppCharacteristics;
    }
 
// ---------------------------------------------------------------------------
// CLcLocationAppInfo::CLcLocationAppInfo
// ---------------------------------------------------------------------------
//
CLcLocationAppInfo::CLcLocationAppInfo()
    :iLaunchMode( EDefaultMode )
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.
    }
                         
// ---------------------------------------------------------------------------
// CLcLocationAppInfo::~CLcLocationAppInfo
// ---------------------------------------------------------------------------
//
EXPORT_C CLcLocationAppInfo::~CLcLocationAppInfo()
    {
    // C++ Destructor. Free all resources associated with this class.
    
    // Delete the buffer containing the Identifier
    delete iId;
    
    // Delete the buffer containing the name of the Location application
    delete iApplicationName;
    
    // Delete the Application Icon
    delete iApplicationIconData;
    }
        
// ---------------------------------------------------------------------------
// CLcLocationAppInfo* CLcLocationAppInfo::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CLcLocationAppInfo* CLcLocationAppInfo::NewL()
    {
    CLcLocationAppInfo* self = NewLC();
    CleanupStack::Pop( self );
    return self;         
    }

// ---------------------------------------------------------------------------
// CLcLocationAppInfo* CLcLocationAppInfo::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CLcLocationAppInfo* CLcLocationAppInfo::NewLC()
    {
    // Symbian Two phased constructor. Leaves the object on the Clean-up
    // stack.
    CLcLocationAppInfo* self = 
                new ( ELeave )CLcLocationAppInfo();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;         
    }

// ---------------------------------------------------------------------------
// CLcLocationAppInfo* CLcLocationAppInfo::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CLcLocationAppInfo* CLcLocationAppInfo::NewL(
                                    const CLcLocationAppInfo&  aLocAppInfo )
    {
    CLcLocationAppInfo* self = NewLC( aLocAppInfo );
    CleanupStack::Pop( self );
    return self;         
    }

// ---------------------------------------------------------------------------
// CLcLocationAppInfo* CLcLocationAppInfo::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CLcLocationAppInfo* CLcLocationAppInfo::NewLC(
                                    const CLcLocationAppInfo&  aLocAppInfo )
    {
    // Symbian Two phased constructor. Leaves the object on the Clean-up
    // stack.
    CLcLocationAppInfo* self = 
                new ( ELeave )CLcLocationAppInfo( aLocAppInfo );
    CleanupStack::PushL( self );
    self->ConstructL( aLocAppInfo );
    return self;         
    }
    
// ---------------------------------------------------------------------------
// void CLcLocationAppInfo::ConstructL
// ---------------------------------------------------------------------------
//
void CLcLocationAppInfo::ConstructL()
    {
    }
 
// ---------------------------------------------------------------------------
// void CLcLocationAppInfo::ConstructL
// ---------------------------------------------------------------------------
//
void CLcLocationAppInfo::ConstructL( const CLcLocationAppInfo&   aLCAppInfo )
    {
    // Copy the Identifier from the Argument Location Application App info.
    iId = aLCAppInfo.Id().AllocL();
    
    // Copy the Name from the Argument Location Application App info.
    iApplicationName = aLCAppInfo.Name().AllocL();
    
    // Create the Icon related objects.
    if ( aLCAppInfo.iApplicationIconData )
    	{
    	iApplicationIconData = aLCAppInfo.iApplicationIconData->Des().Alloc();
    	}
    iApplicationIconType = 	aLCAppInfo.iApplicationIconType;
	iIconId = aLCAppInfo.iIconId;
    }
     
// ---------------------------------------------------------------------------
// TUint32 CLcLocationAppInfo::IdL
// ---------------------------------------------------------------------------
//
EXPORT_C TPtrC CLcLocationAppInfo::Id() const
    {
    // Check whether the UID exists, incase it doesnt exist then return a
    // NULL string.
    if( !iId )
        {
        return TPtrC();
        }
        
    // Return the UID.
    return *iId;
    }
 
// ---------------------------------------------------------------------------
// TDesC& CLcLocationAppInfo::NameL
// ---------------------------------------------------------------------------
//
EXPORT_C TPtrC CLcLocationAppInfo::Name() const
    {
    // Check whether the application name exists, incase it doesnt exist then
    // return a NULL string.
    if( !iApplicationName )
        {
        return TPtrC();
        }
    // Return the Application Name.
    return *iApplicationName;
    }

// ---------------------------------------------------------------------------
// CGulIcon& CLcLocationAppInfo::IconL
// ---------------------------------------------------------------------------
//
EXPORT_C CGulIcon* CLcLocationAppInfo::IconL() const
    {
    CGulIcon* icon = NULL;
    // If there is no Icon file present, then we have to load the default
    // Icon for the Application
    if ( iApplicationIconData )
    	{
    	switch( iApplicationIconType )
    		{
    		case CLcAppInfo::EIconfromApp:
    			{        	      	
	            // Since the request is for a Native S60 application, the Identifer
	            // contains an UID. Obtain it using Lexer
	            TLex lexer( iApplicationIconData->Des());
	            TUint32 uidValue;	            
	            User::LeaveIfError( lexer.BoundedVal( uidValue, EHex, KUidMaxValue ));
	                    	
				// Obtain the Location Icon.	                    	
	        	icon = LcIconLoader::LoadApplicationIconL( TUid::Uid( uidValue ));
    			
    			break;
    			}
    		case CLcAppInfo::EMifFile:
    			{
    			icon = LcIconLoader::LoadMifFileIconL( iApplicationIconData->Des(),
    												    iIconId );
    			break;
    			}
    		default:
    			{
    			icon = LcIconLoader::LoadDefaultIconL();
    			break;
    			}
    		}
    	}
	else
		{
		icon = LcIconLoader::LoadDefaultIconL();  
		}    	
  	return icon;
    }

// ---------------------------------------------------------------------------
// CLcLocationAppInfo::TLcLaunchMode
//                  CLcLocationAppInfo::LocationApplicationIconL
// ---------------------------------------------------------------------------
//
EXPORT_C CLcLocationAppInfo::TLcLaunchMode CLcLocationAppInfo::LaunchMode() const
    {
    return iLaunchMode;
    }

// ---------------------------------------------------------------------------
// TUint32 CLcLocationAppInfo::SystemCharacteristics
// ---------------------------------------------------------------------------
// 
EXPORT_C TUint32 CLcLocationAppInfo::SystemCharacteristics() const
    {
    return iSystemCharacteristics;
    }
 
// ---------------------------------------------------------------------------
// TUint32 CLcLocationAppInfo::ApplicationCharacteristics
// ---------------------------------------------------------------------------
// 
EXPORT_C TUint32 CLcLocationAppInfo::ApplicationCharacteristics() const
    {
    return iAppCharacteristics;
    }
                    
// ---------------------------------------------------------------------------
// TUint32 CLcLocationAppInfo::SetIdL
// ---------------------------------------------------------------------------
//
void CLcLocationAppInfo::SetIdL( const TDesC& aId )
    {
    // Incase, the UID already exists then Leave with KErrAlreadyExists
    if( iId )
        {
        User::Leave( KErrAlreadyExists );
        }
        
    // Copy the Identifier from the Argument Location Application App info.
    iId = aId.AllocL();
    
    }
    
// ---------------------------------------------------------------------------
// void CLcLocationAppInfo::SetNameL
// ---------------------------------------------------------------------------
//
void CLcLocationAppInfo::SetNameL( const TDesC& aName )
    {
    // Incase, the application name exists then Leave with KErrAlreadyExists.
    if( iApplicationName )
        {
        User::Leave( KErrAlreadyExists );
        }
        
    // Copy the Name from the Argument Location Application App info.
    iApplicationName = aName.AllocL();  
    }

// ---------------------------------------------------------------------------
// void CLcLocationAppInfo::SetIconL
// ---------------------------------------------------------------------------
//
void CLcLocationAppInfo::SetIconL( 		 TInt		aIconType,
				   				   const TDesC&		aIconData,
				   		 				 TInt		aIconId )
    {	    
    // Incase, the application icon exists then Leave with KErrAlreadyExists.
    if( iApplicationIconData )
        {
        User::Leave( KErrAlreadyExists );
        }
    
    if( !aIconData.Length())
        {
        User::Leave( KErrArgument );
        }
                    
    // Create the Icon related objects.
    iApplicationIconData = aIconData.Alloc();
    iApplicationIconType = 	aIconType;
	iIconId = aIconId;
    }

// ---------------------------------------------------------------------------
// void CLcLocationAppInfo::SetLocationApplicationLaunchMode
// ---------------------------------------------------------------------------
//
void CLcLocationAppInfo::SetLaunchMode(
                            CLcLocationAppInfo::TLcLaunchMode aLaunchMode )
    {
    iLaunchMode = aLaunchMode;
    }
 

// ---------------------------------------------------------------------------
// void CLcLocationAppInfo::SetSystemCharacteristics
// ---------------------------------------------------------------------------
//    
void CLcLocationAppInfo::SetSystemCharacteristics( 
                            TUint32   aSysCharacteristics )
    {
    iSystemCharacteristics = aSysCharacteristics;
    }                            

// ---------------------------------------------------------------------------
// void CLcLocationAppInfo::SetApplicationCharacteristics
// ---------------------------------------------------------------------------
//    
void CLcLocationAppInfo::SetApplicationCharacteristics( 
                            TUint32   aAppCharacteristics )
    {
    iAppCharacteristics = aAppCharacteristics;
    } 
        
// ----- Member funtions for CLcLocationAppInfoArray -------------
 
// ---------------------------------------------------------------------------
// CLcLocationAppInfoArray::CLcLocationAppInfoArray
// ---------------------------------------------------------------------------
//
CLcLocationAppInfoArray::CLcLocationAppInfoArray()
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.
    }
      
// ---------------------------------------------------------------------------
// CLcLocationAppInfoArray::~CLcLocationAppInfoArray
// ---------------------------------------------------------------------------
//
EXPORT_C CLcLocationAppInfoArray::~CLcLocationAppInfoArray()
    {
    // C++ Destructor. Free all resources associated with this class.
    
    // Delete all the the associated Location application information objects.
	while ( iAppInfoArray.Count() > 0 )
		{
		// Remove the first item
		CLcLocationAppInfo* item = iAppInfoArray[0];
		iAppInfoArray.Remove( 0 );
		
		// Delete the element
		delete item;
		item = NULL;
		}
	iAppInfoArray.Reset();
	iAppInfoArray.Close();    
    }
        
// ---------------------------------------------------------------------------
// CLcLocationAppInfoArray* 
//                      CLcLocationAppInfoArray::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CLcLocationAppInfoArray* 
                                CLcLocationAppInfoArray::NewL()
    {
    CLcLocationAppInfoArray* self = NewLC();
    CleanupStack::Pop( self );
    return self;         
    }

// ---------------------------------------------------------------------------
// CLcLocationAppInfoArray* 
//                      CLcLocationAppInfoArray::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CLcLocationAppInfoArray* 
                                CLcLocationAppInfoArray::NewLC()
    {
    // Symbian Two phased constructor. Leaves the object on the Clean-up
    // stack.
    CLcLocationAppInfoArray* self = 
                new ( ELeave )CLcLocationAppInfoArray();
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;         
    }

// ---------------------------------------------------------------------------
// void CLcLocationAppInfoArray::ConstructL
// ---------------------------------------------------------------------------
//
void CLcLocationAppInfoArray::ConstructL()
    {
    // Second phase of the two phase constructor.
    }

// ---------------------------------------------------------------------------
// void CLcLocationAppInfoArray::Count
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CLcLocationAppInfoArray::Count() const
    {
    // Return the number of elements in the Application information array.
    return  iAppInfoArray.Count();      
    }

// ---------------------------------------------------------------------------
// void CLcLocationAppInfoArray::AppendL
// ---------------------------------------------------------------------------
//
EXPORT_C void CLcLocationAppInfoArray::AppendL( 
                            CLcLocationAppInfo*     aLCAppInfo )
    {
    // Check if the element passed as an argument is NULL. If its NULL, then
    // Leave with KErrArgument
    // Not using User::LeaveIfNull because in that case, the leave would happen
    // with KErrNoMemory which conveys the wrong behaviour.
    if( !aLCAppInfo )
        {
        User::Leave( KErrArgument );
        }
    
    // Append the new element into the array;
    iAppInfoArray.AppendL( aLCAppInfo );     
    }

// ---------------------------------------------------------------------------
// CLcLocationAppInfo* CLcLocationAppInfoArray::RemoveL
// ---------------------------------------------------------------------------
//
EXPORT_C CLcLocationAppInfo* CLcLocationAppInfoArray::Remove( TInt  aIndex )
    {
    // Check if the element requested is out of array bounds
    if( aIndex < 0 || aIndex >= iAppInfoArray.Count())
        {
        User::Panic( KLcPanicCategory, ELcInvalidIndex );
        }
        
    // Since, the remove method is expected to return back the reference
    // of the element removed, first store the element at the aIndex
    // position and return it back                            
    CLcLocationAppInfo* item = iAppInfoArray[ aIndex ];
    iAppInfoArray.Remove( aIndex );
    return item;
    }

// ---------------------------------------------------------------------------
// CLcLocationAppInfo* CLcLocationAppInfoArray::Reset
// ---------------------------------------------------------------------------
//
EXPORT_C void CLcLocationAppInfoArray::Reset()
    {
    // Reset the entire array so that its ok for resuse.
    iAppInfoArray.ResetAndDestroy();
    }
    
// ---------------------------------------------------------------------------
// CLcLocationAppInfo& CLcLocationAppInfoArray::
//                            operator[]
// ---------------------------------------------------------------------------
//     
EXPORT_C CLcLocationAppInfo& CLcLocationAppInfoArray::
                                operator[]( TInt aIndex ) const
    {
    // Check if the element requested is out of array bounds
    if( aIndex < 0 || aIndex >= iAppInfoArray.Count())
        {
        User::Panic( KLcPanicCategory, ELcInvalidIndex );
        }

    // Return only a reference of the object. The ownership is not be 
    // transferred.
    return *(iAppInfoArray[ aIndex ]);    
    }
        
// End of File

