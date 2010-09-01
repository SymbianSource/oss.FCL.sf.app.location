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
#include "UTAppLmkAddEdit.h"

#include <CLmkEditorDlg.h>
#include <Epos_CPosLandmarkDatabase.h>
#include <Epos_Landmarks.h>
#include <coecntrl.h>
#include <lbsposition.h>
#include "UTAppLmkEditorEngine.h"

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
// CUTAppLmkAddEdit::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CUTAppLmkAddEdit::Delete()
    {

    }

// -----------------------------------------------------------------------------
// CUTAppLmkAddEdit::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CUTAppLmkAddEdit::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
		ENTRY( "Example", CUTAppLmkAddEdit::ExampleL ),
		ENTRY( "TestNewL", CUTAppLmkAddEdit::TestNewL ),
		ENTRY( "TestNew2L", CUTAppLmkAddEdit::TestNew2L ),
		ENTRY( "TestExecuteLD", CUTAppLmkAddEdit::TestExecuteLD ),
		ENTRY( "TestSetMopParent", CUTAppLmkAddEdit::TestSetMopParentL ),
		ENTRY( "TestSetHelpContext", CUTAppLmkAddEdit::TestSetHelpContextL ),
		ENTRY( "TestDisableMapAndNaviOptions", CUTAppLmkAddEdit::TestDisableMapAndNaviOptionsL ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CUTAppLmkAddEdit::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUTAppLmkAddEdit::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KUTAppLmkAddEdit, "UTAppLmkAddEdit" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KUTAppLmkAddEdit, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KUTAppLmkAddEdit,
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }

// -----------------------------------------------------------------------------
// CUTAppLmkAddEdit::TestNewL
// This function tests NewL based on landmark id, which exist in the database
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUTAppLmkAddEdit::TestNewL( CStifItemParser& aItem )
	{
	TInt result = KErrNone;

	CPosLandmarkDatabase* ilDb = CPosLandmarkDatabase::OpenL();
	CPosLmOperation* operation = NULL;
	
	if( ilDb->IsInitializingNeeded() )
		{
		operation = ilDb->InitializeL();
		operation->ExecuteL();
		}
	
	CLmkEditorDlg::TLmkEditorParams editParams;
	editParams.iAttributes = CLmkEditorDlg::ELmkAll;
	editParams.iEditorMode = CLmkEditorDlg::ELmkEditor;
	
	CPosLandmark* LmkEditor = CPosLandmark::NewL();
	CleanupStack::PushL( LmkEditor );
	TInt aCase = 0;
	CLmkEditorDlg* editDlg= NULL;	
	aItem.GetNextInt(aCase);
	
	switch(aCase)
		{
		default:
		case DEFAULT:
			TRAPD(error, editDlg = CLmkEditorDlg::NewL(*ilDb,*LmkEditor,editParams));
			if(error != KErrNone||!(editDlg))
				{
				result = KErrGeneral;
				iLog->Log(_L("FAILED: NewL returns %d"), error);
				}
			else
				{
				iLog->Log(_L("PASSED: NewL returns expected results"));
				}
			CleanupStack::PushL( editDlg );
			CleanupStack::PopAndDestroy(2);
			break;
			
		case NULL_DB:
			delete ilDb;
			ilDb = NULL;
			TRAPD(error1, editDlg = CLmkEditorDlg::NewL(*ilDb,*LmkEditor,editParams));
			if(error1 != KErrNone||!(editDlg))
				{
				result = KErrGeneral;
				iLog->Log(_L("FAILED: NewL returns %d"), error1);
				}
			else
				{
				iLog->Log(_L("PASSED: NewL returns expected results"));
				}
			CleanupStack::PushL( editDlg );
			CleanupStack::PopAndDestroy(2);
			break;
		
		case NON_EXISTING_LM:
			TRAPD(error2, editDlg = CLmkEditorDlg::NewL(*ilDb,NULL,editParams));
			if(error2 != KErrNotFound)
				{
				result = KErrGeneral;
				iLog->Log(_L("FAILED: NewL returns %d"), error2);
				}
			else
				{
				iLog->Log(_L("PASSED: NewL returns expected results"));
				}					
			CleanupStack::PopAndDestroy();
			break;
		}

	//ReleaseLandmarkResources();
	
	if(ilDb)
		{
		delete ilDb; 
		ilDb=NULL;
		}
	if(operation)
		{
		delete operation;
		operation=NULL;
		}
	return result;
	}
// -----------------------------------------------------------------------------
// CUTAppLmkAddEdit::TestNew2L
// This function tests NewL based on the landmark object which does not exist
// the database
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUTAppLmkAddEdit::TestNew2L( CStifItemParser& /*aItem*/ )
	{
	//Create an empty landmark object
	CPosLandmark* lmk = CPosLandmark::NewL();
	CleanupStack::PushL(lmk);
	//Specify the various fields for landmark
	TLocality locality;
	locality.SetAccuracy(32.2,32.4);
	locality.SetCoordinate(56,25);
	locality.SetHorizontalAccuracy(33.1);
	locality.SetVerticalAccuracy(24.2);
	lmk->SetLandmarkNameL(_L("Forum Mall"));
	lmk->SetLandmarkDescriptionL(_L("Shopping Complex"));
	//Populate the landmark with the specified field information
	lmk->SetPositionL(locality);
	//lmk->SetLandmarkIdL( 1 );
	
	CPosLandmarkDatabase* ilDb = CPosLandmarkDatabase::OpenL();
	//CleanupStack::PushL(ilDb);
	 ilDb->AddLandmarkL(*lmk);
	CPosLmOperation* operation = NULL;
	if ( ilDb->IsInitializingNeeded() )
		{
		operation = ilDb->InitializeL();
		operation->ExecuteL();
		}
	
	CLmkEditorDlg::TLmkEditorParams editParams;
	editParams.iAttributes = CLmkEditorDlg::ELmkOnlyName;
	editParams.iEditorMode = CLmkEditorDlg::ELmkViewer;
	TPosLmItemId lmItem = lmk->LandmarkId();
	CLmkEditorDlg* editDlg = CLmkEditorDlg::NewL(*ilDb,lmItem,editParams);
	CleanupStack::PushL( editDlg );
	
	//ReleaseLandmarkResources();
	CleanupStack::PopAndDestroy(2); //ilDb, lmk
	//delete ilDb; ilDb=NULL;
	delete operation; operation=NULL;
	return KErrNone;
	}

// -----------------------------------------------------------------------------
// CUTAppLmkAddEdit::TestExecuteLD
// This function tests the launching of the dialog
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CUTAppLmkAddEdit::TestExecuteLD( CStifItemParser& aItem )
	{
	TInt result = KErrNone;
	TPosLmItemId lmItem = 0;
	TInt aCase = 0;

	CPosLandmarkDatabase* ilDb = CPosLandmarkDatabase::OpenL();
	CPosLmOperation* operation = NULL;

	if ( ilDb->IsInitializingNeeded() )
	{
		operation = ilDb->InitializeL();
		operation->ExecuteL();
	}


	CPosLandmark* landmark = CPosLandmark::NewL();
	CleanupStack::PushL( landmark );
	
	CLmkEditorDlg::TLmkEditorParams editParams;	
	aItem.GetNextInt(aCase);
	switch(aCase)
		{
		default:
		case 0:
			editParams.iAttributes = CLmkEditorDlg::ELmkAll;
			editParams.iEditorMode = CLmkEditorDlg::ELmkEditor;
			break;
			
		case 1:
			editParams.iAttributes = CLmkEditorDlg::ELmkAll;
			editParams.iEditorMode = CLmkEditorDlg::ELmkViewer;
			break;	
		}
	CLmkEditorDlg* editDlg = CLmkEditorDlg::NewL(*ilDb,*landmark,editParams);
	CleanupStack::PushL( editDlg );

	aItem.GetNextInt(aCase);

	switch(aCase)
		{
		default:
		case EDIT_MODE://Edit mode
			TRAPD(error, result=editDlg->ExecuteLD());
			if(error != 0 )
				{
				iLog->Log(_L("FAILED: ExecuteLD returns %d in Edit mode"), error);
				result = KErrGeneral;
				}
			else
				{
				iLog->Log(_L("PASSED: ExecuteLD returns %d in Edit mode"), result);
				result = KErrNone;
				}
			break;
			
		case VIEW_MODE: //ViewMode
			TRAPD(error1, result=editDlg->ExecuteLD());
			if(error1 != 0)
				{
				iLog->Log(_L("FAILED: ExecuteLD returns %d in View mode"), error1);	
				result = KErrGeneral;
				}
			else
				result = KErrNone;				
			break;
			
		case 2:
//			TestModuleIf().SetExitReason(CTestModuleIf::EPanic ,47);
//			DrainMemory();
//			TRAPD(error2, result=editDlg->ExecuteLD());
//			RestoreMemory();
//			if(result != 0)
//				iLog->Log(_L("FAILED: ExecuteLD returns %d in Edit mode under low memory condition"), result);
			break;				
		}
	
	//ReleaseLandmarkResources();
	CleanupStack::Pop(2);
		
	if(ilDb)
		{ 
		delete ilDb; 
		ilDb=NULL;
		}
	if(operation)
		{
		delete operation;
		operation=NULL;
		}
	return result;
	}
// -----------------------------------------------------------------------------
// CUTAppLmkAddEdit::TestSetMopParent
// This function tests setting up of the context
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//

 TInt CUTAppLmkAddEdit::TestSetMopParentL( CStifItemParser& /*aItem*/ )
  {
  	CPosLandmarkDatabase* ilDb = CPosLandmarkDatabase::OpenL();
	CPosLmOperation* operation = NULL;
	if ( ilDb->IsInitializingNeeded() )
		{
		operation = ilDb->InitializeL();
		operation->ExecuteL();
		}

	CPosLandmark* landmark = CPosLandmark::NewL();
	CleanupStack::PushL( landmark );

	CLmkEditorDlg::TLmkEditorParams editParams;
	editParams.iAttributes = CLmkEditorDlg::ELmkAll;
	editParams.iEditorMode = CLmkEditorDlg::ELmkEditor;

	engine = new (ELeave) CLmkAddEditEngine;

	CLmkEditorDlg* editDlg = CLmkEditorDlg::NewL(*ilDb,*landmark,editParams);
	CleanupStack::PushL( editDlg );
	
	TRAPD(result,editDlg->SetMopParent(engine));

	//ReleaseLandmarkResources();
	CleanupStack::Pop(2);

	delete ilDb; ilDb=NULL;
	delete operation; 
	operation=NULL;
	
	return KErrNone;
  }
 // -----------------------------------------------------------------------------
 // CUTAppLmkAddEdit::TestSetHelpContext
 // This function tests setting up of the help context
 // (other items were commented in a header).
 // -----------------------------------------------------------------------------
 //
 TInt CUTAppLmkAddEdit::TestSetHelpContextL( CStifItemParser& /*aItem*/ )
 {
	CPosLandmarkDatabase* ilDb = CPosLandmarkDatabase::OpenL();
	CPosLmOperation* operation = NULL;
	if ( ilDb->IsInitializingNeeded() )
		{
		operation = ilDb->InitializeL();
		operation->ExecuteL();
		}
	
	CPosLandmark* landmark = CPosLandmark::NewL();
	CleanupStack::PushL( landmark );
	
	CLmkEditorDlg::TLmkEditorParams editParams;
	editParams.iAttributes = CLmkEditorDlg::ELmkAll;
	editParams.iEditorMode = CLmkEditorDlg::ELmkEditor;
	
	TCoeHelpContext aContext;
	CLmkEditorDlg* editDlg = CLmkEditorDlg::NewL(*ilDb,*landmark,editParams);
	CleanupStack::PushL( editDlg );
	
	editDlg->SetHelpContext(aContext);
	
	//ReleaseLandmarkResources();
	CleanupStack::PopAndDestroy(2);
	
	delete ilDb; ilDb=NULL;
	delete operation; operation=NULL;
	return KErrNone;
 }
 // -----------------------------------------------------------------------------
 // CUTAppLmkAddEdit::TestDisableMapAndNaviOptions
 // This function tests disabling the Map and Navigation related Menu Options
 // (other items were commented in a header).
 // -----------------------------------------------------------------------------
 //
 TInt CUTAppLmkAddEdit::TestDisableMapAndNaviOptionsL( CStifItemParser& /*aItem*/ )
 {
	// #ifdef __SERIES60_31__
	CPosLandmarkDatabase* ilDb = CPosLandmarkDatabase::OpenL();
	CPosLmOperation* operation = NULL;
	if ( ilDb->IsInitializingNeeded() )
		{
		operation = ilDb->InitializeL();
		operation->ExecuteL();
		}
	
	CPosLandmark* landmark = CPosLandmark::NewL();
	CleanupStack::PushL( landmark );
	
	CLmkEditorDlg::TLmkEditorParams editParams;
	editParams.iAttributes = CLmkEditorDlg::ELmkAll;
	editParams.iEditorMode = CLmkEditorDlg::ELmkEditor;
	
	CLmkEditorDlg* editDlg = CLmkEditorDlg::NewL(*ilDb,*landmark,editParams);
	editDlg->DisableMapAndNavigationMenuOptions();
	TRAP_IGNORE(editDlg->ExecuteLD());
	CleanupStack::PopAndDestroy();
	//ReleaseLandmarkResources();
	delete ilDb;
	delete operation;
	//	#endif
	return KErrNone;
 }


// -----------------------------------------------------------------------------
// CUTAppLmkAddEdit::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CUTAppLmkAddEdit::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/
 // -----------------------------------------------------------------------------
 // CCSatelliteUITest::DrainMemory
 // Eats the memory
 // -----------------------------------------------------------------------------
 //
 void CUTAppLmkAddEdit::DrainMemory()
 	{
     for( TInt i = 0; i < MAX_EXPONENT1; i++ )
     {
         cells[i] = NULL;
     }
     // consume all available heap memory
     TInt j=0;
    	for (TInt i = KMaxTInt/2-1; i > 0; )
 	{
 		cells[j] = User::Alloc(i);
 		if( !( cells[j] ) )
 		{
 		    i/=2;
 		}
 		else
 		{
 		    j++;
 		}
 	}
 	}
 // -----------------------------------------------------------------------------
 // CCSatelliteUITest::RestoreMemory
 // Restores the memory
 // -----------------------------------------------------------------------------
 //
 void CUTAppLmkAddEdit::RestoreMemory()
 	{
  	for ( TInt cellIndex = 0; cellIndex < MAX_EXPONENT1; cellIndex++ )
 		{
 			 if ( cells[cellIndex] )
 				{
 				   User::Free( cells[cellIndex] );
 			   }
 		 }
 	}
 // -----------------------------------------------------------------------------

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
