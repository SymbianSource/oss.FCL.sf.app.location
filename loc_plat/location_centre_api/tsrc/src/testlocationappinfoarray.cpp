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
* Description:  Test Class which has the list of test cases for CLcLocationAppInfo
*
*/



// System Includes
#include <lclocationappinfo.h>

// User Includes
#include "testlocationappinfoarray.h"


// Constant Declarations

    
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ Default Constructor
// ---------------------------------------------------------------------------
//
CTestLocationAppInfoArray::CTestLocationAppInfoArray(CStifLogger* aLog)
    : iLcLocationAppInfoArray(NULL),iLog(aLog)
    {

    }

// ---------------------------------------------------------------------------
// C++ Destructor
// ---------------------------------------------------------------------------
//
CTestLocationAppInfoArray::~CTestLocationAppInfoArray()
    {
    }

// ---------------------------------------------------------
// CTestLocationAppInfoArray::CheckArrayNewL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppInfoArray::CheckArrayNewL( )
	{
	TInt err = KErrNone;

	TRAP(err, iLcLocationAppInfoArray = CLcLocationAppInfoArray::NewL());

	iLog->Log(_L("CheckArrayNewL return with %d"), err);

	delete iLcLocationAppInfoArray;
	return err;
	}

// ---------------------------------------------------------
// CTestLocationAppInfoArray::CheckArrayNewLC
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppInfoArray::CheckArrayNewLC( )
	{
	TInt err = KErrNone;

	TRAP(err, iLcLocationAppInfoArray = CLcLocationAppInfoArray::NewLC();
		CleanupStack::Pop(iLcLocationAppInfoArray));

	iLog->Log(_L("CheckArrayNewLC return with %d"), err);

	delete iLcLocationAppInfoArray;
	return err;
	}
	
// ---------------------------------------------------------
// CTestLocationAppInfoArray::CheckCount
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppInfoArray::CheckCount( )
	{
	TInt err = KErrNone;

	iLcLocationAppInfoArray = CLcLocationAppInfoArray::NewL();

	if( 0 != iLcLocationAppInfoArray->Count() )
		err = KErrGeneral;

	iLog->Log(_L("CheckCount return with %d"), err);

	delete iLcLocationAppInfoArray;
	return err;
	}	

// ---------------------------------------------------------
// CTestLocationAppInfoArray::CheckAppendL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppInfoArray::CheckAppendL( )
	{
	TInt err = KErrNone;

	iLcLocationAppInfoArray = CLcLocationAppInfoArray::NewL();
	CLcLocationAppInfo* lcLocationAppInfo = CLcLocationAppInfo::NewL();
	
	iLcLocationAppInfoArray->AppendL(lcLocationAppInfo);

	if( 1 != iLcLocationAppInfoArray->Count() )
		err = KErrGeneral;

	iLog->Log(_L("CheckCount return with %d"), err);

	iLcLocationAppInfoArray->Reset();
	delete iLcLocationAppInfoArray;
	return err;
	}

// ---------------------------------------------------------
// CTestLocationAppInfoArray::CheckRemoveL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppInfoArray::CheckRemoveL( )
	{
	TInt err = KErrNone;

	iLcLocationAppInfoArray = CLcLocationAppInfoArray::NewL();
	CLcLocationAppInfo* lcLocationAppInfo = CLcLocationAppInfo::NewL();
	
	iLcLocationAppInfoArray->AppendL(lcLocationAppInfo);

	if( NULL == iLcLocationAppInfoArray->Remove( 0 ) )
		err = KErrGeneral;

	iLog->Log(_L("CheckCount return with %d"), err);

	delete lcLocationAppInfo;
	delete iLcLocationAppInfoArray;
	return err;
	}

// ---------------------------------------------------------
// CTestLocationAppInfoArray::CheckResetL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppInfoArray::CheckResetL( )
	{
	TInt err = KErrNone;

	iLcLocationAppInfoArray = CLcLocationAppInfoArray::NewL();
	CLcLocationAppInfo* lcLocationAppInfo = CLcLocationAppInfo::NewL();
	
	iLcLocationAppInfoArray->AppendL(lcLocationAppInfo);

	if( 1 != iLcLocationAppInfoArray->Count() )
		err = KErrGeneral;

	iLog->Log(_L("CheckCount return with %d"), err);

	iLcLocationAppInfoArray->Reset();
	delete iLcLocationAppInfoArray;
	return err;
	}

// ---------------------------------------------------------
// CTestLocationAppInfoArray::CheckOperatorL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppInfoArray::CheckOperatorL( )
	{
	TInt err = KErrNone;

	iLcLocationAppInfoArray = CLcLocationAppInfoArray::NewL();
	CLcLocationAppInfo* lcLocationAppInfo = CLcLocationAppInfo::NewL();
	
	iLcLocationAppInfoArray->AppendL(lcLocationAppInfo);

	CLcLocationAppInfo& lcLocationAppInfo1 = (*iLcLocationAppInfoArray)[0];

	iLog->Log(_L("CheckCount return with %d"), err);

	iLcLocationAppInfoArray->Reset();
	delete iLcLocationAppInfoArray;
	return err;
	}

