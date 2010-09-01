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
#include "CLmkSelectorCache.h"
#include "CLmkUiItemBase.h"
#include "CLmkListProviderBase.h"
#include <lmkerrors.h>


// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

const TInt KMaxCacheArraySize( 10 );
#if defined(_DEBUG)
_LIT( KPanicMsg, "CLmkSelectorCache" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
#endif
}  // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkSelectorCache::CLmkSelectorCache
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkSelectorCache::CLmkSelectorCache( CLmkListProviderBase& aListProvider )
    : iListProvider( aListProvider )
    {
    }

// -----------------------------------------------------------------------------
// CLmkSelectorCache::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkSelectorCache::ConstructL()
    {
    iListProvider.AddObserverL( *this );
    }

// -----------------------------------------------------------------------------
// CLmkSelectorCache::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkSelectorCache* CLmkSelectorCache::NewL(
    CLmkListProviderBase& aListProvider )
    {
    CLmkSelectorCache* self = new( ELeave ) CLmkSelectorCache( aListProvider );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CLmkSelectorCache::~CLmkSelectorCache
// ----------------------------------------------------
//
CLmkSelectorCache::~CLmkSelectorCache()
    {
    iListProvider.RemoveObserver(*this);
    iItemList.ResetAndDestroy();
    iItemList.Close();
    }

// -----------------------------------------------------------------------------
// CLmkSelectorCache::ItemAtL
// Cache takes the ownership of item, therefore it receives a pointer,
// but passes a reference to the lb model
// -----------------------------------------------------------------------------
//
CLmkUiItemBase& CLmkSelectorCache::ItemAtL( TInt aIndex )
    {
    __ASSERT_DEBUG( aIndex >= 0, Panic( KLmkPanicInvalidIndex ) );
    TInt count = iItemList.Count();
    for ( TInt i( 0 ); i < count; ++i )
        {
        if ( aIndex == iItemList[i]->iIndex )
            {
            CLinkedItem* tmp = iItemList[i];
            CleanupStack::PushL( tmp );
            iItemList.Remove( i );
            User::LeaveIfError( iItemList.Insert( tmp, 0 ) );
            CleanupStack::Pop( tmp );
            return *(iItemList[0]->iPtr);
            }
        }
    __ASSERT_DEBUG( iListProvider.ItemCount() > aIndex ,
                                Panic(KLmkPanicInvalidIndex) );
    CLinkedItem* tmp = new(ELeave) CLinkedItem();
    CleanupStack::PushL( tmp );
    tmp->iIndex = aIndex;
    tmp->iPtr = iListProvider.ItemAtL( aIndex );
    User::LeaveIfError( iItemList.Insert( tmp, 0 ) );
    CleanupStack::Pop(); // tmp

    if ( iItemList.Count() > KMaxCacheArraySize )
        {
        delete iItemList[KMaxCacheArraySize];
        iItemList.Remove( KMaxCacheArraySize );
        }
    return *( iItemList[0]->iPtr );
    }

// -----------------------------------------------------------------------------
// CLmkSelectorCache::ItemCount
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLmkSelectorCache::ItemCount() const
    {
    // dummy implementation:
    return iListProvider.ItemCount();
    }

// -----------------------------------------------------------------------------
// CLmkSelectorCache::HandleListProviderEventL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CLmkSelectorCache::HandleListProviderEvent(
    TLmkListProviderEventType /*aEvent*/ )
    {
    iItemList.ResetAndDestroy();
    }

// ----------------------------------------------------
// CLmkSelectorCache::HandleListProviderError
// ----------------------------------------------------
//
void CLmkSelectorCache::HandleListProviderError( TInt /*aError*/ )
    {
    iItemList.ResetAndDestroy();
    }

// ----------------------------------------------------
// CLmkSelectorCache::CLinkedItem
// ----------------------------------------------------
//
CLmkSelectorCache::CLinkedItem::CLinkedItem()
    {
    }

// ----------------------------------------------------
// CLmkSelectorCache::~CLinkedItem
// ----------------------------------------------------
//
CLmkSelectorCache::CLinkedItem::~CLinkedItem()
    {
    delete this->iPtr;
    }


//  End of File
