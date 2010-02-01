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
* Description:    Filtered landmarks list provider
*
*/


// SYSTEM INCLUDES
#include <EPos_CPosLmPartialReadParameters.h>
#include <EPos_CPosLandmark.h>
#include <EPos_TPosLmSortPref.h>
#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLmItemIterator.h>
#include <EPos_CPosLmSearchCriteria.h>
#include <EPos_CPosLandmarkSearch.h>
#include <e32math.h>
#include <EPos_CPosLmCompositeCriteria.h>
#include <EPos_CPosLmCategoryCriteria.h>
#include <EPos_CPosLmIdListCriteria.h>
#include <EPos_CPosLmTextCriteria.h>
#include <AknWaitDialog.h>
#include <lmkui.rsg>

// USER INCLUDES
#include "CLmkSearchedLmListProvider.h"
#include "CLmkLandmarkUiItem.h"
#include "CLmkAOOperation.h"
#include "MLmkSelectorIconMgr.h"
#include <lmkerrors.h>

// CONSTANTS
_LIT(KSpaceTextCriteria,"* ");
_LIT(KDefaultTextCriteria,"*");

#if defined(_DEBUG)
// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkSearchedLmListProvider" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace
#endif
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkSearchedLmListProvider::CLmkSearchedLmListProvider
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkSearchedLmListProvider::CLmkSearchedLmListProvider(
    CPosLandmarkDatabase& aDb,
    CPosLmSearchCriteria& aCriteria )
    : CLmkLmItemListProvider( aDb ),
      iCriteria( aCriteria ),
      iIsSecondSearchStarted(EFalse)
    {
    }

// -----------------------------------------------------------------------------
// CLmkSearchedLmListProvider::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkSearchedLmListProvider::ConstructL()
    {
    // base class 2nd phase constructor
    CLmkLmItemListProvider::BaseConstructL();

    iReadParams = CPosLmPartialReadParameters::NewLC();
    CleanupStack::Pop(); // iReadParams
    iReadParams->SetRequestedAttributes( CPosLandmark::ELandmarkName |
                                         CPosLandmark::EIcon | CPosLandmark::ECategoryInfo);

    iSearch = CPosLandmarkSearch::NewL( iDb );
    }

// -----------------------------------------------------------------------------
// CLmkSearchedLmListProvider::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkSearchedLmListProvider* CLmkSearchedLmListProvider::NewL(
    CPosLandmarkDatabase& aDb,
    CPosLmSearchCriteria& aCriteria )
    {
    CLmkSearchedLmListProvider* self =
        new ( ELeave ) CLmkSearchedLmListProvider( aDb, aCriteria );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------
// CLmkSearchedLmListProvider::~CLmkSearchedLmListProvider
// ---------------------------------------------------------
//
CLmkSearchedLmListProvider::~CLmkSearchedLmListProvider()
    {
    delete iReadParams;
    delete iSearch;
    if (iSearchAO)
		{
    	iSearchAO->StopOperation();
    	}
    delete iSearchAO;
    }

// ---------------------------------------------------------
// CLmkSearchedLmListProvider::ItemAtL
// ---------------------------------------------------------
//
CLmkUiItemBase* CLmkSearchedLmListProvider::ItemAtL( TInt aIndex )
    {
    __ASSERT_DEBUG( iIconMgr, Panic( KLmkPanicNullMember ) );
    //landmark with updated icon, if associated with one cat.
    CPosLandmark* landmark = GetLandmarkLC(aIndex);
    TPtrC mbmFile;
    TInt iconFileIndex = -1;
    TInt iconListIndex = -1;
    TInt iconMasktIndex = -1;
    TInt result = landmark->GetIcon( mbmFile, iconFileIndex, iconMasktIndex );
    if ( result == KErrNone )
        {
        TRAPD(err, iconListIndex =
                    iIconMgr->GetIconL( mbmFile, iconFileIndex ) );
        if ( err == KErrNotFound ) // icon file not found -> fallback to default
            {
            iconListIndex = iIconMgr->GetDefaultIconL(
                MLmkSelectorIconMgr::ELandmarkDefaultIcon );
            }
        }
    else
        {
        iconListIndex = iIconMgr->GetDefaultIconL(
            MLmkSelectorIconMgr::ELandmarkDefaultIcon );
        }

    CLmkLandmarkUiItem* uiItem =
        CLmkLandmarkUiItem::NewL( landmark, iconListIndex );
    CleanupStack::Pop(); // landmark, ownership transferred
    return uiItem;
    }

// ---------------------------------------------------------
// CLmkSearchedLmListProvider::PrepareListL
// ---------------------------------------------------------
//
void CLmkSearchedLmListProvider::PrepareListL()
    {    
    iCount = 0;

    iDb.SetPartialReadParametersL( *iReadParams );

    TPosLmSortPref sortOrder( CPosLandmark::ELandmarkName,
                              TPosLmSortPref::EAscending );
    CPosLmOperation* operation =
        iSearch->StartLandmarkSearchL( iCriteria, sortOrder );
    CleanupStack::PushL( operation );
    if (iSearchAO)
		{
    	iSearchAO->StopOperation();
	    delete iSearchAO; // cancel possibly pending operation
	    iSearchAO = NULL;
    	}
    iSearchAO = CLmkAOOperation::NewL( operation,
                                       *this,
                                       MLmkAOOperationObserver::ESearch,
                                       iOperationNotInUse );
	iOperationNotInUse = ETrue;
    CleanupStack::Pop( operation ); // ownership transferred
    iSearchAO->StartOperation();
    }

// -----------------------------------------------------------------------------
// CLmkSearchedLmListProvider::PrepareListL
// -----------------------------------------------------------------------------
//
TBool CLmkSearchedLmListProvider::PrepareListL(const TDesC& aSearchPattern,
    							TBool aSearchOnlyInPreviousMatches)
	{
    iCount = 0;
	iRequestFromFindBox = ETrue;
	iOperationNotInUse = ETrue;
    iIsSecondSearchStarted = EFalse;

	if( aSearchPattern.Compare( KDefaultTextCriteria ) == 0
			|| aSearchPattern.Length() <= 0  )
		{
		PrepareListL();
		iRequestFromFindBox = EFalse;
		return ETrue;
		}

	// We need to cancel if we are searching/reading landmarks.
 	if (iSearchAO)
	 	{
	 	iSearchAO->StopOperation();
	 	delete iSearchAO; // cancel possibly pending operation
		iSearchAO = NULL;
	 	}

	// Create the composite criterion
	CPosLmCompositeCriteria* compCrit = CPosLmCompositeCriteria::NewLC(
	    CPosLmCompositeCriteria::ECompositionOR );

	// Create the text search criterion and add it to composite
	CPosLmTextCriteria* textCrit1 = CPosLmTextCriteria::NewLC();

    const TInt KExtraChars = 3; // 2 chars wildcards
    HBufC* filterBuf = HBufC::NewLC(aSearchPattern.Length() + KExtraChars);
    TPtr filter = filterBuf->Des();
	filter.Copy(KSpaceTextCriteria);
	filter.Append(aSearchPattern);

	textCrit1->SetTextL( filter );
    textCrit1->SetAttributesToSearch(CPosLandmark::ELandmarkName);

	User::LeaveIfError( compCrit->AddArgument( textCrit1 ) );
	// Ownership of the text criterion has been passed to the composite
	CleanupStack::PopAndDestroy( filterBuf );
	CleanupStack::Pop( textCrit1 );

	// Create the text search criterion and add it to composite
	CPosLmTextCriteria* textCrit2 = CPosLmTextCriteria::NewLC();
	textCrit2->SetTextL( aSearchPattern );
    textCrit2->SetAttributesToSearch(CPosLandmark::ELandmarkName);

	User::LeaveIfError( compCrit->AddArgument( textCrit2 ) );
	// Ownership of the text criterion has been passed to the composite
	CleanupStack::Pop( textCrit2 );

    iDb.SetPartialReadParametersL( *iReadParams );

    TPosLmSortPref sortOrder( CPosLandmark::ELandmarkName,
                              TPosLmSortPref::EAscending );
	// Start the search
    // Create search operation
    CPosLmOperation* operation = iSearch->StartLandmarkSearchL(
    					*compCrit, sortOrder, aSearchOnlyInPreviousMatches );

 	CleanupStack::PopAndDestroy( compCrit );
 	CleanupStack::PushL( operation );

	iSearchAO = CLmkAOOperation::NewL( operation,
                                   *this,
                                   MLmkAOOperationObserver::ESearch,
                                   ETrue );
	CleanupStack::Pop( operation ); // ownership transferred

    iSearchAO->StartOperation();

    return ETrue;
 	}

// ---------------------------------------------------------
// CLmkSearchedLmListProvider::StartSecondSearchL
// ---------------------------------------------------------
//
void CLmkSearchedLmListProvider::StartSecondSearchL()
	{
	// Create the composite criterion
	CPosLmCompositeCriteria* compCrit = CPosLmCompositeCriteria::NewLC(
	    CPosLmCompositeCriteria::ECompositionAND );

	// Create the category search criterion and add it to composite
	CPosLmCategoryCriteria* catCrit = CPosLmCategoryCriteria::NewLC();
	catCrit->SetCategoryItemId( (static_cast<CPosLmCategoryCriteria&>(iCriteria)).CategoryItemId() );

	User::LeaveIfError( compCrit->AddArgument( catCrit ) );
	// Ownership of the category criterion has been passed to the composite
	CleanupStack::Pop( catCrit );

	// Create the text search criterion and add it to composite
	CPosLmIdListCriteria* idListCrit = CPosLmIdListCriteria::NewLC();
	idListCrit->SetLandmarkIdsL( iIdArray );

	User::LeaveIfError( compCrit->AddArgument( idListCrit ) );
	// Ownership of the text criterion has been passed to the composite
	CleanupStack::Pop( idListCrit );

    iDb.SetPartialReadParametersL( *iReadParams );

    TPosLmSortPref sortOrder( CPosLandmark::ELandmarkName,
                              TPosLmSortPref::EAscending );
	// Start the search
    // Create search operation
    CPosLmOperation* operation = iSearch->StartLandmarkSearchL( *compCrit, sortOrder );

 	CleanupStack::PopAndDestroy( compCrit );
 	CleanupStack::PushL( operation );

	iSearchAO = CLmkAOOperation::NewL( operation,
                                   *this,
                                   MLmkAOOperationObserver::ESearch,
                                   ETrue );
	CleanupStack::Pop( operation ); // ownership transferred

	iIsSecondSearchStarted = EFalse;
    iSearchAO->StartOperation();

	}

// ---------------------------------------------------------
// CLmkSearchedLmListProvider::HandleOperationL
// ---------------------------------------------------------
//
void CLmkSearchedLmListProvider::HandleOperationL(
    TOperationTypes aType,
    TReal32 aProgress,
    TInt aStatus )
    {
    if ( aType == MLmkAOOperationObserver::EDeleteLandmarks )
    	{
    	return;
    	}
    if ( aType == MLmkAOOperationObserver::ERemoveCategory )
    	{
    	return;
    	}
    if ( aType == MLmkAOOperationObserver::ESearch )
        {
   		ReadItemsToArrayL();
       	if ( aStatus == KErrNone )
	        {
	        if( iSearchAO )
	        	{
	        	iSearchAO->StopOperation();
				delete iSearchAO;
		        iSearchAO = NULL;
	        	}

			// reset operation values
	        iOperationCmd = ELmkCmdStopOperation;//temp default value
	        iCount = 0;

	        if( iIsSecondSearchStarted )
	        	{
	        	StartSecondSearchL();
	        	}

	        if(iOperationNotInUse == EFalse )
	        	{
	        	NotifyObservers( ELmkEventItemAdditionComplete );
	        	}
	        else
		        {
		        if (iRequestFromFindBox)
			        {
			        NotifyObservers( ELmkEventFindListReady );
			        }
				else
					{
		        	NotifyObservers( ELmkEventListReady );
					}
		        }

	        iOperationNotInUse = ETrue;

            if ( iCatDelete )
                {
                NotifyObservers( ELmkEventCategoryDeleted );
                iCatDelete = EFalse;
                }
            else if(iCatUpdate)
	            {
	            NotifyObservers( ELmkEventCategoryUpdated );
                iCatUpdate = EFalse;
                }

    		if(iItemsDeleted < iItemsToDelete)
    			{
    			PrepareForDeleteL();
    			}
    		else
    			{
    			if(iWaitNote)
    				{
		            iWaitNote->ProcessFinishedL();
		            iWaitNote = NULL;
    				}
    			}
	        return;
	        } // end of if ( aStatus == KErrNone )

	    if ( iCount == ELmkStepOne || iCount == ELmkStepTwo )
	        {
			if (iOperationNotInUse == EFalse )
	        	{
	        	NotifyObservers( ELmkEventItemAdditionComplete );
	        	}
	        else
		        {
		        if (iRequestFromFindBox)
			        {
			        NotifyObservers( ELmkEventFindListReady );
			        }
				else
					{
		        	NotifyObservers( ELmkEventListReady );
					}
		        }
		    iOperationNotInUse = EFalse;
	        }
        }
    else
        { // This is not this classes' operation
        CLmkLmItemListProvider::HandleOperationL( aType, aProgress, aStatus);
        }
    }

// ---------------------------------------------------------
// CLmkSearchedLmListProvider::HandleDatabaseEvent
// ---------------------------------------------------------
//
void CLmkSearchedLmListProvider::HandleDatabaseEvent( TPosLmEvent& aEvent )
    {
    iCatDelete = EFalse;

	TBool needRefresh = ETrue;

	// check any items are selected to do operations like iconchanges or add to category.
	if( iSelectedItemsCount > 0 
		&& iOperationCmd != ELmkCmdAddToCat
		&& iOperationCmd != ERemoveFromCat )
		{
		iCount++;
		if( iSelectedItemsCount == iCount )
			{
			iSelectedItemsCount = 0;
			iCount = 0;
			}
		needRefresh = EFalse;
		}

    switch ( aEvent.iEventType )
        {
        case EPosLmEventLandmarkUpdated: 		// lm icon changes, lm renaming
        case EPosLmEventLandmarkUnknownChanges:	// lm multiple deletion, lm add to category
        case EPosLmEventUnknownChanges:
        case EPosLmEventLandmarkCreated:
        case EPosLmEventCategoryDeleted:
        case EPosLmEventCategoryUpdated:
            {
            iOperationNotInUse = EFalse;
           	if (aEvent.iEventType == EPosLmEventCategoryDeleted )
	            {
	            iCatDelete = ETrue;
	            }
	        if (aEvent.iEventType == EPosLmEventCategoryUpdated )
	            {
	            iCatUpdate = ETrue;
	            }

			if( needRefresh )
				{
		        if( iSearchAO )
		        	{
		        	iSearchAO->StopOperation();
					delete iSearchAO;
			        iSearchAO = NULL;
		        	}
	            iOperationNotInUse = EFalse;
	            TInt err = KErrNone;
	            do
	                {
	                TRAP( err, PrepareListL() );
	                }while(err == KErrLocked);
	            if ( err )
	                {
	                HandleError( err );
	                }
				}
			else
				{
				NotifyObservers( ELmkEventListReady );
				}
            break;
            }
        case EPosLmEventLandmarkDeleted:
        	{
		   	TInt index = iIdArray.Find( aEvent.iLandmarkItemId );
		   	if( index != KErrNotFound && index < iIdArray.Count())
		   		{
		   		iIdArray.Remove(index);
			   	NotifyObservers( ELmkEventListReady );
		   		}
		   	break;
        	}
        default:
            { // Not interesting event for this provider
            break;
            }
        }
    // Remember to call base class observer method too
    CLmkLmItemListProvider::HandleDatabaseEvent( aEvent );
    }

// ---------------------------------------------------------
// CLmkSearchedLmListProvider::ReadItemsToArrayL
// ---------------------------------------------------------
//
void CLmkSearchedLmListProvider::ReadItemsToArrayL()
    {
    iIdArray.Reset();
    CPosLmItemIterator* iterator = iSearch->MatchIteratorL();
    CleanupStack::PushL( iterator );
    iCount = iterator->NumOfItemsL();
    if ( iCount > 0 )
        { // can only be called if there are some items
        iterator->GetItemIdsL( iIdArray, 0, iCount ); // array is first reseted
        }
    CleanupStack::PopAndDestroy( iterator );
    }

// ---------------------------------------------------------
// CLmkSearchedLmListProvider::RemoveLandmarksL
// ---------------------------------------------------------
//
void CLmkSearchedLmListProvider::RemoveLandmarksL(const RArray<TPosLmItemId>& aSelectedItems )
	{
	iItemsToDelete = aSelectedItems.Count();
	iSelectedForDelete.Reset();
	for(TInt i=0;i<iItemsToDelete;i++)
		iSelectedForDelete.Append(aSelectedItems[i]);
	iItemsDeleted = 0;
	iIsRemoveLandmarks = ETrue;

	if(iWaitNote)
		{
		delete iWaitNote;
		iWaitNote = NULL;
		}
	PrepareForDeleteL();
	}

// ---------------------------------------------------------
// CLmkSearchedLmListProvider::PrepareForDelete
// ---------------------------------------------------------
//
void CLmkSearchedLmListProvider::PrepareForDeleteL()
	{
	RArray<TPosLmItemId> lmItemsChunk;
	TInt count = iItemsDeleted;
	if(iItemsToDelete > (iItemsDeleted + 10))
		{
		iItemsDeleted = iItemsDeleted + 10;
		}
	else
		{
		iItemsDeleted = iItemsDeleted + (iItemsToDelete - iItemsDeleted);
		}

	for(TInt i=count;i<iItemsDeleted;i++)
		{
		lmItemsChunk.Append(iSelectedForDelete[i]);
		}

	CPosLmOperation* operation = NULL;
	if(iType == MLmkAOOperationObserver::EDeleteLandmarks)
		{
		operation = iDb.RemoveLandmarksL( lmItemsChunk );
		}
	if(iType == MLmkAOOperationObserver::ERemoveCategory)
		{
		CPosLmCategoryManager* mgr = CPosLmCategoryManager::NewL( iDb );
		CleanupStack::PushL(mgr);
		operation = mgr->RemoveCategoryFromLandmarksL( iCategoryId, lmItemsChunk );
		CleanupStack::PopAndDestroy();//mgr
		}
	User::LeaveIfNull( operation );

	CleanupStack::PushL( operation );
	iSearchAO = CLmkAOOperation::NewL( operation,
	               *this,
	               iType,
	               EFalse );
	CleanupStack::Pop( operation ); // ownership transferred

    if( iSearchAO )
        {
        iSearchAO->StartOperation();

        if(!iWaitNote)
            {
            iWaitNote = new (ELeave) CAknWaitDialog(NULL, ETrue);
            if(!iWaitNote->ExecuteLD(R_LMK_PROCESSING_WAIT_NOTE))
                {
                iSearchAO->StopOperation();
                delete iSearchAO;
                iSearchAO = NULL;
                iWaitNote = NULL;
                }
            }
        }
	
	lmItemsChunk.Close();
	}

//  End of File
