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


#ifndef C_EVTPROFILEHANDLER_H
#define C_EVTPROFILEHANDLER_H

// System Includes
#include <e32base.h>
#include <cenrepnotifyhandler.h>

//Forward Declaration

// Class Definition
/**
 *  Interface class to observe for profile settings change.
 *
 *  @lib 
 *  @since S60 v9.1
 */
class MEvtProfileObserver
    {
public:	    
    /**
     * This will notify when there is a change in profile settings            
     */
    virtual void HandleProfileSettingsChange( )=0;

    };
    
/**
 *  Adapter class to get Profile Settings.
 *  This class notifies any changes to Profile Settings if 
 *  observer(MEvtProfileObserver) is set.
 *
 *  @lib 
 *  @since S60 v9.1
 */
NONSHARABLE_CLASS( CEvtProfileHandler ): public CBase,
										 public MCenRepNotifyHandlerCallback
{
public:
    /**
     * Constructs a new instance of Profile Handler.
     *
     * @return The new instance of Profile Handler object.
     * @leave System wide error code if the object creation fails.         
     */
    static CEvtProfileHandler* NewL( MEvtProfileObserver& aObserver );
    
    /**
     * Constructs a new instance of Profile Handler.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of Profile Handler object.
     * @leave System wide error code if the object creation fails.         
     */
    static CEvtProfileHandler* NewLC( MEvtProfileObserver& aObserver );  

    /**
    * Destructor.
    */
    virtual ~CEvtProfileHandler();

public:  // Public Functions	

        /**
        * Volume settings in the currently active profile.
        * @return Volume value of the active profile.
        */
        TInt Volume( );

        /**
        * Check if the Ringtype is silent.
        * @return ETrue if the ringing type is silent.
        */
        TBool IsSilentRingingType( );

        /**
        * Check if the Vibra settings is On/ff.
        * @return ETrue if the Vibra is On in Active Profile.
        */
        TBool IsVibra( );

private: 	// Methods derived from MCenRepNotifyHandlerCallback
        void HandleNotifyInt(TUint32 aId, TInt aNewValue);

private: 

    /**
     * Default C++ Constructor.
     */
	CEvtProfileHandler( MEvtProfileObserver& aObserver );

    /**
     * Second phase of the two phase constructor.
     */
    void ConstructL();

private: // data

	/**
	 * Profile Settings change Observer.
	 */
	 MEvtProfileObserver& iObserver;

	/**
	 * Cenrep Session to Profile Settings.
	 * Own
	 */
     CRepository* iProfileSession;

	/**
	 * Cenrep Session to Vibra Settings.
	 * Own
	 */
     CRepository* iVibraSession;
	 
	/**
	 * Handler to Cenrep Notifier
	 * Own
	 */
	CCenRepNotifyHandler* iVolumeNotifyHandler;
	 
	/**
	 * Handler to Cenrep Notifier
	 * Own
	 */
	CCenRepNotifyHandler* iTypeNotifyHandler;
	 
	/**
	 * Handler to Cenrep Notifier
	 * Own
	 */
	CCenRepNotifyHandler* iVibraNotifyHandler;
	
	/**
	 * Profile Volume Settings.
	 */
	TInt iRingingVolume;
	
	/**
	 * Profile Ringing Type Settings.
	 */
	TInt iRingingType;
	
	/**
	 * Profile Vibra Settings.
	 */
	TInt iVibra;
};

#endif C_EVTPROFILEHANDLER_H