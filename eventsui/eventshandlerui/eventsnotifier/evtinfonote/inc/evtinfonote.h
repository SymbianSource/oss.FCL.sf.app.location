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


#ifndef C_EVTINFONOTE_H
#define C_EVTINFONOTE_H

// System Includes
#include <eiknotapi.h>

// User Includes

// Forward Declarations
class CEikonEnv;
class CEvtInfoNoteImpl;
class CAknNotifierAppServerAppUi;
class CEvtInfoNoteInputParam;

/**
 * Info Note implementation for Event UI Triggers
 *
 * This class provides the notifier implementation which displays the
 * Events UI trigger notifiers. This class is displays the subject,
 * description and the accuracy levels of the triggers fired.
 *
 * @since S60 v9.1
 */
class CEvtInfoNote : public CActive,
                     public MEikSrvNotifierBase2
    {
private: // Constants and Enumerations
	enum TEvtInfoNoteEvent    
		{
		// No event
		ENone,
		
		// Display the event trigger info note
		EDisplayNotifier,
		
		};
		
public:
    /**
     * Static two phase constructor
     */
    static CEvtInfoNote* NewL();
    
    /**
     * Destructor.
     */
    virtual ~CEvtInfoNote();
    
public: // Inherited from MEikSrvNotifierBase2

    /**
     * Frees all the resources owned by the notifier
     */
    virtual void Release();

    /**
     * Gets the notifier parameters.
     */
    TNotifierInfo Info() const;
     
    /**
     * Starts the notifier.
     */
    virtual TPtrC8 StartL( const TDesC8&        aBuffer );
     
    /**
     * Starts the notifier. This is an asynchronous function. The completion
     * of the request can be communicated through the RMessagePtr2 object
     */
    virtual void StartL( const TDesC8&          aBuffer,
                               TInt             aReplySlot,
                         const RMessagePtr2&    aMessage );
     
    /**
     * Cancels an active notifier.
     */
    virtual void Cancel();
     
    /**
     * Updates a currently active notifier with new data.
     */
    virtual TPtrC8 UpdateL( const TDesC8&   aBuffer );
    
    /**
     * Updates a currently active notifier with new data.
     */    
    virtual void UpdateL( const TDesC8&         aBuffer, 
                                TInt            aReplySlot, 
                          const RMessagePtr2&   aMessage);
       
    /**
     * Registers the Info Note display
     */
    virtual TNotifierInfo RegisterL();
	                
protected:
    /**
     * C++ constructor made protected for this class must not be instantiated
     * directly
     */
    CEvtInfoNote();
    
    /**
     * Second phase of the two phase constructor
     */
    void ConstructL();
    
protected: // Inherited from CActive
    virtual void RunL();
    
    virtual void DoCancel();
    
    virtual TInt RunError( TInt aError ); 
        
private:
	/**
	 * Completes any outstanding request with the requested error code.
	 */
	void CompleteMessage( TInt          aError,
	                      TInt          aKeyPressEvent = ENone );
	                      
    /**
     * Converts the current input buffer into the Input parameter
     * structure.
     */
    CEvtInfoNoteInputParam*     GetInputParametersLC();
     
protected:    
    /**
     * Eikon Env pointer
     */
    CEikonEnv*                  iEikEnv;
        
    /**
     * Notifier information structure
     * Owns
     */
    TNotifierInfo               iInfo;
            
private:  // Data members

    /**
     * Handle to the current message
     */
    RMessagePtr2                iMessage;
    
    /** 
     * Confirmation query dialog
     */
    CEvtInfoNoteImpl*    	    iInfoNote;
    
    /**
     * Resource file handle
     */
    TInt                        iResourceFileFlag;
    
	/**
	 * Current Trigger event
	 */
	TEvtInfoNoteEvent			iTriggerEvent;
	
	/**
	 * Boolean flag to indicate whether App switching has been
	 * enabled.
	 */
	TBool                       iAppSwitchSupress;
	
	/**
	 * Reference to the Notifier App server's Application UI
	 */
	CAknNotifierAppServerAppUi* iAppUi;
	
	/**
	 * Input Buffer
	 * Owns
	 */
	HBufC8*                     iInputBuffer;
	
	/**
	 * Reply Slot
	 */
	TInt                        iReplySlot;
    };

#endif // C_EVTINFONOTE_H
