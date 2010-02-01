/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   	LandmarksUi Content File
*				Provides LM Application class methods.
*
*/







// INCLUDE FILES
#include "CLmkApp.h"
#include "CLmkDocument.h"
#include "LmkUID.h"
#include <eikstart.h>
// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CLmkApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CLmkApp::AppDllUid() const
    {
    return TUid::Uid( KLmkAppUID3 );
    }

// ---------------------------------------------------------
// CLocApp::CreateDocumentL()
// Creates CLmkDocument object
// ---------------------------------------------------------
//
CApaDocument* CLmkApp::CreateDocumentL()
    {
    return CLmkDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============

LOCAL_C CApaApplication* NewApplication()
    {
    return new CLmkApp;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication( NewApplication );
    }

#if defined (__WINS__)
EXPORT_C TInt WinsMain( TDesC* /*aCmdLine*/ )
    {
    //return EikStart::RunApplication( NewApplication, aCmdLine );
    return EikStart::RunApplication( NewApplication);
    }
#endif // defined (__WINS__)

// End of File
