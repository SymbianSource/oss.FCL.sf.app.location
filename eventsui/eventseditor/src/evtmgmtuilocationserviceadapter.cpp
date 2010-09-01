/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: 
*     This file contains the methods which interface with Location Acquisition 
*     API for getting location information 
*
*/

// INCLUDE FILES
#include <evtmgmteditorui.rsg>
#include <f32file.h>
#include <sysutil.h>
#include <lbs/epos_cposmoduleidlist.h>
#include <lbs/epos_cposmodules.h>
#include <aknnotewrappers.h> 
#include <eikenv.h>
#include <StringLoader.h>
#include <centralrepository.h>
#include "locnotprefplugindomaincrkeys.h"
#include "evtmgmtuilocationserviceadapter.h"
#include "evteditorconsts.h"
#include "evtdebug.h"

// ----------------------------------------------------------------------------
// CEvtMgmtUiLocationServiceAdapter::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
CEvtMgmtUiLocationServiceAdapter* CEvtMgmtUiLocationServiceAdapter::NewL( MEvtMgmtUiLocationServiceObserver& aObserver )
	{
	CEvtMgmtUiLocationServiceAdapter* self = new ( ELeave ) CEvtMgmtUiLocationServiceAdapter( aObserver );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

// ---------------------------------------------------------
// CEvtMgmtUiLocationServiceAdapter::~CEvtMgmtUiLocationServiceAdapter
// ---------------------------------------------------------
CEvtMgmtUiLocationServiceAdapter::~CEvtMgmtUiLocationServiceAdapter()
    {
    EVTUIDEBUG("+ CEvtMgmtUiLocationServiceAdapter::~CEvtMgmtUiLocationServiceAdapter()" );
    Cancel();
    iPositioner.Close();
    iServer.Close();
    EVTUIDEBUG("- CEvtMgmtUiLocationServiceAdapter::~CEvtMgmtUiLocationServiceAdapter()" );
    }

// ---------------------------------------------------------
// CEvtMgmtUiLocationServiceAdapter::ConstructL
// ---------------------------------------------------------
void CEvtMgmtUiLocationServiceAdapter::ConstructL()
	{
	//conect to position server
	User::LeaveIfError(iServer.Connect());
	
	//create subsesion with server
	User::LeaveIfError(iPositioner.Open( iServer ));
	
	//set timeout value
	TPositionUpdateOptions updateOptions;        
	updateOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(KLocAcqTimeOut));
	User::LeaveIfError( iPositioner.SetUpdateOptions(updateOptions) );
	
	//set requestor
    _LIT(KRequestorName, "LocationSerivce");    
	User::LeaveIfError( iPositioner.SetRequestor( 
	                    CRequestor::ERequestorService,
	                    CRequestor::EFormatApplication,
	                    KRequestorName )); 
	
	//add to active scheduler for asychrnous operation
	 CActiveScheduler::Add( this );
	}

// ----------------------------------------------------------------------------
// CLmkLocationService::CLmkLocationService
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
CEvtMgmtUiLocationServiceAdapter::CEvtMgmtUiLocationServiceAdapter( MEvtMgmtUiLocationServiceObserver& aObserver ) 
: CActive(EPriorityStandard), iObserver( aObserver )
    {   
    }

// ---------------------------------------------------------
// CEvtMgmtUiLocationServiceAdapter::LocationRequestL
// It is used to request current location
// ---------------------------------------------------------
 void CEvtMgmtUiLocationServiceAdapter::LocationRequestL()
	{    
	EVTUIDEBUG("+ CEvtMgmtUiLocationServiceAdapter::LocationRequestL()" );
	if ( IsActive() )
        {
        User::Leave( KErrInUse );            
        }
    SetActive();
    iStatus = KRequestPending;
	iPositioner.NotifyPositionUpdate( iPositionInfo, iStatus );  
	EVTUIDEBUG("- CEvtMgmtUiLocationServiceAdapter::LocationRequestL()" );      	
	}

// ---------------------------------------------------------
// CEvtMgmtUiLocationServiceAdapter::CurrentPosition
// It is used to retrieve current location
// @ret Recently retrieved current location 
// ---------------------------------------------------------
TPosition& CEvtMgmtUiLocationServiceAdapter::CurrentPosition()
	{
	return iPosition;
	}

// ---------------------------------------------------------
// CEvtMgmtUiLocationServiceAdapter::AllPositionMethodsDisabled
// Check if all positioning methods are disabled.
// @ret true if disabled else false
// ---------------------------------------------------------
//
TBool CEvtMgmtUiLocationServiceAdapter::AllPositionMethodsDisabledL()
    {
    CPosModules* moduleDb = CPosModules::OpenL();
    CleanupStack::PushL(moduleDb);
    CPosModuleIdList* psyList = moduleDb->ModuleIdListL();
    CleanupStack::PushL(psyList);
    
    TBool allDisabled =ETrue;
    
    TPositionModuleInfo moduleInfo;
     for(TInt index=0; index< psyList->Count(); index++)
         {
        TPositionModuleId moduleId = psyList->At( index );
        TRAPD(error, moduleDb->GetModuleInfoL( moduleId, moduleInfo););
        if(error == KErrNone ) 
            {
            if( moduleInfo.IsAvailable() )
                {
                allDisabled = EFalse;
                break;
                }
            }        
         }
    
    CleanupStack::PopAndDestroy(2); // psyList, moduleDb
    return allDisabled;
    }

// ---------------------------------------------------------
// CEvtMgmtUiLocationServiceAdapter::CancelRequest
// It is used to cancel recently issued location request
// ---------------------------------------------------------
void CEvtMgmtUiLocationServiceAdapter::CancelRequest()
	{
	Cancel();
	TRAP_IGNORE( iObserver.HandleLocationServiceErrorL( KErrCancel ) );
	}

// ---------------------------------------------------------
// CEvtMgmtUiLocationServiceAdapter::RunL
// Derived from CActive
// ---------------------------------------------------------
void CEvtMgmtUiLocationServiceAdapter::RunL()
	{
    EVTUIDEBUG1("+ CEvtMgmtUiLocationServiceAdapter::RunL() - %d", iStatus.Int() );

   //TInt err = KErrTimedOut;
   //switch ( err )
   switch ( iStatus.Int() )
		{
		case KErrNone:
		case KPositionPartialUpdate:        
			{
			//retrieve current location information
			iPositionInfo.GetPosition( iPosition );
			TReal32 altitude = iPosition.Altitude();
			if ( Math::IsNaN( altitude ) )
				{
				TRealX nan;
				nan.SetNaN();
				iPosition.SetVerticalAccuracy( nan );	
				}	
			
			//notify observer about successful completion of operation
			iObserver.HandleLocationServiceResponseL( iStatus.Int() );
			break;
			}
		case KErrAccessDenied:			
		case KPositionQualityLoss:
		case KErrTimedOut:
		case KErrNotFound: // No PSY selected.
		case KErrUnknown:
		case KErrCancel:
		case KErrArgument:
		default:
			{		
		    TRAP_IGNORE( iObserver.HandleLocationServiceErrorL( iStatus.Int() ) );
			break;
			}
		}
	}	

// ---------------------------------------------------------
// CEvtMgmtUiLocationServiceAdapter::RunError
// Derived from CActive
// ---------------------------------------------------------
//
TInt CEvtMgmtUiLocationServiceAdapter::RunError(TInt aError)
    {
    TRAP_IGNORE( iObserver.HandleLocationServiceErrorL( aError ) );
    return KErrNone;
    }

// ---------------------------------------------------------
// CEvtMgmtUiLocationServiceAdapter::DoCancel
// Derived from CActive
// ---------------------------------------------------------
//
void CEvtMgmtUiLocationServiceAdapter::DoCancel()
    {
    EVTUIDEBUG("+ CEvtMgmtUiLocationServiceAdapter::DoCancel()" );
    iPositioner.CancelRequest( EPositionerNotifyPositionUpdate );
    EVTUIDEBUG("- CEvtMgmtUiLocationServiceAdapter::DoCancel()" );
    }
// End of File
