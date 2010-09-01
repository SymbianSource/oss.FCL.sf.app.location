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
* Description:  Application class for Events Management UI.
*
*/


#ifndef C_EVTMGMTUIAPPUI_H
#define C_EVTMGMTUIAPPUI_H

// SYSTEM INCLUDES
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknViewAppUi.h>

// FORWARD DECLARATIONS
class CEikStatusPane;
class CEvtMgmtUiLegalNote;

/**
 * Events Management UI application class. This class extends the Location
 * S60 Application UI class to provide the Events management UI related
 * Application
 *
 * @lib evtmgmtui.exe
 * @since S60 v5.0
 */
class CEvtMgmtUiAppUi : public CAknViewAppUi
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
    virtual ~CEvtMgmtUiAppUi();  
  
public: // Inherited from Base classes

  	/**
  	 * Inherited from CAknViewAppUi
  	 */
    void HandleCommandL( TInt aCommand );
                       
private: // Inherited from Base classes
  	/**
  	 * Inherited from CAknViewAppUi
  	 */
    virtual TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent,
                                                TEventCode aType );
                                                
private:
    /**
     * Create Events UI title text
     */                      
    void MakeTitleL( TInt aResourceText );
    
private: //Data
    /**
     * Retrieve the application specific status pane
     */
    CEikStatusPane* StatusPane();
    
    /**
     * Legal Note
     */
    CEvtMgmtUiLegalNote* iLegalNote;
    };

#endif  // C_EVTMGMTUIAPPUI_H

// End of File
