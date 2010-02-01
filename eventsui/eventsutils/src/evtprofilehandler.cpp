/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Profile Settings Handler.
*
*/


// System Includes
#include <e32base.h>
#include <centralrepository.h>
#include <ProfileEngineSDKCRKeys.h>
#include <hwrmvibrasdkcrkeys.h>

// User Includes
#include "evtutilsconsts.h"
#include "evtprofilehandler.h"
#include "evtdebug.h"

// Constants
static const TInt KSilentRing = 4;

// ================ Member funtions for CEvtProfileHandler class ===============

// ---------------------------------------------------------------------------
// CEvtProfileHandler::CEvtProfileHandler
// ---------------------------------------------------------------------------
//
CEvtProfileHandler::CEvtProfileHandler( MEvtProfileObserver& aObserver ):
									iObserver(aObserver)
	{
	}

// ---------------------------------------------------------------------------
// CEvtProfileHandler::~CEvtProfileHandler
// ---------------------------------------------------------------------------
//
CEvtProfileHandler::~CEvtProfileHandler()
	{

	if( iVibraNotifyHandler )
		{
		iVibraNotifyHandler->StopListening();
		}

	if( iTypeNotifyHandler )
		{
		iTypeNotifyHandler->StopListening();
		}

	if( iVolumeNotifyHandler )
		{
		iVolumeNotifyHandler->StopListening();
		}

    delete iVibraNotifyHandler;
	delete iTypeNotifyHandler;
    delete iVolumeNotifyHandler;

    delete iProfileSession;
    delete iVibraSession;
	}
	
// ---------------------------------------------------------------------------
// CEvtProfileHandler::NewL
// ---------------------------------------------------------------------------
//
CEvtProfileHandler* CEvtProfileHandler::NewL( MEvtProfileObserver& aObserver )
    {
	CEvtProfileHandler* self = NewLC( aObserver );
	CleanupStack::Pop( self );
	return self;
    }

// ---------------------------------------------------------------------------
// CEvtProfileHandler::NewLC
// ---------------------------------------------------------------------------
//
CEvtProfileHandler* CEvtProfileHandler::NewLC( MEvtProfileObserver& aObserver )
    {
	CEvtProfileHandler* self = new ( ELeave )CEvtProfileHandler( aObserver );
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
    } 

// ---------------------------------------------------------------------------
// CEvtProfileHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CEvtProfileHandler::ConstructL()
    {
    EVTUIDEBUG( "+ CEvtProfileHandler::ConstructL()" );
    
	// Create a cenrep session each for Profile and Vibra Cenrep Settings.
	iProfileSession = CRepository::NewL( KCRUidProfileEngine );
	iVibraSession = CRepository::NewL( KCRUidVibraCtrl );

	// Create the cenrep notification handlers to listen to the settings 
	// value changes.
    iVolumeNotifyHandler = CCenRepNotifyHandler::NewL(*this,
                                                     *iProfileSession,
                                                     CCenRepNotifyHandler::EIntKey,
                                                     KProEngActiveRingingVolume);

	iTypeNotifyHandler = CCenRepNotifyHandler::NewL(*this,
                                                     *iProfileSession,
                                                     CCenRepNotifyHandler::EIntKey,
                                                     KProEngActiveRingingType);

	iVibraNotifyHandler = CCenRepNotifyHandler::NewL(*this,
                                                     *iVibraSession,
                                                     CCenRepNotifyHandler::EIntKey,
                                                     KVibraCtrlProfileVibraEnabled);

	// Get the initial value of the Settings.
	User::LeaveIfError( iProfileSession->Get( KProEngActiveRingingVolume, iRingingVolume ) );
    User::LeaveIfError( iProfileSession->Get( KProEngActiveRingingType, iRingingType ) );
    User::LeaveIfError( iVibraSession->Get( KVibraCtrlProfileVibraEnabled, iVibra ) );

	// Start listening to settings change.
    iVolumeNotifyHandler->StartListeningL();
    iTypeNotifyHandler->StartListeningL();
    iVibraNotifyHandler->StartListeningL();
	
    EVTUIDEBUG( "- CEvtProfileHandler::ConstructL()" );
    }

// ---------------------------------------------------------------------------
// CEvtProfileHandler::Volume
// ---------------------------------------------------------------------------
//
TInt CEvtProfileHandler::Volume( )
	{
    EVTUIDEBUG1( "= CEvtProfileHandler::Volume() - %d", iRingingVolume );
	return iRingingVolume;
    }

// ---------------------------------------------------------------------------
// CEvtProfileHandler::IsSilentRingingType
// ---------------------------------------------------------------------------
//
TBool CEvtProfileHandler::IsSilentRingingType( )
	{
    EVTUIDEBUG1( "= CEvtProfileHandler::IsSilentRingingType() - %d", iRingingType );
    if( iRingingType == KSilentRing )
		return ETrue;
    else
    	return EFalse;
    }

// ---------------------------------------------------------------------------
// CEvtProfileHandler::IsVibra
// ---------------------------------------------------------------------------
//
TBool CEvtProfileHandler::IsVibra( )
	{
    EVTUIDEBUG1( "= CEvtProfileHandler::IsVibra() - %d", iVibra );
    return iVibra;
    }

// -----------------------------------------------------------------------------
// CEvtProfileHandler::HandleNotifyInt()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CEvtProfileHandler::HandleNotifyInt(TUint32 aId, TInt aNewValue)
    {

   	if ( aId == KProEngActiveRingingVolume )
   		{
   		iRingingVolume = aNewValue;
   		}
   	else if ( aId == KProEngActiveRingingType )
		{
   		iRingingType = aNewValue;
   		}
   	else if ( aId == KVibraCtrlProfileVibraEnabled )
   		{
   		iVibra = aNewValue;
   		}
	iObserver.HandleProfileSettingsChange();
    }
