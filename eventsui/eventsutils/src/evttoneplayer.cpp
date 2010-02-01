/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  File Player Events Info Note
*
*/


// System Includes
#include <e32base.h>
#include <mdaaudiotoneplayer.h>
#include <AudioPreference.h>
#include <e32property.h>
#include <coreapplicationuisdomainpskeys.h>

// User Includes
#include "evttoneplayer.h"
#include "evtutilsconsts.h"
#include "evttoneutils.h"
#include "evtdebug.h"

// Constants
const TInt KMicroSeconds = 1000000;

// ================ Member funtions for CEvtTonePlayer class ===============

// ---------------------------------------------------------------------------
// CEvtTonePlayer::CEvtTonePlayer
// ---------------------------------------------------------------------------
//
CEvtTonePlayer::CEvtTonePlayer( MEvtTonePlayObserver& aObserver ):
									iObserver(aObserver)
	{
	}

// ---------------------------------------------------------------------------
// CEvtTonePlayer::~CEvtTonePlayer
// ---------------------------------------------------------------------------
//
CEvtTonePlayer::~CEvtTonePlayer()
	{ 
	Stop();
	delete iProfileHandler;
	delete iCallHandler;

    // Delete the timer
    if( iTimer )
        {
    	StopTimer();
        delete iTimer;
        }
	}
	
// ---------------------------------------------------------------------------
// CEvtTonePlayer::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtTonePlayer* CEvtTonePlayer::NewL( MEvtTonePlayObserver& aObserver )
    {
	CEvtTonePlayer* self = NewLC( aObserver );
	CleanupStack::Pop( self );
	return self;
    }

// ---------------------------------------------------------------------------
// CEvtTonePlayer::NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CEvtTonePlayer* CEvtTonePlayer::NewLC( MEvtTonePlayObserver& aObserver )
    {
	CEvtTonePlayer* self = new ( ELeave )CEvtTonePlayer( aObserver );
	CleanupStack::PushL( self );
	self->ConstructL( );
	return self;
    } 

// ---------------------------------------------------------------------------
// CEvtTonePlayer::ConstructL
// ---------------------------------------------------------------------------
//
void CEvtTonePlayer::ConstructL()
    {
    EVTUIDEBUG( "+ CEvtTonePlayer::ConstructL()" );
    iProfileHandler = CEvtProfileHandler::NewL( *this );
    iCallHandler = CEvtCallHandler::NewL( *this );
    EVTUIDEBUG( "- CEvtTonePlayer::ConstructL()" );
    }

// ---------------------------------------------------------------------------
// CEvtTonePlayer::PlayFileL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtTonePlayer::PlayFileL( const TFileName& aAudioFileName, 
						TBool aLoop, TInt aTimeInterval )
	{
    EVTUIDEBUG( "+ CEvtTonePlayer::PlayFileL()" );
    
	// Stop if some tone is already playing.
	Stop();

	// Stop the Timer, if it is On
    	StopTimer();
	
	// Play a beep sound if there is an active/incoming call.
	if( iCallHandler->IsCallActive() || iCallHandler->IsCallIncoming() )
		{
		iPlayerState = ENotReady;
		iTonePlayer = CMdaAudioToneUtility::NewL( *this );
		
		iTonePlayer->PrepareToPlayDesSequence( KPhoneBeepSequence() ); 
		return;
		}
	
	TBool exist(ETrue);
	TBool playable(ETrue);
	
	// Check if Tone exists and is playable. 
	TRAPD(existErr, exist = EvtToneUtils::IsExistL( aAudioFileName ) );
	TRAPD(playableErr, playable = EvtToneUtils::IsExistL( aAudioFileName ) );
	
	// Play tone if,
	// 1. File exists and is playable
	// 2. profile is NOT "Silent"
	if( !(iProfileHandler->IsSilentRingingType() || aAudioFileName.Length() == 0 || 
		existErr || playableErr || !exist || !playable ) )
		{
		// If the Mime type of ring tone is "KRngMimeType", then we need to play that file using
		// CMdaAudioToneUtility else we need to use CDrmPlayerUtility
		if ( EvtToneUtils::IsRNGTypeL( aAudioFileName ) )
			{
			// Mime type of tone is "KRngMimeType"
			iPlayerState = ENotReady;
			iTonePlayer = CMdaAudioToneUtility::NewL( *this );
			
			// Set the repeat flag based on Audio loop argument.
			if( aLoop )
				iAudioPlayer->SetRepeats( KMdaAudioToneRepeatForever, TTimeIntervalMicroSeconds(KMicroSeconds) );
			else
				iAudioPlayer->SetRepeats( 0 ,TTimeIntervalMicroSeconds(KMicroSeconds) );
				
			iTonePlayer->PrepareToPlayFileSequence( aAudioFileName );
			
			// Start the Timer if we have a Time Interval argument set.
			if( aTimeInterval )
			    {
			    StartTimerL( aTimeInterval );
			    }
			}
		else
			{
			// Mime type of tone is NOT "KRngMimeType"
			iPlayerState = ENotReady;
	        iAudioPlayer = CDrmPlayerUtility::NewFilePlayerL(
	            aAudioFileName,
	            *this, 
	            KAudioPriorityAlarm, 
	            static_cast<TMdaPriorityPreference>( KAudioPrefCalendarAlarm ) );

			// Set the repeat flag based on Audio loop argument.
			if( aLoop )
				iAudioPlayer->SetRepeats( KMdaAudioToneRepeatForever, TTimeIntervalMicroSeconds(KMicroSeconds) );
			else
				iAudioPlayer->SetRepeats( 0 ,TTimeIntervalMicroSeconds(KMicroSeconds) );

			// Start the Timer if we have a Time Interval argument set.
            if( aTimeInterval )
                {
			    StartTimerL( aTimeInterval );
                }
			}
		}
	else 
		{
		// Since we cannot play the Tone file, Play the Vibra tone if the Vibration settings is ON.
		if( iProfileHandler->IsVibra() )
			{
			iPlayerState = ENotReady;
			iTonePlayer = CMdaAudioToneUtility::NewL( *this );
			
			// Set to repeat forever.
			iTonePlayer->SetRepeats( KMdaAudioToneRepeatForever, TTimeIntervalMicroSeconds(KMicroSeconds) );
			
			// No Sound sequence which is the file to play Vibration ONLY.
			iTonePlayer->PrepareToPlayDesSequence( KPhoneNoSoundSequence() ); 

			// Start the Timer if we have a Time Interval argument set.
            if( aTimeInterval )
                {
			    StartTimerL( aTimeInterval );
                }
			}
		// Notify the Tone Player Observer
		iObserver.TonePlayCompleteL( EFalse );
		}
	
    EVTUIDEBUG( "- CEvtTonePlayer::PlayFileL()" );
	}

// ---------------------------------------------------------------------------
// CEvtTonePlayer::StopPlayingL
// ---------------------------------------------------------------------------
//
EXPORT_C void CEvtTonePlayer::StopPlayingL( TBool aTimeout )
	{
    EVTUIDEBUG( "+ CEvtTonePlayer::StopPlayingL()" );
	// Stop the currently playing tone file.
	Stop();

	// If the tone is stopped NOT because of the time out, then we continue to
	// play vibration tone till the time out.
	if( !aTimeout && iProfileHandler->IsVibra() )
		{
		iPlayerState = ENotReady;
		iTonePlayer = CMdaAudioToneUtility::NewL( *this );
		
		iTonePlayer->SetRepeats( KMdaAudioToneRepeatForever, TTimeIntervalMicroSeconds(KMicroSeconds) );
		
		iTonePlayer->PrepareToPlayDesSequence( KPhoneNoSoundSequence() );
		}
	else
		{
    		StopTimer();
		}
		
	// Notify the Tone Player Observer
	iObserver.TonePlayCompleteL( aTimeout );
    EVTUIDEBUG( "- CEvtTonePlayer::StopPlayingL()" );
	}

// ---------------------------------------------------------------------------
// CEvtTonePlayer::MdapcInitComplete
// ---------------------------------------------------------------------------
//
void CEvtTonePlayer::MdapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
    EVTUIDEBUG( "+ CEvtTonePlayer::MdapcInitComplete()" );
	if( KErrNone == aError )
		{
		iPlayerState = EReady;
		if( iAudioPlayer )
			{
			iPlayerState = EPlaying;
			iAudioPlayer->SetVolume( PlayerVolume(iAudioPlayer->MaxVolume(), iProfileHandler->Volume()) );
			iAudioPlayer->SetPriority( KAudioPriorityAlarm, static_cast<TMdaPriorityPreference>( KAudioPrefCalendarAlarm ) );
			iAudioPlayer->Play();
			}
		}
	else
		{
		iPlayerState = ENotReady;
		}
    EVTUIDEBUG( "- CEvtTonePlayer::MdapcInitComplete()" );
	}

// ---------------------------------------------------------------------------
// CEvtTonePlayer::MdapcPlayComplete
// ---------------------------------------------------------------------------
//
void CEvtTonePlayer::MdapcPlayComplete(TInt /*aError*/)
	{
    EVTUIDEBUG( "+ CEvtTonePlayer::MdapcPlayComplete()" );
	// Notify the Tone Player Observer
	TRAPD( err, iObserver.TonePlayCompleteL( EFalse ) );
	if(!err)
		EVTUIDEBUG1( "TonePlayCompleteL Error - %d", err );
    EVTUIDEBUG( "- CEvtTonePlayer::MdapcPlayComplete()" );
	}

// ---------------------------------------------------------------------------
// CEvtTonePlayer::MatoPrepareComplete
// ---------------------------------------------------------------------------
//
void CEvtTonePlayer::MatoPrepareComplete(TInt aError)
	{
    EVTUIDEBUG( "+ CEvtTonePlayer::MatoPrepareComplete()" );
	if( KErrNone == aError )
		{
		iPlayerState = EReady;
		if( iTonePlayer )
			{
			iPlayerState = EPlaying;
			iTonePlayer->SetVolume( PlayerVolume(iTonePlayer->MaxVolume(), iProfileHandler->Volume()) );
	    	iTonePlayer->SetPriority( KAudioPriorityAlarm, static_cast<TMdaPriorityPreference>( KAudioPrefCalendarAlarm ) );
			iTonePlayer->Play();
			}
		}
	else
		{
		iPlayerState = ENotReady;
		}
    EVTUIDEBUG( "- CEvtTonePlayer::MatoPrepareComplete()" );
	}

// ---------------------------------------------------------------------------
// CEvtTonePlayer::MatoPlayComplete
// ---------------------------------------------------------------------------
//
void CEvtTonePlayer::MatoPlayComplete(TInt /*aError*/)
	{
    EVTUIDEBUG( "+ CEvtTonePlayer::MdapcPlayComplete()" );
	// Notify the Tone Player Observer
	TRAPD( err, iObserver.TonePlayCompleteL( EFalse ) );
	if(!err)
		EVTUIDEBUG1( "TonePlayCompleteL Error - %d", err );
    EVTUIDEBUG( "- CEvtTonePlayer::MdapcPlayComplete()" );
	}

// ---------------------------------------------------------------------------
// CEvtTonePlayer::Stop
// ---------------------------------------------------------------------------
//
void CEvtTonePlayer::Stop()
	{
    EVTUIDEBUG( "+ CEvtTonePlayer::Stop()" );
	if( iTonePlayer )
    	{
        if( iTonePlayer->State() == EMdaAudioToneUtilityPlaying )
	        {
	            iTonePlayer->CancelPlay();
	        }
        else
	        {
	            iTonePlayer->CancelPrepare();
	        }
    	}

    if( iAudioPlayer )
    	{
        iAudioPlayer->Stop();
        iAudioPlayer->Close();
    	}

    if( iTonePlayer )
    	{
		delete iTonePlayer;
		iTonePlayer = NULL;
    	}

    if( iAudioPlayer )
    	{
		delete iAudioPlayer;
		iAudioPlayer = NULL;
    	}
	
	iPlayerState = EStop;
    EVTUIDEBUG( "- CEvtTonePlayer::Stop()" );
	}
	
// ---------------------------------------------------------------------------
// CEvtTonePlayer::HandleIncomingCallL()
// ---------------------------------------------------------------------------
//
void CEvtTonePlayer::HandleIncomingCallL()
    {
    EVTUIDEBUG( "= CEvtTonePlayer::HandleIncomingCallL()" );
    StopTimer();
	Stop();
		
	// Notify the Tone Player Observer
	iObserver.TonePlayCompleteL( EFalse );
    }
	
// ---------------------------------------------------------------------------
// CEvtTonePlayer::HandleProfileSettingsChange()
// ---------------------------------------------------------------------------
//
void CEvtTonePlayer::HandleProfileSettingsChange()
    {
    // Nothing to do now.
    }

// ---------------------------------------------------------------------------
// CEvtTonePlayer::StartTimerL
// ---------------------------------------------------------------------------
//
void CEvtTonePlayer::StartTimerL( TInt aTimeInterval )
	{
        if( !iTimer )
             {
             iTimer = CPeriodic::NewL( 0 ); // neutral priority
             }
        iTimer->Start( TTimeIntervalMicroSeconds32( aTimeInterval * KMicroSeconds ),
                       TTimeIntervalMicroSeconds32( aTimeInterval * KMicroSeconds ),
                       TCallBack(TickL, this));
	}

// ---------------------------------------------------------------------------
// CEvtTonePlayer::StopTimer
// ---------------------------------------------------------------------------
//
void CEvtTonePlayer::StopTimer( )
	{
    	if( iTimer )
        	{
        	iTimer->Cancel();
        	}
	}

// ---------------------------------------------------------------------------
// CEvtTonePlayer::TickL()
// ---------------------------------------------------------------------------
//
TInt CEvtTonePlayer::TickL(TAny* aObject)
    {
		((CEvtTonePlayer*)aObject)->StopPlayingL( ETrue );
		
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// Set volume between zero and player's maximum volume
// -----------------------------------------------------------------------------
//
TInt CEvtTonePlayer::PlayerVolume(TInt aPlayerMaxVol, TInt aVol)
{    
    const TInt KMaxProfileVolume( 10 );
    TInt vol( 0 ); // leave as 0 if using off tone
    // calculate value between 0 and aPlayerMaxVolume
    vol = Max( 0, Min( aPlayerMaxVol / KMaxProfileVolume * aVol, aPlayerMaxVol ) ); 
 
    return vol;
}
