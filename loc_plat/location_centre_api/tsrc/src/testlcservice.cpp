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
* Description:  Test Class which has the list of test cases for CLcService
*
*/


// System Includes
#include <lcservice.h>

// User Includes
#include "testlcservice.h"


// Constant Declarations

    
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ Default Constructor
// ---------------------------------------------------------------------------
//
CTestLcService::CTestLcService(CStifLogger* aLog)
    :CActive(0),iLog(aLog)
    {
    CActiveScheduler::Add(this);
    }
    
// ---------------------------------------------------------------------------
// C++ Destructor
// ---------------------------------------------------------------------------
//
CTestLcService::~CTestLcService()
    {
      if( IsActive() )
	      {
	      Cancel(); 	
	      }
    }

// ---------------------------------------------------------
// CTestLcService::RunL 
//
// RunL to Handle Completed Request
// ---------------------------------------------------------
//	
void CTestLcService::RunL()
	{
	if(iStatus != KErrCancel)	
		{
		iLog->Log(_L("RunL %d"),iStatus.Int());		
		CActiveScheduler::Stop();				
		}
	}
	
// ---------------------------------------------------------
// CTestLcService::DoCancel 
//
// To Handle Cancel Request
// ---------------------------------------------------------
//	
void CTestLcService::DoCancel()
	{
	iLcService->CancelGetLocationApplications();
	iLog->Log(_L("CancelL Done"));

	}

// ---------------------------------------------------------
// CTestLcService::RunError 
//
// To Handle Error in RunL
// ---------------------------------------------------------
//
TInt CTestLcService::RunError(TInt aError)
	{
	iLog->Log(_L("Leave in RunError %d"),aError);
	CActiveScheduler::Stop();
	return KErrNone;
	}

// ---------------------------------------------------------
// CTestLcService::LcStatusChangedL 
//
// ---------------------------------------------------------
//
void  CTestLcService::LcStatusChangedL( TInt /*aErrorCode*/ )
	{
	// No implementation
	}

// ---------------------------------------------------------
// CTestLcService::HandleChainedLocationAppExitL 
//
// ---------------------------------------------------------
//
void  CTestLcService::HandleChainedLocationAppExitL( TInt aReason )
	{
	iLog->Log(_L("In HandleChainedLocationAppExitL returned with : %d"), aReason);
	}

// ---------------------------------------------------------
// CTestLcService::CheckServiceNewL
//
// (other items are commented in a header).
// ---------------------------------------------------------
//
TInt CTestLcService::CheckServiceNewL()
	{
	TInt err = KErrNone;
	TRAP(err, iLcService = CLcService::NewL());
	iLog->Log(_L("CheckServiceNewL return - %d"),err);
	delete iLcService;
	return err;
	}	

// ---------------------------------------------------------
// CTestLcService::CheckServiceNewLC
//
// (other items are commented in a header).
// ---------------------------------------------------------
//
TInt CTestLcService::CheckServiceNewLC()
	{
	TInt err = KErrNone;
	TRAP(err, iLcService = CLcService::NewLC();
		CleanupStack::Pop(iLcService));
	iLog->Log(_L("CheckServiceNewLC return - %d"),err);
	delete iLcService;
	return err;
	}	  

// ---------------------------------------------------------
// CTestLcService::TestGetLocationApplicationsL
//
// (other items are commented in a header).
// ---------------------------------------------------------
//
TInt CTestLcService::TestGetLocationApplicationsL()
	{
	TInt err = KErrNone;
	iLcService = CLcService::NewL();
	CLcLocationAppInfoArray* array = iLcService->GetLocationApplicationsL( );
	TInt countItems = array->Count( );
	CLcLocationAppInfo& info1 =(*array)[countItems - 1];

	TPtrC name1 = info1.Name( );
	TPtrC id1 = info1.Id( );
		
	if( 0 == name1.Compare(_L("Positioning settings")) &&
		0 == id1.Compare(_L("1028313B")) &&
		CLcLocationAppInfo::EChainedMode == info1.LaunchMode() &&
		CLcLocationAppInfo::ESysCharRomBased == info1.SystemCharacteristics() &&
		CLcLocationAppInfo::EAppCharSettings == info1.ApplicationCharacteristics() )
		{
		iLog->Log(_L("TestGetLocationApplicationsL Passed"));
		}
	else
		{
		err = KErrGeneral;
		iLog->Log(_L("TestGetLocationApplicationsL Failed"));
		}
	
	array->Remove(countItems - 1);
	array->Reset();
	
    delete array;
	delete iLcService;
	return err;
	}	

// ---------------------------------------------------------
// CTestLcService::TestGetLocationApplicationsAsynL
//
// (other items are commented in a header).
// ---------------------------------------------------------
//
TInt CTestLcService::TestGetLocationApplicationsAsynL()
	{
	TInt err = KErrNone;
	iLcService = CLcService::NewL();
	CLcLocationAppInfoArray* array;
	iLcService->GetLocationApplications( iStatus, array );
	SetActive();
	CActiveScheduler::Start();

	TInt countItems = array->Count( );
	CLcLocationAppInfo& info1 =(*array)[countItems - 1];

	TPtrC name1 = info1.Name( );
	TPtrC id1 = info1.Id( );
		
	if( 0 == name1.Compare(_L("Positioning settings")) &&
		0 == id1.Compare(_L("1028313B")) &&
		CLcLocationAppInfo::EChainedMode == info1.LaunchMode() &&
		CLcLocationAppInfo::ESysCharRomBased == info1.SystemCharacteristics() &&
		CLcLocationAppInfo::EAppCharSettings == info1.ApplicationCharacteristics() )
		{
		iLog->Log(_L("TestGetLocationApplicationsL Passed"));
		}
	else
		{
		err = KErrGeneral;
		iLog->Log(_L("TestGetLocationApplicationsL Failed"));
		}
	
    delete array;
	delete iLcService;
	return err;
	}

// ---------------------------------------------------------
// CTestLcService::TestGetLocationApplicationsFilterL
//
// (other items are commented in a header).
// ---------------------------------------------------------
//
TInt CTestLcService::TestGetLocationApplicationsFilterL()
	{
	TInt err = KErrNone;
	iLcService = CLcService::NewL();
	TLcLocationAppFilter filter;
    filter.SetApplicationCharacteristics( CLcLocationAppInfo::EAppCharSettings );
    filter.SetSystemCharacteristic( CLcLocationAppInfo::ESysCharRomBased  );
    filter.SetAppCharFilterConfiguration( TLcLocationAppFilter::EFilterStrict );
    filter.SetSysCharFilterConfiguration( TLcLocationAppFilter::EFilterStrict );
        
    CLcLocationAppInfoArray* array=iLcService->GetLocationApplicationsL(filter);
    
	TInt countItems = array->Count( );
	CLcLocationAppInfo& info1 =(*array)[countItems - 1];

	TPtrC name1 = info1.Name( );
	TPtrC id1 = info1.Id( );
		
	if( 0 == name1.Compare(_L("Positioning settings")) &&
		0 == id1.Compare(_L("1028313B")) )
		{
		iLog->Log(_L("TestGetLocationApplicationsFilterL Passed"));
		}
	else
		{
		err = KErrGeneral;
		iLog->Log(_L("TestGetLocationApplicationsFilterL Failed"));
		}
	
    delete array;
	delete iLcService;
	return err;
	}	

// ---------------------------------------------------------
// CTestLcService::TestGetLocationApplicationsFilterAsynL
//
// (other items are commented in a header).
// ---------------------------------------------------------
//
TInt CTestLcService::TestGetLocationApplicationsFilterAsynL()
	{
	TInt err = KErrNone;
	iLcService = CLcService::NewL();
	TLcLocationAppFilter filter;
    filter.SetApplicationCharacteristics( CLcLocationAppInfo::EAppCharSettings );
    filter.SetSystemCharacteristic( CLcLocationAppInfo::ESysCharRomBased  );
    filter.SetAppCharFilterConfiguration( TLcLocationAppFilter::EFilterStrict );
    filter.SetSysCharFilterConfiguration( TLcLocationAppFilter::EFilterStrict );
        
    CLcLocationAppInfoArray* array;
    iLcService->GetLocationApplications(iStatus, filter, array);

	SetActive();
	CActiveScheduler::Start();

	TInt countItems = array->Count( );
	CLcLocationAppInfo& info1 =(*array)[countItems - 1];

	TPtrC name1 = info1.Name( );
	TPtrC id1 = info1.Id( );
		
		iLog->Log(_L("countItems = %d"), countItems);
		
	if( 0 == name1.Compare(_L("Positioning settings")) &&
		0 == id1.Compare(_L("1028313B")) )
		{
		iLog->Log(_L("TestGetLocationApplicationsFilterL Passed"));
		}
	else
		{
		err = KErrGeneral;
		iLog->Log(_L("TestGetLocationApplicationsFilterL Failed"));
		}
	
    delete array;
	delete iLcService;
	return err;
	}

// ---------------------------------------------------------
// CTestLcService::TestSetRemoveObserverL
//
// (other items are commented in a header).
// ---------------------------------------------------------
//
TInt CTestLcService::TestSetRemoveObserverL(TInt aObserverState)
	{
	TInt err = KErrNone;
	iLcService = CLcService::NewL();

	if( 1 == aObserverState )
		iLcService->SetObserverL(*this);
	
	err = iLcService->RemoveObserver();

	if( KErrNone == err )
		{
		iLog->Log(_L("TestSetRemoveObserverL Passed with %d"), err);
		}
	else
		{
		iLog->Log(_L("TestSetRemoveObserverL Failed with %d"), err);
		}

	delete iLcService;
	return err;
	}

// ---------------------------------------------------------
// CTestLcService::TestCancelGetLocationApplications
//
// (other items are commented in a header).
// ---------------------------------------------------------
//
TInt CTestLcService::TestCancelGetLocationApplications()
	{
	iLcService = CLcService::NewL();
	CLcLocationAppInfoArray* array;
	iLcService->GetLocationApplications( iStatus, array );
	SetActive();
	
	Cancel();
	
    iLog->Log(_L("In TestCancelGetLocationApplications: returned with : %d"), iStatus.Int());

	delete iLcService;
	return iStatus.Int();
	}

// ---------------------------------------------------------
// CTestLcService::TestCLcLaunchParamNewL
//
// (other items are commented in a header).
// ---------------------------------------------------------
//
TInt CTestLcService::TestCLcLaunchParamNewL()
	{
	TInt err = KErrNone;
	CLcService::CLcLaunchParam* lcLaunchParam;
	TBuf<128> buf;
	
	TRAP(err, lcLaunchParam = CLcService::CLcLaunchParam::NewL(buf));

	iLog->Log(_L("CheckNewL return with %d"), err);

	delete lcLaunchParam;
	return err;
	}
	
// ---------------------------------------------------------
// CTestLcService::TestCLcLaunchParamNewLC
//
// (other items are commented in a header).
// ---------------------------------------------------------
//
TInt CTestLcService::TestCLcLaunchParamNewLC()
	{
	TInt err = KErrNone;
	CLcService::CLcLaunchParam* lcLaunchParam;
	TBuf<128> buf;
	
	TRAP(err, lcLaunchParam = CLcService::CLcLaunchParam::NewLC(buf);
		CleanupStack::Pop(lcLaunchParam));

	iLog->Log(_L("CheckNewLC return with %d"), err);

	delete lcLaunchParam;
	return err;
	}

