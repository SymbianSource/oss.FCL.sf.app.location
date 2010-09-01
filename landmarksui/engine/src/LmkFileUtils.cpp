/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -    Static utility class for file operations.
*
*/







// INCLUDE FILES
#include "LmkFileUtils.h"
#include <f32file.h>

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// LmkFileUtils::DeleteFileL
// ----------------------------------------------------
//
EXPORT_C void LmkFileUtils::DeleteFileL( const TDesC& aFile )
    {
    RFs fs;
    CleanupClosePushL( fs );
    User::LeaveIfError( fs.Connect() );
    TInt result = fs.Delete( aFile );
    if ( result != KErrNone && result != KErrNotFound )
        {
        User::Leave( result );
        }
    CleanupStack::PopAndDestroy(); // fs
    }
// ----------------------------------------------------
// LmkFileUtils::IsValidPath
// ----------------------------------------------------
//
EXPORT_C TBool LmkFileUtils::IsValidPathL( TPtrC aFile )
    {
    RFs fs;
    CleanupClosePushL( fs );
    User::LeaveIfError( fs.Connect() );
    TBool result = fs.IsValidName( aFile );
    CleanupStack::PopAndDestroy(); // fs
    return result;
    }

//  End of File
