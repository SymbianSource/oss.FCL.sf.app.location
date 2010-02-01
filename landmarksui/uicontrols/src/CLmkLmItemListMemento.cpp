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
#include "CLmkLmItemListMemento.h"
#include "CLmkLmItemListProvider.h"
#include <eiklbx.h>
#include <AknUtils.h>
#include <lmkerrors.h>


// Initial value for iMemorizedCurrentItemId, this is a quick fix for focus on
// uncategorised category bug.
//const TInt KIdNotFound = KErrNotFound;
const TUint32 KInititialValue = 0xFFFFFFFE;
#if defined(_DEBUG)

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkLmItemListMemento" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace

#endif
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkLmItemListMemento::CLmkLmItemListMemento
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkLmItemListMemento::CLmkLmItemListMemento()
    {
    //iMemorizedCurrentItemId = KPosLmNullItemId;
    iMemorizedCurrentItemId = KInititialValue;
    }

// -----------------------------------------------------------------------------
// CLmkLmItemListMemento::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkLmItemListMemento::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CLmkLmItemListMemento::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkLmItemListMemento* CLmkLmItemListMemento::NewL()
    {
    CLmkLmItemListMemento* self = new( ELeave ) CLmkLmItemListMemento();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ----------------------------------------------------
// CLmkLmItemListMemento::~CLmkLmItemListMemento
// ----------------------------------------------------
//
CLmkLmItemListMemento::~CLmkLmItemListMemento()
    {
    }

// ----------------------------------------------------
// CLmkLmItemListMemento::MemorizedItemId
// ----------------------------------------------------
//
EXPORT_C TPosLmItemId CLmkLmItemListMemento::MemorizedItemId() const
    {
    return iMemorizedCurrentItemId;
    }

// ----------------------------------------------------
// CLmkLmItemListMemento::Store
// ----------------------------------------------------
//
void CLmkLmItemListMemento::Store(
    const CEikListBox& aListBox,
    const CLmkListProviderBase& aProvider,
    TBool aFiltered )
    {
    if ( aFiltered )
        {
        CAknFilteredTextListBoxModel* model =
            static_cast<CAknFilteredTextListBoxModel*>( aListBox.Model() );
        __ASSERT_DEBUG( model, Panic( KLmkPanicNoLbModel ) );
        StoreFiltered( aListBox, aProvider, *model );
        }
    else
        {
        StoreNonFiltered( aListBox, aProvider );
        }
    }

// ----------------------------------------------------
// CLmkLmItemListMemento::Restore
// ----------------------------------------------------
//
void CLmkLmItemListMemento::Restore(
    const CEikListBox& aListBox,
    const CLmkListProviderBase& aProvider,
    TBool aFiltered )
    {
    if ( aFiltered )
        {
        CAknFilteredTextListBoxModel* model =
            static_cast<CAknFilteredTextListBoxModel*>( aListBox.Model() );
        __ASSERT_DEBUG( model, Panic( KLmkPanicNoLbModel ) );
        RestoreFiltered( aListBox, aProvider, *model );
        }
    else
        {
        RestoreNonFiltered( aListBox, aProvider );
        }
    }

// ----------------------------------------------------
// CLmkLmItemListMemento::StoreFiltered
// ----------------------------------------------------
//
void CLmkLmItemListMemento::StoreFiltered(
    const CEikListBox& aListBox,
    const CLmkListProviderBase& aProvider,
    const CAknFilteredTextListBoxModel& aModel )
    {
    const CLmkLmItemListProvider& provider =
        static_cast <const CLmkLmItemListProvider&> ( aProvider );
    if ( aListBox.Model()->NumberOfItems() > 0 &&
         aListBox.CurrentItemIndex() >= 0 )
        {
        iMemorizedCurrentItemIndex =
            aModel.Filter()->FilteredItemIndex( aListBox.CurrentItemIndex() );
        provider.GetSelectedItemId( iMemorizedCurrentItemId,
                                iMemorizedCurrentItemIndex );
        }
    else
        {
        iMemorizedCurrentItemIndex = 0;
       // iMemorizedCurrentItemId = KPosLmNullItemId;
        iMemorizedCurrentItemId = KInititialValue;
        }
    }

// ----------------------------------------------------
// CLmkLmItemListMemento::RestoreFiltered
// ----------------------------------------------------
//
void CLmkLmItemListMemento::RestoreFiltered(
    const CEikListBox& aListBox,
    const CLmkListProviderBase& aProvider,
    const CAknFilteredTextListBoxModel& /*aModel*/ )
    {
    const CLmkLmItemListProvider& provider =
        static_cast <const CLmkLmItemListProvider&> ( aProvider );
    TInt index = provider.SelectedIndex( iMemorizedCurrentItemId );
    if ( index != KErrNotFound &&
         index < aListBox.Model()->NumberOfItems() )
        {
        aListBox.SetCurrentItemIndex( index );
        aListBox.ScrollToMakeItemVisible( index );
        }
    else
        {
        aListBox.SetCurrentItemIndex( 0 );
        }
    aListBox.DrawDeferred();
    }

// ----------------------------------------------------
// CLmkLmItemListMemento::StoreNonFiltered
// ----------------------------------------------------
//
void CLmkLmItemListMemento::StoreNonFiltered(
    const CEikListBox& aListBox,
    const CLmkListProviderBase& aProvider )
    {
    // TODO: these are not used and implementation is quite sketchy.
    // Replace with leave KErrNotSupported if this method won't be used.
    const CLmkLmItemListProvider& provider =
        static_cast <const CLmkLmItemListProvider&> ( aProvider );
    iMemorizedCurrentItemIndex = aListBox.CurrentItemIndex();
    provider.GetSelectedItemId( iMemorizedCurrentItemId,
                                iMemorizedCurrentItemIndex );
    iMemorizedTopItemIndex = aListBox.TopItemIndex();
    }

// ----------------------------------------------------
// CLmkLmItemListMemento::RestoreNonFiltered
// ----------------------------------------------------
//
void CLmkLmItemListMemento::RestoreNonFiltered(
    const CEikListBox& aListBox,
    const CLmkListProviderBase& aProvider )
    {
    // TODO: these are not used and implementation is quite sketchy.
    // Replace with leave KErrNotSupported if this method won't used.
    const CLmkLmItemListProvider& provider =
        static_cast <const CLmkLmItemListProvider&> ( aProvider );
    TInt index = provider.SelectedIndex( iMemorizedCurrentItemId );
    if ( index != KErrNotFound )
        {
        if ( index == iMemorizedCurrentItemIndex )
            {
            aListBox.SetCurrentItemIndex( iMemorizedCurrentItemIndex );
            aListBox.SetTopItemIndex( iMemorizedTopItemIndex );
            }
        else
            {
            aListBox.ScrollToMakeItemVisible( index );
            }
        }
    else
        {
        aListBox.SetCurrentItemIndex( 0 );
        }
    aListBox.DrawDeferred();
    }

// ----------------------------------------------------
// CLmkLmItemListMemento::IsModelAvailable
// ----------------------------------------------------
//anwar
TBool CLmkLmItemListMemento::IsModelAvailable()
	{
	return iModelAvailable;
	}
// ----------------------------------------------------
// CLmkLmItemListMemento::UpdateModelAvailability
// ----------------------------------------------------
//anwar
void  CLmkLmItemListMemento::UpdateModelAvailability(TBool aModelAvailable)
	{
	iModelAvailable = aModelAvailable;
	}
//  End of File
