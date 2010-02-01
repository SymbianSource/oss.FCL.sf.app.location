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
* Description:  Application class for Events Handler Application UI.
*
*/


#ifndef C_EVTHANDLERAPPUI_H
#define C_EVTHANDLERAPPUI_H

// SYSTEM INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknViewAppUi.h>

#include "evtdlglauncher.h"	// Dialog Launcher
#include "evtdefs.h"
#include "evtrequesttype.h"

// FORWARD DECLARATIONS
class CEvtRequest;	
class CEvtMgmtUiEngine;
	
/**
 *  Events Handler Application's App UI class. This class extents the CAknAppUi
 *  class for Events Handler Application.
 *
 *  @since S60 v5.0
 */
class CEvtHandlerAppUi : public CAknAppUi,
                 public MEvtDlgObserver
    {
public:   
	/**
	 * Second phase of the two phase constructor
	 *
	 * Called by the Application framework
	 */ 
    void ConstructL();
  
  	/**
  	 * Virtual destructor
  	 */
    virtual ~CEvtHandlerAppUi();   
  
public: // Inherited from Base classes

  	/**
  	 * Inherited from CAknViewAppUi
  	 */
    void HandleCommandL(TInt aCommand);
        
    /**
     * Inherited from MEvtDlgObserver
     */
    void NotifyDlgDismissedL( );
                       
private: // Inherited from Base classes

  	/**
  	 * Inherited from CAknViewAppUi
  	 */
    virtual TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent,
                                                TEventCode aType );
                                                
    /**
     * Inherited from EEikAppUi
     */  
	TBool ProcessCommandParametersL( CApaCommandLine &aCommandLine );
	
    /**
     * Inherited from EEikAppUi
     */  
	void HandleWsEventL(const TWsEvent &aEvent, CCoeControl *aDestination);
	
    /**
     * Inherited from EEikAppUi
     */                                                
    void ProcessMessageL( TUid aUid, 
                          const TDesC8& aParams );   
	
    /**
     * Method to handle the Next Message in the Queue
     */                                 
	void HandleNextMessageL( );
	
    /**
     * Method to handle the Recieved message
     */                                           
	void HandleRecievedMessageL( const TLbtTriggerId &  aTriggerId, 
                      TEvtRequestType aReqestType );
                           
private:
    /**
     * Events Engine
     */
    CEvtMgmtUiEngine* iEngine;
    
	/**
	 * Generic Dialog Launcher for Events
	 */       
    CEvtDlgLauncher*    		iDlg;
    
	/**
	 * Stack which maitains the requests for Events
	 */       
    RPointerArray<CEvtRequest>   iEventQue;
    
	/**
	 * Flag to maintain the Frameworks's Cancel method.
	 */       
    TBool iFlag;
    };

#endif  // C_EVTHANDLERAPPUI_H

// End of File
