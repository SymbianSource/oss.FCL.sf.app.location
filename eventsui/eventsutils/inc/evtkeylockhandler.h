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
* Description:  Key Lock Settings Handler.
*
*/


#ifndef C_EVTKEYLOCKHANDLER_H
#define C_EVTKEYLOCKHANDLER_H

// System Includes
#include <e32base.h>
#include <aknkeylock.h> 
    
/**
 *  Interface class to maintain Key Lock Settings.
 *
 *  @lib 
 *  @since S60 v9.1
 */
NONSHARABLE_CLASS( CEvtKeyLockHandler ): public CBase
{
public:
    /**
     * Constructs a new instance of Key Lock Handler.
     *
     * @return The new instance of Key Lock Handler object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CEvtKeyLockHandler* NewL( );
    
    /**
     * Constructs a new instance of Key Lock Handler.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of Key Lock Handler object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CEvtKeyLockHandler* NewLC( );  

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CEvtKeyLockHandler();

public:  // Public Functions	

        /**
        * Maintains the Current State of Key Lock - Lock or Unlock.
        */
        IMPORT_C void MaintainState( );

        /**
        * Set the Flag to Enable or Diable the Key Lock.
        * @param TBool Flag to Enable/Diable the Key Lock.
        */
        IMPORT_C void EnableKeyLock( TBool aFlag );

        /**
        * Update the updated state of Key Lock With Note - Lock or Unlock.
        */
        IMPORT_C void UpdateStateWithNote( );

        /**
        * Update the updated state of Key Lock Without Note - Lock or Unlock.
        */
        IMPORT_C void UpdateStateWithoutNote( );

private: 

    /**
     * Default C++ Constructor.
     */
	CEvtKeyLockHandler( );

    /**
     * Second phase of the two phase constructor.
     */
    void ConstructL();

private: // data

	/**
	 * Handle to Key Lock Settings.
	 */
	 RAknKeylock2 iKeyLock;

	/**
	 * Key Lock Settings.
	 */
     TBool iKeyLocked;
};

#endif C_EVTKEYLOCKHANDLER_H