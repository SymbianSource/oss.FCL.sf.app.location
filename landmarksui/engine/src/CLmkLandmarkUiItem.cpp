/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Landmarks info for landmarks list box model
*
*/






// INCLUDE FILES
#include "CLmkLandmarkUiItem.h"
#include <EPos_CPosLandmark.h>
#include "CLmkFields.h"
#include <lmkerrors.h>

#if defined(_DEBUG)
// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkLandmarkUiItem" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace
#endif
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkLandmarkUiItem::CLmkLandmarkUiItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkLandmarkUiItem::CLmkLandmarkUiItem(
    CPosLandmark* aLandmark,
    TInt aIconListIndex )
    : iLandmark( aLandmark ),
      iIconListIndex( aIconListIndex )
    {
    TPtrC name;
    iLandmark->GetLandmarkName( name );
    TPtr des1 = CLmkFields::RemoveEnterCharacter( name );
    iText.Set( des1 );
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkUiItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkLandmarkUiItem::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkUiItem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkLandmarkUiItem* CLmkLandmarkUiItem::NewL(
    CPosLandmark* aLandmark,
    TInt aIconListIndex )
    {
    __ASSERT_DEBUG( aLandmark, Panic( KLmkPanicNullMember ) );
    CLmkLandmarkUiItem* self =
        new ( ELeave ) CLmkLandmarkUiItem( aLandmark, aIconListIndex );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkUiItem::~CLmkLandmarkUiItem
// -----------------------------------------------------------------------------
//
CLmkLandmarkUiItem::~CLmkLandmarkUiItem()
    {
    delete iLandmark;
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkUiItem::Type
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TLmkItemType CLmkLandmarkUiItem::Type()
    {
    return ELmkItemTypeLandmark;
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkUiItem::Text
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC CLmkLandmarkUiItem::Text()
    {
    return iText;
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkUiItem::GetIconId
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLmkLandmarkUiItem::GetIconId()
    {
    return iIconListIndex;
    }

//  End of File
