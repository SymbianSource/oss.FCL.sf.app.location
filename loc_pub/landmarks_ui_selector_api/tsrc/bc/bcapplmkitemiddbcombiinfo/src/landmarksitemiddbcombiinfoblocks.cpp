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
#include "LandmarksItemIdDbCombiInfo.h"

#include <tlmkitemiddbcombiinfo.h>
#include <CLmkLandmarkSelectorDlg.h>
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
// CLandmarksItemIdDbCombiInfo::Delete
// Delete here all resources allocated and opened from test methods.
// Called from destructor.
// -----------------------------------------------------------------------------
//
void CLandmarksItemIdDbCombiInfo::Delete()
    {

    }

// -----------------------------------------------------------------------------
// CLandmarksItemIdDbCombiInfo::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CLandmarksItemIdDbCombiInfo::RunMethodL(
    CStifItemParser& aItem )
    {

    static TStifFunctionInfo const KFunctions[] =
        {
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function.
        ENTRY( "Example", CLandmarksItemIdDbCombiInfo::ExampleL ),
        ENTRY( "TestItemIdDbCombiInfo", CLandmarksItemIdDbCombiInfo::TestItemIdDbCombiInfo ),
        ENTRY( "TestGetItemId", CLandmarksItemIdDbCombiInfo::TestGetItemId ),
        ENTRY( "TestGetLmDb", CLandmarksItemIdDbCombiInfo::TestGetLmDb ),
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) /
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CLandmarksItemIdDbCombiInfo::ExampleL
// Example test method function.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CLandmarksItemIdDbCombiInfo::ExampleL( CStifItemParser& aItem )
    {

    // Print to UI
    _LIT( KLandmarksItemIdDbCombiInfo, "LandmarksItemIdDbCombiInfo" );
    _LIT( KExample, "In Example" );
    TestModuleIf().Printf( 0, KLandmarksItemIdDbCombiInfo, KExample );
    // Print to log file
    iLog->Log( KExample );

    TInt i = 0;
    TPtrC string;
    _LIT( KParam, "Param[%i]: %S" );
    while ( aItem.GetNextString ( string ) == KErrNone )
        {
        TestModuleIf().Printf( i, KLandmarksItemIdDbCombiInfo,
                                KParam, i, &string );
        i++;
        }

    return KErrNone;

    }

TInt CLandmarksItemIdDbCombiInfo::TestItemIdDbCombiInfo( CStifItemParser& aItem )
{
	 	TLmkItemIdDbCombiInfo selItem = TLmkItemIdDbCombiInfo();
	 	return KErrNone;
}

TInt CLandmarksItemIdDbCombiInfo::TestGetItemId( CStifItemParser& aItem )
{
			TLmkItemIdDbCombiInfo selItem;
			TPosLmItemId lmItem;

			CLmkLandmarkSelectorDlg* iLmkSelectDlg = CLmkLandmarkSelectorDlg::NewL();
    		TRAPD( result, iLmkSelectDlg->ExecuteLD(selItem));

      	    lmItem = selItem.GetItemId();
      	    return KErrNone;
 }

 TInt CLandmarksItemIdDbCombiInfo::TestGetLmDb( CStifItemParser& aItem )
 {

			TLmkItemIdDbCombiInfo selItem;

			CLmkLandmarkSelectorDlg* iLmkSelectDlg = CLmkLandmarkSelectorDlg::NewL();
    		TRAPD( result, iLmkSelectDlg->ExecuteLD(selItem));

    	    CPosLandmarkDatabase* ilDb = selItem.GetLmDb();
    	    return KErrNone;
    	    }
// -----------------------------------------------------------------------------
// CLandmarksItemIdDbCombiInfo::?member_function
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
/*
TInt CLandmarksItemIdDbCombiInfo::?member_function(
   CItemParser& aItem )
   {

   ?code

   }
*/

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
