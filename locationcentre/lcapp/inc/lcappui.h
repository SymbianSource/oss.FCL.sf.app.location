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
* Description:  Application class for Location Centre Application UI.
*
*/


#ifndef C_LCAPPUI_H
#define C_LCAPPUI_H

// SYSTEM INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknViewAppUi.h>
#include <lcnotification.h>

// FORWARD DECLARATIONS
class CLcView;
class CLcService;
class CLcListBoxModel;

/**
 * Location Centre Application Event Handler.
 * 
 * @since v5.0 
 */
class MLcEventHandler
	{
public:
	/**
	 * Abstract function which is implemented by the Location Centre Event handler.
	 * The implmentation must handle the event specified by the Event type.
	 *
	 * @param[in] aEvent, Event Type.
	 */
	virtual void HandleEventL( TInt		aEvent ) = 0;
	};
	
/**
 *  Location Centre Application's App UI class. This class extents the CAknAppUi
 *  class for Location Centre.
 *
 *  @since S60 v5.0
 */
class CLcAppUi : public CAknViewAppUi,
                 public MLcNotification,
                 public MLcEventHandler
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
    virtual ~CLcAppUi();   
  
public: // Inherited from Base classes

  	/**
  	 * Inherited from CAknViewAppUi
  	 */
    void HandleCommandL(TInt aCommand);    
    
  	/**
  	 * Inherited from MLcNotification
  	 */    
    void LcStatusChangedL( TInt aErrorCode );
        
    /**
     * Inherited from MLcEventHandler
     */
    void HandleEventL( TInt	aEvent );
                       
private: // Inherited from Base classes

  	/**
  	 * Inherited from CAknViewAppUi
  	 */
    virtual TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent,
                                                TEventCode aType );
                           
private:
	/**
	 * Location Centre Application's view class
	 */       
    CLcView*    		iLcView;
    
    /**
     * Location Centre API interface object
     *
     * Owns
     */
    CLcService*    		iLcService;
    
    /**
     * Location Centre list box model
     *
     * Owns
     */
    CLcListBoxModel*	iListBoxModel;
    };

#endif  // C_LCAPPUI_H

// End of File
