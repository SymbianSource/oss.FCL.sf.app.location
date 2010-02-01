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
* Description:  Info Note implementation
*
*/


#ifndef C_EVTINFONOTEIMPL_H
#define C_EVTINFONOTEIMPL_H

// System Includes
#include <e32base.h>
#include <AknQueryDialog.h>

// User Includes
#include "evttoneplayer.h"
#include "evtinfonoteconsts.hrh"

// Forward Declarations
class CEikonEnv;
class CEvtInfoNoteInputParam;
class CEvtTonePlayer;
class CEvtKeyLockHandler;

/**
 * Info Note implementation for Event UI Triggers
 *
 * This class provides the notifier pop-up implementation.
 *
 * @since S60 v9.1
 */
NONSHARABLE_CLASS( CEvtInfoNoteImpl ): public CAknQueryDialog, 
									   public MEvtTonePlayObserver
    {
public:
    /**
     * Static two phase constructor
     */
    static CEvtInfoNoteImpl* NewLC( CEvtInfoNoteInputParam*      aInputParam );
    
    /**
     * Destructor.
     */
    virtual ~CEvtInfoNoteImpl();
    
public:
    // Inherited from CAknQueryDialog    
    TInt RunLD();
    
    // Inherited from MEvtTonePlayObserver 
    void TonePlayCompleteL( TBool aTimeout );
    
    //     
    TInt RunLD( TEvtInfoNoteCmds* aKey );
                                       
protected:
    // Inherited from CAknQueryDialog
    TBool OkToExitL( TInt aButtonId );
    
    void PostLayoutDynInitL();        

    /**
     * Handles pointer events
    */
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);                                   

private:
    /**
     * Get Msk Option from Popup
     */
	TInt GetMskOptionL();
    
    /**
     * Default Constructor Implementation
     */
    CEvtInfoNoteImpl( CEvtInfoNoteInputParam*         aInputParam );
    
    /**
     * Called when timer is expired
     */
    static TInt TickL( TAny* aObject );
    
    /**
     * Second phase constructor
     */
    void ConstructLC();
    
    /**
     * Loads the Image for the control
     */
    void LoadImageL();
        
private:

    /**
     * Input parameters for the dialog
     * Owns
     */
    CEvtInfoNoteInputParam*         iInputParam;
    
    /**
     * Tone player
     * Owns
     */
    CEvtTonePlayer*                 iTonePlayer;
    
    /**
     * Key Lock Handler
     * Owns
     */
    CEvtKeyLockHandler* 			iKeyLockHandler;
    
    /**
     * Dialog Exit Flag
     */
    TEvtInfoNoteCmds* iExitFlag;
    
    /**
     * Periodic Timer active object.
     * Own:
     */
    CPeriodic* iTimer; 
    
    };

#endif // C_EVTINFONOTEIMPL_H
