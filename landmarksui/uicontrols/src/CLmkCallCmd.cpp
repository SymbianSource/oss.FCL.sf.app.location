/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains methods which implements methods for
*                interfacing with call UI component for making a phonecall
*
*/






// INCLUDE FILES
#include <AiwServiceHandler.h>      // AIW
#include <AiwCommon.hrh>          // AIW
#include <CPhCltEmergencyCall.h>
#include "landmarks.hrh"            // AIW
#include <coemain.h>
#include "clmkcallcmd.h"
#include <aiwdialdata.h>

//============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
//  CLmkCallCmd::CLmkCallCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkCallCmd::CLmkCallCmd()
	{
	}

// ----------------------------------------------------
//  CLmkCallCmd::ConstructL()
// ----------------------------------------------------
//
void CLmkCallCmd::ConstructL( )
	{
	iAiwCallServiceHandler = CAiwServiceHandler::NewL();
	iEmergencyCall = CPhCltEmergencyCall::NewL(this);
	}

// -----------------------------------------------------------------------------
// CLmkCallCmd::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkCallCmd* CLmkCallCmd::NewL()
	{
	CLmkCallCmd* self =
	    new( ELeave ) CLmkCallCmd;
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

// ----------------------------------------------------
// CLmkCallCmd::~CLmkCallCmd()
// ----------------------------------------------------
//
CLmkCallCmd::~CLmkCallCmd()
	{
	delete iAiwCallServiceHandler;
	delete iEmergencyCall;
	}

// ----------------------------------------------------
// CLmkCallCmd::AttachMenuPaneL()
// ----------------------------------------------------
//
void CLmkCallCmd::AttachMenuPaneL(TInt aMenuResourceId, TInt aInterestResourceId)
	{
	// Attach the wanted menu with wanted interest to AIW.
	iAiwCallServiceHandler->AttachMenuL( aMenuResourceId, aInterestResourceId );
	}

// ----------------------------------------------------
// CLmkCallCmd::AttachInterestL()
// ----------------------------------------------------
//
void CLmkCallCmd::AttachInterestL ( RCriteriaArray& aInterestId )
	{
	// For using non-menu services
	iAiwCallServiceHandler->AttachL( aInterestId );
	}

// ----------------------------------------------------
// CLmkCallCmd::AttachInterestL()
// ----------------------------------------------------
//
void CLmkCallCmd::AttachInterestL ( TInt aInterestId )
	{
	// For using non-menu services
	iAiwCallServiceHandler->AttachL( aInterestId );
	}
// CLmkCallCmd::InitializeMenuPaneL()
// ----------------------------------------------------
//
void CLmkCallCmd::InitializeMenuPaneL(
		CEikMenuPane& aMenuPane,
		TInt aMenuResourceId,
		TInt aBaseMenuCmdId)
	{
	//AIW service menu initialization
	iAiwCallServiceHandler->InitializeMenuPaneL(
					aMenuPane,
					aMenuResourceId,
					aBaseMenuCmdId, // Must not overlap with the other menu ids!
					iAiwCallServiceHandler->InParamListL() );
	}

// -----------------------------------------------------------------------------
// CLmkCallCmd::ExecuteAIWCallCmdL()
// -----------------------------------------------------------------------------
//
void CLmkCallCmd::ExecuteAIWCallCmdL( TInt aCommandId, TDesC& aPhoneNum )
	{
	//first give a chance to emergency call
	if(!HandleEmergencyCallL(aPhoneNum))
		{
		//number is not an emergency number, call normal.
		HandleNormalCallL( aCommandId,aPhoneNum );
		}
	}
// ----------------------------------------------------------------------------
// CLmkCallCmd::HandleEmergencyDialL
// ----------------------------------------------------------------------------
//
void CLmkCallCmd::HandleEmergencyDialL( const TInt /*aStatus */)
    {
    // TODO: Show some note to user if there is error?
    // belongs to MPhCltEmergencyCallObserver interface
    }
// -----------------------------------------------------------------------------
// CLmkCallCmd::HandleEmergencyCall
// -----------------------------------------------------------------------------
//
TBool CLmkCallCmd::HandleEmergencyCallL( TDesC& aPhoneNum )
	{
	TBool isEmerNum = EFalse;
	iEmergencyCall->IsEmergencyPhoneNumber(aPhoneNum,isEmerNum);
	if( isEmerNum)
		{
		iEmergencyCall->DialEmergencyCallL(aPhoneNum);
		}
	return isEmerNum;
	}
// -----------------------------------------------------------------------------
// CLmkCallCmd::HandleNormalCall
// -----------------------------------------------------------------------------
//
void CLmkCallCmd::HandleNormalCallL( TInt aCommandId, TDesC& aPhoneNum )
	{
		// Set basic dial data
	CAiwDialData* dialData = CAiwDialData::NewLC();
	dialData->SetPhoneNumberL( aPhoneNum );
	dialData->SetWindowGroup( AIWDialData::KAiwGoToIdle );
	CAiwGenericParamList& paramList = iAiwCallServiceHandler->InParamListL();
	dialData->FillInParamListL( paramList );
	CleanupStack::PopAndDestroy();

	if ( ELmkCmdCall == aCommandId )
		{// invoked from 'sendkey'
		iAiwCallServiceHandler->ExecuteServiceCmdL(
            KAiwCmdCall,
            paramList,
            iAiwCallServiceHandler->OutParamListL(),
            0,
            NULL );
		}
	else // invoked from 'menu'
		{
		iAiwCallServiceHandler->ExecuteMenuCmdL(
			aCommandId,
			paramList,
			iAiwCallServiceHandler->OutParamListL(),
			0, 		// No options used.
			NULL);  // No need for callback
		}
	}

// -----------------------------------------------------------------------------
// CLmkCallCmd::ServiceCmdByMenuCmd
// -----------------------------------------------------------------------------
//
TInt CLmkCallCmd::ServiceCmdByMenuCmd( TInt aMenuCmd )
	{
	return iAiwCallServiceHandler->ServiceCmdByMenuCmd( aMenuCmd );
	}
//  End of File
