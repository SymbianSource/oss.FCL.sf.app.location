/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains search utilities to query the landmarks Db.
*				 For example getting all landmarks under a category, Name of a
*				 particular category etc.
*
*/







// INCLUDE FILES
#include <EPos_CPosLandmark.h>
#include <EPos_CPosLandmarkDatabase.h>
#include <EPos_CPosLandmarkSearch.h>
#include <EPos_CPosLmCategoryCriteria.h>
#include "mlmkdbsearchutilsobserver.h"
#include "MLmkAOOperationObserver.h"
#include "CLmkAOOperation.h"
#include "clmkdbsearchutils.h"

// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// CLmkDbSearchUtils::CLmkDbSearchUtils
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkDbSearchUtils::CLmkDbSearchUtils():
	CActive(EPriorityStandard)
	{
	CActiveScheduler::Add( this );
	}

// -----------------------------------------------------------------------------
// CLmkDbSearchUtils::~CLmkDbSearchUtils
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkDbSearchUtils::~CLmkDbSearchUtils()
	{
	delete iSearch;
	delete iDb;
    if (iSearchAO)
		{
    	iSearchAO->StopOperation();
    	}
	delete iSearchAO;
	delete iCriteria;
	if ( IsActive() )
    	{
        Cancel();
    	}
    iLmItemId.Close();
	}

// -----------------------------------------------------------------------------
// CLmkDbSearchUtils::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkDbSearchUtils* CLmkDbSearchUtils::NewL()
	{
	CLmkDbSearchUtils* self = new (ELeave) CLmkDbSearchUtils();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();// self
	return self;
	}

// -----------------------------------------------------------------------------
// CLmkDbSearchUtils::ConstructL
// -----------------------------------------------------------------------------
//
void CLmkDbSearchUtils::ConstructL()
	{
	iDb = CPosLandmarkDatabase::OpenL();
	iSearch = CPosLandmarkSearch::NewL( *iDb );
	iCriteria = CPosLmCategoryCriteria::NewLC();
	CleanupStack::Pop(  iCriteria );
	iSearchAO = NULL;
	}

// -----------------------------------------------------------------------------
// CLmkDbSearchUtils::GetLandmarksUnderCategoryL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkDbSearchUtils::GetLandmarksUnderCategoryL(MLmkDbSearchUtilsObserver* aObserver,
				  									        TPosLmItemId aCategoryId)
	{
	iObserver = aObserver;
	iCriteria->SetCategoryItemId(aCategoryId);
	TPosLmSortPref sortOrder( CPosLandmark::ELandmarkName,
				              TPosLmSortPref::EAscending );

    CPosLmOperation* operation =
				iSearch->StartLandmarkSearchL( *iCriteria , sortOrder );

	CleanupStack::PushL( operation );
	if (iSearchAO)
		{
    	iSearchAO->StopOperation();
    	}
	delete iSearchAO; // Cancel if any pending requests
	iSearchAO = NULL;
	iSearchAO = CLmkAOOperation::NewL( operation,
				                   	   *this,
				                  	   MLmkAOOperationObserver::ESearch,
				                   	   EFalse );

	CleanupStack::Pop( operation ); // ownership transferred
	iSearchAO->StartOperation();
	}

// -----------------------------------------------------------------------------
// CLmkDbSearchUtils::GetCategoryNameL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CLmkDbSearchUtils::GetCategoryNameL(TPosLmItemId aCategoryId)
	{
	CPosLmCategoryManager* mgr = CPosLmCategoryManager::NewL( *iDb );
	CleanupStack::PushL( mgr );
	// Leaves with KErrNotFound if category doesn't exist
	CPosLandmarkCategory* category = mgr->ReadCategoryLC( aCategoryId );
	TPtrC categoryName;
	User::LeaveIfError(category->GetCategoryName(categoryName));
	// The caller will take ownership of the buffer
	HBufC* retCategoryName = HBufC::NewL(categoryName.Length());
	TPtr ptr =  retCategoryName->Des();
	ptr.Copy(categoryName);
	CleanupStack::PopAndDestroy( 2 ); // category and mgr
	return retCategoryName;
	}

// -----------------------------------------------------------------------------
// CLmkDbSearchUtils::GetLandmarkForIdL
// -----------------------------------------------------------------------------
//
EXPORT_C CPosLandmark* CLmkDbSearchUtils::GetLandmarkForIdL(TPosLmItemId aLandmarkId)
	{
	CPosLandmark* lmk = iDb->ReadLandmarkLC(aLandmarkId);
	CleanupStack::Pop(); // ReadLandmarkLC
	return lmk;
	}

// -----------------------------------------------------------------------------
// CLmkDbSearchUtils::GetLandmarksL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkDbSearchUtils::GetLandmarksL(MLmkDbSearchUtilsObserver* aObserver,
				  							   RArray<TPosLmItemId> aLmItemId,
									  		   RPointerArray<CPosLandmark>* aArray)
	{
	iLmkArray = aArray;
	iLmItemId = aLmItemId;
	iSearchIndex = 0;
	iObserver = aObserver;
	SetActive();
	TRequestStatus *status = &iStatus;
	User::RequestComplete(status, KErrNone);
	}

// -----------------------------------------------------------------------------
// CLmkDbSearchUtils::RunL
// -----------------------------------------------------------------------------
//
void CLmkDbSearchUtils::RunL()
	{
	if(iSearchIndex >= iLmItemId.Count())
		{
		iLmItemId.Close();
		iObserver->HandleLmkSearchNotifyL();
		}
	else
		{
		TPosLmItemId itemId = iLmItemId[iSearchIndex];
		CPosLandmark* landmark = iDb->ReadLandmarkLC(itemId);
		iLmkArray->Append(landmark);
		CleanupStack::Pop(); // ReadLandmarkLC
		++iSearchIndex;
		SetActive();
		TRequestStatus *status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	}

// -----------------------------------------------------------------------------
// CLmkDbSearchUtils::DoCancel
// -----------------------------------------------------------------------------
//
void CLmkDbSearchUtils::DoCancel()
	{

	}

// -----------------------------------------------------------------------------
// CLmkDbSearchUtils::HandleOperationL
// -----------------------------------------------------------------------------
//
void CLmkDbSearchUtils::HandleOperationL(TOperationTypes /*aType*/,
									     TReal32 /*aProgress*/,
							             TInt /*aStatus*/ )
	{
	CPosLmItemIterator* iterator = iSearch->MatchIteratorL();
	CleanupStack::PushL( iterator );
	RArray<TPosLmItemId> itemIdArray;
	CleanupClosePushL( itemIdArray );
	iterator->GetItemIdsL( itemIdArray, 0, iterator->NumOfItemsL() );
	if ( iObserver )
    	{
	    iObserver->HandleCategorySearchNotifyL( itemIdArray );
    	}
    CleanupStack::PopAndDestroy(); //itemIdArray
	CleanupStack::PopAndDestroy( iterator );
	}


