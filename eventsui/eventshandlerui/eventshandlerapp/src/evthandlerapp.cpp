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
* Description:  App class for Events Handler Application UI.
*
*/


// SYSTEM INCLUDES
#include <eikstart.h>

// USER INCLUDES
#include "evthandlerapp.h"
#include "evthandlerappdocument.h"
#include "evthandlerappuid.hrh"

// ------------------------ Local Functions ----------------------------------

// ---------------------------------------------------------------------------
// CApaApplication* NewApplication
// ---------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    return new CEvtHandlerApp;
    }

// ----------------- Member funtions for CEvtHandlerApp-----------------------
	
// ---------------------------------------------------------------------------
// TUid CEvtHandlerApp::AppDllUid
// ---------------------------------------------------------------------------
//
TUid CEvtHandlerApp::AppDllUid() const
    {
    return TUid::Uid( KEvtHandlerAppUid );
    }

// ---------------------------------------------------------------------------
// CApaDocument* CEvtHandlerApp::CreateDocumentL
// ---------------------------------------------------------------------------
//
CApaDocument* CEvtHandlerApp::CreateDocumentL()
    {
    return CEvtHandlerAppDocument::NewL( *this );
    }

// ------------------------ Global Functions ----------------------------------

// ---------------------------------------------------------------------------
// TInt E32Main
// ---------------------------------------------------------------------------
//
GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

// End of File  

