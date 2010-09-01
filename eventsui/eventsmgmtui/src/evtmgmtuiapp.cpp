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


// SYSTEM INCLUDES
#include <eikstart.h>

// USER INCLUDES
#include "evtmgmtuiapp.h"
#include "evtmgmtuidocument.h"
#include "evtmgmtuiuid.hrh"

// ------------------------ Local Functions ----------------------------------

// ---------------------------------------------------------------------------
// CApaApplication* NewApplication
// ---------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    return new CEvtMgmtUiApp;
    }

// ----------------- Member funtions for CEvtMgmtUiApp ------------------------------
	
// ---------------------------------------------------------------------------
// TUid CEvtMgmtUiApp::AppDllUid
// ---------------------------------------------------------------------------
//
TUid CEvtMgmtUiApp::AppDllUid() const
    {
    return TUid::Uid( KEvtMgmtUiUid );
    }

// ---------------------------------------------------------------------------
// CApaDocument* CEvtMgmtUiApp::CreateDocumentL
// ---------------------------------------------------------------------------
//
CApaDocument* CEvtMgmtUiApp::CreateDocumentL()
    {
    return CEvtMgmtUiDocument::NewL( *this );
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

