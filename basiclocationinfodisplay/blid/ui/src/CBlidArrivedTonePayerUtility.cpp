/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Blid application arrived tone player utility.
*
*/


// SYSTEM INCLUDES

// USER INCLUDES
#include "CBlidArrivedTonePayerUtility.h"
#include "bliduiconsts.h"
#include "Debug.h"


// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CBlidArrivedTonePlayerUtility::CBlidArrivedTonePlayerUtility
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CBlidArrivedTonePlayerUtility::CBlidArrivedTonePlayerUtility( )
    {
    }

// -----------------------------------------------------------------------------
// CBlidArrivedTonePlayerUtility::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CBlidArrivedTonePlayerUtility::ConstructL()
    {
    iAudioPlayer = CMdaAudioPlayerUtility::NewL(
        *this,
        EMdaPriorityNormal,
        EMdaPriorityPreferenceTimeAndQuality );
    }

// -----------------------------------------------------------------------------
// CBlidArrivedTonePlayerUtility::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CBlidArrivedTonePlayerUtility* CBlidArrivedTonePlayerUtility::NewL( )
    {
    CBlidArrivedTonePlayerUtility* self = new( ELeave ) CBlidArrivedTonePlayerUtility( );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// ---------------------------------------------------------------------------
// CBlidArrivedTonePlayerUtility::~CBlidArrivedTonePlayerUtility
// Destructor
// ---------------------------------------------------------------------------
//
CBlidArrivedTonePlayerUtility::~CBlidArrivedTonePlayerUtility()
	{
    if ( iAudioPlayer )
        {
        iAudioPlayer->Stop();
        iAudioPlayer->Close();
        }
    delete iAudioPlayer;
    iAudioPlayer = NULL;	
	}
  
  
// ---------------------------------------------------------
// CBlidArrivedTonePlayerUtility::PlayToneL
// Plays the appropriate tone
// ---------------------------------------------------------
//
void CBlidArrivedTonePlayerUtility::PlayToneL( const TFileName& aFileName )
    {
    iFileName = aFileName;
    CanclePayer();
    iAudioPlayer->OpenFileL( aFileName );        
    }

// ---------------------------------------------------------------------------
// From MMdaAudioPlayerCallback
// Callback when an attempt to open and initialise an audio sample 
// has completed.
// ---------------------------------------------------------------------------
//
void CBlidArrivedTonePlayerUtility::MapcInitComplete(
    TInt aError,
    const TTimeIntervalMicroSeconds& aDuration )
    {
    if( aError == KErrNone )
	    {
	    iAudioDuration = aDuration;
        if ( iAudioPlayer )        
            {
    	    iAudioPlayer->Play();	
            }
	    }
	// Do nothing
    }

// ---------------------------------------------------------------------------
// From MMdaAudioPlayerCallback
// Callback when an attempt to playback an audio sample has completed.
// ---------------------------------------------------------------------------
//
void CBlidArrivedTonePlayerUtility::MapcPlayComplete(
    TInt aError )
    {
    if( aError == KErrNone )
	    {
	    TInt duration = I64INT( iAudioDuration.Int64() );
        if( duration < KMinArrivalTime )
	        {
	        if ( iAudioPlayer )        
	            {
	            CanclePayer();
	    	    TRAP_IGNORE( iAudioPlayer->OpenFileL( iFileName ) );	
	            }
	        }
	    }
    }

// ---------------------------------------------------------------------------
// CBlidArrivedTonePlayerUtility::CanclePayer().
// ---------------------------------------------------------------------------
//
void CBlidArrivedTonePlayerUtility::CanclePayer()
	{
    if ( iAudioPlayer )
        {
        iAudioPlayer->Stop();
        iAudioPlayer->Close();
        }
	}
//End Of file

