/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of CFileSystemInfo class which provides application's private path information
*
*/



// INCLUDE FILES
#include "FileSystemInfo.h"
#include <coemain.h>
#include <apgcli.h>

// ========================= MEMBER FUNCTIONS ================================

// ----------------------------------------------------------
// CFileSystemInfo::NewL()
// ----------------------------------------------------------
//
CFileSystemInfo* CFileSystemInfo::NewL(const TUid & aApplicationUid)
    {
    CFileSystemInfo* self = CFileSystemInfo::NewLC(aApplicationUid);
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------
// CFileSystemInfo::NewLC()
// ----------------------------------------------------------
//
CFileSystemInfo* CFileSystemInfo::NewLC(const TUid & aApplicationUid)
    {
    CFileSystemInfo* self = new (ELeave) CFileSystemInfo();
    CleanupStack::PushL( self );
    self->ConstructL(aApplicationUid);
    return self;
    }

// ---------------------------------------------------------
// CFileSystemInfo::CFileSystemInfo()
// ---------------------------------------------------------
//
CFileSystemInfo::CFileSystemInfo()
    {
    }

// ---------------------------------------------------------
// CFileSystemInfo::~CFileSystemInfo()
// ---------------------------------------------------------
//
CFileSystemInfo::~CFileSystemInfo()
    {
    }

// ---------------------------------------------------------
// CFileSystemInfo::PrivatePath()
// ---------------------------------------------------------
//
TFileName CFileSystemInfo::PrivatePath() const
	{
	return iPrivatePath;
	}

// ---------------------------------------------------------
// CFileSystemInfo::ConstructL()
// ---------------------------------------------------------
//
void CFileSystemInfo::ConstructL(const TUid & aApplicationUid)
    {
    RApaLsSession apaSession;
    
    // Connect to application architecture service.
    User::LeaveIfError( apaSession.Connect() );
        
    // Retrieve the application info
    TApaAppInfo appInfo;
    User::LeaveIfError( apaSession.GetAppInfo( appInfo, aApplicationUid ) );
    
    // Close connection to application architecture service.
    apaSession.Close();
            
    RFs fs;
    User::LeaveIfError(fs.Connect());
    
    // Parse the installation drive from the application path.
    TParse driveParse;
    driveParse.Set( appInfo.iFullName, NULL, NULL );
    iPrivatePath.Append( driveParse.Drive() );
    
    // Append the private path to installation drive.
    TFileName privatePath;
    fs.PrivatePath( privatePath ); 
    iPrivatePath.Append( privatePath );    
    fs.Close();
    
    }
	
// End of file

