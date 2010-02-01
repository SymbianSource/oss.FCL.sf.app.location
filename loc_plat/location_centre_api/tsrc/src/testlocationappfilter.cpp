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
* Description:  Test Class which has the list of test cases for TLcLocationAppFilter
*
*/


// System Includes
#include <lclocationappfilter.h>
#include <lclocationappinfo.h>

// User Includes
#include "testlocationappfilter.h"


// Constant Declarations

    
// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ Default Constructor
// ---------------------------------------------------------------------------
//
CTestLocationAppFilter::CTestLocationAppFilter(CStifLogger* aLog)
    : iLog(aLog)
    {

    }

// ---------------------------------------------------------------------------
// C++ Destructor
// ---------------------------------------------------------------------------
//
CTestLocationAppFilter::~CTestLocationAppFilter()
    {

    }

// ---------------------------------------------------------
// CTestLocationAppFilter::CheckTLcLocationAppFilter
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppFilter::CheckTLcLocationAppFilter( )
    {
    TInt err = KErrNone;
    TLcLocationAppFilter lcLocationAppFilter;
    
   	if( CLcLocationAppInfo::ESysCharNone == lcLocationAppFilter.SystemCharacteristics() &&
		CLcLocationAppInfo::EAppCharNone == lcLocationAppFilter.ApplicationCharacteristics() &&
		TLcLocationAppFilter::EFilterStrict == lcLocationAppFilter.SysCharFilterConfiguration() &&
		TLcLocationAppFilter::EFilterStrict == lcLocationAppFilter.AppCharFilterConfiguration() )
		{
		iLog->Log(_L("TestGetLocationApplicationsL Passed"));
		}
	else
		{
		err = KErrGeneral;
		iLog->Log(_L("TestGetLocationApplicationsL Failed"));
		} 
    
	return err;
    }

// ---------------------------------------------------------
// CTestLocationAppFilter::CheckFilterSystemCharacteristics
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppFilter::CheckFilterSystemCharacteristics( TInt aChoice )
    {
    TInt err = KErrNone;
    TLcLocationAppFilter lcLocationAppFilter;
    
    switch(static_cast<CLcLocationAppInfo::TLcSystemCharacteristics>(aChoice))
    	{
    		case CLcLocationAppInfo::ESysCharNone:
    			if( (KErrNone != (err=lcLocationAppFilter.SetSystemCharacteristic(CLcLocationAppInfo::ESysCharNone))) &&
    				(CLcLocationAppInfo::ESysCharNone != lcLocationAppFilter.SystemCharacteristics()) )
    				err = KErrArgument;
    		break;
    		default:
    			if( (KErrNone != (err=lcLocationAppFilter.SetSystemCharacteristic(CLcLocationAppInfo::ESysCharRomBased))) &&
    				(CLcLocationAppInfo::ESysCharRomBased != lcLocationAppFilter.SystemCharacteristics()) )
    				err = KErrArgument;	
    	}
    
	return err;
    }

// ---------------------------------------------------------
// CTestLocationAppFilter::CheckFilterApplicationCharacteristics
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppFilter::CheckFilterApplicationCharacteristics( TInt aChoice )
    {
    TInt err = KErrNone;
    TLcLocationAppFilter lcLocationAppFilter;
    
    switch(static_cast<CLcLocationAppInfo::TLcApplicationCharacteristics>(aChoice))
    	{
    		case CLcLocationAppInfo::EAppCharNone:
    			if( (KErrNone != (err=lcLocationAppFilter.SetApplicationCharacteristics(CLcLocationAppInfo::EAppCharNone))) &&
    				(CLcLocationAppInfo::EAppCharNone != lcLocationAppFilter.ApplicationCharacteristics()) )
    				err = KErrArgument;
    		break;
    		case CLcLocationAppInfo::EAppCharSettings:
    			if( (KErrNone != (err=lcLocationAppFilter.SetApplicationCharacteristics(CLcLocationAppInfo::EAppCharSettings))) &&
    				(CLcLocationAppInfo::EAppCharSettings != lcLocationAppFilter.ApplicationCharacteristics()) )
    				err = KErrArgument;
    		break;
    		case CLcLocationAppInfo::EAppCharPersonalDataManagement:
    			if( (KErrNone != (err=lcLocationAppFilter.SetApplicationCharacteristics(CLcLocationAppInfo::EAppCharPersonalDataManagement))) ||
    				(CLcLocationAppInfo::EAppCharPersonalDataManagement != lcLocationAppFilter.ApplicationCharacteristics()) )
    				err = KErrArgument;
    		break;
    		case CLcLocationAppInfo::EAppCharNavigation:
    			if( (KErrNone != (err=lcLocationAppFilter.SetApplicationCharacteristics(CLcLocationAppInfo::EAppCharNavigation))) ||
    				(CLcLocationAppInfo::EAppCharNavigation != lcLocationAppFilter.ApplicationCharacteristics()) )
    				err = KErrArgument;
    		break;
    		case CLcLocationAppInfo::EAppCharLocationBasedEvents:
    			if( (KErrNone != (err=lcLocationAppFilter.SetApplicationCharacteristics(CLcLocationAppInfo::EAppCharLocationBasedEvents))) ||
    				(CLcLocationAppInfo::EAppCharLocationBasedEvents != lcLocationAppFilter.ApplicationCharacteristics()) )
    				err = KErrArgument;
    		break;
    		case CLcLocationAppInfo::EAppCharRemotePositioning:
    			if( (KErrNone != (err=lcLocationAppFilter.SetApplicationCharacteristics(CLcLocationAppInfo::EAppCharRemotePositioning))) ||
    				(CLcLocationAppInfo::EAppCharRemotePositioning != lcLocationAppFilter.ApplicationCharacteristics()) )
    				err = KErrArgument;
    		break;
    		case CLcLocationAppInfo::EAppCharNetworkService:
    			if( (KErrNone != (err=lcLocationAppFilter.SetApplicationCharacteristics(CLcLocationAppInfo::EAppCharNetworkService))) ||
    				(CLcLocationAppInfo::EAppCharNetworkService != lcLocationAppFilter.ApplicationCharacteristics()) )
    				err = KErrArgument;
    		break;
    		case CLcLocationAppInfo::EAppCharLocationEnhancedMessages:
    			if( (KErrNone != (err=lcLocationAppFilter.SetApplicationCharacteristics(CLcLocationAppInfo::EAppCharLocationEnhancedMessages))) ||
    				(CLcLocationAppInfo::EAppCharLocationEnhancedMessages != lcLocationAppFilter.ApplicationCharacteristics()) )
    				err = KErrArgument;
    		break;
    		case CLcLocationAppInfo::EAppCharOnDeviceLocationBasedContent:
    			if( (KErrNone != (err=lcLocationAppFilter.SetApplicationCharacteristics(CLcLocationAppInfo::EAppCharOnDeviceLocationBasedContent))) ||
    				(CLcLocationAppInfo::EAppCharOnDeviceLocationBasedContent != lcLocationAppFilter.ApplicationCharacteristics()) )
    				err = KErrArgument;
    		break;
    		case CLcLocationAppInfo::EAppCharRemoteLocationBasedContent:
    			if( (KErrNone != (err=lcLocationAppFilter.SetApplicationCharacteristics(CLcLocationAppInfo::EAppCharRemoteLocationBasedContent))) ||
    				(CLcLocationAppInfo::EAppCharRemoteLocationBasedContent != lcLocationAppFilter.ApplicationCharacteristics()) )
    				err = KErrArgument;
    		break;
    		default:
    			if( (KErrNone != (err=lcLocationAppFilter.SetApplicationCharacteristics(CLcLocationAppInfo::EAppCharMaps))) ||
    				(CLcLocationAppInfo::EAppCharMaps != lcLocationAppFilter.ApplicationCharacteristics()) )
    				err = KErrArgument;	
    	}
    
	return err;
    }
 
// ---------------------------------------------------------
// CTestLocationAppFilter::CheckSysCharFilterConfiguration
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppFilter::CheckSysCharFilterConfiguration( TInt aChoice )
    {
    TInt err = KErrNone;
    TLcLocationAppFilter lcLocationAppFilter;
    
    switch(static_cast<TLcLocationAppFilter::TLcFilterConfig>(aChoice))
    	{
    		case TLcLocationAppFilter::EFilterStrict:
    			lcLocationAppFilter.SetSysCharFilterConfiguration(TLcLocationAppFilter::EFilterStrict);
    			if( (TLcLocationAppFilter::EFilterStrict != lcLocationAppFilter.SysCharFilterConfiguration()) )
    				err = KErrArgument;
    		break;
    		case TLcLocationAppFilter::EFilterInclusive:
    		lcLocationAppFilter.SetSysCharFilterConfiguration(TLcLocationAppFilter::EFilterInclusive);
    			if( (TLcLocationAppFilter::EFilterInclusive != lcLocationAppFilter.SysCharFilterConfiguration()) )
    				err = KErrArgument;
    		break;
    		default:
    			lcLocationAppFilter.SetSysCharFilterConfiguration(TLcLocationAppFilter::EFilterExclusive);
    			if( (TLcLocationAppFilter::EFilterExclusive != lcLocationAppFilter.SysCharFilterConfiguration()) )
    				err = KErrArgument;	
    	}
    
	return err;
    }   

// ---------------------------------------------------------
// CTestLocationAppFilter::CheckAppCharFilterConfiguration
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CTestLocationAppFilter::CheckAppCharFilterConfiguration( TInt aChoice )
    {
    TInt err = KErrNone;
    TLcLocationAppFilter lcLocationAppFilter;
    
    switch(static_cast<TLcLocationAppFilter::TLcFilterConfig>(aChoice))
    	{
    		case TLcLocationAppFilter::EFilterStrict:
    			lcLocationAppFilter.SetAppCharFilterConfiguration(TLcLocationAppFilter::EFilterStrict);
    			if( (TLcLocationAppFilter::EFilterStrict != lcLocationAppFilter.AppCharFilterConfiguration()) )
    				err = KErrArgument;
    		break;
    		case TLcLocationAppFilter::EFilterInclusive:
    			lcLocationAppFilter.SetAppCharFilterConfiguration(TLcLocationAppFilter::EFilterInclusive);
    			if( (TLcLocationAppFilter::EFilterInclusive != lcLocationAppFilter.AppCharFilterConfiguration()) )
    				err = KErrArgument;
    		break;
    		default:
    			lcLocationAppFilter.SetAppCharFilterConfiguration(TLcLocationAppFilter::EFilterExclusive);
    			if( (TLcLocationAppFilter::EFilterExclusive != lcLocationAppFilter.AppCharFilterConfiguration()) )
    				err = KErrArgument;	
    	}
    
	return err;
    }
