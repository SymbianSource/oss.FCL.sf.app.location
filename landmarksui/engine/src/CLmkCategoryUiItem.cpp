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
* Description:    Category info item for landmarks list box model
*
*/




// INCLUDE FILES
#include "CLmkCategoryUiItem.h"
#include <EPos_CPosLandmarkCategory.h>
#include <lmkerrors.h>

#if defined(_DEBUG)
// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkCategoryUiItem" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace
#endif
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkCategoryUiItem::CLmkCategoryUiItem
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkCategoryUiItem::CLmkCategoryUiItem(
    CPosLandmarkCategory* aCategory,
    TInt aIconListIndex )
    : iCategory( aCategory ),
      iIconListIndex( aIconListIndex )
    {
    iCategory->GetCategoryName( iText );
    }

// -----------------------------------------------------------------------------
// CLmkCategoryUiItem::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkCategoryUiItem::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CLmkCategoryUiItem::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkCategoryUiItem* CLmkCategoryUiItem::NewL(
    CPosLandmarkCategory* aCategory,
    TInt aIconListIndex )
    {
    __ASSERT_DEBUG( aCategory, Panic( KLmkPanicNullMember ) );
    CLmkCategoryUiItem* self =
        new ( ELeave ) CLmkCategoryUiItem( aCategory, aIconListIndex );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CLmkCategoryUiItem::~CLmkCategoryUiItem
// -----------------------------------------------------------------------------
//
CLmkCategoryUiItem::~CLmkCategoryUiItem()
    {
    delete iCategory;
    }

// -----------------------------------------------------------------------------
// CLmkCategoryUiItem::Type
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TLmkItemType CLmkCategoryUiItem::Type()
    {
    return ELmkItemTypeCategory;
    }

// -----------------------------------------------------------------------------
// CLmkCategoryUiItem::Text
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TPtrC CLmkCategoryUiItem::Text()
    {
    return iText;
    }

// -----------------------------------------------------------------------------
// CLmkCategoryUiItem::GetIconId
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLmkCategoryUiItem::GetIconId()
    {
    return iIconListIndex;
    }

//  End of File
