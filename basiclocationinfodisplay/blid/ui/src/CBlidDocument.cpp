/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides blid document class methods.
*
*/


// INCLUDE FILES
#include <sysutil.h>
#include <f32file.h>
#include <pathinfo.h>
#include <driveinfo.h>

#include "CBlidDocument.h"
#include "CBlidAppUi.h"
#include "CBlidEng.h" 
#include "bliduiconsts.h"

// constants
const TInt KBytesToWrite = 2*sizeof(TInt);

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CBlidDocument::CBlidDocument
// First phase constructor, may not leave
// ----------------------------------------------------------------------------
//
CBlidDocument::CBlidDocument(CEikApplication& aApp)
: CAknDocument(aApp)    
    {
    }

// ----------------------------------------------------------------------------
// CBlidDocument::~CBlidDocument
// Destructor, frees allocated resources
// ----------------------------------------------------------------------------
//
CBlidDocument::~CBlidDocument()
    {
    delete iEngine;
    }

// ----------------------------------------------------------------------------
// CBlidDocument::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CBlidDocument::ConstructL()
    {
    CreateEngineL();
    }

// ----------------------------------------------------------------------------
// CBlidDocument::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBlidDocument* CBlidDocument::NewL(
        CEikApplication& aApp)     // CLocApp reference
    {
    CBlidDocument* self = new (ELeave) CBlidDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// ----------------------------------------------------
// CBlidDocument::CreateAppUiL
// constructs CBlidAppUi
// ----------------------------------------------------
//
CEikAppUi* CBlidDocument::CreateAppUiL()
    {
    return new (ELeave) CBlidAppUi;
    }

// ----------------------------------------------------------------------------
// CBlidDocument::CreateEngineL
// Create instance of CBlidEng
// ----------------------------------------------------------------------------
//
void CBlidDocument::CreateEngineL()
    {
    iEngine = CBlidEng::NewL();
    }

// ----------------------------------------------------------------------------
// CBlidDocument::Engine
// Returns the CBlidEng instance
// ----------------------------------------------------------------------------
//
CBlidEng* CBlidDocument::Engine()
    {
    return iEngine;
    }

// ----------------------------------------------------------------------------
// CBlidDocument::StoreL
// Stores the app's document
// ----------------------------------------------------------------------------
//
void CBlidDocument::StoreL( CStreamStore& aStore, 
            CStreamDictionary& aStreamDic ) const
    {
    TStreamId id = iEngine->StoreL( aStore, *EditStore() );
    aStreamDic.AssignL( KBlidUI, id );
    }

// ----------------------------------------------------------------------------
// CBlidDocument::RestoreL
// Restore the app's document
// ----------------------------------------------------------------------------
//
void CBlidDocument::RestoreL( const CStreamStore& aStore,
            const CStreamDictionary& aStreamDic )
    {
    TStreamId streamId = aStreamDic.At( KBlidUI );
    iEngine->RestoreL( aStore, streamId );
    SetChanged( EFalse );
    }

// ----------------------------------------------------------------------------
// CBlidDocument::OpenFileL
// Restores the document's state from the specified file
// ----------------------------------------------------------------------------
//
CFileStore* CBlidDocument::OpenFileL(TBool aDoOpen, 
                      const TDesC& aFileName, 
                      RFs& aFs)
    {
    // changes done for multiple drive support	   
    TChar colon = ':';
	TFileName wpFileName;
	wpFileName.Append(PathInfo::PhoneMemoryRootPath()[0]);
	wpFileName.Append(colon);
	wpFileName.Append(aFileName);

	if(!aDoOpen )
	   {
	   if ( SysUtil::DiskSpaceBelowCriticalLevelL( &aFs, KBytesToWrite, DriveInfo::EDefaultPhoneMemory ) )
	       {
	       User::Leave(KErrNoMemory);
	       }
	   }
    return CEikDocument::OpenFileL( aDoOpen, wpFileName, aFs );
    }

// End of File  
