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
#include "CLmkParsedLmListProvider.h"
#include "CLmkLandmarkUiItem.h"
#include "MLmkSelectorIconMgr.h"
#include "CLmkParser.h"
#include <EPos_CPosLandmark.h>
#include <lmkerrors.h>

#if defined(_DEBUG)
// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkParsedLmListProvider" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace
#endif
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkParsedLmListProvider::CLmkParsedLmListProvider
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkParsedLmListProvider::CLmkParsedLmListProvider(
    CLmkParser& aParser )
    : iParser( aParser )
    {
    }

// -----------------------------------------------------------------------------
// CLmkParsedLmListProvider::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkParsedLmListProvider::ConstructL()
    {
    // base class 2nd phase constructor
    CLmkListProviderBase::BaseConstructL();

    }

// -----------------------------------------------------------------------------
// CLmkParsedLmListProvider::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkParsedLmListProvider* CLmkParsedLmListProvider::NewL(
    CLmkParser& aParser )
    {
    CLmkParsedLmListProvider* self =
        new ( ELeave ) CLmkParsedLmListProvider( aParser );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------
// CLmkParsedLmListProvider::~CLmkParsedLmListProvider
// ---------------------------------------------------------
//
CLmkParsedLmListProvider::~CLmkParsedLmListProvider()
    {
    }

// ---------------------------------------------------------
// CLmkParsedLmListProvider::InitializeL
// ---------------------------------------------------------
//
void CLmkParsedLmListProvider::InitializeL()
    {
    // This class is not responsible of db initialization or
    // parser initialization.
    }

// ---------------------------------------------------------
// CLmkParsedLmListProvider::StartSearchingLandmarksL()
// ---------------------------------------------------------
//
TBool CLmkParsedLmListProvider::StartSearchingLandmarksL(
								const TDesC& /*aSearchPattern*/,
    							TBool /*aSearchOnlyInPreviousMatches*/)
	{
	// nothing to do
	return ETrue;
	}

// ---------------------------------------------------------
// CLmkParsedLmListProvider::ItemCount
// ---------------------------------------------------------
//
TInt CLmkParsedLmListProvider::ItemCount() const
    {
    return iParser.NumOfParsedLandmarks();
    }

// ---------------------------------------------------------
// CLmkParsedLmListProvider::ItemAtL
// ---------------------------------------------------------
//
CLmkUiItemBase* CLmkParsedLmListProvider::ItemAtL( TInt aIndex )
    {
    __ASSERT_DEBUG( iIconMgr, Panic( KLmkPanicNullMember ) );

    CPosLandmark* landmark = iParser.LandmarkLC( aIndex );

    // Icons are not exchanged when messaging so
    // use default icon for landmark:
    TInt iconListIndex = iIconMgr->GetDefaultIconL(
            MLmkSelectorIconMgr::ELandmarkDefaultIcon );

    CLmkLandmarkUiItem* uiItem =
        CLmkLandmarkUiItem::NewL( landmark, iconListIndex );
    CleanupStack::Pop(); // landmark, ownership transferred
    return uiItem;
    }

//  End of File
