/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CLandmarksCategoriesEngine
*
*/



#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLmCatNameCriteria.h>
#include <EPos_CPosLandmarkSearch.h>
#include <EPos_CPosLmCategoryManager.h>

#include "LandmarksCategoriesEngine.h"
#include "LandmarksLmOpWrapper.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksCategoriesEngine::CLandmarksCategoriesEngine(
    CPosLandmarkDatabase& aDb)
:   CLandmarksEngineBase(aDb)
    {
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesEngine::ConstructL()
    {
    CLandmarksEngineBase::ConstructL();
    iCatManager = CPosLmCategoryManager::NewL(iDb);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksCategoriesEngine* CLandmarksCategoriesEngine::NewL(
    CPosLandmarkDatabase& aDb)
    {
    CLandmarksCategoriesEngine* self = 
        new (ELeave) CLandmarksCategoriesEngine(aDb);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksCategoriesEngine::~CLandmarksCategoriesEngine()
    {
    Cancel();
    delete iCatManager;
    iSubSetOfIds.Close();
    iItemIds.Close();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesEngine::AddCategoryL(CPosLandmarkCategory& aCategory)
    {
    iCatManager->AddCategoryL(aCategory);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesEngine::UpdateCategoryL(const CPosLandmarkCategory& aCategory)
    {
    iCatManager->UpdateCategoryL(aCategory);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesEngine::DeleteCategoryL(TPosLmItemId aItemId)
    {
    ExecuteAndDeleteLD(iCatManager->RemoveCategoryL(aItemId));
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CPosLandmarkCategory* CLandmarksCategoriesEngine::CategoryLC(TPosLmItemId aItemId)
    {
    return iCatManager->ReadCategoryLC(aItemId);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesEngine::StartSearchingCategoriesL(
    MLandmarksOperationObserver* aObserver)
    {
    // We need to cancel if we are searching/reading categories.
    Cancel();

    iObserver = aObserver;
    iSearchResultExists = EFalse;
    iFilterSearch = EFalse;

    // Complete ourselves and start search for all categories in db in RunL().
    iStatus = KRequestPending;
    SetPriorityAndSetActive();
    iActiveOperation = ECategorySearch;
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesEngine::StartSearchingCategoriesL(
    const TDesC& aSearchPattern, 
    TBool aSearchOnlyInPreviousMatches,
    MLandmarksOperationObserver* aObserver)
    {
    // We need to cancel if we are searching/reading categories.
    Cancel();

    if (!iSearchResultExists)
        {
        // There are no previous results or previous search did not complete.
        // We cannot use the result from it.
        aSearchOnlyInPreviousMatches = EFalse;
        }
    iSearchResultExists = EFalse;
    iFilterSearch = ETrue;

    // Set search criteria
    CPosLmCatNameCriteria* criteria = CPosLmCatNameCriteria::NewLC();
    criteria->SetSearchPatternL(aSearchPattern);

    // Create search operation
    CPosLmOperation* operation = iSearcher->StartCategorySearchL(
        *criteria, 
        CPosLmCategoryManager::ECategorySortOrderNameAscending, 
        aSearchOnlyInPreviousMatches);
    CleanupStack::PopAndDestroy(criteria);

    // Start search operation
    iObserver = aObserver;
    iStatus = KRequestPending;
    iLmOpWrapper->StartOperation(operation, iStatus);
    SetPriorityAndSetActive();
    iActiveOperation = ECategorySearch;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
RArray<TPosLmItemId>* CLandmarksCategoriesEngine::FetchSearchResult()
    {
    return &iItemIds;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesEngine::StartReadingCategoriesL(
    TInt aNrOfItemsToReadPerBundle,
    MLandmarksOperationObserver* aObserver)
    {
    // We need to cancel if we are searching/reading landmarks.
    Cancel();

    iNrOfItemsToRead = aNrOfItemsToReadPerBundle;
    iObserver = aObserver;
    iCurrentItemId = 0;

    // Start reading a bundle of categories
    ReadSomeCategoriesL();
    iActiveOperation = ECategoryRead;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CArrayPtr<CPosLandmarkCategory>* CLandmarksCategoriesEngine::FetchCategoriesLC()
    {
    TInt nrOfItems = iSubSetOfIds.Count();
    CArrayPtr<CPosLandmarkCategory>* categories = 
        new (ELeave) CArrayPtrFlat<CPosLandmarkCategory>(nrOfItems);
    CleanupStack::PushL(categories);

    // Start the actual read of landmarks. This is done synchronously since 
    // there is no asynchronous read method as there is for landmarks:
    // (Compare TakePreparedPartialLandmarksL()) 
    for (TInt i = 0; i < nrOfItems; i++)
        {
        CPosLandmarkCategory* category = 
            iCatManager->ReadCategoryLC(iSubSetOfIds[i]);
        categories->AppendL(category);
        CleanupStack::Pop(category);
        }

    return categories;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CArrayPtr<CPosLandmarkCategory>* CLandmarksCategoriesEngine::CategoriesL()
    {
    const TInt KGranularity = 8;
    CArrayPtrFlat<CPosLandmarkCategory>* categories = 
        new (ELeave) CArrayPtrFlat<CPosLandmarkCategory>(KGranularity);
    CleanupStack::PushL(categories);
    
    // Get an iterator for all existing categories sorted ascending by name
    CPosLmItemIterator* itemIterator = iCatManager->CategoryIteratorL(
        CPosLmCategoryManager::ECategorySortOrderNameAscending);
    CleanupStack::PushL(itemIterator);

    // Read all categories and add them to an array
    TPosLmItemId catId = itemIterator->NextL();
    while (catId != KPosLmNullItemId)
        {
        CPosLandmarkCategory* category = iCatManager->ReadCategoryLC(catId);
        categories->AppendL(category);
        CleanupStack::Pop(category);

        catId = itemIterator->NextL();
        }
    CleanupStack::PopAndDestroy(itemIterator);
    CleanupStack::Pop(categories);

    return categories;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesEngine::RunL()
    {
    if (iStatus == KErrNone)
        {
        if (iActiveOperation == ECategorySearch)
            {
            // Fetch search result
            PopulateItemIdArrayL();
            }
        else if (iActiveOperation == ECategoryRead)
            {
            if (iCurrentItemId < iItemIds.Count())
                {
                // There are still found landmarks to read
                iObserver->NotifyOperationProgressL(
                    iActiveOperation, KOperationNotReady, KErrNone);
                ReadSomeCategoriesL();
                }
            else
                {
                // No more matches to read
                NotifyOperationReadyL(iActiveOperation, KErrNone);
                }
            }
        }
    else
        {
        // Operation failed. Forward error code
        NotifyOperationReadyL(iActiveOperation, iStatus.Int());
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesEngine::DoCancel()
    {
    iLmOpWrapper->Cancel();
    if (iActiveOperation == ECategorySearch)
        {
        iSearchResultExists = EFalse;
        }
    iActiveOperation = ENoOperation;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TInt CLandmarksCategoriesEngine::RunError(TInt aError)
    {
    // Notify user and ignore error code
    TInt err;
    TRAP(err, NotifyOperationReadyL(iActiveOperation, aError));

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesEngine::ReadSomeCategoriesL()
    {
    // Create a subset of the found matches to read
    iSubSetOfIds.Reset();
    TInt nrOfMatches = iItemIds.Count();
    TInt i = 0;
    while (i < iNrOfItemsToRead && iCurrentItemId < nrOfMatches)
        {
        User::LeaveIfError(
            iSubSetOfIds.Append(iItemIds[iCurrentItemId]));
        iCurrentItemId++;
        i++;
        }

    // Simulate start reading found matches. There is no asynchronous search 
    // functionality for categories as there is for landmarks, 
    // (Compare PreparePartialLandmarksL())
    iStatus = KRequestPending;
    SetPriorityAndSetActive();
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesEngine::NotifyOperationReadyL(
    TOperation aOperation, 
    TInt aErrorCode)
    {
    iActiveOperation = ENoOperation;
    iObserver->NotifyOperationProgressL(
        aOperation, 
        KOperationReady, 
        aErrorCode);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksCategoriesEngine::PopulateItemIdArrayL()
    {
    // Fetch itemIterator
    CPosLmItemIterator* itemIterator = NULL;
    if (iFilterSearch)
        {
        itemIterator = iSearcher->MatchIteratorL();
        iSearchResultExists = ETrue;
        }
    else
        {
        itemIterator = iCatManager->CategoryIteratorL(
            CPosLmCategoryManager::ECategorySortOrderNameAscending);
        }
    CleanupStack::PushL(itemIterator);
    itemIterator->Reset();

    // Fetch iItemIds
    TUint nrOfItems = itemIterator->NumOfItemsL();
    if (nrOfItems > 0)
        {
        TInt startIndex = 0;
        iItemIds.Reset();
        itemIterator->GetItemIdsL(iItemIds, startIndex, nrOfItems);
        NotifyOperationReadyL(iActiveOperation, KErrNone);
        }
    else
        {
        // No matches found
        NotifyOperationReadyL(iActiveOperation, KErrNotFound);
        }
    CleanupStack::PopAndDestroy(itemIterator);
    }

  
