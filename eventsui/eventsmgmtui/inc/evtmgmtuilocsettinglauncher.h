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
* Description:  Responsible for launching positioning settings
*
*/

#ifndef EvtMgmtUiLocSettingsLauncher_H
#define EvtMgmtUiLocSettingsLauncher_H

// INCLUDES
#include <e32base.h>
#include <locsettingsuiclient.h>

// CLASS DECLARATION
/**
*  CEvtMgmtUiLocSettingsLauncher class
*  This class is a wrapper for launching Position settings
*/
class CEvtMgmtUiLocSettingsLauncher : public CActive
	{
	private : // Constructors and destructor
	      /**
	       * C++ default constructor.
	       */
	    CEvtMgmtUiLocSettingsLauncher();
	      
	public:
	      /**
	       * Destructor.
	       */
	      ~CEvtMgmtUiLocSettingsLauncher();		
	
	public:	
		/**
         * Two-phased constructor.
         *
         */
	     static CEvtMgmtUiLocSettingsLauncher* NewL();
			      
	private: 
		 /**
          * Second phase of construction.
          */
	      void ConstructL();
	      
	public: // new functions
	      void LaunchL();	      
	      	      
	protected: // from CActive
	      void RunL();
	      
	      void DoCancel();
	      
	      void CancelRequest();
	      
	private:
	    // Pointer to Location setings client library
        CLocSettingsUiClient*       iClientLibrary;
	};
	
#endif // EvtMgmtUiLocSettingsLauncher_H

// End of file