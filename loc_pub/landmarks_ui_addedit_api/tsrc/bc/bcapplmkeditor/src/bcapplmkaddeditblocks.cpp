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
#include "BCAppLmkAddEdit.h"

#include <CLmkEditorDlg.h>
#include <Epos_CPosLandmarkDatabase.h>
#include <Epos_Landmarks.h>
#include <coecntrl.h>
#include <lbsposition.h>
#include "BCAppLmkEditorEngine.h"

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
// CBCAppLmkAddEdit::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CBCAppLmkAddEdit::Delete()
    {

    }

// -----------------------------------------------------------------------------
// CBCAppLmkAddEdit::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CBCAppLmkAddEdit::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "Example", CBCAppLmkAddEdit::ExampleL ),
        ENTRY( "TestNewL", CBCAppLmkAddEdit::TestNewL ),
        ENTRY( "TestNew2L", CBCAppLmkAddEdit::TestNew2L ),
        ENTRY( "TestExecuteLD", CBCAppLmkAddEdit::TestExecuteLD ),
         ENTRY( "TestSetMopParent", CBCAppLmkAddEdit::TestSetMopParent ),
          ENTRY( "TestSetHelpContext", CBCAppLmkAddEdit::TestSetHelpContext ),
          ENTRY( "TestDisableMapAndNaviOptions", CBCAppLmkAddEdit::TestDisableMapAndNaviOptions ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CBCAppLmkAddEdit::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CBCAppLmkAddEdit::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KBCAppLmkAddEdit, "BCAppLmkAddEdit" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KBCAppLmkAddEdit, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KBCAppLmkAddEdit,
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }

TInt CBCAppLmkAddEdit::TestNewL( CStifItemParser& /*aItem*/ )
{
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

			CLmkEditorDlg* editDlg = CLmkEditorDlg::NewL(*ilDb,*LmkEditor,editParams);
			CleanupStack::PushL( editDlg );
			//ReleaseLandmarkResources();
			CleanupStack::PopAndDestroy(2);
			delete ilDb; ilDb=NULL;
			delete operation; operation=NULL;
			return KErrNone;
}

TInt CBCAppLmkAddEdit::TestNew2L( CStifItemParser& /*aItem*/ )
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
			editParams.iAttributes = CLmkEditorDlg::ELmkAll;
			editParams.iEditorMode = CLmkEditorDlg::ELmkEditor;
			TPosLmItemId lmItem = lmk->LandmarkId();
			CLmkEditorDlg* editDlg = CLmkEditorDlg::NewL(*ilDb,lmItem,editParams);
			CleanupStack::PushL( editDlg );

			//ReleaseLandmarkResources();
			CleanupStack::PopAndDestroy(2); //ilDb, lmk
			//delete ilDb; ilDb=NULL;
			delete operation; operation=NULL;
			return KErrNone;
		}

TInt CBCAppLmkAddEdit::TestExecuteLD( CStifItemParser& /*aItem*/ )
	{
			TPosLmItemId lmItem = 0;

			CPosLandmarkDatabase* ilDb = CPosLandmarkDatabase::OpenL();
			CPosLmOperation* operation = NULL;

		  if ( ilDb->IsInitializingNeeded() )
		     {
		        operation = ilDb->InitializeL();
		        operation->ExecuteL();
		     }

			CLmkEditorDlg::TLmkEditorParams editParams;
			editParams.iAttributes = CLmkEditorDlg::ELmkAll;
			editParams.iEditorMode = CLmkEditorDlg::ELmkEditor;

			CPosLandmark* landmark = CPosLandmark::NewL();
			CleanupStack::PushL( landmark );

			CLmkEditorDlg* editDlg = CLmkEditorDlg::NewL(*ilDb,*landmark,editParams);
			CleanupStack::PushL( editDlg );
        TRAPD(result,editDlg->ExecuteLD());

           	//ReleaseLandmarkResources();
           	CleanupStack::Pop(2);
           	delete ilDb; ilDb=NULL;
						delete operation; operation=NULL;
			return KErrNone;

  }

 TInt CBCAppLmkAddEdit::TestSetMopParent( CStifItemParser& /*aItem*/ )
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
			delete operation; operation=NULL;


			return KErrNone;
 }

 TInt CBCAppLmkAddEdit::TestSetHelpContext( CStifItemParser& /*aItem*/ )
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


 TInt CBCAppLmkAddEdit::TestDisableMapAndNaviOptions( CStifItemParser& /*aItem*/ )
 {
      #ifdef __SERIES60_31__
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
			#endif
			return KErrNone;
            }


// -----------------------------------------------------------------------------
// CBCAppLmkAddEdit::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CBCAppLmkAddEdit::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
