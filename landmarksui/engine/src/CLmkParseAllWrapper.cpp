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
#include "EPos_CPosLandmarkDatabase.h"
#include <EPos_CPosLmCategoryManager.h>
#include "CLmkParseAllWrapper.h"
#include <EPos_CPosLandmarkParser.h>
#include <EPos_CPosLmOperation.h>
#include <EPos_CPosLandmark.h>
#include <EPos_CPosLmOperation.h>
#include "CLmkAOOperation.h"
#include "CLmkLandMarkCategoriesName.h"
#include <lmkerrors.h>

#if defined(_DEBUG)
// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

_LIT( KPanicMsg, "CLmkParseAllWrapper" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
}  // namespace
#endif
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkParseAllWrapper::CLmkParseAllWrapper
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkParseAllWrapper::CLmkParseAllWrapper(
    CPosLandmarkParser& aParser,
    MLmkAOOperationObserver& aObserver,
    CPosLandmarkDatabase& aDb )
    : iParser( aParser ),
      iObserver( aObserver),
      iDb(aDb)
    {
    }

// -----------------------------------------------------------------------------
// CLmkParseAllWrapper::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkParseAllWrapper::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CLmkParseAllWrapper::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CLmkParseAllWrapper* CLmkParseAllWrapper::NewL(
    CPosLandmarkParser& aParser,
    MLmkAOOperationObserver& aObserver,
    CPosLandmarkDatabase& aDb )
    {
    CLmkParseAllWrapper* self =
        new ( ELeave ) CLmkParseAllWrapper( aParser, aObserver, aDb );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CLmkParseAllWrapper::~CLmkParseAllWrapper
// -----------------------------------------------------------------------------
//
CLmkParseAllWrapper::~CLmkParseAllWrapper()
    {
    iLmkLandmarks.ResetAndDestroy();
    iCategoriesNames.ResetAndDestroy();
    delete iAOOperation;
    }

// -----------------------------------------------------------------------------
// CLmkParseAllWrapper::ParseAllL
// -----------------------------------------------------------------------------
//
void CLmkParseAllWrapper::ParseAllL()
    {
    CPosLmOperation* operation = iParser.ParseContentL();

    __ASSERT_DEBUG( !iAOOperation, Panic( KLmkPanicAlreadyActive ) );

    iAOOperation =
        CLmkAOOperation::NewL( operation,
                               *this,
                               MLmkAOOperationObserver::EParse,
                               ETrue );
    iAOOperation->StartOperation();
    }

// -----------------------------------------------------------------------------
// CLmkParseAllWrapper::LandmarkLC
// -----------------------------------------------------------------------------
//
CPosLandmark* CLmkParseAllWrapper::LandmarkLC( TUint aLandmarkIndex )
    {
    __ASSERT_DEBUG( aLandmarkIndex < TUint( iLmkLandmarks.Count() ),
                    Panic( KLmkPanicInvalidIndex ) );

    CLmkLandmark* lmkLandmarks = iLmkLandmarks[aLandmarkIndex];
    CPosLandmark* landmark = lmkLandmarks->PosLandmark();
    return CPosLandmark::NewLC( *landmark );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// CLmkParseAllWrapper::CategoryNameLC
// -----------------------------------------------------------------------------
//
CLmkLandMarkCategoriesName* CLmkParseAllWrapper::CategoryNameLC( TUint aLandmarkIndex )
    {
    __ASSERT_DEBUG( aLandmarkIndex < TUint( iCategoriesNames.Count() ),
                    Panic( KLmkPanicInvalidIndex ) );

	CLmkLandMarkCategoriesName* category = CLmkLandMarkCategoriesName::NewL( *iCategoriesNames[aLandmarkIndex]);
	CleanupStack::PushL(category);
	return category;
    }
// CLmkParseAllWrapper::HandleOperationL
// -----------------------------------------------------------------------------
//
void CLmkParseAllWrapper::HandleOperationL(
    TOperationTypes aType,
    TReal32 aProgress,
    TInt aStatus )
    {
    __ASSERT_DEBUG( aType == EParse, Panic( KLmkPanicUnknownOperation ) );

    if ( aStatus == KPosLmOperationNotComplete ||
         aStatus == KErrNone )
        {
        CPosLandmark* landmark = iParser.LandmarkLC();
	        CLmkLandMarkCategoriesName* categoryNames = CLmkLandMarkCategoriesName::NewL();
	    	CleanupStack::PushL(categoryNames);

	        RArray<TPosLmItemId>*  categoryIdArray = new(ELeave) RArray<TPosLmItemId>;
	        CleanupClosePushL(*categoryIdArray);
	    	landmark->GetCategoriesL( *categoryIdArray );
	    	HBufC* catName = HBufC::NewLC( KPosLmMaxCategoryNameLength );
	    	TPtrC categoryPtr(catName->Des());
	    	RPointerArray<CPosLandmarkCategory>* categories = new(ELeave) RPointerArray<CPosLandmarkCategory>;
	    	CleanupClosePushL(*categories);
	        for( TInt cnt = 0; cnt < categoryIdArray->Count(); cnt++ )
	            {
	            CPosLandmarkCategory *category = iParser.LandmarkCategoryLC((*categoryIdArray)[cnt]);
	            category->GetCategoryName( categoryPtr );
	            /*
	             * For predefined category, find the name from local database and
	             * display the localised names to the user
	             * If received predefined category does not exist in the local db,
	             * then treat is as a newly received category
	             */
	            if ( category->GlobalCategory() != KPosLmNullGlobalCategory ) // global category
	                {
	                // Get the name from db (localised name for predefined category is stored in db)
	                HBufC* globCatName = NULL;
	                CPosLmCategoryManager* categoryMgr = CPosLmCategoryManager::NewL( iDb );
		            CleanupStack::PushL( categoryMgr );
		            globCatName = categoryMgr->GlobalCategoryNameL( category->GlobalCategory() );
		            if ( globCatName == NULL )
	    	            {
	    	            categoryNames->AddCategoryName( categoryPtr );
	    	            }
	    	        else
	        	        {
	        	        CleanupStack::PushL( globCatName );
	                    if ( globCatName->Length() > 0 )
	                        {
	                        // Add it to categories to be shown
	                        categoryNames->AddCategoryName( *globCatName );
	                        }
	                    CleanupStack::PopAndDestroy( globCatName );
	        	        }
		            CleanupStack::PopAndDestroy( categoryMgr );
	                }
	            else // User defined categories
	                {
	                categoryNames->AddCategoryName( categoryPtr );
	                }
	            User::LeaveIfError(categories->Append(category));
	            CleanupStack::Pop(category);
	            }
	    	//categoryIdArray->Close();
	    	CLmkLandmark* lmkLandmarks = CLmkLandmark::NewL(landmark,*categories);
	    	CleanupStack::PushL(lmkLandmarks);
	        User::LeaveIfError( iLmkLandmarks.Append( lmkLandmarks ) );
	        User::LeaveIfError( iCategoriesNames.Append( categoryNames ) ); //Owner ship transferred to array
	        CleanupStack::Pop();//lmkLandmarks
	        CleanupStack::Pop( );//categories
	        categories->ResetAndDestroy();
	        delete categories;
	        CleanupStack::PopAndDestroy( 2 ); //catName,categoryIdArray
	        delete categoryIdArray;
	        CleanupStack::Pop();//categoryNames
	        CleanupStack::PopAndDestroy();//landmark

        }

    if ( aStatus != KPosLmOperationNotComplete )
        { // parsing finished or error occured
        __ASSERT_DEBUG( iAOOperation, Panic( KLmkPanicNullMember ) );

        delete iAOOperation;
        iAOOperation = NULL;

        iObserver.HandleOperationL( aType, aProgress, aStatus );
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// CLmkParseAllWrapper::NumOfLandmarks
// -----------------------------------------------------------------------------
//
TInt CLmkParseAllWrapper::NumOfLandmarks()
	{
	return iLmkLandmarks.Count();
	}

// -----------------------------------------------------------------------------
// CLmkParseAllWrapper::LmkLandmarkLC
// -----------------------------------------------------------------------------
//
CLmkLandmark* CLmkParseAllWrapper::LmkLandmarkLC( TInt aLandmarkIndex )
	{
	__ASSERT_DEBUG( aLandmarkIndex < TUint( iLmkLandmarks.Count() ),
                    Panic( KLmkPanicInvalidIndex ) );

    CLmkLandmark* landmark = iLmkLandmarks[aLandmarkIndex];
    CLmkLandmark* lmkLandmarks = CLmkLandmark::NewL( *landmark );
    CleanupStack::PushL(lmkLandmarks);
    return lmkLandmarks;
	}
//  End of File
