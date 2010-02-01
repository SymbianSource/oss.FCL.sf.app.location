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


// ----- Member funtions for CLcBasicAppInfo ------------------------------

// ---------------------------------------------------------------------------
// CLcBasicAppInfo::CLcBasicAppInfo
// ---------------------------------------------------------------------------
//
inline CLcBasicAppInfo::CLcBasicAppInfo()
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.
    }
                         
// ---------------------------------------------------------------------------
// CLcBasicAppInfo::~CLcBasicAppInfo
// ---------------------------------------------------------------------------
//
inline CLcBasicAppInfo::~CLcBasicAppInfo()
    {
    // C++ Destructor. Free all resources associated with this class.
    
    // Delete the Application data content
    delete iAppData;
    
    // Delete the buffer containing the command line arguments
    delete iCmdLineParams;
    }
        
// ---------------------------------------------------------------------------
// CLcBasicAppInfo* CLcBasicAppInfo::NewL
// ---------------------------------------------------------------------------
//
inline CLcBasicAppInfo* CLcBasicAppInfo::NewL()
    {
    return  ( new ( ELeave ) CLcBasicAppInfo );        
    }

// ---------------------------------------------------------------------------
// CLcBasicAppInfo* CLcBasicAppInfo::NewLC
// ---------------------------------------------------------------------------
//
inline CLcBasicAppInfo* CLcBasicAppInfo::NewLC()
    {
    CLcBasicAppInfo* self = new ( ELeave ) CLcBasicAppInfo;
    CleanupStack::PushL( self );
    return self;        
    }

// ---------------------------------------------------------------------------
// TUint32 CLcBasicAppInfo::ApplicationType
// ---------------------------------------------------------------------------
//
inline TUint32 CLcBasicAppInfo::ApplicationType() const
    {
    return iAppType;
    }

// ---------------------------------------------------------------------------
// void CLcBasicAppInfo::SetApplicationType
// ---------------------------------------------------------------------------
//
inline void CLcBasicAppInfo::SetApplicationType( TUint32      aAppType )
    {
    iAppType = aAppType;
    }

// ---------------------------------------------------------------------------
// TUint32 CLcBasicAppInfo::LocationApplicationIconL
// ---------------------------------------------------------------------------
//
inline TUint32 CLcBasicAppInfo::LaunchMode() const
    {
    return iLaunchMode;
    }

// ---------------------------------------------------------------------------
// void CLcBasicAppInfo::SetLocationApplicationLaunchMode
// ---------------------------------------------------------------------------
//
inline void CLcBasicAppInfo::SetLaunchMode( TUint32  aLaunchMode )
    {
    iLaunchMode = aLaunchMode;
    }
        
// ---------------------------------------------------------------------------
// TPtrC CLcBasicAppInfo::ApplicationData
// ---------------------------------------------------------------------------
//
inline TPtrC CLcBasicAppInfo::ApplicationData() const
    {
    // Check whether the application data exists, incase it doesnt exist then
    // return a NULL string else return Application data.
    if ( iAppData )
        {
        return *iAppData;
        }
    else
        {
        return TPtrC();
        }
    }

// ---------------------------------------------------------------------------
// void CLcBasicAppInfo::ApplicationData
// ---------------------------------------------------------------------------
//
inline void CLcBasicAppInfo::SetApplicationDataL( const TDesC&   aAppData )
    {   
    delete iAppData;
    
    // Copy the Identifier from the Argument Location Application App info.
    iAppData = aAppData.AllocL();
    }
    
// ---------------------------------------------------------------------------
// TPtrC CLcBasicAppInfo::CommandLineParameters
// ---------------------------------------------------------------------------
//
inline TPtrC CLcBasicAppInfo::CommandLineParamters() const
    {
    // Check whether the command line args exists, incase it doesnt exist then
    // return a NULL string else return command line arguments.
    if ( iCmdLineParams )
        {
        return *iCmdLineParams;
        }
    else
        {
        return TPtrC();
        }
    }

// ---------------------------------------------------------------------------
// void CLcBasicAppInfo::SetCommandLineParameters
// ---------------------------------------------------------------------------
//
inline void CLcBasicAppInfo::SetCommandLineParametersL( const TDesC&   aCmdLineParams )
    {
    delete iCmdLineParams;
       
    // Copy the Identifier from the Argument Location Application App info.
    iCmdLineParams = aCmdLineParams.AllocL();
    }    
    
// ----- Member funtions for CLcAppInfo ------------------------------

// ---------------------------------------------------------------------------
// CLcAppInfo::CLcAppInfo
// ---------------------------------------------------------------------------
//
inline CLcAppInfo::CLcAppInfo()
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.
    }
                         
// ---------------------------------------------------------------------------
// CLcAppInfo::~CLcAppInfo
// ---------------------------------------------------------------------------
//
inline CLcAppInfo::~CLcAppInfo()
    {
    // C++ Destructor. Free all resources associated with this class.
    
    // Delete the buffer containing the Identifier
    delete iId;
    
    // Delete the buffer containing the name of the Location application
    delete iApplicationName;
    
    // Delete the buffer containing the Icon file name
    delete iIconFile;
    }
        
// ---------------------------------------------------------------------------
// CLcAppInfo* CLcAppInfo::NewL
// ---------------------------------------------------------------------------
//
inline CLcAppInfo* CLcAppInfo::NewL()
    {
    return  ( new ( ELeave ) CLcAppInfo );        
    }

// ---------------------------------------------------------------------------
// CLcAppInfo* CLcAppInfo::NewLC
// ---------------------------------------------------------------------------
//
inline CLcAppInfo* CLcAppInfo::NewLC()
    {
    CLcAppInfo* self = new ( ELeave ) CLcAppInfo;
    CleanupStack::PushL( self );
    return self;        
    }
    
// ---------------------------------------------------------------------------
// TPtrC CLcAppInfo::IdL
// ---------------------------------------------------------------------------
//
inline TPtrC CLcAppInfo::Id() const
    {
    // Check whether the UID exists, incase it doesnt exist then return a
    // NULL string else return the UID.
    if ( iId )
        {
        return *iId;
        }
    else
        {
        return TPtrC();
        }
    }
    
// ---------------------------------------------------------------------------
// TUint32 CLcAppInfo::SetIdL
// ---------------------------------------------------------------------------
//
inline void CLcAppInfo::SetIdL( const TDesC& aId )
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
// TPtrC CLcAppInfo::NameL
// ---------------------------------------------------------------------------
//
inline TPtrC CLcAppInfo::Name() const
    {
    // Check whether the application name exists, incase it doesnt exist then
    // return a NULL string else return Application name.
    if ( iApplicationName )
        {
        return *iApplicationName;
        }
    else
        {
        return TPtrC();
        } 
    }

// ---------------------------------------------------------------------------
// void CLcAppInfo::SetNameL
// ---------------------------------------------------------------------------
//
inline void CLcAppInfo::SetNameL( const TDesC& aName )
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
// CLcAppInfo::TLcIconFileType CLcAppInfo::IconFileType
// ---------------------------------------------------------------------------
//
inline CLcAppInfo::TLcIconFileType CLcAppInfo::IconFileType() const
    {
    return iIconFileType;
    }

// ---------------------------------------------------------------------------
// void CLcAppInfo::SetLocationApplicationLaunchMode
// ---------------------------------------------------------------------------
//
inline void CLcAppInfo::SetIconFileType( 
                    const CLcAppInfo::TLcIconFileType  aIconFileType )
    {
    iIconFileType = aIconFileType;
    }

// ---------------------------------------------------------------------------
// TPtrC CLcAppInfo::IconFile
// ---------------------------------------------------------------------------
//
inline TPtrC CLcAppInfo::IconFile() const
    {
    // Check whether the icon file exists, incase it doesnt exist then
    // return a NULL string else return icon file name.
    if ( iIconFile )
        {
        return *iIconFile;
        }
    else
        {
        return TPtrC();
        } 
    }

// ---------------------------------------------------------------------------
// void CLcAppInfo::SetIconFileL
// ---------------------------------------------------------------------------
//
inline void CLcAppInfo::SetIconFileL( const TDesC& aIconFileType )
    {
    // Incase, the icon file exists then Leave with KErrAlreadyExists.
    if( iIconFile )
        {
        User::Leave( KErrAlreadyExists );
        }
        
    // Copy the Name from the Argument Location Application App info.
    iIconFile = aIconFileType.AllocL();  
    }

// ---------------------------------------------------------------------------
// TUint32 CLcAppInfo::FrameNo
// ---------------------------------------------------------------------------
//
inline TUint32 CLcAppInfo::FrameNo() const
    {
    return iFrameNo;
    }

// ---------------------------------------------------------------------------
// void CLcAppInfo::SetFrameNo
// ---------------------------------------------------------------------------
//
inline void CLcAppInfo::SetFrameNo( TUint32  aFrameNo )
    {
    iFrameNo = aFrameNo;
    }
    
// ---------------------------------------------------------------------------
// TUint32 CLcAppInfo::SystemCharacteristics
// ---------------------------------------------------------------------------
// 
inline TUint32 CLcAppInfo::SystemCharacteristics() const
    {
    return iSystemCharacteristics;
    }

// ---------------------------------------------------------------------------
// void CLcAppInfo::SetSystemCharacteristics
// ---------------------------------------------------------------------------
//    
inline void CLcAppInfo::SetSystemCharacteristics( 
                            TUint32   aSysCharacteristics )
    {
    iSystemCharacteristics = aSysCharacteristics;
    } 
     
// ---------------------------------------------------------------------------
// TUint32 CLcAppInfo::ApplicationCharacteristics
// ---------------------------------------------------------------------------
// 
inline TUint32 CLcAppInfo::ApplicationCharacteristics() const
    {
    return iAppCharacteristics;
    }                           

// ---------------------------------------------------------------------------
// void CLcAppInfo::SetApplicationCharacteristics
// ---------------------------------------------------------------------------
//    
inline void CLcAppInfo::SetApplicationCharacteristics( 
                            TUint32   aAppCharacteristics )
    {
    iAppCharacteristics |= aAppCharacteristics;
    }
