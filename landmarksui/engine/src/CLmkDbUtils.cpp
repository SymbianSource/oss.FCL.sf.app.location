/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains utility methods for performing
*                landmark database related operations like adding, deleting
*                or updating a landmark and category etc.
*
*/



// INCLUDE FILES

#include <eikenv.h>
#include <eikcapc.h>
#include <EPos_CPosLmCategoryManager.h>
#include "LmkConsts.h"
#include "landmarks.hrh"
#include "Debug.h"
#include "CLmkAOOperation.h"
#include "MLmkAOOperationObserver.h"
#include "CLmkIconOperation.h"
#include "CLmkAddToCatOperation.h"
#include "CLmkDbUtils.h"
#include "CLmkFields.h"
#include <lmkerrors.h>


// CONSTANTS
/// Unnamed namespace for local definitions
namespace {
#if defined(_DEBUG)
_LIT( KPanicMsg, "CLmkDbUtils" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
#endif
const TInt KUrlProtocolEnds (3);
_LIT( KDoubleSlash, "//"); // Empty Mime Type indicator
}  // namespace

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkDbUtils::CLmkDbUtils
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkDbUtils::CLmkDbUtils(
CPosLandmarkDatabase& aDb,
MLmkAOOperationObserver& aObserver,
TBool aProgressNote )
: iDb( aDb ),
  iObserver( &aObserver ),
  iProgressNote( aProgressNote )
	{
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::NewL()
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkDbUtils* CLmkDbUtils::NewL(
CPosLandmarkDatabase& aDb,
MLmkAOOperationObserver& aObserver,
TBool aProgressNote )
	{
	CLmkDbUtils* self =
	    new ( ELeave ) CLmkDbUtils( aDb, aObserver, aProgressNote );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(); //self
	return self;
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::ConstructL()
// -----------------------------------------------------------------------------
//
void CLmkDbUtils::ConstructL()
	{
	iCategoryMgr = CPosLmCategoryManager::NewL( iDb );
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::~CLmkDbUtils()
// -----------------------------------------------------------------------------
//
CLmkDbUtils::~CLmkDbUtils()
	{
	CancelLmOperation();
	delete iCategoryMgr;
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::HandleOperationL
// -----------------------------------------------------------------------------
//
void CLmkDbUtils::HandleOperationL(
TOperationTypes aType,
TReal32 aProgress,
TInt aStatus )
	{
	DEBUG2( CLmkDbUtils::HandleOperationL entered: progress=%d status=%d,
	    aProgress, aStatus );
	if ( aStatus != KPosLmOperationNotComplete )
		{
		// iAOOperation usually always exists when this method is called.
		// Exception may be e.g. that a leave is trapped by CLmkDbUtils user.
		delete iAOOperation;
		iAOOperation = NULL;
		}
	//this handle operation performs db compact operation.
	TRAPD(err,iObserver->HandleOperationL( aType, aProgress, aStatus ));
	if (err == KErrLocked)
		{
		//since db compact operation failed, db has to be re-initialized.
		InitializeDbL();
		}
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::InitializeDbL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkDbUtils::InitializeDbL()
	{
	DEBUG( CLmkDbUtils::InitializeDbL entered );
	LeaveIfInUseL();

	// We do not check IsInitializingNeeded() since it is better to do this
	// always asynchronously to ensure consistent program flow.

	CPosLmOperation* operation = NULL;
	operation = iDb.InitializeL();

	SetupAndStartOperationL( operation, EInitialize );
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::CompactDbL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkDbUtils::CompactDbL()
	{
	DEBUG( CLmkDbUtils::CompactDbL entered );
	LeaveIfInUseL();

	CPosLmOperation* operation = iDb.CompactL();

	SetupAndStartOperationL( operation, ECompact );
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::AddCategoryL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkDbUtils::AddCategoryL(
	CPosLandmarkCategory& aCategory,
	CPosLandmarkDatabase& aDb )
	{
	CPosLmCategoryManager* categoryMgr = CPosLmCategoryManager::NewL( aDb );
	CleanupStack::PushL( categoryMgr );

	categoryMgr->AddCategoryL( aCategory );

	CleanupStack::PopAndDestroy( categoryMgr );
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::RenameCategoryL
// -----------------------------------------------------------------------------
//
	EXPORT_C void CLmkDbUtils::RenameCategoryL(
	TDesC& aCategoryName,
	TPosLmItemId aId,
	CPosLandmarkDatabase& aDb )
	{
	CPosLmCategoryManager* categoryMgr = CPosLmCategoryManager::NewL( aDb );
	CleanupStack::PushL( categoryMgr );

	CPosLandmarkCategory* category = categoryMgr->ReadCategoryLC( aId );
	category->SetCategoryNameL( aCategoryName );
	categoryMgr->UpdateCategoryL( *category );

	CleanupStack::PopAndDestroy( 2 ); //category, categoryMgr
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::CategoryNameL
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC* CLmkDbUtils::CategoryNameL(
	CPosLandmarkDatabase& aDb,
	TPosLmItemId  aCategoryId )
	{
	CPosLmCategoryManager* categoryMgr = CPosLmCategoryManager::NewL( aDb );
	CleanupStack::PushL( categoryMgr );

	CPosLandmarkCategory* category = categoryMgr->ReadCategoryLC(aCategoryId);
	TPtrC catName;
	User::LeaveIfError( category->GetCategoryName ( catName ) );
	HBufC* result = catName.AllocL();
	CleanupStack::PopAndDestroy( 2 ); //category, categoryMgr
	return result;
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::ChangeIconL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkDbUtils::ChangeIconL(
	CPosLandmarkDatabase& aDb,
	const TPosLmItemId aId,
	TLmkItemType aItemType,
	const TDesC& aMbmFileName,
	TInt aIconIndex, TInt aMaskIndex)
	{

	if ( aItemType == ELmkItemTypeLandmark )
		{
		CPosLandmark* landmark = aDb.ReadLandmarkLC( aId );

		landmark->SetIconL( aMbmFileName, aIconIndex, aMaskIndex); //KLmkDefaultId+1 );
		aDb.UpdateLandmarkL( *landmark );
		CleanupStack::PopAndDestroy(); // landmark
		}
	else
		{
		__ASSERT_DEBUG( aItemType == ELmkItemTypeCategory,
		            Panic( KLmkPanicUnknownItemType ) );

		CPosLmCategoryManager* categoryMgr = CPosLmCategoryManager::NewL(aDb);
		CleanupStack::PushL( categoryMgr );
		CPosLandmarkCategory* category = categoryMgr->ReadCategoryLC( aId );
		category->SetIconL( aMbmFileName, aIconIndex, aMaskIndex);//KCategoryDefaultId+1);

		categoryMgr->UpdateCategoryL( *category );
		CleanupStack::PopAndDestroy( 2 ); //category, categoryMgr
		}
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::ChangeIconsL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkDbUtils::ChangeIconsL(
	const RArray<TPosLmItemId>& aIdArray,
	TLmkItemType aItemsType,
	const TDesC& aMbmFileName,
	TInt aIconIndex, TInt /*aMaskIndex*/)
	{
	LeaveIfInUseL();

	CPosLmOperation* operation = CLmkIconOperation::NewL( iDb,
	                                                      aIdArray,
	                                                      aItemsType,
	                                                      aMbmFileName,
	                                                      aIconIndex );

	SetupAndStartOperationL( operation, EChangeIcons );
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::DeleteCategoriesL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkDbUtils::DeleteCategoriesL(
	const RArray<TPosLmItemId>&  aCategoryIdArray )
	{
	LeaveIfInUseL();

	CPosLmOperation* operation =
	                iCategoryMgr->RemoveCategoriesL( aCategoryIdArray );

	SetupAndStartOperationL( operation, EDeleteCategories );
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::DeleteLandmarkL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkDbUtils::DeleteLandmarkL(
	TPosLmItemId aId,
	CPosLandmarkDatabase& aDb )
	{
	aDb.RemoveLandmarkL( aId );
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::DeleteLandmarksL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkDbUtils::DeleteLandmarksL(
	const RArray<TPosLmItemId>& aLandmarkIdArray )
	{
	LeaveIfInUseL();

	CPosLmOperation* operation =
	                    iDb.RemoveLandmarksL( aLandmarkIdArray );

	SetupAndStartOperationL( operation, EDeleteLandmarks );
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::AddLmToCategoryL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkDbUtils::AddLmToCategoryL(
	TPosLmItemId aLandmarkId,
	TPosLmItemId aCategoryId,
	CPosLandmarkDatabase& aDb )
	{
	CPosLandmark* landmark = aDb.ReadLandmarkLC( aLandmarkId );
	landmark->AddCategoryL( aCategoryId );
	aDb.UpdateLandmarkL( *landmark );
	CleanupStack::PopAndDestroy(); // landmark
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::AddLmToCategoriesL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkDbUtils::AddLmToCategoriesL(
	TPosLmItemId aLandmarkId,
	const RArray<TPosLmItemId>& aCategoryIdArray )
	{
	LeaveIfInUseL();

	CPosLmOperation* operation =
	    CLmkAddToCatOperation::NewL( *iCategoryMgr,
	                                 aLandmarkId,
	                                 aCategoryIdArray );

	SetupAndStartOperationL( operation, EAddToCategory );
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::AddLmsToCategoriesL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkDbUtils::AddLmsToCategoriesL(
	const RArray<TPosLmItemId>& aLandmarkIdArray,
	const RArray<TPosLmItemId>& aCategoryIdArray )
	{
	LeaveIfInUseL();

	CPosLmOperation* operation =
	    CLmkAddToCatOperation::NewL( *iCategoryMgr,
	                                 aLandmarkIdArray,
	                                 aCategoryIdArray );

	SetupAndStartOperationL( operation, EAddToCategory );
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::CancelLmOperation
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkDbUtils::CancelLmOperation()
	{
	if ( iAOOperation )
	    {
	    iAOOperation->Cancel();
	    delete iAOOperation;
	    iAOOperation = NULL;
	    }
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::LeaveIfInUseL
// -----------------------------------------------------------------------------
//
void CLmkDbUtils::LeaveIfInUseL()
	{
	if ( iAOOperation )
		{
		User::Leave( KErrInUse );
		}
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::SetupAndStartOperationL
// -----------------------------------------------------------------------------
//
void CLmkDbUtils::SetupAndStartOperationL(
	CPosLmOperation* aOperation,
	MLmkAOOperationObserver::TOperationTypes aOpType )
	{
	CleanupStack::PushL( aOperation );
	iAOOperation =
	CLmkAOOperation::NewL( aOperation,
	               *this,
	               aOpType,
	               iProgressNote );
	CleanupStack::Pop( aOperation ); // ownership transferred

	iAOOperation->StartOperation();
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::CategoryIdL()
// -----------------------------------------------------------------------------
//
EXPORT_C  TPosLmItemId CLmkDbUtils::CategoryIdL(
	CPosLandmarkDatabase& aDb,
	const TDesC&  aCategoryName )
	{

	CPosLmCategoryManager* categoryMgr = CPosLmCategoryManager::NewL( aDb );
	CleanupStack::PushL( categoryMgr );

	TPosLmItemId  categoryId = categoryMgr->GetCategoryL(aCategoryName);
	if (categoryId == KPosLmNullItemId)
		{
		    User::Leave(KErrArgument);
		}

	CleanupStack::PopAndDestroy(categoryMgr); //, categoryMgr
	return categoryId;
	}


// -----------------------------------------------------------------------------
// CLmkDbUtils::IsLmWithThisFieldExist
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CLmkDbUtils::IsLmWithThisFieldExistL( TPositionFieldId  aFieldId )
	{
	RArray<TPosLmItemId> lmIdArray;
	CleanupClosePushL( lmIdArray );
	lmIdArray.Reset();

	TPosLmSortPref sortOrder( CPosLandmark::ELandmarkName,
	                          TPosLmSortPref::EAscending );
	CPosLmItemIterator* iterator = iDb.LandmarkIteratorL( sortOrder );
	CleanupStack::PushL( iterator );
	TUint count = iterator->NumOfItemsL();
	if ( count > 0 )
		{ // can only be called if there are some items
		iterator->GetItemIdsL( lmIdArray, 0, count );
		}
	CleanupStack::PopAndDestroy( iterator );

	// search for lm with given position field
	TBool  lmkFieldDefined = EFalse;
	for (TInt i = 0; i< lmIdArray.Count(); i++)
		{
		CPosLandmark* landmark = iDb.ReadLandmarkLC(lmIdArray[i]);
		if ( landmark->IsPositionFieldAvailable( aFieldId ) )
			{
			// Check if field info availabe or not
			TPtrC  fieldValue;
			if (landmark->GetPositionField( aFieldId, fieldValue ) == KErrNone )
				{
				if (fieldValue.Length() > 0 )
					{
					lmkFieldDefined = ETrue;
					CleanupStack::PopAndDestroy ( landmark );
					break;
					}
				}
			}
		CleanupStack::PopAndDestroy ( landmark );
		}
	CleanupStack::PopAndDestroy( ); // LmIdArray
	return lmkFieldDefined;
	}


// -----------------------------------------------------------------------------
// CLmkDbUtils::CheckToTrimUrlProtocol
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkDbUtils::RemoveDefaultProtocolL(TPtr& aWebUrl)
	{
 	TInt emptySlashPos(0);
    if ( aWebUrl.Length() > 0 )
        {
        emptySlashPos = aWebUrl.Find( KDoubleSlash );
        if ( emptySlashPos == 0 )
            {
            aWebUrl.Delete(emptySlashPos, 2);
            }
        if(aWebUrl.Length() > KMaxUrlFieldLen)
	    	{
	    	TInt pos = aWebUrl.Find(KProtocol);
	    	aWebUrl.Delete(emptySlashPos, (pos + KUrlProtocolEnds));
	    	}
        }
	}

// -----------------------------------------------------------------------------
// CLmkDbUtils::AddDefaultProtocolL
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkDbUtils::AddDefaultProtocolL(CPosLandmark* landmark)
	{
 	HBufC* webUrl = HBufC::NewL(KMaxBufferLen);
    CleanupStack::PushL(webUrl);
    TPtrC urlDes = webUrl->Des();
    landmark->GetPositionField(ELmkPositionFieldWebAddress,urlDes);
    if(urlDes.Length() > KMaxUrlFieldLenWithDefaultProtocol)
    	{
    	TInt pos = urlDes.Find(KProtocol);
    	if (pos == KErrNotFound)
    		{
    		webUrl = webUrl->ReAllocL( KHttp().Length() +  urlDes.Length() );
    		TPtr url = webUrl->Des();
    		webUrl->Des().Copy(urlDes);
    		webUrl->Des().Insert(0, KHttp);
    		landmark->SetPositionFieldL(ELmkPositionFieldWebAddress,webUrl->Des());
    		}
    	}
    CleanupStack::PopAndDestroy();//webUrl
	}
//  End of File
