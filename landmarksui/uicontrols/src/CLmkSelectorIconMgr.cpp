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
* Description:    LandmarksUi Content File -
*
*/








// INCLUDE FILES
#include "CLmkSelectorIconMgr.h"
#include "LmkConsts.h"
#include "CLmkUiUtils.h"
#include <data_caging_path_literals.hrh>
#include <gulicon.h>
#include <AknIconArray.h>
#include <avkon.mbg>
#include <AknsConstants.h>
#include <AknsUtils.h>
#include <AknIconUtils.h>
#include <AknUtils.h>
#include <lmkerrors.h>


// CONSTANTS
/// Unnamed namespace for local definitions
namespace {
const TInt KLmkIconCacheGranularity = 30;
const TInt KLmkIconFilesGranularity = 5;
#if defined(_DEBUG)

_LIT(KPanicMsg,"CLmkSelectorIconMgr");

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
#endif
}  // namespace

// ----------------------------------------------------
// CLmkSelectorIconMgr::TIconData
// ----------------------------------------------------
//
CLmkSelectorIconMgr::TIconData::TIconData():
    iFileIndex( KErrNotFound ),
    iIconIndexWithinFile( KErrNotFound ),
    iIconType( EDefinedIcon ),
    iIconArrayId( KErrNotFound )
    {
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkSelectorIconMgr::CLmkSelectorIconMgr
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkSelectorIconMgr::CLmkSelectorIconMgr( CAknIconArray& aArray )
    : iArray( &aArray ),
      iCache( KLmkIconCacheGranularity )
    {
    }

// -----------------------------------------------------------------------------
// CLmkSelectorIconMgr::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkSelectorIconMgr::ConstructL()
    {
    iIconFiles = new( ELeave ) CDesCArrayFlat( KLmkIconFilesGranularity );
    ResetMgrL();
    }

// -----------------------------------------------------------------------------
// CLmkSelectorIconMgr::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//

CLmkSelectorIconMgr* CLmkSelectorIconMgr::NewL(
    CAknIconArray& aArray )
    {
    CLmkSelectorIconMgr* self = new( ELeave ) CLmkSelectorIconMgr( aArray );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CLmkSelectorIconMgr::~CLmkSelectorIconMgr
// ----------------------------------------------------
//
CLmkSelectorIconMgr::~CLmkSelectorIconMgr()
    {
    iCache.Close();
    delete iIconFiles;
    }

// ----------------------------------------------------
// CLmkSelectorIconMgr::GetIconL
// ----------------------------------------------------
//
TInt CLmkSelectorIconMgr::GetIconL(
    const TDesC& aMbmFileName,
    TInt aIconIdInFile )
    {
    TInt position( 0 );
    if ( iIconFiles->Find( aMbmFileName, position, ECmpNormal ) == 0 )
        { // same file has been used already and so it exists in the array
        TInt count = iCache.Count();
        for ( TInt i( 0 ); i < count; ++i )
            {
            if ( position == iCache[i].iFileIndex  &&
                aIconIdInFile == iCache[i].iIconIndexWithinFile )
                { // icon was found in icon manager!
                // If Array Index was not set properly, then set the index propely
                if (( iCache[i].iIconArrayId < 0) ||
                	  (iCache[i].iIconArrayId > KLmkIconCacheGranularity) )
	                {
	                break;
	                }
                return iCache[i].iIconArrayId;
                }
            }
        }

    // aMbmFileName does not yet exist in the file name array OR it exists but
    // this specific icon hasn't been requested yet:

    // create icon manager item:
    TInt iconMgrId = CreateIconMgrItemL( aMbmFileName, aIconIdInFile );

    // try to load new icon:
    iCache[iconMgrId].iIconArrayId =
        LoadAndAppendIconL( aMbmFileName, aIconIdInFile );

    return iCache[iconMgrId].iIconArrayId;
    }

// ----------------------------------------------------
// CLmkSelectorIconMgr::GetDefaultIconL
// ----------------------------------------------------
//
TInt CLmkSelectorIconMgr::GetDefaultIconL( TIconType aIcon )
    {
    TInt count = iCache.Count();
    for ( TInt i( 0  ); i < count ; ++i )
        {
        TIconData tmp = iCache[i];
        if ( tmp.iIconType == aIcon )
            { // default icon was found
            if ( tmp.iIconArrayId < 0 )
                { // try to load new icon.
                TPtrC iconFile = ( *iIconFiles )[tmp.iFileIndex];
                tmp.iIconArrayId =
                    LoadAndAppendIconL( iconFile, tmp.iIconIndexWithinFile );
                }
            return tmp.iIconArrayId;
            }
        }
    return KErrNotFound; // default icon was not found
    }

// ----------------------------------------------------
// CLmkSelectorIconMgr::SetDefaultIconL
// ----------------------------------------------------
//
void CLmkSelectorIconMgr::SetDefaultIconL(
    TIconType aIcon,
    const TDesC& aMbmFileName,
    TInt aIconIdInFile )
    {
    if ( GetDefaultIconL( aIcon ) >= 0 )
        { // default icon already set
        User::Leave( KErrAlreadyExists );
        }

    TInt iconMgrId = CreateIconMgrItemL( aMbmFileName, aIconIdInFile );
    iCache[iconMgrId].iIconType = aIcon;
    // Added for updating Icon index
    iCache[iconMgrId].iIconArrayId =
        LoadAndAppendIconL( aMbmFileName, aIconIdInFile );
    }

// ----------------------------------------------------
// CLmkSelectorIconMgr::SetIconAtIndexZeroL
// ----------------------------------------------------
//
void CLmkSelectorIconMgr::SetIconAtIndexZeroL(
    const TDesC& aMbmFileName,
    TAknsItemID aSkinID,
    TInt aIconId,
    TInt aMaskId )
    {
    __ASSERT_DEBUG( iArray->Count() >= 2,
                    Panic( KLmkPanicIndexesZeroOneNotSet ) );

    CGulIcon* icon = LoadIconL( aMbmFileName, aSkinID, aIconId, aMaskId );
    CleanupStack::PushL( icon );

    CGulIcon* old = iArray->At( 0 );
    iArray->Delete( 0 );
    delete old;
    iArray->InsertL( 0, icon );

    CleanupStack::Pop( icon ); // ownership transferred to icon array
    }

// ----------------------------------------------------
// CLmkSelectorIconMgr::SetIconAtIndexOneL
// ----------------------------------------------------
//
void CLmkSelectorIconMgr::SetIconAtIndexOneL(
    const TDesC& aMbmFileName,
    TAknsItemID aSkinID,
    TInt aIconId,
    TInt aMaskId )
    {
    __ASSERT_DEBUG( iArray->Count() >= 2,
                    Panic( KLmkPanicIndexesZeroOneNotSet ) );

    CGulIcon* icon = LoadIconL( aMbmFileName, aSkinID, aIconId, aMaskId );
    CleanupStack::PushL( icon );

    CGulIcon* old = iArray->At( 1 );
    iArray->Delete( 1 );
    delete old;
    iArray->InsertL( 1, icon );

    CleanupStack::Pop( icon ); // ownership transferred to icon array
    }

// ----------------------------------------------------
// CLmkSelectorIconMgr::ResetMgrL
// ----------------------------------------------------
//
void CLmkSelectorIconMgr::ResetMgrL()
    {
    iArray->ResetAndDestroy();
    iIconFiles->Reset();
    iCache.Reset();

    // By default set empty icons as selection icons:

    TFileName* iconFile = CLmkUiUtils::AvkonIconFileLC();
    CGulIcon* icon = LoadIconL( *iconFile,
                                KAknsIIDQgnPropEmpty,
                                EMbmAvkonQgn_prop_empty,
                                EMbmAvkonQgn_prop_empty_mask );
    CleanupStack::PushL( icon );
    iArray->AppendL( icon );
    CleanupStack::Pop( icon ); // ownership transferred to icon array
    icon = NULL;

    icon = LoadIconL( *iconFile,
                      KAknsIIDQgnPropEmpty,
                      EMbmAvkonQgn_prop_empty,
                      EMbmAvkonQgn_prop_empty_mask );
    CleanupStack::PushL( icon );
    iArray->AppendL( icon );
    CleanupStack::Pop( icon ); // ownership transferred to icon array
    CleanupStack::PopAndDestroy( ); // iconFile
    }

// ============================= HELPER METHODS ================================

// ----------------------------------------------------
// CLmkSelectorIconMgr::LoadIconL
// ----------------------------------------------------
//
CGulIcon* CLmkSelectorIconMgr::LoadIconL(
    const TDesC& aMbmFileName,
    TInt aIconIdInFile )
    {
    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* bitmapMask = NULL;

	AknIconUtils::CreateIconLC(bitmap, bitmapMask, aMbmFileName,
                             aIconIdInFile, aIconIdInFile + 1);
    CGulIcon* icon = CGulIcon::NewL( bitmap, bitmapMask );
    CleanupStack::Pop( 2 ); // bitmapMask, bitmap. icon has now ownership
    return icon;
    }

// ----------------------------------------------------
// CLmkSelectorIconMgr::LoadIconL
// ----------------------------------------------------
//
CGulIcon* CLmkSelectorIconMgr::LoadIconL(
    const TDesC& aMbmFileName,
    TAknsItemID aSkinID,
    TInt aIconId,
    TInt aMaskId )
    {
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    CFbsBitmap* bitmap = NULL;
    CFbsBitmap* bitmapMask = NULL;
    AknsUtils::CreateIconLC( skin, aSkinID, bitmap, bitmapMask, aMbmFileName,
                             aIconId, aMaskId );
    CGulIcon* icon = CGulIcon::NewL( bitmap, bitmapMask );
    CleanupStack::Pop( 2 ); // bitmapMask, bitmap. icon has now ownership
    return icon;
    }

// ----------------------------------------------------
// CLmkSelectorIconMgr::LoadAndAppendIconL
// ----------------------------------------------------
//
TInt CLmkSelectorIconMgr::LoadAndAppendIconL(
    const TDesC& aMbmFileName,
    TInt aIconIdInFile )
    {
    CGulIcon* icon = LoadIconL( aMbmFileName, aIconIdInFile );
    CleanupStack::PushL( icon );
    iArray->AppendL( icon );
    CleanupStack::Pop( icon );
    // Added for avoiding returning -1 as bitmap Index
    if (iArray->Count() > 0)
	    {
	    return iArray->Count() - 1;
	    }
    else
	    {
	    return 0;
	    }
    }

// ----------------------------------------------------
// CLmkSelectorIconMgr::CreateIconMgrItemL
// ----------------------------------------------------
//
TInt CLmkSelectorIconMgr::CreateIconMgrItemL(
    const TDesC& aMbmFileName,
    TInt aIconIdInFile )
    {
    TIconData tmp;


    tmp.iFileIndex = AppendedIconFileIndexL( aMbmFileName );
    tmp.iIconIndexWithinFile = aIconIdInFile;

    User::LeaveIfError( iCache.Append( tmp ) );
    if (iCache.Count() > 0)
	    {
	    return iCache.Count() - 1;
	    }
    else
	    {
	    return 0;
	    }
    }

// ----------------------------------------------------
// CLmkSelectorIconMgr::AppendedIconFileIndexL
// ----------------------------------------------------
//
TInt CLmkSelectorIconMgr::AppendedIconFileIndexL( const TDesC& aMbmFileName )
    {
    TInt position( 0 );
    if ( iIconFiles->Find( aMbmFileName, position, ECmpNormal ) != 0 )
        { // file does not yet exist in the file name array
        iIconFiles->AppendL( aMbmFileName );
        return iIconFiles->Count() - 1;
        }
    else
        { // file already exists, just return its position
        return position;
        }
    }

//  End of File
