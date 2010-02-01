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
* Description:  This class provides functionality for editing Events.
*
*/



#ifndef EVTEVENTSDLG_H
#define EVTEVENTSDLG_H

//  INCLUDES
#include <e32base.h>            // CBase

// User Includes

// FORWARD DECLARATIONS
class CEvtEvent;
class CEvtEditor;
class MObjectProvider;
class TCoeHelpContext;
class TCoordinate;
class CEvtMgmtUiEngine;

// CLASS DECLARATION

/**
*  This is a dialog class, which is used to launch Events editor dialog.
*/
class CEvtEventsDlg : public CBase
    {
    public:  // Constructors and destructor
        /**
        * This is a static function, which creates and returns an instance of this class.
        * The Event that has to be edited is shown in the editor.
    	*
        * @leave  Leaves with KErrNotSupported if framework functionality is not available.
        * @panic  Panics with system-wide panic codes.
        * @return new instance of this class
        */
        IMPORT_C static CEvtEventsDlg* NewL(TBool                              aIsEditable, 
                                            CEvtEvent&                         aEvent,                               
                                            CEvtMgmtUiEngine&                  aEventEngine,
                                            TBool                              aDisplayExitMenuItem = ETrue
                                            ); 

        /**
        * Destructor.
        */
        IMPORT_C ~CEvtEventsDlg();

    public: // New functions
        /**
        * This function sets the context - that is, the enclosing parent control - for this control.
        *
        * @param [in] aParent The parent object which is the context for the control.
        * @panic Panics with KLmkPanicNullMember, if the selector is not
        *        constructed properly.
        */
        IMPORT_C void SetMopParent( MObjectProvider* aParent );

        /**
        * This method sets help context. This provides an option to show the help context
        * of the application that launches editor. By default, we show the Events Ui help context.
        * @param[in] aContext help context to use
        */
        IMPORT_C void SetHelpContext( TCoeHelpContext aContext );

        /**
        * This function launches the Events editor dialog. Client uses this function
        * to launch events editor dialog.
        * This object is destroyed when this function returns or leaves.
        * @leave Leaves with system-wide leave codes.
        * @panic Panics with KEvtPanicNullMember, if the selector is not constructed properly.
        * @return Returns non-zero if accepted, else zero.
        */          
        IMPORT_C TInt ExecuteLD( );
    private:
        /**
        * C++ default constructor.
        * @return newly instantiated object
        */
        CEvtEventsDlg();
        		
        /**
        * By default Symbian 2nd phase constructor is private.        
        */
        
        void ConstructL( TBool                              aIsEditable, 
                        CEvtEvent&                         aEvent,        
                        CEvtMgmtUiEngine&                  aEventEngine,
                        TBool                              aDisplayExitMenuItem = ETrue
                        );
        

    private:    // Data
    
        /// Own: Editor object
        CEvtEditor* iEvtEditor;  

        /// Own: A pointer to TBool
        TBool* iDestroyedPtr;

    };

#endif      // EVTEVENTSDLG_H   
            
// End of File
