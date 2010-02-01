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
* Description:  Application class for Events Handler Server.
*
*/


// SYSTEM INCLUDES
#include <eikstart.h>

// USER INCLUDES
#include "evtserverapp.h"
#include "evtserverappdocument.h"
#include "evthandlerserveruid.hrh"

// ------------------------ Local Functions ----------------------------------

// ----------------- Member funtions for CEvtServerApp ------------------------------
	
// ---------------------------------------------------------------------------
// TUid CEvtServerApp::AppDllUid
// ---------------------------------------------------------------------------
//
TUid CEvtServerApp::AppDllUid() const
    {
    return TUid::Uid( KEvtHandlerServerUid );
    }

// ---------------------------------------------------------------------------
// CApaDocument* CEvtServerApp::CreateDocumentL
// ---------------------------------------------------------------------------
//
CApaDocument* CEvtServerApp::CreateDocumentL()
    {
    return new (ELeave) CEvtServerAppDocument( *this );
    }

// End of File  

