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
* Description:    LandmarksUi Content File -    Provides LM document class methods.
*
*/







// INCLUDE FILES

#include <f32file.h>
#include "CLmkMsgViewerDocument.h"
#include "CLmkMsgViewerAppUi.h"

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CLmkMsgViewerDocument::CLmkMsgViewerDocument
// ----------------------------------------------------------------------------
//
CLmkMsgViewerDocument::CLmkMsgViewerDocument( CEikApplication& aApp )
    : CEikDocument( aApp )
    {
    }

// ----------------------------------------------------------------------------
// CLmkMsgViewerDocument::~CLmkMsgViewerDocument
// ----------------------------------------------------------------------------
//
CLmkMsgViewerDocument::~CLmkMsgViewerDocument()
    {
    iLandmarkContentFileHandle.Close();
    delete iFileFullPath;
    }

// ----------------------------------------------------------------------------
// CLmkMsgViewerDocument::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CLmkMsgViewerDocument::ConstructL()
    {
    }

// ----------------------------------------------------------------------------
// CLmkMsgViewerDocument::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CLmkMsgViewerDocument* CLmkMsgViewerDocument::NewL( CEikApplication& aApp )
    {
    CLmkMsgViewerDocument* self = new( ELeave ) CLmkMsgViewerDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CLmkMsgViewerDocument::CreateAppUiL
// constructs CLmkMsgViewerAppUi
// ----------------------------------------------------
//
CEikAppUi* CLmkMsgViewerDocument::CreateAppUiL()
    {
    return new( ELeave ) CLmkMsgViewerAppUi;
    }

// -----------------------------------------------------------------------------
// CLmkMsgViewerDocument::OpenFileL()
// Open document.
// -----------------------------------------------------------------------------
CFileStore* CLmkMsgViewerDocument::OpenFileL(
    TBool /*aDoOpen*/,
    const TDesC& aFilename,
    RFs& aFs )
    {
	//static_cast<CLmkMsgViewerAppUi*>( iAppUi )->OpenL( aFilename );

	// Function is called with Full-path.
    iIsFileFullPathValid = ETrue;
    // Store the File's fullpath.
    iFileFullPath = aFilename.AllocL();

    // Open File Handle for the file
    TInt openError = iLandmarkContentFileHandle.Open( aFs, aFilename,
        EFileShareReadersOnly);
    if ( openError != KErrNone )
        {
        User::Leave( openError );
        }
    iIsFileHandleValid = ETrue;
    return NULL;
    }

// -----------------------------------------------------------------------------
// CLmkMsgViewerDocument::OpenFileL()
// Open document.
// -----------------------------------------------------------------------------

void CLmkMsgViewerDocument::OpenFileL(
    CFileStore*& /* aFileStore */,
    RFile& aFile)
    {
    // Duplicate the file handle
    // Function is not called with Full-path.
    iIsFileFullPathValid = EFalse;

    // File-handle is valid
    iIsFileHandleValid = ETrue;
    iLandmarkContentFileHandle.Duplicate(aFile);
    aFile.Close();
	//static_cast<CLmkMsgViewerAppUi*>( iAppUi )->OpenL( aFile );
    //aFileStore = NULL;
    }

// -----------------------------------------------------------------------------
//  CLmkMsgViewerDocument::GetLandmarkContentFileHandle
// This function is a get function to get the file-handle of the file content.
// This is used by AppUI.
// Returns: File handle of content by reference .
// -----------------------------------------------------------------------------
//
RFile& CLmkMsgViewerDocument::GetLandmarkContentFileHandle()
    {
    return iLandmarkContentFileHandle;
    }

    // -----------------------------------------------------------------------------
// CLmkMsgViewerDocument::IsFileHandleValid()
// Used by UI class to check if file's handle is valid
// -----------------------------------------------------------------------------
//
TBool CLmkMsgViewerDocument::IsFileHandleValid() const
    {
    return (iIsFileHandleValid != NULL);
    }


// End of File
