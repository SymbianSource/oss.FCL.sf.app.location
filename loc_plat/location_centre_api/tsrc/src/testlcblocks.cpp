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
* Description:  Test Class which has the list of test cases
*
*/



// INCLUDE FILES
#include <e32svr.h>
#include <StifParser.h>
#include <Stiftestinterface.h>

#include "testlc.h"
#include "testlcservice.h"
#include "testlocationappinfo.h"
#include "testlocationappfilter.h"
#include "testlocationappinfoarray.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CTestLc::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CTestLc::Delete() 
    {

    }

// -----------------------------------------------------------------------------
// CTestLc::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CTestLc::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        ENTRY( "CheckServiceNewL", CTestLc::CheckServiceNewL ),
        ENTRY( "CheckServiceNewLC", CTestLc::CheckServiceNewLC ),
        ENTRY( "TestGetLocationApplicationsL", CTestLc::TestGetLocationApplicationsL ),
        ENTRY( "TestGetLocationApplicationsAsynL", CTestLc::TestGetLocationApplicationsAsynL ),
        ENTRY( "TestGetLocationApplicationsFilterL", CTestLc::TestGetLocationApplicationsFilterL ),
        ENTRY( "TestGetLocationApplicationsFilterAsynL", CTestLc::TestGetLocationApplicationsFilterAsynL ),
        ENTRY( "TestSetRemoveObserverL", CTestLc::TestSetRemoveObserverL ),
        ENTRY( "TestCancelGetLocationApplications", CTestLc::TestCancelGetLocationApplications ),
        ENTRY( "TestCLcLaunchParamNewL", CTestLc::TestCLcLaunchParamNewL ),
        ENTRY( "TestCLcLaunchParamNewLC", CTestLc::TestCLcLaunchParamNewLC ),
        
        ENTRY( "CheckNewL", CTestLc::CheckNewL ),
        ENTRY( "CheckNewLC", CTestLc::CheckNewLC ),
        ENTRY( "CheckCopyNewL", CTestLc::CheckCopyNewL ),
        ENTRY( "CheckCopyNewLC", CTestLc::CheckCopyNewLC ),
        ENTRY( "CheckId", CTestLc::CheckId ),
        ENTRY( "CheckName", CTestLc::CheckName ),
        ENTRY( "CheckLaunchMode", CTestLc::CheckLaunchMode ),
        ENTRY( "CheckSystemCharacteristics", CTestLc::CheckSystemCharacteristics ),
        ENTRY( "CheckApplicationCharacteristics", CTestLc::CheckApplicationCharacteristics ),

        ENTRY( "CheckArrayNewL", CTestLc::CheckArrayNewL ),
        ENTRY( "CheckArrayNewLC", CTestLc::CheckArrayNewLC ),
        ENTRY( "CheckCount", CTestLc::CheckCount ),
        ENTRY( "CheckAppendL", CTestLc::CheckAppendL ),
        ENTRY( "CheckRemoveL", CTestLc::CheckRemoveL ),
        ENTRY( "CheckResetL", CTestLc::CheckResetL ),
        ENTRY( "CheckOperatorL", CTestLc::CheckOperatorL ),

        ENTRY( "CheckTLcLocationAppFilter", CTestLc::CheckTLcLocationAppFilter ),
        ENTRY( "CheckFilterSystemCharacteristics", CTestLc::CheckFilterSystemCharacteristics ),
        ENTRY( "CheckFilterApplicationCharacteristics", CTestLc::CheckFilterApplicationCharacteristics ),
        ENTRY( "CheckSysCharFilterConfiguration", CTestLc::CheckSysCharFilterConfiguration ),
        ENTRY( "CheckAppCharFilterConfiguration", CTestLc::CheckAppCharFilterConfiguration ),


        //ADD NEW ENTRY HERE

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }
    
// ---------------------------------------------------------
// CTestLc::CheckServiceNewL
//
// (other items are commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckServiceNewL( CStifItemParser& /*aItem*/ )
	{
	CTestLcService* testLcService = new(ELeave)
							CTestLcService(iLog);
	TInt err = testLcService->CheckServiceNewL();
	delete testLcService;
	return err;
	}	

// ---------------------------------------------------------
// CTestLc::CheckServiceNewLC
//
// (other items are commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckServiceNewLC( CStifItemParser& /*aItem*/ )
	{
	CTestLcService* testLcService = new(ELeave)
							CTestLcService(iLog);
	TInt err = testLcService->CheckServiceNewLC();
	delete testLcService;
	return err;
	}
	
// ---------------------------------------------------------
// CTestLcService::TestGetLocationApplicationsL
//
// (other items are commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::TestGetLocationApplicationsL( CStifItemParser& /*aItem*/ )
	{
	CTestLcService* testLcService = new(ELeave)
							CTestLcService(iLog);
	TInt err = testLcService->TestGetLocationApplicationsL();
	delete testLcService;
	return err;
	}

// ---------------------------------------------------------
// CTestLcService::TestGetLocationApplicationsAsynL
//
// (other items are commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::TestGetLocationApplicationsAsynL( CStifItemParser& /*aItem*/ )
	{
	CTestLcService* testLcService = new(ELeave)
							CTestLcService(iLog);
	TInt err = testLcService->TestGetLocationApplicationsAsynL();
	delete testLcService;
	return err;
	}

// ---------------------------------------------------------
// CTestLcService::TestGetLocationApplicationsFilterL
//
// (other items are commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::TestGetLocationApplicationsFilterL( CStifItemParser& /*aItem*/ )
	{
	CTestLcService* testLcService = new(ELeave)
							CTestLcService(iLog);
	TInt err = testLcService->TestGetLocationApplicationsFilterL();
	delete testLcService;
	return err;
	}

// ---------------------------------------------------------
// CTestLcService::TestGetLocationApplicationsFilterAsynL
//
// (other items are commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::TestGetLocationApplicationsFilterAsynL( CStifItemParser& /*aItem*/ )
	{
	CTestLcService* testLcService = new(ELeave)
							CTestLcService(iLog);
	TInt err = testLcService->TestGetLocationApplicationsFilterAsynL();
	delete testLcService;
	return err;
	}

// ---------------------------------------------------------
// CTestLcService::TestSetRemoveObserverL
//
// (other items are commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::TestSetRemoveObserverL( CStifItemParser& aItem )
	{
	TInt choice;
	User::LeaveIfError(aItem.GetNextInt(choice));
	
	CTestLcService* testLcService = new(ELeave)
							CTestLcService(iLog);
	TInt err = testLcService->TestSetRemoveObserverL(choice);
	delete testLcService;
	return err;
	}
	
// ---------------------------------------------------------
// CTestLcService::TestCancelGetLocationApplications
//
// (other items are commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::TestCancelGetLocationApplications( CStifItemParser& /*aItem*/ )
	{
	CTestLcService* testLcService = new(ELeave)
							CTestLcService(iLog);
	TInt err = testLcService->TestCancelGetLocationApplications();
	delete testLcService;
	return err;
	}

// ---------------------------------------------------------
// CTestLcService::TestCLcLaunchParamNewL
//
// (other items are commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::TestCLcLaunchParamNewL( CStifItemParser& /*aItem*/ )
	{	
	CTestLcService* testLcService = new(ELeave)
							CTestLcService(iLog);
	TInt err = testLcService->TestCLcLaunchParamNewL();
	delete testLcService;
	return err;
	}
	
// ---------------------------------------------------------
// CTestLcService::TestCLcLaunchParamNewLC
//
// (other items are commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::TestCLcLaunchParamNewLC( CStifItemParser& /*aItem*/ )
	{
	CTestLcService* testLcService = new(ELeave)
							CTestLcService(iLog);
	TInt err = testLcService->TestCLcLaunchParamNewLC();
	delete testLcService;
	return err;
	}
	
//-------------------Info

// ---------------------------------------------------------
// CTestLc::CheckNewL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckNewL( CStifItemParser& /*aItem*/ )
	{
	CTestLocationAppInfo* testLocationAppInfo = new(ELeave)
							CTestLocationAppInfo(iLog);
	TInt err = testLocationAppInfo->CheckNewL();
	delete testLocationAppInfo;
	return err;
	}

// ---------------------------------------------------------
// CTestLc::CheckNewLC
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckNewLC( CStifItemParser& /*aItem*/ )
	{
	CTestLocationAppInfo* testLocationAppInfo = new(ELeave)
							CTestLocationAppInfo(iLog);
	TInt err = testLocationAppInfo->CheckNewLC();
	delete testLocationAppInfo;
	return err;
	}

// ---------------------------------------------------------
// CTestLc::CheckCopyNewL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckCopyNewL( CStifItemParser& /*aItem*/ )
	{
	CTestLocationAppInfo* testLocationAppInfo = new(ELeave)
							CTestLocationAppInfo(iLog);
	TInt err = testLocationAppInfo->CheckCopyNewL();
	delete testLocationAppInfo;
	return err;
	}

// ---------------------------------------------------------
// CTestLc::CheckCopyNewLC
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckCopyNewLC( CStifItemParser& /*aItem*/ )
	{
	CTestLocationAppInfo* testLocationAppInfo = new(ELeave)
							CTestLocationAppInfo(iLog);
	TInt err = testLocationAppInfo->CheckCopyNewLC();
	delete testLocationAppInfo;
	return err;
	}

// ---------------------------------------------------------
// CTestLc::CheckId
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckId( CStifItemParser& /*aItem*/ )
	{
	CTestLocationAppInfo* testLocationAppInfo = new(ELeave)
							CTestLocationAppInfo(iLog);
	TInt err = testLocationAppInfo->CheckId();
	delete testLocationAppInfo;
	return err;
	}


// ---------------------------------------------------------
// CTestLc::CheckName
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckName( CStifItemParser& /*aItem*/ )
	{
	CTestLocationAppInfo* testLocationAppInfo = new(ELeave)
							CTestLocationAppInfo(iLog);
	TInt err = testLocationAppInfo->CheckName();
	delete testLocationAppInfo;
	return err;
	}

// ---------------------------------------------------------
// CTestLc::CheckLaunchMode
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckLaunchMode( CStifItemParser& /*aItem*/ )
	{
	CTestLocationAppInfo* testLocationAppInfo = new(ELeave)
							CTestLocationAppInfo(iLog);
	TInt err = testLocationAppInfo->CheckLaunchMode();
	delete testLocationAppInfo;
	return err;
	}


// ---------------------------------------------------------
// CTestLc::CheckSystemCharacteristics
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckSystemCharacteristics( CStifItemParser& /*aItem*/ )
	{
	CTestLocationAppInfo* testLocationAppInfo = new(ELeave)
							CTestLocationAppInfo(iLog);
	TInt err = testLocationAppInfo->CheckSystemCharacteristics();
	delete testLocationAppInfo;
	return err;
	}

// ---------------------------------------------------------
// CTestLc::CheckApplicationCharacteristics
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckApplicationCharacteristics( CStifItemParser& /*aItem*/ )
	{
	CTestLocationAppInfo* testLocationAppInfo = new(ELeave)
							CTestLocationAppInfo(iLog);
	TInt err = testLocationAppInfo->CheckApplicationCharacteristics();
	delete testLocationAppInfo;
	return err;
	}
	
//-------------------Array

// ---------------------------------------------------------
// CTestLc::CheckArrayNewL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckArrayNewL( CStifItemParser& /*aItem*/ )
	{
	CTestLocationAppInfoArray* testLocationAppInfoArray = new(ELeave)
							CTestLocationAppInfoArray(iLog);
	TInt err = testLocationAppInfoArray->CheckArrayNewL();
	delete testLocationAppInfoArray;
	return err;
	}

// ---------------------------------------------------------
// CTestLc::CheckArrayNewLC
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckArrayNewLC( CStifItemParser& /*aItem*/ )
	{
	CTestLocationAppInfoArray* testLocationAppInfoArray = new(ELeave)
							CTestLocationAppInfoArray(iLog);
	TInt err = testLocationAppInfoArray->CheckArrayNewLC();
	delete testLocationAppInfoArray;
	return err;
	}

// ---------------------------------------------------------
// CTestLc::CheckCount
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckCount( CStifItemParser& /*aItem*/ )
	{
	CTestLocationAppInfoArray* testLocationAppInfoArray = new(ELeave)
							CTestLocationAppInfoArray(iLog);
	TInt err = testLocationAppInfoArray->CheckCount();
	delete testLocationAppInfoArray;
	return err;
	}

// ---------------------------------------------------------
// CTestLc::CheckAppendL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckAppendL( CStifItemParser& /*aItem*/ )
	{
	CTestLocationAppInfoArray* testLocationAppInfoArray = new(ELeave)
							CTestLocationAppInfoArray(iLog);
	TInt err = testLocationAppInfoArray->CheckAppendL();
	delete testLocationAppInfoArray;
	return err;
	}

// ---------------------------------------------------------
// CTestLc::CheckRemoveL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckRemoveL( CStifItemParser& /*aItem*/ )
	{
	CTestLocationAppInfoArray* testLocationAppInfoArray = new(ELeave)
							CTestLocationAppInfoArray(iLog);
	TInt err = testLocationAppInfoArray->CheckRemoveL();
	delete testLocationAppInfoArray;
	return err;
	}

// ---------------------------------------------------------
// CTestLc::CheckResetL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckResetL( CStifItemParser& /*aItem*/ )
	{
	CTestLocationAppInfoArray* testLocationAppInfoArray = new(ELeave)
							CTestLocationAppInfoArray(iLog);
	TInt err = testLocationAppInfoArray->CheckResetL();
	delete testLocationAppInfoArray;
	return err;
	}

// ---------------------------------------------------------
// CTestLc::CheckOperatorL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckOperatorL( CStifItemParser& /*aItem*/ )
	{
	CTestLocationAppInfoArray* testLocationAppInfoArray = new(ELeave)
							CTestLocationAppInfoArray(iLog);
	TInt err = testLocationAppInfoArray->CheckOperatorL();
	delete testLocationAppInfoArray;
	return err;
	}
	
//-----------Filter	
// ---------------------------------------------------------
// CTestLc::CheckTLcLocationAppFilter
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckTLcLocationAppFilter( CStifItemParser& /*aItem*/ )
	{
	CTestLocationAppFilter* testLocationAppFilter = new(ELeave)
							CTestLocationAppFilter(iLog);
	TInt err = testLocationAppFilter->CheckTLcLocationAppFilter( );
	delete testLocationAppFilter;
	return err;
	}

// ---------------------------------------------------------
// CTestLc::CheckFilterSystemCharacteristics
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckFilterSystemCharacteristics( CStifItemParser& aItem )
	{
	TInt choice;
	User::LeaveIfError(aItem.GetNextInt(choice));
	
	CTestLocationAppFilter* testLocationAppFilter = new(ELeave)
							CTestLocationAppFilter(iLog);
	TInt err = testLocationAppFilter->CheckFilterSystemCharacteristics( choice );
	delete testLocationAppFilter;
	return err;
	}

// ---------------------------------------------------------
// CTestLc::CheckFilterApplicationCharacteristics
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckFilterApplicationCharacteristics( CStifItemParser& aItem )
	{
	TInt choice;
	User::LeaveIfError(aItem.GetNextInt(choice));
	
	CTestLocationAppFilter* testLocationAppFilter = new(ELeave)
							CTestLocationAppFilter(iLog);
	TInt err = testLocationAppFilter->CheckFilterApplicationCharacteristics( choice );
	delete testLocationAppFilter;
	return err;
	}

// ---------------------------------------------------------
// CTestLc::CheckSysCharFilterConfiguration
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckSysCharFilterConfiguration( CStifItemParser& aItem )
	{
	TInt choice;
	User::LeaveIfError(aItem.GetNextInt(choice));
	
	CTestLocationAppFilter* testLocationAppFilter = new(ELeave)
							CTestLocationAppFilter(iLog);
	TInt err = testLocationAppFilter->CheckSysCharFilterConfiguration( choice );
	delete testLocationAppFilter;
	return err;
	}

// ---------------------------------------------------------
// CTestLc::CheckAppCharFilterConfiguration
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLc::CheckAppCharFilterConfiguration( CStifItemParser& aItem )
	{
	TInt choice;
	User::LeaveIfError(aItem.GetNextInt(choice));
	
	CTestLocationAppFilter* testLocationAppFilter = new(ELeave)
							CTestLocationAppFilter(iLog);
	TInt err = testLocationAppFilter->CheckAppCharFilterConfiguration( choice );
	delete testLocationAppFilter;
	return err;
	}
	
//  End of File
