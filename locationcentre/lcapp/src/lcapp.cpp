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
* Description:  App class for Location Centre Application UI.
*
*/


// SYSTEM INCLUDES
#include <eikstart.h>

// USER INCLUDES
#include "lcapp.h"
#include "lcdocument.h"
#include "lcappuid.hrh"

// ------------------------ Local Functions ----------------------------------

// ---------------------------------------------------------------------------
// CApaApplication* NewApplication
// ---------------------------------------------------------------------------
//
LOCAL_C CApaApplication* NewApplication()
    {
    return new CLcApp;
    }

// ----------------- Member funtions for CLcApp ------------------------------
	
// ---------------------------------------------------------------------------
// TUid CLcApp::AppDllUid
// ---------------------------------------------------------------------------
//
TUid CLcApp::AppDllUid() const
    {
    return TUid::Uid( KLcAppUid );
    }

// ---------------------------------------------------------------------------
// CApaDocument* CLcApp::CreateDocumentL
// ---------------------------------------------------------------------------
//
CApaDocument* CLcApp::CreateDocumentL()
    {
    return CLcDocument::NewL( *this );
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

