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
* Description:  Events Dialog Launcher.
*
*/


#ifndef C_EVTDLGLAUNCHER_H
#define C_EVTDLGLAUNCHER_H

// System Includes
#include <e32base.h>

#include "evtmgmtuiengine.h"
#include "evtdefs.h"

// Forward Declarations
class CEvtEventsDlg;
class CEvtRequest;	

/**
 * Events Dialog Exit Handler.
 * 
 * @since v5.0 
 */
class MEvtDlgObserver
    {
public:
    /**
     * It will be used to notify observer that event editor is 
     * dismissed         
     */    
    virtual void NotifyDlgDismissedL() = 0;
    };

/**
 *  Interface class to Handle the Event's Dialog launching Request.
 *  This class allows for 
 *  1. launching of an event viewer dialog.
 *  2. launching of Notepad Editor for Viewing description.
 *  3. Cancelling of any outstanding Dialogs
 *
 *  @lib 
 *  @since S60 v9.1
 */
class CEvtDlgLauncher : public CActive,
                              public MEvtMgmtUiEngineObserver
{
public:
    /**
     * Constructs a new instance of Dialog Launcher.
     *
     * @return The new instance of Dialog Launcher object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CEvtDlgLauncher* NewL( CEvtMgmtUiEngine& aEngine, MEvtDlgObserver& aObserver );
    
    /**
     * Constructs a new instance of Dialog Launcher.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of Dialog Launcher object.
     * @leave System wide error code if the object creation fails.         
     */
    IMPORT_C static CEvtDlgLauncher* NewLC( CEvtMgmtUiEngine& aEngine, MEvtDlgObserver& aObserver );  

    /**
    * Destructor.
    */
    virtual ~CEvtDlgLauncher();

public:  // Public Functions	
    /**
     * Launch the dialog for the given Event Request.
     *
     * @since S60 v9.1  
     * @param[in] aRequest Event Request
     * @leave System wide Leave error code.
     */
	void LaunchDlgL( CEvtRequest& aRequest );
	
    /**
     * Cancel the currently open dialog.
     *
     * @since S60 v9.1  
     * @leave System wide Leave error code.
     */
	void CancelDlgL( TBool aIsForcedCancel );

    /**
     * Inherited from MEvtMgmtUiEngineObserver
     */
    void NotifyEventChangeL(TInt aErrorCode, TInt aState );
    
    /**
     * Inherited from MEvtMgmtUiEngineObserver
     */
    void NotifyModelChangeL();
    
private: // Inherited from CActive
    virtual void RunL();
    
    virtual void DoCancel();
    
    virtual TInt RunError( TInt aError ); 

private: 

    /**
     * Default C++ Constructor.
     */
	CEvtDlgLauncher( CEvtMgmtUiEngine& aEngine, MEvtDlgObserver& aObserver );

    /**
     * Second phase of the two phase constructor.
     */
    void ConstructL();
    
private: // data

    /**
     * Observer for Dialog Exit
     */ 
     MEvtDlgObserver& iObserver;
     
    /**
     * Events Engine
     */
    CEvtMgmtUiEngine& iEngine;
    
    /**
     * Event for which the Dialog has to be launched
     * Not owned
     */
    CEvtRequest* iRequest;
    
    /**
     * Dialog Launcher for Editing the Events.
     */
    CEvtEventsDlg* iDialog;
        
};

#endif C_EVTDLGLAUNCHER_H