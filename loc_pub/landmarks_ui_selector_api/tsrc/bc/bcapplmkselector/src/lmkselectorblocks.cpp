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
* Description:    LandmarkUi Content File
*
*/








// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>
#include "LmkSelector.h"


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
// CLmkSelector::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CLmkSelector::Delete()
    {

    }

// -----------------------------------------------------------------------------
// CLmkSelector::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CLmkSelector::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "Example", CLmkSelector::ExampleL ),
        ENTRY( "TestLmkNewL", CLmkSelector::TestLmkNewL ),
        ENTRY( "TestLmkSetMopParent", CLmkSelector::TestLmkSetMopParent ),
        ENTRY( "TestLmkExecuteLD", CLmkSelector::TestLmkExecuteLD ),
        ENTRY( "TestLmkExecuteLD2", CLmkSelector::TestLmkExecuteLD2 ),
        ENTRY( "TestCatNewL", CLmkSelector::TestCatNewL ),
        ENTRY( "TestCatSetMopParent", CLmkSelector::TestCatSetMopParent ),
        ENTRY( "TestCatExecuteLD", CLmkSelector::TestCatExecuteLD ),
        ENTRY( "TestCatExecuteLD2", CLmkSelector::TestCatExecuteLD2 ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CLmkSelector::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLmkSelector::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KLmkSelector, "LmkSelector" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KLmkSelector, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KLmkSelector,
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }



 void CLmkSelector::CreateNewLandmark()
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




TInt CLmkSelector::TestLmkNewL( CStifItemParser& aItem )
{
			CLmkLandmarkSelectorDlg* aLmkSelectDlg = CLmkLandmarkSelectorDlg::NewL();
			CleanupStack::PushL( aLmkSelectDlg );
			CleanupStack::PopAndDestroy();
			//delete aLmkSelectDlg;
			//aLmkSelectDlg = NULL;

			return KErrNone;
}

TInt CLmkSelector::TestLmkSetMopParent( CStifItemParser& aItem )
{
      CLmkLandmarkSelectorDlg* iLmkSelectDlg = CLmkLandmarkSelectorDlg::NewL();
			//iLmkSelectDlg->SetMopParent(this);
			delete iLmkSelectDlg;
			iLmkSelectDlg = NULL;
	//		CleanupStack::Pop();
			return KErrNone;
}

TInt CLmkSelector::TestLmkExecuteLD( CStifItemParser& aItem )
{

CreateNewLandmark();

			TLmkItemIdDbCombiInfo selItem;
			CLmkLandmarkSelectorDlg* iLmkSelectDlg = CLmkLandmarkSelectorDlg::NewL();
        	TRAPD( result, iLmkSelectDlg->ExecuteLD(selItem));
        	return KErrNone;
}

TInt CLmkSelector::TestLmkExecuteLD2( CStifItemParser& aItem )
{
      RArray<TLmkItemIdDbCombiInfo> selItems;
			CreateNewLandmark();
			CLmkLandmarkSelectorDlg* iLmkSelectDlg = CLmkLandmarkSelectorDlg::NewL();
        	TRAPD( result, iLmkSelectDlg->ExecuteLD(selItems));
        	selItems.Close();
        	return KErrNone;
}

TInt CLmkSelector::TestCatNewL( CStifItemParser& aItem )
{
      CLmkCategorySelectorDlg* iLmkCategoryDlg = CLmkCategorySelectorDlg::NewL();
			delete iLmkCategoryDlg;
			iLmkCategoryDlg = NULL;
			return KErrNone;
}

TInt CLmkSelector::TestCatSetMopParent( CStifItemParser& aItem )
{

			engine = new (ELeave) CLmkEngine;
			CLmkCategorySelectorDlg* iLmkCategoryDlg = CLmkCategorySelectorDlg::NewL();
			iLmkCategoryDlg->SetMopParent(engine);
			delete iLmkCategoryDlg;
			iLmkCategoryDlg = NULL;
			return KErrNone;
}

TInt CLmkSelector::TestCatExecuteLD( CStifItemParser& aItem )
{
			TLmkItemIdDbCombiInfo selItem;
			//CreateNewLandmark();
			CLmkCategorySelectorDlg* iLmkCategoryDlg = CLmkCategorySelectorDlg::NewL();
        	TRAPD( result, iLmkCategoryDlg->ExecuteLD(selItem));
			//delete iLmkCategoryDlg;
			//iLmkCategoryDlg = NULL;
			return KErrNone;
}

TInt CLmkSelector::TestCatExecuteLD2( CStifItemParser& aItem )
{
					RArray<TLmkItemIdDbCombiInfo> selItems;
					//CreateNewLandmark();
            CLmkCategorySelectorDlg* iLmkCategoryDlg = CLmkCategorySelectorDlg::NewL();
        	TRAPD( result, iLmkCategoryDlg->ExecuteLD(selItems));
        	//delete iLmkCategoryDlg;
			//iLmkCategoryDlg = NULL;
        	selItems.Close();
        	return KErrNone;
}
// -----------------------------------------------------------------------------
// CLmkSelector::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CLmkSelector::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
