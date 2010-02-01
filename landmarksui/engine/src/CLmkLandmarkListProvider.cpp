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
* Description:  This file mainly is responsible for handling the database event
*               w.r.t landmarks & preparing landmark lists accordingly.
*
*/







// SYSTEM INCLUDE
#include <EPos_CPosLmPartialReadParameters.h>
#include <EPos_CPosLandmark.h>
#include <EPos_TPosLmSortPref.h>
#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLmItemIterator.h>
#include <EPos_CPosLandmarkSearch.h>
#include <EPos_CPosLmTextCriteria.h>
#include <EPos_CPosLmCompositeCriteria.h>
#include <e32math.h>
#include <AknWaitDialog.h>
#include <lmkui.rsg>

// USER INCLUDES
#include "CLmkLandmarkListProvider.h"
#include "CLmkLandmarkUiItem.h"
#include "CLmkAOOperation.h"
#include "MLmkSelectorIconMgr.h"
#include <lmkerrors.h>

// CONSTANTS
_LIT(KDefaultTextCriteria,"*");
_LIT(KSpaceTextCriteria,"* ");


#if defined(_DEBUG)
// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkLandmarkListProvider" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace
#endif
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkLandmarkListProvider::CLmkLandmarkListProvider
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkLandmarkListProvider::CLmkLandmarkListProvider(
    CPosLandmarkDatabase& aDb )
    : CLmkLmItemListProvider( aDb )
    {
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkListProvider::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkLandmarkListProvider::ConstructL()
    {
    // base class 2nd phase constructor
    CLmkLmItemListProvider::BaseConstructL();

    iReadParams = CPosLmPartialReadParameters::NewLC();
    CleanupStack::Pop(); // iReadParams
    iReadParams->SetRequestedAttributes( CPosLandmark::ELandmarkName |
                                         CPosLandmark::EIcon | CPosLandmark::ECategoryInfo );
    iSearch = CPosLandmarkSearch::NewL( iDb );
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkListProvider::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkLandmarkListProvider* CLmkLandmarkListProvider::NewL(
    CPosLandmarkDatabase& aDb )
    {
    CLmkLandmarkListProvider* self =
        new ( ELeave ) CLmkLandmarkListProvider( aDb );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkListProvider::~CLmkLandmarkListProvider
// -----------------------------------------------------------------------------
//
CLmkLandmarkListProvider::~CLmkLandmarkListProvider()
    {
    delete iReadParams;
    if (iSearchAO)
	    {
	    iSearchAO->StopOperation();
	    }
    delete iSearchAO;
    delete iSearch;
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkListProvider::ItemAtL
// -----------------------------------------------------------------------------
//
CLmkUiItemBase* CLmkLandmarkListProvider::ItemAtL( TInt aIndex )
    {
    __ASSERT_DEBUG( iIconMgr, Panic( KLmkPanicNullMember ) );
    //landmark with updated icon, if associated with one cat.
    CPosLandmark* landmark = GetLandmarkLC(aIndex);
    TPtrC mbmFile;
    TInt iconFileIndex = -1;
    TInt iconListIndex = -1;
    TInt iconMaskIndex = -1;

    TInt result = landmark->GetIcon( mbmFile, iconFileIndex, iconMaskIndex );
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

// -----------------------------------------------------------------------------
// CLmkLandmarkListProvider::PrepareListL
// -----------------------------------------------------------------------------
//
void CLmkLandmarkListProvider::PrepareListL()
    {
    if ( iListAllItemsInDb )
        {        
        iCount = 0;
        iDb.SetPartialReadParametersL( *iReadParams );
	    CPosLmTextCriteria* criteria = CPosLmTextCriteria::NewLC();
	    criteria->SetTextL(KDefaultTextCriteria);
	    TPosLmSortPref sortOrder( CPosLandmark::ELandmarkName,
					          TPosLmSortPref::EAscending );

		CPosLmOperation* searchOperation = iSearch->StartLandmarkSearchL( *criteria , sortOrder );
	    CleanupStack::PopAndDestroy( criteria ); //criteria
		CleanupStack::PushL( searchOperation );
    	if (iSearchAO)
			{
    		iSearchAO->StopOperation();
		    delete iSearchAO; // cancel possibly pending operation
		    iSearchAO = NULL;
    		}

		iSearchAO = CLmkAOOperation::NewL( searchOperation,
		                   				   *this,
		                  				   MLmkAOOperationObserver::ESearch,
		                  				   iOperationNotInUse );

		iOperationNotInUse = ETrue;
		CleanupStack::Pop( searchOperation ); // ownership transferred
		if( iSearchAO )
		    iSearchAO->StartOperation();
        }
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkListProvider::PrepareListL
// -----------------------------------------------------------------------------
//
TBool CLmkLandmarkListProvider::PrepareListL(const TDesC& aSearchPattern,
    							TBool aSearchOnlyInPreviousMatches)
	{
    iCount = 0;
	iRequestFromFindBox = ETrue;
	iOperationNotInUse = ETrue;

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
	CPosLmTextCriteria* textCrit = CPosLmTextCriteria::NewLC();

    const TInt KExtraChars = 3; // 2 chars wildcards
    HBufC* filterBuf = HBufC::NewLC(aSearchPattern.Length() + KExtraChars);
    TPtr filter = filterBuf->Des();
	filter.Copy(KSpaceTextCriteria);
	filter.Append(aSearchPattern);

	textCrit->SetTextL( filter );
    textCrit->SetAttributesToSearch(CPosLandmark::ELandmarkName);

	User::LeaveIfError( compCrit->AddArgument( textCrit ) );
	// Ownership of the text criterion has been passed to the composite
	CleanupStack::PopAndDestroy( filterBuf );
	CleanupStack::Pop( textCrit );

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

    if( iSearchAO )
        iSearchAO->StartOperation();
    return ETrue;
	}

// -----------------------------------------------------------------------------
// CLmkLandmarkListProvider::HandleDatabaseEvent
// -----------------------------------------------------------------------------
//
void CLmkLandmarkListProvider::HandleDatabaseEvent( TPosLmEvent& aEvent )
    {
// TODO: we need a helper method like ValidatedIdArrayL() to check id
// array given by the client (that means: iListAllItemsInDb == EFalse)
// in case when database items are modified. Some items might have been
// deleted for instance. Possibly CLmkLmItemListProvider functionality?

	TBool needRefresh = ETrue;

	// check any items are selected to do operations like iconchanges or add to category.
	if( iSelectedItemsCount > 0 )
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
            {
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
//  CLmkLandmarkListProvider::HandleOperationL
// ---------------------------------------------------------
//
void CLmkLandmarkListProvider::HandleOperationL(
    TOperationTypes aType,
    TReal32 aProgress,
    TInt aStatus )
    {
    if ( aType == MLmkAOOperationObserver::EDeleteLandmarks )
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
	        iOperationNotInUse = ETrue;

	        iCount = 0;
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

	    else if ( iCount == ELmkStepOne || iCount == ELmkStepTwo )
        	{
	        if (iRequestFromFindBox)
		        {
		        NotifyObservers( ELmkEventFindListReady );
		        }
	        else
		        {
    			NotifyObservers( ELmkEventListReady );
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
// CLmkLandmarkListProvider::ReadItemsToArrayL
// ---------------------------------------------------------
//
void CLmkLandmarkListProvider::ReadItemsToArrayL()
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
// CLmkLandmarkListProvider::RemoveLandmarksL
// ---------------------------------------------------------
//
void CLmkLandmarkListProvider::RemoveLandmarksL(const RArray<TPosLmItemId>& aSelectedItems )
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
// CLmkLandmarkListProvider::PrepareForDelete
// ---------------------------------------------------------
//
void CLmkLandmarkListProvider::PrepareForDeleteL()
	{
	RArray<TPosLmItemId> bufItems;
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
		bufItems.Append(iSelectedForDelete[i]);
		}

	CPosLmOperation* operation =
	                    iDb.RemoveLandmarksL( bufItems );

	CleanupStack::PushL( operation );
	iSearchAO = CLmkAOOperation::NewL( operation,
	               *this,
	               MLmkAOOperationObserver::EDeleteLandmarks,
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
	bufItems.Close();
	}

//  End of File
