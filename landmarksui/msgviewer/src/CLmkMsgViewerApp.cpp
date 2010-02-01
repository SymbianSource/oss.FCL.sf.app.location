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
* Description:    LandmarksUi Content File -    Provides LM Application class methods.
*
*/







// INCLUDE FILES
#include "CLmkMsgViewerApp.h"
#include "CLmkMsgViewerDocument.h"
#include "LmkUID.h"
#include<eikstart.h>


// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CLmkMsgViewerApp::AppDllUid()
// Returns application UID
// ---------------------------------------------------------
//
TUid CLmkMsgViewerApp::AppDllUid() const
    {
    return TUid::Uid( KLmkMsgViewerAppUID3 );
    }

// ---------------------------------------------------------
// CLocApp::CreateDocumentL()
// Creates CLmkMsgViewerDocument object
// ---------------------------------------------------------
//
CApaDocument* CLmkMsgViewerApp::CreateDocumentL()
    {
    return CLmkMsgViewerDocument::NewL( *this );
    }

// ================= OTHER EXPORTED FUNCTIONS ==============
//

LOCAL_C CApaApplication* NewApplication()
  {
  return new CLmkMsgViewerApp;
  }

GLDEF_C TInt E32Main()
  {
  return  EikStart::RunApplication(NewApplication);
  }

#if defined(_WINS_)
EXPORT_C TInt WinsMain(TDesC* /*aCmdLine*/)
  {
  //return EikStart::RunApplication(NewApplication, aCmdLine);
  return EikStart::RunApplication(NewApplication);
  }
#endif
// End of File
