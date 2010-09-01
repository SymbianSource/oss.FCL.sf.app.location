/*
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Provides Blid Application class methods.
*
*/


// INCLUDE FILES
#include <eikstart.h>
#include "CBlidApp.h"
#include "CBlidDocument.h"
#include "BlidUID.h"

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CBlidApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CBlidApp::AppDllUid() const
    {
    return TUid::Uid(KBLIDUID3);
    }
   
// ---------------------------------------------------------
// CBlidApp::CreateDocumentL()
// Creates CBlidDocument object
// ---------------------------------------------------------
//
CApaDocument* CBlidApp::CreateDocumentL()
    {
    return CBlidDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

LOCAL_C CApaApplication* NewApplication()
    {
    return new CBlidApp;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

// End of File
