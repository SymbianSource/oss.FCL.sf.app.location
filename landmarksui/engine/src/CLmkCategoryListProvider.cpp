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
* Description:    Category List info provider for listbox model
*
*/




// INCLUDE FILES

#include <eikenv.h>
#include <StringLoader.h>
#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLandmarkCategory.h>
#include <EPos_CPosLmCategoryManager.h>
#include <EPos_CPosLmItemIterator.h>
#include <EPos_TPosLmSortPref.h>
#include <EPos_CPosLmSearchCriteria.h>
#include <EPos_CPosLandmarkSearch.h>
#include <EPos_CPosLmCategoryCriteria.h>
#include <lmkui.rsg>
#include "LmkConsts.h"
#include "CLmkAOOperation.h"
#include "CLmkCategoryUiItem.h"
#include "MLmkSelectorIconMgr.h"
#include "CLmkCategoryListProvider.h"
#include <lmkerrors.h>

#if defined(_DEBUG)
// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkCategoryListProvider" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace
#endif
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkCategoryListProvider::CLmkCategoryListProvider
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkCategoryListProvider::CLmkCategoryListProvider(
    CPosLandmarkDatabase& aDb,
    TLmkProviderType aShowType )
    : CLmkLmItemListProvider( aDb ),
      iShowType( aShowType ),
      iAreUncategorisedItems( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CLmkCategoryListProvider::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkCategoryListProvider::ConstructL()
	{
	// base class 2nd phase constructor
	CLmkLmItemListProvider::BaseConstructL();
	iCategoryMgr = CPosLmCategoryManager::NewL( iDb );
	iSearch = CPosLandmarkSearch::NewL( iDb );
	}

// -----------------------------------------------------------------------------
// CLmkCategoryListProvider::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkCategoryListProvider* CLmkCategoryListProvider::NewL(
    CPosLandmarkDatabase& aDb,
    TLmkProviderType aShowType )
    {
    CLmkCategoryListProvider* self =
        new ( ELeave ) CLmkCategoryListProvider( aDb, aShowType );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CLmkCategoryListProvider::~CLmkCategoryListProvider
// -----------------------------------------------------------------------------
//
CLmkCategoryListProvider::~CLmkCategoryListProvider()
    {
    delete iCategoryMgr;
    delete iSearch;
    if( iSearchAO )
    	{
    	delete iSearchAO;
    	}
    delete iCriteria;
    iUsageArray.Close();
    }

// -----------------------------------------------------------------------------
// CLmkCategoryListProvider::ItemAtL
// -----------------------------------------------------------------------------
//
CLmkUiItemBase* CLmkCategoryListProvider::ItemAtL( TInt aIndex )
    {
    __ASSERT_DEBUG( iIconMgr, Panic( KLmkPanicNullMember ) );
    CLmkCategoryUiItem* uiItem = NULL;

	if (iIdArray[aIndex] != KPosLmNullItemId)
		{
		CPosLandmarkCategory* category =
		iCategoryMgr->ReadCategoryLC( iIdArray[aIndex] );

		TPtrC mbmFile;
		TInt iconFileIndex = -1;
		TInt iconListIndex = -1;

		TInt iconMaskIndex = -1;
		TInt result = category->GetIcon( mbmFile, iconFileIndex ,iconMaskIndex);
		if ( result == KErrNone )
			{
			TRAPD(err, iconListIndex =
			    iIconMgr->GetIconL( mbmFile, iconFileIndex ) );
			if ( err == KErrNotFound ) // icon file not found -> fallback to default
				{
				iconListIndex = iIconMgr->GetDefaultIconL(
				MLmkSelectorIconMgr::ECategoryDefaultIcon );
				}
			}
		else
			{
			iconListIndex = iIconMgr->GetDefaultIconL(
			MLmkSelectorIconMgr::ECategoryDefaultIcon );
			}

		switch( iShowType )
			{
			//these are same
			case ELmkNotShowEmptyCategories:
			case ELmkShowEmptyCategories:
				{
				uiItem =
				CLmkCategoryUiItem::NewL( category, iconListIndex );
				break;
				}
			case ELmkShowExtendedCategories:
				{
				uiItem =
				CLmkCategoryUiItem::NewL( category, iconListIndex );
				uiItem->SetExtensionInt( IsUsedL( *category) );
				break;
				}
			default:
				{
				break;
				}
			}
		}
	else // if category id is 'uncategorised'
		{
		CPosLandmarkCategory* category = CPosLandmarkCategory::NewL();
		CleanupStack::PushL( category);

		// Set category name
		CEikonEnv* env = CEikonEnv::Static();
		HBufC* catNameText = NULL;
		catNameText = StringLoader::LoadLC( R_LMK_LM_LIST_UNCATEGORISED, env );
		TPtrC categoryName;
		categoryName.Set(*catNameText);
		category->SetCategoryNameL(categoryName);
		CleanupStack::PopAndDestroy( catNameText );
		TInt iconListIndex = -1;
		iconListIndex
		    = iIconMgr->GetDefaultIconL(
			 			            MLmkSelectorIconMgr::ECategoryDefaultIcon );
		uiItem = CLmkCategoryUiItem::NewL( category, iconListIndex );
		}
    CleanupStack::Pop(); // category, ownership transferred
    return uiItem;
    }

// ---------------------------------------------------------
// CLmkCategoryListProvider::ReadItemsToArrayL
// ---------------------------------------------------------
//
void CLmkCategoryListProvider::ReadItemsToArrayL()
	{
	// Update the IdArray to have 'uncategorised' category Id
	CPosLmItemIterator* lmIterator = iSearch->MatchIteratorL();
	CleanupStack::PushL( lmIterator );
	TInt lmCount = lmIterator->NumOfItemsL();
	CleanupStack::PopAndDestroy( lmIterator );
	if ( lmCount > 0 )
		{
		// There is atleast 'one' landmark which is 'uncategorised'
		// can only be called if there are some items
		// Append 'uncategorised' category ID to the idArray
		iIdArray.Append(KPosLmNullItemId);
		iAreUncategorisedItems = ETrue;
		if(iCount == -1 )
			{
			NotifyObservers( ELmkEventItemAdditionComplete );
			}
		else
       		{
       		NotifyObservers( ELmkEventListReady );
       		}
	    iSearchAO->StopOperation();
	    iCount = 1;
		}
	}

// ---------------------------------------------------------
//  CLmkCategoryListProvider::HandleOperationL
// ---------------------------------------------------------
//
void CLmkCategoryListProvider::HandleOperationL(
    TOperationTypes aType,
    TReal32 aProgress,
    TInt aStatus )
    {
    if ( aType == MLmkAOOperationObserver::ESearch )
        {
        if( iCount < 1 )
        	{
        	ReadItemsToArrayL();
        	}
       	if ( aStatus == KErrNone )
            {
            if(iCount == 0)
            	{
            	NotifyObservers( ELmkEventListReady );
            	}
	        delete iSearchAO;
	        iSearchAO = NULL;
	        iCount = 0;
            }
        }
    else
        { // This is not this classes' operation
        CLmkLmItemListProvider::HandleOperationL( aType, aProgress, aStatus);
        }
    }

// -----------------------------------------------------------------------------
// CLmkCategoryListProvider::PrepareListL
// -----------------------------------------------------------------------------
//
void CLmkCategoryListProvider::PrepareListL()
    {
    if ( iListAllItemsInDb )
        {
        CPosLmItemIterator* iterator = NULL;

        switch( iShowType )
            {
			case ELmkNotShowEmptyCategories:
				{
				iCount = 0;
				iIdArray.Reset();

				// Query for 'referenced categories' from Category manager here
				CPosLmItemIterator* categoryIterator  = iCategoryMgr->ReferencedCategoryIteratorL(
				    CPosLmCategoryManager::ECategorySortOrderNameAscending );
				CleanupStack::PushL( categoryIterator );
				TUint categoryCount = categoryIterator->NumOfItemsL();

				if ( categoryCount > 0 )
					{ // can only be called if there are some items
					categoryIterator->GetItemIdsL( iIdArray, 0, categoryCount );
					if( iAreUncategorisedItems )
						{
						iIdArray.Append(KPosLmNullItemId);
						}
					NotifyObservers( ELmkEventListReady );
					iCount = -1;
					}
				CleanupStack::PopAndDestroy( categoryIterator );

				if( iAreUncategorisedItems )
					{
					if( iIdArray.Count() == 0)
						{
						iIdArray.Append(KPosLmNullItemId);
						}
					NotifyObservers( ELmkEventListReady );
					return;
					}

				// Set criteria parameters here -- for uncategorised landmarks
				if (iCriteria)
					{
					delete   iCriteria;
					iCriteria = NULL;
					}
				iCriteria = CPosLmCategoryCriteria::NewLC();
				CleanupStack::Pop(  iCriteria );
				iCriteria->SetCategoryItemId(KPosLmNullItemId);

				TPosLmSortPref sortOrder( CPosLandmark::ELandmarkName,
				          TPosLmSortPref::EAscending );


				CPosLmOperation* operation =
				iSearch->StartLandmarkSearchL( *iCriteria , sortOrder );
				CleanupStack::PushL( operation );
				delete iSearchAO; // cancel possibly pending operation
				iSearchAO = NULL;
				iSearchAO = CLmkAOOperation::NewL( operation,
				                   				   *this,
				                  				   MLmkAOOperationObserver::ESearch,
				                   				   iOperationNotInUse );

				CleanupStack::Pop( operation ); // ownership transferred
				iSearchAO->StartOperation();
				return;
				}
            case ELmkShowEmptyCategories:
                {
                iterator = iCategoryMgr->CategoryIteratorL(
                    CPosLmCategoryManager::ECategorySortOrderNameAscending );
                CleanupStack::PushL( iterator );
                break;
                }
            case ELmkShowExtendedCategories:
                {
                iterator = iCategoryMgr->CategoryIteratorL(
                    CPosLmCategoryManager::ECategorySortOrderNameAscending );
                CleanupStack::PushL( iterator );
                PrepareCategoryUsageListL();
                break;
                }
            default:
                {
                break;
                }
            }
        
        if( iterator )
            {
            TUint count = iterator->NumOfItemsL();
            iIdArray.Reset();
            if ( count > 0 )
                { // can only be called if there are some items
                iterator->GetItemIdsL( iIdArray, 0, count );
                }
            CleanupStack::PopAndDestroy( iterator );
            }
        }

    NotifyObservers( ELmkEventListReady );
    }

// -----------------------------------------------------------------------------
// CLmkCategoryListProvider::PrepareListL
// -----------------------------------------------------------------------------
//
TBool CLmkCategoryListProvider::PrepareListL(const TDesC& /*aSearchPattern*/,
    							TBool /*aSearchOnlyInPreviousMatches*/)
	{
	// not preparing the new list based on search criteria,
	// as using the avkon's search api
	return EFalse;
	}

// ---------------------------------------------------------
// CLmkCategoryListProvider::RemoveLandmarksL
// ---------------------------------------------------------
//
void CLmkCategoryListProvider::RemoveLandmarksL(const RArray<TPosLmItemId>& /*aSelectedItems*/ )
	{
	// no implementation required
	}

// -----------------------------------------------------------------------------
// CLmkCategoryListProvider::HandleDatabaseEvent
// -----------------------------------------------------------------------------
//
void CLmkCategoryListProvider::HandleDatabaseEvent( TPosLmEvent& aEvent )
    {
    TBool needRefresh = EFalse;
    	
    switch ( aEvent.iEventType )
        {
	 	case EPosLmEventLandmarkUnknownChanges:
        case EPosLmEventLandmarkDeleted:
        case EPosLmEventLandmarkUpdated:
        case EPosLmEventUnknownChanges:
			{
			iAreUncategorisedItems = EFalse;
			needRefresh = ETrue;
			break;
			}
        case EPosLmEventLandmarkCreated:
        case EPosLmEventCategoryUnknownChanges:
        case EPosLmEventCategoryCreated:
        case EPosLmEventCategoryDeleted:
        case EPosLmEventCategoryUpdated:
            {
            needRefresh = ETrue;
            break;
            }
        default:
            {
            // Not interesting event for this provider
            break;
            }
        }
        
    if( needRefresh )    
    	{
		TRAPD( err, PrepareListL() );
		if ( err )
			{
			HandleError( err );
			}  				
    	}
    	
    // Remember to call base class observer method too
    CLmkLmItemListProvider::HandleDatabaseEvent( aEvent );
    }

// -----------------------------------------------------------------------------
// CLmkCategoryListProvider::PrepareCategoryUsageListL
// -----------------------------------------------------------------------------
//
void CLmkCategoryListProvider::PrepareCategoryUsageListL()
    {
    iUsageArray.Reset();
    iUsageIndex = 0;
    CPosLmItemIterator* iterator = iCategoryMgr->ReferencedCategoryIteratorL(
                      CPosLmCategoryManager::ECategorySortOrderNameAscending );

    CleanupStack::PushL( iterator );
    TUint count = iterator->NumOfItemsL();
    if ( count > 0 )
        { // can only be called if there are some items
        iterator->GetItemIdsL( iUsageArray, 0, count );
        }
    CleanupStack::PopAndDestroy( iterator );
    }

// -----------------------------------------------------------------------------
// CLmkCategoryListProvider::IsUsedL
// -----------------------------------------------------------------------------
//
TBool CLmkCategoryListProvider::IsUsedL( CPosLandmarkCategory& aCategory )
    {
    TBool result( EFalse );

    TInt count( iUsageArray.Count() );
    CPosLandmarkCategory* usageCat = NULL;

    for ( TInt i=0; i<count; i++ )
        {
        usageCat = iCategoryMgr->ReadCategoryLC( iUsageArray[i] );

        if ( aCategory.CategoryId() ==  usageCat->CategoryId() )
            {
            result = ETrue;
            CleanupStack::PopAndDestroy();
            break;
            }
        CleanupStack::PopAndDestroy();
        }
    return result;
    }



//  End of File
