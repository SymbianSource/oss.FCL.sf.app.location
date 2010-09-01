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
* Description:    LandmarksUi Content File -  LandmarkUi selector Test class for STIF Test Framework TestScripter
*
*/








#ifndef CTESTLMUISELECTOR_H
#define CTESTLMUISELECTOR_H

//  INCLUDES
#include <StifLogger.h>
#include <TestScripterInternal.h>
#include <StifTestModule.h>
#include <clmklandmarkselectordlg.h>
#include <clmkcategoryselectordlg.h>
#include <tlmkitemiddbcombiinfo.h>

#include "testLmkEngine.h"


// CONSTANTS
_LIT( KUserDatabaseUri, "file://c:StifTestDbLoc.ldb" );

//const ?type ?constant_var = ?constant;

// MACROS
//#define ?macro ?macro_def
// Logging path
_LIT( KTestLmUiSelectorLogPath, "\\logs\\testframework\\testlmuiselector\\" );
// Log file
_LIT( KTestLmUiSelectorLogFile, "testlmuiselector.txt" );

// FUNCTION PROTOTYPES
//?type ?function_name(?arg_list);

// FORWARD DECLARATIONS
//class ?FORWARD_CLASSNAME;
class CTestLmUiSelector;

// DATA TYPES
//enum ?declaration
//typedef ?declaration
//extern ?data_type;

// CLASS DECLARATION

/**
*  CTestLmUiSelector test class for STIF Test Framework TestScripter.
*  ?other_description_lines
*
*  @lib ?library
*  @since ?Series60_version
*/
NONSHARABLE_CLASS(CTestLmUiSelector) : public CScriptBase
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CTestLmUiSelector* NewL( CTestModuleIf& aTestModuleIf );

        /**
        * Destructor.
        */
        virtual ~CTestLmUiSelector();

    public: // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    public: // Functions from base classes

        /**
        * From CScriptBase Runs a script line.
        * @since ?Series60_version
        * @param aItem Script line containing method name and parameters
        * @return Symbian OS error code
        */
        virtual TInt RunMethodL( CStifItemParser& aItem );

    protected:  // New functions

        /**
        * ?member_description.
        * @since ?Series60_version
        * @param ?arg1 ?description
        * @return ?description
        */
        //?type ?member_function( ?type ?arg1 );

    protected:  // Functions from base classes

        /**
        * From ?base_class ?member_description
        */
        //?type ?member_function();

    private:

        /**
        * C++ default constructor.
        */
        CTestLmUiSelector( CTestModuleIf& aTestModuleIf );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

        // Prohibit copy constructor if not deriving from CBase.
        // ?classname( const ?classname& );
        // Prohibit assigment operator if not deriving from CBase.
        // ?classname& operator=( const ?classname& );

        /**
        * Frees all resources allocated from test methods.
        * @since ?Series60_version
        */
        void Delete();

        /**
        * Test methods are listed below.
        */

        //ADD NEW METHOD DEC HERE

        /**
        * Set & get the item id.
        */
        virtual TInt GetItemIdL( CStifItemParser& aItem );

        /**
        * Set & get the Landmarks Db handle.
        */

        virtual TInt GetLmDbL( CStifItemParser& aItem );

        /**
        * Set & get the item id.
        */
        virtual TInt SetGetItemIdL( CStifItemParser& aItem );

        /**
        * Set & get the Landmarks Db handle.
        */

        virtual TInt SetGetLmDbL( CStifItemParser& aItem );
        
        virtual TInt TestLmkNewL( CStifItemParser& aItem );
        virtual TInt TestLmkSetMopParent( CStifItemParser& aItem );
        virtual TInt TestLmkExecuteLD( CStifItemParser& aItem );
        virtual TInt TestLmkExecuteLD2( CStifItemParser& aItem );
        virtual TInt TestCatNewL( CStifItemParser& aItem );
        virtual TInt TestCatSetMopParent( CStifItemParser& aItem );
        virtual TInt TestCatExecuteLD( CStifItemParser& aItem );
        virtual TInt TestCatExecuteLD2( CStifItemParser& aItem );
        virtual TInt TestLmkNewLWithUserDatabase( CStifItemParser& aItem );
        virtual TInt TestSetDialogTitle( CStifItemParser& aItem );
        virtual TInt TestSetDialogTitle1( CStifItemParser& aItem );
       
        void CreateNewLandmark();
        void CreateOwnLandMarks();


    public:     // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    protected:  // Data
        // ?one_line_short_description_of_data
        //?data_declaration;

    private:    // Data
    	CTestLmkEngine* engine;

        // ?one_line_short_description_of_data
        //?data_declaration;

        // Reserved pointer for future extension
        //TAny* iReserved;
    public:     // Friend classes
        //?friend_class_declaration;
    protected:  // Friend classes
        //?friend_class_declaration;
    private:    // Friend classes
        //?friend_class_declaration;

    };

#endif      // CTESTLMUISELECTOR_H

// End of File
