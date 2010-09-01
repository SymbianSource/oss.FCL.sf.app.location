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
* Description:    LandmarksUi Content File -    Responsible for launching positioning settings
*
*/






#ifndef CLmkLocSettingsLauncher_H
#define CLmkLocSettingsLauncher_H

// INCLUDES
#include <e32base.h>
#include <locsettingsuiclient.h>

// CLASS DECLARATION
/**
*  CLmkLocSettingsLauncher class
*  This class is a wrapper for launching Position settings
*/
class CLmkLocSettingsLauncher : public CActive
	{
	private : // Constructors and destructor
	      /**
	       * C++ default constructor.
	       */
	      CLmkLocSettingsLauncher();

	public:
	      /**
	       * Destructor.
	       */
	      ~CLmkLocSettingsLauncher();

	public:
		/**
         * Two-phased constructor.
         *
         */
	     static CLmkLocSettingsLauncher* NewL();

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

#endif // CLmkLocSettingsLauncher_H

// End of file