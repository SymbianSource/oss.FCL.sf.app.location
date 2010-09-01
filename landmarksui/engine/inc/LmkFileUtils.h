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







#ifndef LMKFILEUTILS_H
#define LMKFILEUTILS_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
*  Static utility class for file operations.
*/
class LmkFileUtils
    {
    public: // New functions

        /**
        * Helper method for deleting a file.
        * Error KErrNotFound is omitted but in case of other errors
        * this method leaves.
        * @param aFile full name of the file which is deleted
        */
        IMPORT_C static void DeleteFileL( const TDesC& aFile );
        /**
        * This function tests whether the file name or path is valid
        * or not.
        * @param aFile file name/path
        * @return ETrue if the name/path is valid else EFalse
        */
        IMPORT_C static TBool IsValidPathL( TPtrC aFile );
    };

#endif // LMKFILEUTILS_H

// End of File
