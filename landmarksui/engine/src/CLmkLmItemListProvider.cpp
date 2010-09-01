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
* Description:    Base class landmarks list provider for listbox model
*
*/


// SYSTEM INCLUDES
#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLmOperation.h>
#include <EPos_CPosLmCategoryManager.h>
#include <AknWaitDialog.h>

// USER INCLUDES
#include "CLmkLmItemListProvider.h"
#include "MLmkListProviderObserver.h"
#include "CLmkDbUtils.h"
#include "CLmkDbInitializer.h"
#include "CLmkEventListenerSingleton.h"
#include "CLmkAOOperation.h"
#include "LmkConsts.h"
#include <lmkerrors.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkLmItemListProvider" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::CLmkLmItemListProvider
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkLmItemListProvider::CLmkLmItemListProvider(
    CPosLandmarkDatabase& aDb )
    : iDb( aDb ),
      iListAllItemsInDb( ETrue ),
      iIdArray( KLmItemArrayGranularity ),
      iSelectedItemsCount( 0 ), iCount( 0 ),
      iRequestFromFindBox(EFalse),
      iOperationNotInUse( ETrue ),
      iOperationCmd( ELmkCmdStopOperation ),  //temp default value
      iSelectedForDelete(KLmItemArrayGranularity),
      iItemsToDelete( 0 ),
      iItemsDeleted( 0 ),
      iIsRemoveLandmarks( EFalse ),
      iCategoryId(KPosLmNullItemId)
    {
    }

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkLmItemListProvider::BaseConstructL()
    {
    // Create db utils, request callback only when operations completed:
    iDbUtils = CLmkDbUtils::NewL( iDb, *this, EFalse );
    //Create landmark array, which will hold all those landmarks, which belong
    //to one category and needs icon change.
    iIconChangeLmkArray = new(ELeave) RPointerArray<CPosLandmark>;
    // base class 2nd phase constructor
    CLmkListProviderBase::BaseConstructL();
    }

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::~CLmkLmItemListProvider
// -----------------------------------------------------------------------------
//
CLmkLmItemListProvider::~CLmkLmItemListProvider()
    {
    if ( iWaitNote )
        {
        TRAPD( err, iWaitNote->ProcessFinishedL() );
        if (err != KErrNone)
    		{
		    delete iWaitNote;
            iWaitNote = NULL;
	    	}
        }
    // Stop db event observing:
    CLmkEventListenerSingleton::RemoveObserver( *this );
	if (iIconChangeLmkArray)
		{
		iIconChangeLmkArray->ResetAndDestroy();
    	delete iIconChangeLmkArray;
		}
    delete iInitializer;
    delete iDbUtils;

    iSelectedForDelete.Close();
    iIdArray.Close();
    }

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::SelectedIndexesL
// -----------------------------------------------------------------------------
//
EXPORT_C CArrayFix<TInt>* CLmkLmItemListProvider::SelectedIndexesL(
    const RArray<TPosLmItemId>& aSelectedItems ) const
    {
    CArrayFix<TInt>* array = NULL;
    TInt count = aSelectedItems.Count();
    if ( count < KLmkMultiSelectionGranularity )
        { // decent minimum granularity because of possible user selections
        array =
            new ( ELeave ) CArrayFixFlat<TInt>( KLmkMultiSelectionGranularity );
        }
    else
        {
        array = new ( ELeave ) CArrayFixFlat<TInt>( aSelectedItems.Count() );
        }
    CleanupStack::PushL( array );

    GetSelectedIndexesL( *array, aSelectedItems );

    CleanupStack::Pop( array );
    return array;
    }

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::GetSelectedIndexesL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkLmItemListProvider::GetSelectedIndexesL(
    CArrayFix<TInt>& aSelectedIndexes,
    const RArray<TPosLmItemId>& aSelectedItems ) const
    {
    aSelectedIndexes.Reset();
    TInt count = aSelectedItems.Count();
    for ( TInt i( 0 ); i < count; ++i )
        {
        TInt index = iIdArray.Find( aSelectedItems[i] );
        if ( index >= 0 )
            { // item found --> append it's index
            aSelectedIndexes.AppendL( index );
            }
        }
    }

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::GetSelectedItemIdsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkLmItemListProvider::GetSelectedItemIdsL(
    RArray<TPosLmItemId>& aSelectedItems,
    const CArrayFix<TInt>& aSelectedIndexes ) const
    {
    aSelectedItems.Reset();
    TInt itemCount = iIdArray.Count();
    TInt selectionCount = aSelectedIndexes.Count();

    for ( TInt i( 0 ); i < selectionCount; ++i )
        {
        TInt index = aSelectedIndexes[i];
        __ASSERT_ALWAYS( index >= 0 && index < itemCount,
                         Panic( KLmkPanicInvalidIndex ) );

        User::LeaveIfError( aSelectedItems.Append( iIdArray[index] ) );
        }
    }

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::GetAllItemIdsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkLmItemListProvider::GetAllItemIdsL(
    RArray<TPosLmItemId>& aSelectedItems ) const
    {
    aSelectedItems.Reset();
    TInt itemCount = iIdArray.Count();

    for ( TInt i( 0 ); i < itemCount; ++i )
        {
        User::LeaveIfError( aSelectedItems.Append( iIdArray[i] ) );
        }
    }

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::SelectedIndex
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CLmkLmItemListProvider::SelectedIndex(
    TPosLmItemId aSelectedItem ) const
    {
    return iIdArray.Find( aSelectedItem );
    }

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::GetSelectedItemId
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkLmItemListProvider::GetSelectedItemId(
    TPosLmItemId& aSelected,
    TInt aIndex ) const
    {
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < iIdArray.Count(),
                     Panic( KLmkPanicInvalidIndex ) );

    aSelected = iIdArray[aIndex];
    }

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::SetSelectedItemsCount
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkLmItemListProvider::SetSelectedItemsCount( TInt aSelectedItemsCount )
	{
	iSelectedItemsCount = aSelectedItemsCount;
	}

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::RemoveItem
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkLmItemListProvider::RemoveItem( TPosLmItemId aItemId )
	{
   	TInt index = iIdArray.Find(aItemId);
   	if( index != KErrNotFound && index < iIdArray.Count())
   		{
   		iIdArray.Remove(index);
		NotifyObservers( ELmkEventListReady );
   		}
	}

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::RemoveItemsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkLmItemListProvider::RemoveItemsL(
											const RArray<TPosLmItemId>& aSelectedItems,
											MLmkAOOperationObserver::TOperationTypes aType,
											TPosLmItemId aCategoryId )
	{
	iCategoryId = aCategoryId;
	iType = aType;
	RemoveLandmarksL(aSelectedItems);
	}

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::SetOperationCmd
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkLmItemListProvider::SetOperationCmd( TLmkAppCmdId aOperationCmd )
	{
	iOperationCmd = aOperationCmd;
	}

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::InitializeL
// -----------------------------------------------------------------------------
//
void CLmkLmItemListProvider::InitializeL()
    {
    __ASSERT_DEBUG( !iInitializer, Panic( KLmkPanicIllegalMethodCall ) );
    // We don't want intermediate callbacks -> 3rd parameter EFalse
    iInitializer = CLmkDbInitializer::NewL( iDb, *this, EFalse );
    iInitializer->StartInitAndCompactL();
    }

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::StartSearchingLandmarksL()
// -----------------------------------------------------------------------------
//
TBool CLmkLmItemListProvider::StartSearchingLandmarksL(
								const TDesC& aSearchPattern,
    							TBool aSearchOnlyInPreviousMatches)
	{
	iRequestFromFindBox = ETrue;
	return PrepareListL(aSearchPattern, aSearchOnlyInPreviousMatches);
	}

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::ItemCount
// -----------------------------------------------------------------------------
//
TInt CLmkLmItemListProvider::ItemCount() const
    {
    return iIdArray.Count();
    }

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::HandleOperationL
// -----------------------------------------------------------------------------
//
void CLmkLmItemListProvider::HandleOperationL(
    TOperationTypes /*aType*/,
    TReal32 /*aProgress*/,
    TInt aStatus )
    {
    __ASSERT_DEBUG( aStatus != KPosLmOperationNotComplete,
                    Panic( KLmkPanicIntermediateCallback ) );

    // init and compact completed, delete unnecessary helper object:
    delete iInitializer;
    iInitializer = NULL;

    TInt err( aStatus );

    if ( aStatus == KErrNone )
        {
        TRAP( err, HandleDatabaseInitializedL() );
        }

    if ( err != KErrNone )
        { // Error already in aStatus or within this method
        HandleError( err );
        }
    }

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::HandleDatabaseInitializedL
// -----------------------------------------------------------------------------
//
void CLmkLmItemListProvider::HandleDatabaseInitializedL()
    {
    // Start db event observing:
    CLmkEventListenerSingleton::AddObserverL( *this, iDb );
    
    iRequestFromFindBox = EFalse;
    
    // Call provider-specific templated method for preparing list:
    PrepareListL();
    }

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::HandleError
// -----------------------------------------------------------------------------
//
void CLmkLmItemListProvider::HandleError( TInt aError )
    {
    if ( aError != KErrNone )
        {
        NotifyError( aError );
        }
    }

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::HandleDatabaseEvent
// -----------------------------------------------------------------------------
//
void CLmkLmItemListProvider::HandleDatabaseEvent( TPosLmEvent& aEvent )
    {
    switch ( aEvent.iEventType )
        {
        case EPosLmEventNewDefaultDatabaseLocation:
        case EPosLmEventMediaRemoved:
            {
            //TODO:
            break;
            }
        default:
            {
            //TODO:
            // not interesting event for this class
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CLmkLmItemListProvider::GetLandmarkLC
// -----------------------------------------------------------------------------
//
CPosLandmark* CLmkLmItemListProvider::GetLandmarkLC(TInt aIndex)
	{
	RArray<TPosLmItemId> itemArray;
	CPosLandmark* landmark = iDb.ReadPartialLandmarkLC( iIdArray[aIndex] );
	//check whether landmark is associated with more than one cat.
	landmark->GetCategoriesL(itemArray);
	if (itemArray.Count() == 1)
		{
		//if associated with one cat, check if the icon is not set by the
		//user explicitly
		TPtrC iconPath;
	    TInt iconId;
	    TInt iconMaskIndex;
		if (landmark->GetIcon(iconPath,iconId,  iconMaskIndex))
	        {
	       	//lmk has one cat & icon is not set by the user:
			CPosLmCategoryManager* categoryMgr = CPosLmCategoryManager::NewL(iDb);
			CleanupStack::PushL( categoryMgr );
			CPosLandmarkCategory* category = categoryMgr->ReadCategoryLC( itemArray[0] );
			//get the cat icon and set it as landmark icon only for display
			if (category->GetIcon(iconPath,iconId,  iconMaskIndex) == KErrNone )
				{
				landmark->SetIconL(iconPath,iconId,iconMaskIndex);
				}
			CleanupStack::PopAndDestroy();//category
			CleanupStack::PopAndDestroy();//categoryMgr
	        }
		}
	itemArray.Close();
	return landmark;
	}
//  End of File
