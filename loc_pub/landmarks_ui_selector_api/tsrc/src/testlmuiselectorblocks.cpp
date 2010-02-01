/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -  Entry point for test cases in stif module
*
*/








// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include <EPos_CPosLmDatabaseManager.h>
#include "testlmuiselector.h"
#include "testlmkitemiddbcombiinf.h"


// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS
//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def

// LOCAL CONSTANTS AND MACROS
//const ?type ?constant_var = ?constant;
//#define ?macro_name ?macro_def

// MODULE DATA STRUCTURES
//enum ?declaration
//typedef ?declaration

// LOCAL FUNCTION PROTOTYPES
//?type ?function_name( ?arg_type, ?arg_type );

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// ?function_name ?description.
// ?description
// Returns: ?value_1: ?description
//          ?value_n: ?description_line1
//                    ?description_line2
// -----------------------------------------------------------------------------
//
/*
?type ?function_name(
    ?arg_type arg,  // ?description
    ?arg_type arg)  // ?description
    {

    ?code  // ?comment

    // ?comment
    ?code
    }
*/

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestLmUiSelector::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CTestLmUiSelector::Delete()
    {

    }

// -----------------------------------------------------------------------------
// CTestLmUiSelector::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CTestLmUiSelector::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
    {
            // Copy this line for every implemented function.
            // First string is the function name used in TestScripter script file.
            // Second is the actual implementation member function.
            ENTRY( "GetItemId", CTestLmUiSelector::GetItemIdL ),
            ENTRY( "GetLmDb", CTestLmUiSelector::GetLmDbL ),
            ENTRY( "SetGetItemId", CTestLmUiSelector::SetGetItemIdL ),
            ENTRY( "SetGetLmDb", CTestLmUiSelector::SetGetLmDbL ),
            ENTRY( "TestLmkNewL", CTestLmUiSelector::TestLmkNewL ),
            ENTRY( "TestLmkSetMopParent", CTestLmUiSelector::TestLmkSetMopParent ),
            ENTRY( "TestLmkExecuteLD", CTestLmUiSelector::TestLmkExecuteLD ),
            ENTRY( "TestLmkExecuteLD2", CTestLmUiSelector::TestLmkExecuteLD2 ),
            ENTRY( "TestCatNewL", CTestLmUiSelector::TestCatNewL ),
            ENTRY( "TestCatSetMopParent", CTestLmUiSelector::TestCatSetMopParent ),
            ENTRY( "TestCatExecuteLD", CTestLmUiSelector::TestCatExecuteLD ),
            ENTRY( "TestCatExecuteLD2", CTestLmUiSelector::TestCatExecuteLD2 ),
            ENTRY( "TestSetDialogTitle", CTestLmUiSelector::TestSetDialogTitle ),
            ENTRY( "TestSetDialogTitle1", CTestLmUiSelector::TestSetDialogTitle1 ),
            ENTRY("TestLmkNewLWithUserDatabase",CTestLmUiSelector::TestLmkNewLWithUserDatabase)
            
    };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

//------------------------------------------------------------------------------
//------------------Test Code for TLmkItemIdDbCombiInfo.h-----------------------
//------------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CTestLmUiSelector::GetItemIdL
// Get item id.
// -----------------------------------------------------------------------------
//
TInt CTestLmUiSelector::GetItemIdL( CStifItemParser& /* aItem */ )
	{
	TLmkItemIdDbCombiInf info;
	info.GetItemIdL();

	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestLmUiSelector::GetLmDbL
// Get Lm db handle.
// -----------------------------------------------------------------------------
//
TInt CTestLmUiSelector::GetLmDbL( CStifItemParser& /* aItem */ )
	{
	TLmkItemIdDbCombiInf info;
	info.GetLmDbL();

	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestLmUiSelector::SetGetItemIdL
// Set & get item id.
// -----------------------------------------------------------------------------
//
TInt CTestLmUiSelector::SetGetItemIdL( CStifItemParser& /* aItem */ )
	{
	TLmkItemIdDbCombiInf info;
	info.SetGetItemIdL();

	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CTestLmUiSelector::SetGetLmDbL
// Set & get Lm db handle.
// -----------------------------------------------------------------------------
//
TInt CTestLmUiSelector::SetGetLmDbL( CStifItemParser& /* aItem */ )
	{
	TLmkItemIdDbCombiInf info;
	info.SetGetLmDbL();

	return KErrNone;
	}

//------------------------------------------------------------------------------
//------------------End of Test Code for TLmkItemIdDbCombiInfo.h----------------
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//------------------Test Code for CLmkLandmarkSelectorDlg.h---------------------
//------------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CTestLmUiSelector::TestLmkNewL
// To test the creation of instance for CLmkLandmarkSelectorDlg.
// -----------------------------------------------------------------------------
//

TInt CTestLmUiSelector::TestLmkNewL( CStifItemParser& aItem )
{
	TInt result = KErrNone;
	CLmkLandmarkSelectorDlg* iLmkSelectDlg = NULL;
	TRAPD(error, iLmkSelectDlg = CLmkLandmarkSelectorDlg::NewL());
	if(error!= KErrNone ||!iLmkSelectDlg)
		{
		iLog->Log(_L("FAILED:Creating instance for CLmkLandmarkSelectorDlg failed, result %d"), error);
		result = KErrGeneral;
		}
	else
		{
		delete iLmkSelectDlg;
		iLmkSelectDlg = NULL;
		iLog->Log(_L("PASSED: Creating instance for CLmkLandmarkSelectorDlg passed"));
		}
	return result;
}

// -----------------------------------------------------------------------------
// CTestLmUiSelector::TestLmkSetMopParent
// To test setting the context for this control
// -----------------------------------------------------------------------------
//
TInt CTestLmUiSelector::TestLmkSetMopParent( CStifItemParser& aItem )
{
	TInt result = KErrNone;
	CLmkLandmarkSelectorDlg* iLmkSelectDlg = NULL;
	TRAPD(error, iLmkSelectDlg = CLmkLandmarkSelectorDlg::NewL());
	if(error!= KErrNone ||!iLmkSelectDlg)
		{
		iLog->Log(_L("FAILED:Creating instance for CLmkLandmarkSelectorDlg failed, result %d"), error);
		result = KErrGeneral;
		}
	else
		{
		iLmkSelectDlg->SetMopParent(engine);
		delete iLmkSelectDlg;
		iLmkSelectDlg = NULL;
		iLog->Log(_L("PASSED: Testing Setting context passed"));
		}
	return result;
}

// -----------------------------------------------------------------------------
// CTestLmUiSelector::TestLmkExecuteLD2
// Test test the launch single category selector dialog.
// -----------------------------------------------------------------------------
//
TInt CTestLmUiSelector::TestLmkExecuteLD( CStifItemParser& aItem )
{
	
	CreateNewLandmark();	
	TLmkItemIdDbCombiInfo selItem;
	TInt result = KErrNone;
	CLmkLandmarkSelectorDlg* iLmkSelectDlg = NULL;
	TRAPD(error, iLmkSelectDlg = CLmkLandmarkSelectorDlg::NewL());
	if(error!= KErrNone ||!iLmkSelectDlg)
		{
		iLog->Log(_L("FAILED:Creating instance for CLmkLandmarkSelectorDlg failed, result %d"), error);
		result = KErrGeneral;
		}
	else
		{
		TRAP(error, result = iLmkSelectDlg->ExecuteLD(selItem));
		if(error != KErrNone)
			{
			iLog->Log(_L("FAILED: ExecuteLD fails. Result = %d"), result);
			result = KErrGeneral;
			}
		else
		    {
			iLog->Log(_L("PASSED:launching single landmark dialog passes"));
			result = KErrNone;
		    }
//		delete iLmkSelectDlg;
//		iLmkSelectDlg = NULL;
		}
	return result;
}

// -----------------------------------------------------------------------------
// CTestLmUiSelector::TestLmkExecuteLD2
// To test the launch multiple landmark selector dialog
// -----------------------------------------------------------------------------
//
TInt CTestLmUiSelector::TestLmkExecuteLD2( CStifItemParser& aItem )
{
	RArray<TLmkItemIdDbCombiInfo> selItems;
	CreateNewLandmark();
	TInt result = KErrNone;
	CLmkLandmarkSelectorDlg* iLmkSelectDlg = NULL;
	TRAPD(error, iLmkSelectDlg = CLmkLandmarkSelectorDlg::NewL());
	if(error!= KErrNone ||!iLmkSelectDlg)
		{
		iLog->Log(_L("FAILED:Creating instance for CLmkLandmarkSelectorDlg failed, result %d"), error);
		result = KErrGeneral;
		}     
	else
		{
		TRAP( error, result = iLmkSelectDlg->ExecuteLD(selItems));
		if(error != KErrNone)
			{
			iLog->Log(_L("FAILED: ExecuteLD fails. Error = %d"),error);
			result = KErrGeneral;
			}
		else
		    {
			iLog->Log(_L("PASSED:launching multiple landmark dialog passes"));
			result = KErrNone;
		    }
//
//		delete iLmkSelectDlg;
//		iLmkSelectDlg = NULL;
		}
	selItems.Close();
	return result;
}
// -----------------------------------------------------------------------------
// CTestLmUiSelector::NewL
// To test the SetDialog Title of landmark selector dialog
// -----------------------------------------------------------------------------
//
TInt CTestLmUiSelector::TestLmkNewLWithUserDatabase( CStifItemParser& aItem )
    {
    CreateOwnLandMarks();    
    TLmkItemIdDbCombiInfo selItem;
    TInt result = KErrNone;
    CLmkLandmarkSelectorDlg* iLmkSelectDlg = NULL;
    TRAPD(error, iLmkSelectDlg = CLmkLandmarkSelectorDlg::NewL(KUserDatabaseUri));
    if(result!= KErrNone ||!iLmkSelectDlg)
        {
        iLog->Log(_L("FAILED:Creating instance for CLmkLandmarkSelectorDlg failed, result %d"), error);
        result = KErrGeneral;
        }
    else
        {
        TRAPD(error1, result = iLmkSelectDlg->ExecuteLD(selItem));
        if(error1 != KErrNone || result != KErrNone)
            {
            iLog->Log(_L("FAILED: ExecuteLD fails. Error = %d, Result = %d"), error1, result);
            result = KErrGeneral;
            }
        else
            iLog->Log(_L("PASSED:launching single landmark dialog passes"));
        }
    return KErrNone;
    }



// -----------------------------------------------------------------------------
// CTestLmUiSelector::TestSetDialogTitle
// To test the SetDialog Title of landmark selector dialog
// -----------------------------------------------------------------------------
//
TInt CTestLmUiSelector::TestSetDialogTitle( CStifItemParser& aItem )
    {
    TInt result = KErrNone;
    CreateNewLandmark(); 
    TLmkItemIdDbCombiInfo selItem;
    CLmkLandmarkSelectorDlg* iLmkSelectDlg = NULL;
    TRAPD(error, iLmkSelectDlg = CLmkLandmarkSelectorDlg::NewL());
    if(result!= KErrNone ||!iLmkSelectDlg)
        {
        iLog->Log(_L("FAILED:Creating instance for CLmkLandmarkSelectorDlg failed, result %d"), error);
        result = KErrGeneral;
        }
    else
        {
        _LIT(Kdes,"Test Title");
        TBuf<20> buf;
        buf=Kdes;
        TRAPD(error1,iLmkSelectDlg->SetDialogTitleL(buf));
        if(error1 != KErrNone || result != KErrNone)
            {
            iLog->Log(_L("FAILED: SetDialogTitleL fails. Error = %d, Result = %d"), error1, result);
            result = KErrGeneral;
            }
        else
            {
            iLog->Log(_L("PASSED: SetDialogTitleL for CLmkLandmarkSelectorDlg passed"));
            TRAPD(error2,iLmkSelectDlg->ExecuteLD(selItem));
            if(error2!=KErrNone ||  result!=KErrNone)
                {
                iLog->Log(_L("FAILED: ExecuteLD fails. Error = %d, Result = %d"), error2, result);
                result = KErrGeneral;
                }
            else
                iLog->Log(_L("PASSED: Test of Set Dialog Title Passed"));
            }
        }
    return result;
    }



// -----------------------------------------------------------------------------
// CTestLmUiSelector::TestSetDialogTitle1
// To test the SetDialog Title of landmark selector dialog(Null Value : Landmarks)
// -----------------------------------------------------------------------------
//
TInt CTestLmUiSelector::TestSetDialogTitle1( CStifItemParser& aItem )
    {
    TInt result = KErrNone;
    CreateNewLandmark(); 
    TBuf<50> buf;
    buf=KNullDesC;
    TLmkItemIdDbCombiInfo selItem;
    CLmkLandmarkSelectorDlg* iLmkSelectDlg = NULL;
    TRAPD(error, iLmkSelectDlg = CLmkLandmarkSelectorDlg::NewL());
    if(result!= KErrNone ||!iLmkSelectDlg)
        {
        iLog->Log(_L("FAILED:Creating instance for CLmkLandmarkSelectorDlg failed, result %d"), error);
        result = KErrGeneral;
        }
    else
        {
        TRAPD(error1,iLmkSelectDlg->SetDialogTitleL(buf));
        if(error1 != KErrNone || result != KErrNone)
            {
            iLog->Log(_L("FAILED: SetDialogTitleL fails. Error = %d, Result = %d"), error1, result);
            result = KErrGeneral;
            }
        else
            {
            iLog->Log(_L("PASSED: SetDialogTitleL for CLmkLandmarkSelectorDlg passed"));
            TRAPD(error2,iLmkSelectDlg->ExecuteLD(selItem));
            if(error2!=KErrNone ||  result!=KErrNone)
                {
                iLog->Log(_L("FAILED: ExecuteLD fails. Error = %d, Result = %d"), error2, result);
                result = KErrGeneral;
                }
            else
                iLog->Log(_L("PASSED: Test of Set Dialog Title with NULL value Passed"));
            }
        }
    return result;
    }


//------------------------------------------------------------------------------
//------------------End of Test Code for CLmkLandmarkSelectorDlg.h--------------
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//------------------Test Code for CLmkCategorySelectorDlg.h---------------------
//------------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CTestLmUiSelector::TestCatNewL
// To test the creation of instance for CLmkCategorySelectorDlg.
// -----------------------------------------------------------------------------
//
TInt CTestLmUiSelector::TestCatNewL( CStifItemParser& aItem )
{
	TInt result = KErrNone;
	CLmkCategorySelectorDlg* iLmkCategoryDlg = NULL;
	TRAPD(error, iLmkCategoryDlg = CLmkCategorySelectorDlg::NewL());
	if(error!= KErrNone || !iLmkCategoryDlg)
		{
		iLog->Log(_L("FAILED:Creating instance for CLmkCategorySelectorDlg failed, result %d"), error);
		result = KErrGeneral;
		}
	
	if(iLmkCategoryDlg)
		{
		delete iLmkCategoryDlg;
		iLmkCategoryDlg = NULL;
		iLog->Log(_L("PASSED:Creating instance for CLmkCategorySelectorDlg passes"));
		}	
	return result;
}

// -----------------------------------------------------------------------------
// CTestLmUiSelector::TestCatSetMopParent
// To test setting the context for this control
// -----------------------------------------------------------------------------
//
TInt CTestLmUiSelector::TestCatSetMopParent( CStifItemParser& aItem )
{
	TInt result = KErrNone;
	CLmkCategorySelectorDlg* iLmkCategoryDlg = NULL;
	TRAPD(error, iLmkCategoryDlg = CLmkCategorySelectorDlg::NewL());
	if(error!= KErrNone || !iLmkCategoryDlg)
	{
	iLog->Log(_L("FAILED:Creating instance for CLmkCategorySelectorDlg failed, result %d"), error);
	result = KErrGeneral;
	}	
	else
		iLmkCategoryDlg->SetMopParent(engine);
	if(iLmkCategoryDlg)
		{
		delete iLmkCategoryDlg;
		iLmkCategoryDlg = NULL;
		iLog->Log(_L("PASSED:setting context passes"));
		}
	return result;
}

// -----------------------------------------------------------------------------
// CTestLmUiSelector::TestCatExecuteLD
// Test test the launch single category selector dialog.
// -----------------------------------------------------------------------------
//
TInt CTestLmUiSelector::TestCatExecuteLD( CStifItemParser& aItem )
{
	TLmkItemIdDbCombiInfo selItem;
	TInt result = KErrNone;
	CLmkCategorySelectorDlg* iLmkCategoryDlg = NULL;
	TRAPD(error, iLmkCategoryDlg = CLmkCategorySelectorDlg::NewL());
	if(error!= KErrNone || !iLmkCategoryDlg)
		{
		iLog->Log(_L("FAILED:Creating instance for CLmkCategorySelectorDlg failed, result %d"), error);
		result = KErrGeneral;
		} 	
	else
		{
		TRAP( error, result = iLmkCategoryDlg->ExecuteLD(selItem));
		if(error != KErrNone)
			{
			iLog->Log(_L("FAILED: Testing ExeculteLD fails. Error = %d"), error);
			result = KErrGeneral;
			}
		else
		    {
			iLog->Log(_L("PASSED:launching single category dialog passes"));
			result = KErrNone;
		    }
		}
	return result;
}

// -----------------------------------------------------------------------------
// CTestLmUiSelector::TestCatExecuteLD2
// To test the launch multiple category selector dialog
// -----------------------------------------------------------------------------
//
TInt CTestLmUiSelector::TestCatExecuteLD2( CStifItemParser& aItem )
{
	RArray<TLmkItemIdDbCombiInfo> selItems;
	TInt result = KErrNone;
	CLmkCategorySelectorDlg* iLmkCategoryDlg = NULL;
	TRAPD(error, iLmkCategoryDlg = CLmkCategorySelectorDlg::NewL());
	if(error!= KErrNone || !iLmkCategoryDlg)
		{
		iLog->Log(_L("FAILED:Creating instance for CLmkCategorySelectorDlg failed, result %d"), error);
		result = KErrGeneral;
		} 	
	else
		{
		TRAP( error, result = iLmkCategoryDlg->ExecuteLD(selItems));
		if(error != KErrNone)
			{
			iLog->Log(_L("FAILED: Testing ExeculteLD fails. Error = %d"), error);
			error = KErrGeneral;
			}
		else
		    {
			iLog->Log(_L("PASSED:launching multiple category dialog passes"));
			result = KErrNone;
		    }
		}
	selItems.Close();
	return result;
}
//------------------------------------------------------------------------------
//------------------End of Test Code for CLmkCategorySelectorDlg.h--------------
//------------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// CTestLmUiSelector::CreateNewLandmark
// Creates Landmarks
// -----------------------------------------------------------------------------
//
void CTestLmUiSelector::CreateNewLandmark()
   {

 TRAP_IGNORE(
	CPosLandmarkDatabase* dBase = CPosLandmarkDatabase::OpenL();
	CleanupStack::PushL( dBase );
	if (dBase->IsInitializingNeeded())
		{
	ExecuteAndDeleteLD(dBase->InitializeL()); // Synchronous since no argument
		}

	CPosLmItemIterator*  iterator = dBase->LandmarkIteratorL();
   CleanupStack::PushL(iterator);


   if( iterator->NumOfItemsL() == 0 )
	    {
	    CPosLandmark* landmark = CPosLandmark::NewL();
		CleanupStack::PushL( landmark );

		landmark->SetLandmarkNameL(_L("Nokia"));

		dBase->AddLandmarkL(*landmark);
		CleanupStack::PopAndDestroy();//landmark
	    }

	CleanupStack::PopAndDestroy(2);	//iterator,dBase
	)
	}
// -----------------------------------------------------------------------------
// CTestLmUiSelector::CreateNewLandmark
// Creates Landmarks
// -----------------------------------------------------------------------------
//
void CTestLmUiSelector::CreateOwnLandMarks()
    {
    TRAP_IGNORE(
    CPosLmDatabaseManager* dbManager = CPosLmDatabaseManager::NewL();
    CleanupStack::PushL( dbManager); 
    if ( !dbManager->DatabaseExistsL( KUserDatabaseUri ) )
        {
        HPosLmDatabaseInfo* dbInfo = HPosLmDatabaseInfo::NewLC( KUserDatabaseUri );
        dbManager->CreateDatabaseL( *dbInfo );
        CleanupStack::PopAndDestroy( dbInfo );
        }
    CleanupStack::Pop(dbManager);
    CPosLandmarkDatabase *iUserLandmarksDb;
    //Open and initialize the custom landmark database
    TRAPD( err, ( iUserLandmarksDb = CPosLandmarkDatabase::OpenL( KUserDatabaseUri ) ) );
    if( iUserLandmarksDb->IsInitializingNeeded() )
        {
        ExecuteAndDeleteLD( iUserLandmarksDb->InitializeL() ); 
        }
    ExecuteAndDeleteLD( iUserLandmarksDb->RemoveAllLandmarksL() ); 
    CPosLandmark* landmark = CPosLandmark::NewL();
    CleanupStack::PushL( landmark );
    TRAP_IGNORE(landmark->SetLandmarkNameL( _L("Testlandmark")));
    TPosLmItemId landmarkId = iUserLandmarksDb->AddLandmarkL( *landmark );
    CleanupStack::PopAndDestroy( landmark );
    )
   
    }


//  End of File
