/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Responsible for launching positioning settings
*
*/

#ifndef EVTMGMTUILOCSETTINGSLAUNCHER_H
#define EVTMGMTUILOCSETTINGSLAUNCHER_H

// INCLUDES
#include <e32base.h>
#include <locsettingsuiclient.h>

class MEvtMgmtUiLocSettingsLauncherObserver;
// CLASS DECLARATION
/**
*  CEvtMgmtUiLocSettingsLauncher class
*  This class is a wrapper for launching Position settings
*/

class CEvtMgmtUiLocSettingsLauncher : public CActive
                                      
    {     
    public: // Constructors and destructor
    /**
     * Two-phased constructor.
     *
     */
     static CEvtMgmtUiLocSettingsLauncher* NewL();
                  
     /**
      * Destructor.
      */
     ~CEvtMgmtUiLocSettingsLauncher();             
          
    public: // new functions
       /*
       * It is used to launch position settings
       * 
       * @since Symbian v9.1
       *  
       */
       void LaunchL();    
       
       /*
        * It is used to set observer which will get notification 
        * after completion of operation.
        * 
        * @since Symbian v9.1
        *  
        */
       void SetObserver( MEvtMgmtUiLocSettingsLauncherObserver* aObserver );
       
    protected: // from CActive
      /*
       *  Derived from CActive
       */
      void RunL();
      /*
       *  Derived from CActive
       */ 
      void DoCancel();
          
    private: 
     /**
      * Second phase of construction.
      */
      void ConstructL();      
      
      /**
       * C++ default constructor.
       */
      CEvtMgmtUiLocSettingsLauncher();
      
    private:
        // Owns: Pointer to Location setings client library
        CLocSettingsUiClient*                   iClientLibrary;
        //Does not own : Observer to which completion of operation is notified.
        MEvtMgmtUiLocSettingsLauncherObserver*  iObserver;
    };
    
#endif // EVTMGMTUILOCSETTINGSLAUNCHER_H

// End of file
