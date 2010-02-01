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
* Description:  Location Centre Server object.
*
*/


#ifndef C_LCREGISTRYUPDATE_H
#define C_LCREGISTRYUPDATE_H

/**
 *  @file lcregistryupdate.h
 *
 *  This file provides the definitions for the file server notifier class which enables
 *  lc server to listen to notifications(like new registration file addition/removal also for MMC removal/addition) 
 *  from file server.
 *
 *  @since S60 v5.0 
 */

// SYSTEM INCLUDES
#include <e32base.h>
#include <f32file.h>

//USER INCLUDE


//FORWARD DECLARATION
class MLcRegistryUpdateNotifier;

/**
 *  @class CLcRegistryUpdate
 *
 *  This class provides all the interface for the file server notifications
 *  (like new registration file addition/removal etc) from file server.
 *
 *  @since S60 v5.0
 */

NONSHARABLE_CLASS( CLcRegistryUpdate ) : public CActive
	{
public:
    /**
     * Constructs a new instance of CLcRegistryUpdate.
     *
     * @return The new instance of CLcRegistryUpdate object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcRegistryUpdate* NewL(MLcRegistryUpdateNotifier& aFileNotifier,
    								RFs& aFs);

    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Centre
     * server registry handler
     */
    virtual ~CLcRegistryUpdate();
    
private:
    /**
     * C++ Default constructor
     */
    CLcRegistryUpdate( MLcRegistryUpdateNotifier& aFileNotifier, RFs& aFs );
    
    /**
     * Private Second phase constructor
     */
    void ConstructL();

public:
	void StartFileNotifation();
	    
private: // From CActive
    /**
     * Inherited from CActive
     */
    void RunL();

    /**
     * Inherited from CActive
     */
    void DoCancel();		
    
    /**
     * Inherited from CActive
     */
    TInt RunError( TInt aError );
        
private: //data member
	MLcRegistryUpdateNotifier&		iFileNotifier;
	RFs&  iFs;    
	RTimer                          iTimer;
	TBool                           iFileNotifyPending;
	};
	
#endif // C_LCREGISTRYUPDATE_H

