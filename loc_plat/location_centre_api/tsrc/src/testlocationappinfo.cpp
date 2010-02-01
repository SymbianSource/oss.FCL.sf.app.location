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
#include "testlocationappinfo.h"


// Constant Declarations

    
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ Default Constructor
// ---------------------------------------------------------------------------
//
CTestLocationAppInfo::CTestLocationAppInfo(CStifLogger* aLog)
    : iLcLocationAppInfo(NULL),iLog(aLog)
    {

    }

// ---------------------------------------------------------------------------
// C++ Destructor
// ---------------------------------------------------------------------------
//
CTestLocationAppInfo::~CTestLocationAppInfo()
    {

    }

// ---------------------------------------------------------
// CTestLocationAppInfo::CheckNewL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppInfo::CheckNewL()
	{
	TInt err = KErrNone;

	TRAP(err, iLcLocationAppInfo = CLcLocationAppInfo::NewL());

	iLog->Log(_L("CheckNewL return with %d"), err);

	delete iLcLocationAppInfo;
	return err;
	}

// ---------------------------------------------------------
// CTestLocationAppInfo::CheckNewLC
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppInfo::CheckNewLC()
	{
	TInt err = KErrNone;

	TRAP(err, iLcLocationAppInfo = CLcLocationAppInfo::NewLC();
		CleanupStack::Pop(iLcLocationAppInfo));

	iLog->Log(_L("CheckNewLC return with %d"), err);

	delete iLcLocationAppInfo;
	return err;
	}

// ---------------------------------------------------------
// CTestLocationAppInfo::CheckCopyNewL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppInfo::CheckCopyNewL()
	{
	TInt err = KErrNone;
	
	CLcLocationAppInfo* lcLocationAppInfo = CLcLocationAppInfo::NewL();
	
	TRAP(err, iLcLocationAppInfo = CLcLocationAppInfo::NewL(*lcLocationAppInfo));

	iLog->Log(_L("CheckCopyNewL return with %d"), err);

	delete lcLocationAppInfo;
	delete iLcLocationAppInfo;
	return err;
	}

// ---------------------------------------------------------
// CTestLocationAppInfo::CheckCopyNewLC
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppInfo::CheckCopyNewLC()
	{
	TInt err = KErrNone;

	CLcLocationAppInfo* lcLocationAppInfo = CLcLocationAppInfo::NewLC();

	TRAP(err, iLcLocationAppInfo = CLcLocationAppInfo::NewLC(*lcLocationAppInfo);
		CleanupStack::Pop(iLcLocationAppInfo));

	iLog->Log(_L("CheckCopyNewLC return with %d"), err);

	CleanupStack::PopAndDestroy(lcLocationAppInfo);

	delete iLcLocationAppInfo;
	return err;
	}

// ---------------------------------------------------------
// CTestLocationAppInfo::CheckId
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppInfo::CheckId()
	{
	TInt err = KErrNone;

	iLcLocationAppInfo = CLcLocationAppInfo::NewL();

	if(0 != (iLcLocationAppInfo->Id()).Compare(_L("")) )
		err = KErrGeneral;

	iLog->Log(_L("CheckId return with %d"), err);

	delete iLcLocationAppInfo;
	return err;
	}

// ---------------------------------------------------------
// CTestLocationAppInfo::CheckName
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppInfo::CheckName()
	{
	TInt err = KErrNone;

	iLcLocationAppInfo = CLcLocationAppInfo::NewL();
	
	if(0 != (iLcLocationAppInfo->Name()).Compare(_L("")) )
		err = KErrGeneral;

	iLog->Log(_L("CheckName return with %d"), err);

	delete iLcLocationAppInfo;
	return err;
	}
	
// ---------------------------------------------------------
// CTestLocationAppInfo::CheckLaunchMode
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppInfo::CheckLaunchMode()
	{
	TInt err = KErrNone;

	iLcLocationAppInfo = CLcLocationAppInfo::NewL();
	
	if(CLcLocationAppInfo::EDefaultMode != iLcLocationAppInfo->LaunchMode())
		err = KErrGeneral;

	iLog->Log(_L("LaunchMode return with %d"), err);

	delete iLcLocationAppInfo;
	return err;
	}

// ---------------------------------------------------------
// CTestLocationAppInfo::CheckSystemCharacteristics
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppInfo::CheckSystemCharacteristics()
	{
	TInt err = KErrNone;

	iLcLocationAppInfo = CLcLocationAppInfo::NewL();
	
	if(CLcLocationAppInfo::ESysCharNone != iLcLocationAppInfo->SystemCharacteristics())
		err = KErrGeneral;

	iLog->Log(_L("CheckSystemCharacteristics return with %d"), err);

	delete iLcLocationAppInfo;
	return err;
	}

// ---------------------------------------------------------
// CTestLocationAppInfo::CheckApplicationCharacteristics
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppInfo::CheckApplicationCharacteristics()
	{
	TInt err = KErrNone;

	iLcLocationAppInfo = CLcLocationAppInfo::NewL();
	
	if(CLcLocationAppInfo::EAppCharNone != iLcLocationAppInfo->ApplicationCharacteristics())
		err = KErrGeneral;

	iLog->Log(_L("CheckApplicationCharacteristics return with %d"), err);

	delete iLcLocationAppInfo;
	return err;
	}
