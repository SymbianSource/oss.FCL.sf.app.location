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

#include <f32file.h>
#include <sysutil.h>
#include "LmkConsts.h"
#include "MLmkLocationObserver.h"
#include "CLmkLocationService.h"


// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// CLmkLocationService::CLmkLocationService
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CLmkLocationService::CLmkLocationService() : CActive(EPriorityStandard)
	{
	CActiveScheduler::Add( this );
	}

// ----------------------------------------------------------------------------
// CLmkLocationService::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CLmkLocationService* CLmkLocationService::NewL(const TDesC& aSrvName)
	{
	CLmkLocationService* self = new ( ELeave ) CLmkLocationService;
	CleanupStack::PushL( self );
	self->ConstructL(aSrvName);
	CleanupStack::Pop(); // self
	return self;
	}

// ---------------------------------------------------------
// CLmkLocationService::ConstructL
// ---------------------------------------------------------
//
void CLmkLocationService::ConstructL( const TDesC& aSrvName )
	{
	User::LeaveIfError(iServer.Connect());
	User::LeaveIfError(iPositioner.Open( iServer ));

	/*TPositionUpdateOptions updateOptions;
	updateOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(KLmkLRTimeOut));
	User::LeaveIfError( iPositioner.SetUpdateOptions(updateOptions) );*/

	User::LeaveIfError( iPositioner.SetRequestor(
	                    CRequestor::ERequestorService,
	                    CRequestor::EFormatApplication,
	                    aSrvName ));
	}

// ---------------------------------------------------------
// CLmkLocationService::~CLmkLocationService
// ---------------------------------------------------------
//
CLmkLocationService::~CLmkLocationService()
	{
	Cancel();
	iPositioner.Close();
	iServer.Close();
	}

// ---------------------------------------------------------
// CLmkLocationService::SetObserver
// ---------------------------------------------------------
//
EXPORT_C void CLmkLocationService::SetObserver(MLmkLocationObserver& aObserver)
	{
	iObserver = &aObserver;
	}

// ---------------------------------------------------------
// CLmkLocationService::RemoveModelObserver
// ---------------------------------------------------------
//
EXPORT_C void CLmkLocationService::RemoveModelObserver()
    {
    iObserver = NULL;
    }

// ---------------------------------------------------------
// CLmkLocationService::LocationRequestL
// ---------------------------------------------------------
//
EXPORT_C void CLmkLocationService::LocationRequestL()
	{
	iPositioner.NotifyPositionUpdate( iPositionInfo, iStatus );
	SetActive();
	}

// ---------------------------------------------------------
// CLmkLocationService::CurrentPosition
// ---------------------------------------------------------
//
EXPORT_C TPosition& CLmkLocationService::CurrentPosition()
	{
	return iPosition;
	}

// ---------------------------------------------------------
// CLmkLocationService::CancelRequest
// ---------------------------------------------------------
//
EXPORT_C void CLmkLocationService::CancelRequest()
	{
	Cancel();
	}

// ---------------------------------------------------------
// CLmkLocationService::RunL
// ---------------------------------------------------------
//
void CLmkLocationService::RunL()
	{
	switch ( iStatus.Int() )
		{
		case KErrNone:
		case KPositionPartialUpdate:
			{
			iPositionInfo.GetPosition( iPosition );
			TReal32 altitude = iPosition.Altitude();
			if (Math::IsNaN( altitude))
				{
				TRealX nan;
				nan.SetNaN();
				iPosition.SetVerticalAccuracy( nan );
				}
			iObserver->NotifyL( iStatus.Int() );
			break;
			}
		case KErrAccessDenied:
			{
			User::Leave( KErrAccessDenied );
			break;
			}
		case KPositionQualityLoss:
		case KErrTimedOut:
		case KErrNotFound: // No PSY selected.
		case KErrUnknown:
		case KErrCancel:
		case KErrArgument:
		default:
			{
			User::Leave( iStatus.Int() );
			break;
			}
		}
	}

// ---------------------------------------------------------
// CLmkLocationService::DoCancel
// ---------------------------------------------------------
//
void CLmkLocationService::DoCancel()
    {
    iPositioner.CancelRequest( EPositionerNotifyPositionUpdate );
    }

// ---------------------------------------------------------
// CLmkLocationService::RunError
// ---------------------------------------------------------
//
TInt CLmkLocationService::RunError(TInt aError)
    {
    if ( iObserver )
        {
        TRAPD( error, iObserver->NotifyErrorL( aError ) );
        if (error != KErrNone)
        {
        error = KErrNone;
        }
        }
    return KErrNone;
    }

// End of File
