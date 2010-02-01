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
* Description:  File Player Utility.
*
*/


#ifndef C_EVTTONEPLAYER_H
#define C_EVTTONEPLAYER_H

// System Includes
#include <e32base.h>
#include <DrmAudioSamplePlayer.h>
#include <mdaaudiotoneplayer.h>
#include <mda/common/controller.h>

// User Includes
#include "evtprofilehandler.h"
#include "evtcallhandler.h"

//Forward Declaration
class CEvtProfileHandler;

// Class Definition
/**
 *  Interface class to observe for completion of tone playing.
 *
 *  @lib 
 *  @since S60 v9.1
 */
class MEvtTonePlayObserver
    {
public:
    /**
     * This will notify the tone Player observer on the completion
     * of tone play.
     * @param aTimeout Boolean value to indicate if the tone play 
	 * was complete due to timeout.             
     */
    virtual void TonePlayCompleteL( TBool aTimeout )=0;

    };
    
/**
 *  Interface class to play an Audio File.
 *  This class allows for an audio file to be played. 
 *  This class also allows for monitoring any incoming calls and
 *  give the priority for the incoming call tone.
 *
 *  @lib 
 *  @since S60 v9.1
 */
NONSHARABLE_CLASS( CEvtTonePlayer ): public CBase, public MDrmAudioPlayerCallback,
													 public MMdaAudioToneObserver,
													 public MEvtProfileObserver,
													 public MEvtCallObserver
{
public:
    /**
     * Constructs a new instance of Tone Player.
     *
     * @return The new instance of Tone Player object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CEvtTonePlayer* NewL( MEvtTonePlayObserver& aObserver );
    
    /**
     * Constructs a new instance of Tone Player.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of Tone Player object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CEvtTonePlayer* NewLC( MEvtTonePlayObserver& aObserver );  

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CEvtTonePlayer();

public:  // Public Functions	
    /**
     * Initializes the player to play an audio file.
     *
     * @since S60 v9.1  
     * @param[in] aAudioFileName Audio file name
     * @param[in] aLoop Audio Loop for playing
     * @param[in] aTimeInterval Time Interval for playing in Seconds
     * @leave KErrNotFound If the file does not exist.
     */
	IMPORT_C void PlayFileL( const TFileName& aAudioFileName, 
				TBool aLoop, TInt aTimeInterval );
	
    /**
     * Stops the currently playing audio file.
     *
     * @since S60 v9.1  
     * @param[in] aAudioFileName Audio file name
     */
	IMPORT_C void StopPlayingL( TBool aTimeout );

private: // From MDrmAudioPlayerCallback
    /**
     * Framework will notify you the end of the playback by a call to
     * MapcPlayComplete().
     */
	void MdapcPlayComplete( TInt aError );
	
    /**
     * Framework will notify you the end of the initalizing of a player
     * file by a call to MdapcInitComplete().
     */
	void MdapcInitComplete( TInt aError, 
							const TTimeIntervalMicroSeconds& aDuration );
							
private: // from MMdaAudioToneObserver
    /**
     * Framework will notify you at the end of prepare is completed
     */
    void MatoPrepareComplete(TInt aError);
    
    /**
     * Framework will notify you the end of the playback.
     */
    void MatoPlayComplete(TInt aError);
							
private: // from MEvtProfileObserver
    void HandleProfileSettingsChange( );
    
    // from MEvtCallObserver
    void HandleIncomingCallL( );

private: 
    
    /**
     * Start the Timer.
     */
	void StartTimerL( TInt aTimeInterval );
    
    /**
     * Stop the Timer.
     */
	void StopTimer();
    
    /**
     * Stop the playing file.
     */
	void Stop();
	
    /**
     * Called when timer is expired
     */
    static TInt TickL( TAny* aObject );

    /**
     * Set volume between zero and player's maximum volume
     */    
    TInt PlayerVolume(TInt aPlayerMaxVol, TInt aVol);
    
    /**
     * Default C++ Constructor.
     */
	CEvtTonePlayer( MEvtTonePlayObserver& aObserver );

    /**
     * Second phase of the two phase constructor.
     */
    void ConstructL();

private: // Enums
	/**
	 * @enum TState
	 * Player State.
	 *
	 */
	enum TState
	    {
	    /**
	     * Player State for Not Ready.
	     */
	    ENotReady                            = 1,
	    /**
	     * Player State for Ready.
	     */
	    EReady                               = 2,
	    /**
	     * Player State for Playing.
	     */
	    EPlaying                             = 3,
	    /**
	     * Player State for Stop.
	     */
	    EStop                                = 4
	    };

private: // data

	/**
	 * Tone Player Observer.
	 */
	 MEvtTonePlayObserver& iObserver;
	 
	/**
	 * DRM Player instance.
	 * Own:
	 */
	CMdaAudioToneUtility* iTonePlayer;
	 
	/**
	 * DRM Player instance.
	 * Own:
	 */
	CDrmPlayerUtility* iAudioPlayer;
	 
	/**
	 * Profile Handler instance.
	 * Own:
	 */
	CEvtProfileHandler* iProfileHandler;
	 
	/**
	 * Call Handler instance.
	 * Own:
	 */
	CEvtCallHandler* iCallHandler;
	
	/**
	 * Periodic Timer active object.
	 * Own:
	 */
	CPeriodic* iTimer; 
	
	/**
	 * Audio Player State.
	 */
	TState iPlayerState;
};

#endif C_EVTTONEPLAYER_H