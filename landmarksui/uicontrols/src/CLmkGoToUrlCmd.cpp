/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   This file contains methods which implements methods for
*                interfacing with launching browser
*
*/






// INCLUDE FILES
#include <BrowserLauncher.h>
#include <DownloadedContentHandler.h>
#include "clmkgotourlcmd.h"

_LIT(KProtocolForURL, "4 ");
//============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TLmkLauncherContentHandler::TLmkLauncherContentHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
TLmkLauncherContentHandler::TLmkLauncherContentHandler()
	{
	}

// -----------------------------------------------------------------------------
// TLmkLauncherContentHandler::HandleContentL
// -----------------------------------------------------------------------------
//
TBool TLmkLauncherContentHandler::HandleContentL(const TDesC& /*aFileName*/,
												 const CAiwGenericParamList& /*aParamList*/,
												 TBool& aContinue )
	{
	aContinue = ETrue;
	return EFalse;//ETrue;
	}

// -----------------------------------------------------------------------------
// TLmkLauncherContentHandler::DownloadedContentHandlerReserved
// -----------------------------------------------------------------------------
//
TAny* TLmkLauncherContentHandler::DownloadedContentHandlerReserved1( TAny* /*aAnyParam*/ )
	{
	// Empty
	return NULL;
	}

// -----------------------------------------------------------------------------
// TLmkLauncherContentHandler::HandleServerAppExit
// -----------------------------------------------------------------------------
//
void TLmkLauncherContentHandler::HandleServerAppExit( TInt /*aReason*/ )
	{
	}

// -----------------------------------------------------------------------------
//  CLmkGoToURLCmd::CLmkGoToURLCmd
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
 CLmkGoToURLCmd::CLmkGoToURLCmd()
    {
    }

// ----------------------------------------------------
//  CLmkGoToURLCmd::ConstructL
// ----------------------------------------------------
//
void CLmkGoToURLCmd::ConstructL( )
    {
     /* Nothing right now */

    }

// -----------------------------------------------------------------------------
// CLmkGoToURLCmd::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkGoToURLCmd* CLmkGoToURLCmd::NewL()
    {
    CLmkGoToURLCmd* self =
        new( ELeave ) CLmkGoToURLCmd;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CLmkGoToURLCmd::~CLmkGoToURLCmd()
// ----------------------------------------------------
//
CLmkGoToURLCmd::~CLmkGoToURLCmd()
    {
    delete iContentHandler;
	delete iLauncher;
    }

// ----------------------------------------------------
// CLmkGoToURLCmd::LaunchBrowserL()
// ----------------------------------------------------
//
void CLmkGoToURLCmd::LaunchBrowserL(const TDesC& aUrl)
	{
	delete iContentHandler;
	delete iLauncher;

	iContentHandler = NULL;
	iLauncher = NULL;

	/**
	* From the Browser API specification
	* 4 means that there is URL coming after that.
	* So the message format is "4 http://www.mydomain.com".
	*/

	HBufC* parameter = HBufC::NewLC( KProtocolForURL().Length() +  aUrl.Length() );
	parameter->Des().Copy( KProtocolForURL );
	parameter->Des().Append( aUrl );
	iContentHandler = new (ELeave) TLmkLauncherContentHandler;
	iLauncher = CBrowserLauncher::NewL();
	iLauncher->LaunchBrowserEmbeddedL( *parameter, iContentHandler, iContentHandler );
	CleanupStack::PopAndDestroy(parameter);
	}

//  End of File
