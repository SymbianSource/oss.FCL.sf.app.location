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
* Description:    LandmarksUi Content File -    This file contains class which implements methods for
*                interfacing with launching browser
*
*/






#ifndef CLMKGOTOURLCMD_H
#define CLMKGOTOURLCMD_H

//  INCLUDES
#include <browserlauncher.h>
#include <downloadedcontenthandler.h>
#include <AknServerApp.h>

class MDownloadedContentHandler;
class MAknServerAppExitObserver;
class CAiwGenericParamList;
class CBrowserLauncher;

// CLASS DECLARATION
/**
* TLmkLauncherContentHandler class
*
*/
class TLmkLauncherContentHandler
: public MDownloadedContentHandler,
  public MAknServerAppExitObserver
    {
    public:
        TLmkLauncherContentHandler();
        TBool HandleContentL( const TDesC& aFileName,
                              const CAiwGenericParamList& aParamList,
                              TBool& aContinue );
        TAny* DownloadedContentHandlerReserved1( TAny* /*aAnyParam*/ );
        void HandleServerAppExit( TInt aReason );
    };

/**
* CLmkGoToURLCmd class
* This class implements the methods for interacting with Browser launcher
* API for launching web browser
*/
class CLmkGoToURLCmd : public CBase
	{
	public:  // Constructors and destructor

		/**
		* Two-phased constructor.
		* @return
		*/
		IMPORT_C static CLmkGoToURLCmd* NewL();

		/**
		* Destructor.
		*/
		virtual ~CLmkGoToURLCmd();

	private:  // Constructors and destructor
		/**
		* Default Constructor
		*/
		CLmkGoToURLCmd();

		/**
		* By default Symbian 2nd phase constructor is private.
		*/
		void ConstructL( );

	public:
	    /**
	    * Launches Browser for the given web address
	    * @param aUrl web address
	    */
		void LaunchBrowserL(const TDesC& aUrl);

	private:
		// Data
		TLmkLauncherContentHandler* iContentHandler;
		CBrowserLauncher* iLauncher;
	};
#endif
//  End of File
