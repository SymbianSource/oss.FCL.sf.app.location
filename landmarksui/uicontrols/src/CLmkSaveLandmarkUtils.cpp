/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains methods for aiding the process of saving
*              : landmarks which are received via messaging
*
*/







// INCLUDE FILES
#include <e32std.h>
#include <eikenv.h>
#include <StringLoader.h>
#include <AknQueryDialog.h>
#include <AknWaitDialog.h>
#include <aknnotewrappers.h>
#include <EPos_CPosLmCategoryManager.h>
#include <lmkui.rsg>
#include "CLmkParser.h"
#include "CLmkLandMarkCategoriesName.h"
#include "CLmkSaveLandmarkUtils.h"
#include "CLmkPkgFindNewCatOp.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkSaveLandmarkUtils::CLmkSaveLandmarkUtils
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkSaveLandmarkUtils::CLmkSaveLandmarkUtils(
    CPosLandmarkDatabase& aDb,
    CLmkParser& aParser)
    : iDb( aDb ),
      iParser( aParser )
    {
    }

// -----------------------------------------------------------------------------
// CLmkSaveLandmarkUtils::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkSaveLandmarkUtils::ConstructL()
    {
    iEnv = CEikonEnv::Static();
    }

// -----------------------------------------------------------------------------
// CLmkSaveLandmarkUtils::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkSaveLandmarkUtils* CLmkSaveLandmarkUtils::NewL(
    CPosLandmarkDatabase& aDb,
    CLmkParser& aParser)
    {
    CLmkSaveLandmarkUtils* self =
        new( ELeave )CLmkSaveLandmarkUtils( aDb, aParser );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CLmkSaveLandmarkUtils::~CLmkSaveLandmarkUtils
// ----------------------------------------------------
//
CLmkSaveLandmarkUtils::~CLmkSaveLandmarkUtils()
    {
    CancelSearchNewCat();
    }

// ----------------------------------------------------
// CLmkSaveLandmarkUtils::~CheckForNewCategoriesL
// ----------------------------------------------------
//
TInt CLmkSaveLandmarkUtils::CheckForNewCategoriesL(RArray<TUint> & aSelectedIndices,
                                                   TDes& aNewFoundCategory,
                                                   RArray<TCategoryName>& aNewcategories)
	{
	TInt TotalCount = aSelectedIndices.Count();
	RArray<TCategoryName> categories;
	CleanupClosePushL(categories);

	for(TInt cnt = 0; cnt < TotalCount; cnt++ )
		{
		CLmkLandMarkCategoriesName* categoryNames = iParser.CategoryNameLC(aSelectedIndices[cnt]);
		categories.Reset();
		categoryNames->GetCategoryNames(categories);
		TInt totCategories = categories.Count();
		for(TInt catCount = 0; catCount < totCategories; catCount++ )
			{
			CheckCategoryInDatabaseL(aNewcategories,categories[catCount]);
			}

		CleanupStack::PopAndDestroy(); //categoryNames
		}

	TInt newCategoriesFound = aNewcategories.Count();
	if ( newCategoriesFound == 1 )
		{
		aNewFoundCategory.Copy(aNewcategories[0]);
		}
	CleanupStack::PopAndDestroy(); // categories
	return newCategoriesFound;
	}

// ----------------------------------------------------
// CLmkSaveLandmarkUtils::CheckCategoryInDatabase
// This routine check wheather category associated with landmark is already exist in database or not.
// ----------------------------------------------------
void CLmkSaveLandmarkUtils::CheckCategoryInDatabaseL(RArray<TCategoryName>& aNewcategories,const TDesC& aCategoryTobeAdded)
	{
	TInt Total = aNewcategories.Count();
	for(TInt cnt = 0; cnt < Total; cnt++ )
		{
		if (aNewcategories[cnt] == aCategoryTobeAdded)
		return;
		}

	CPosLmCategoryManager* categoryManager = CPosLmCategoryManager::NewL( iDb );
	CleanupStack::PushL( categoryManager );

	if ( categoryManager->GetCategoryL(aCategoryTobeAdded) ==  KPosLmNullItemId)
		{
		aNewcategories.Append(aCategoryTobeAdded);
		}
	CleanupStack::PopAndDestroy(); //categoryManager
	}

// ----------------------------------------------------
// CLmkSaveLandmarkUtils::CategoryAddConfirmationQueryL
// This routine ask confirmation about adding landmark , with OR without category
// ----------------------------------------------------
TInt CLmkSaveLandmarkUtils::CategoryAddConfirmationQueryL( TInt aAmount,const TDesC& aNewCategoryName )
	{
	HBufC* noteText = NULL;

	if ( aAmount > 1)
		{
		noteText = StringLoader::LoadLC( R_LMK_ADD_CATEGORIES,aAmount, iEnv );
		}
	else if ( aAmount == 1)
		{
		noteText = StringLoader::LoadLC( R_LMK_ADD_CATEGORY,aNewCategoryName, iEnv );
		}
	else
		{
		return 1;
		}

	CAknQueryDialog* query =
	new( ELeave ) CAknQueryDialog( *noteText );
	TInt ret = query->ExecuteLD( R_LMK_QUERY );	// noteText

	CleanupStack::PopAndDestroy(noteText); //noteText
	return ret;
	}

// ----------------------------------------------------
// CLmkSaveLandmarkUtils::LmsSavedConfirmationNoteL
// This method takes care of importing the selected Landmarks
// ----------------------------------------------------
void CLmkSaveLandmarkUtils::LmsSavedConfirmationNoteL(RArray<TUint> & aImportIndexes,
                                           			  TInt aEditedItemIndex)

   	{
	HBufC* noteText = NULL;

    // Display Landmark numbers in the Note
	CPosLandmark* landmark = iParser.LandmarkLC(aEditedItemIndex);
	TPtrC lmName;
	User::LeaveIfError(landmark->GetLandmarkName(lmName));
	TUint lmCount = aImportIndexes.Count();

	if (lmCount > 1)
		{
		noteText = StringLoader::LoadLC( R_LMK_NOTE_LMS_SAVED,lmCount, iEnv );
		}
	else if (lmCount == 1)
		{
		noteText = StringLoader::LoadLC( R_LMK_NOTE_LANDMARK_SAVED, lmName, iEnv );
		}
	else
		{
		return;
		}

	CAknConfirmationNote* note = new (ELeave) CAknConfirmationNote( ETrue );
	TInt retVal = note->ExecuteLD( *noteText );
	CleanupStack::PopAndDestroy(2);//  noteText
	}

// ----------------------------------------------------
// CLmkSaveLandmarkUtils::CheckForNewCategories2L
// ----------------------------------------------------
//
TInt CLmkSaveLandmarkUtils::CheckForNewCategories2L(
                                     TDes& aNewFoundCategory,
                                     RArray<TCategoryName>& aNewcategories)
	{
	TInt TotalCount = iParser.NumOfParsedLandmarks();
	RArray<TCategoryName> categories;
	CleanupClosePushL(categories);

	for(TInt cnt = 0; cnt < TotalCount; cnt++ )
		{
		CLmkLandMarkCategoriesName* categoryNames = iParser.CategoryNameLC(cnt);
		categories.Reset();
		categoryNames->GetCategoryNames(categories);
		TInt totCategories = categories.Count();
		for(TInt catCount = 0; catCount < totCategories; catCount++ )
			{
			CheckCategoryInDatabaseL(aNewcategories,categories[catCount]);
			}

		CleanupStack::PopAndDestroy(); //categoryNames
		}

	TInt newCategoriesFound = aNewcategories.Count();
	if ( newCategoriesFound == 1 )
		{
		aNewFoundCategory.Copy(aNewcategories[0]);
		}
	CleanupStack::PopAndDestroy(); // categories
	return newCategoriesFound;
	}

// ----------------------------------------------------
// CLmkSaveLandmarkUtils::StartSearchNewCatL
// ----------------------------------------------------
//
void CLmkSaveLandmarkUtils::StartSearchNewCatL(
                            TInt& aNoOfNewCategories,
                            RArray<TCategoryName>& aNewCategories,
                            TCategoryName& aNewCategoryFound)
	{
	CPosLmOperation* operation = CLmkPkgFindNewCatOp::NewL
	                                (aNoOfNewCategories,
	                                 aNewCategories,
	                                 aNewCategoryFound,
	                                 this);
    CleanupStack::PushL( operation );
	iAOOperation =
	CLmkAOOperation::NewL( operation,
	               *this,
	               ENewPkgCategory,
	               ETrue );
	CleanupStack::Pop( operation ); // ownership transferred

	iAOOperation->StartOperation();
	}
// ----------------------------------------------------
// CLmkSaveLandmarkUtils::CancelSearchNewCat
// ----------------------------------------------------
//
void CLmkSaveLandmarkUtils::CancelSearchNewCat()
	{
	if (iAOOperation)
    	{
    	delete iAOOperation;
        iAOOperation = NULL;
    	}
	}
// ----------------------------------------------------
// CLmkSaveLandmarkUtils::HandleOperationL
// ----------------------------------------------------
//
void CLmkSaveLandmarkUtils::HandleOperationL(
                                       TOperationTypes aType,
                                       TReal32 aProgress,
                                       TInt aStatus )
    {
    if ( iObserver )
        {
        iObserver->HandleOperationL(aType,aProgress,aStatus);
        }
    }
// ----------------------------------------------------
// CLmkSaveLandmarkUtils::SetObserver
// ----------------------------------------------------
//
void CLmkSaveLandmarkUtils::SetObserver(MLmkAOOperationObserver* aObserver)
    {
    iObserver = aObserver;
    }
// End of File
