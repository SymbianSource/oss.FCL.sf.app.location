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
* Description:  Document class for Events Management UI.
*
*/


// INCLUDE FILES
#include "evtmgmtuidocument.h"
#include "evtmgmtuiappui.h"

// ----------------- Member funtions for CEvtMgmtUiDocument class ------------------

// ---------------------------------------------------------------------------
// CEvtMgmtUiDocument::CEvtMgmtUiDocument
// ---------------------------------------------------------------------------
//
CEvtMgmtUiDocument::CEvtMgmtUiDocument( CEikApplication&  aApp )
	:CAknDocument( aApp )    
    {
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiDocument::~CEvtMgmtUiDocument
// ---------------------------------------------------------------------------
//
CEvtMgmtUiDocument::~CEvtMgmtUiDocument()
    { 
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiDocument* CEvtMgmtUiDocument::NewL
// ---------------------------------------------------------------------------
//
CEvtMgmtUiDocument* CEvtMgmtUiDocument::NewL( CEikApplication&    aApp )     
    {
    CEvtMgmtUiDocument* self = new (ELeave) CEvtMgmtUiDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    return self;
    }
    
// ---------------------------------------------------------------------------
// CEvtMgmtUiDocument::ConstructL
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiDocument::ConstructL()
    {
    }
    
// ---------------------------------------------------------------------------
// CEvtMgmtUiDocument::CEvtMgmtUiDocument
// ---------------------------------------------------------------------------
//    
CEikAppUi* CEvtMgmtUiDocument::CreateAppUiL()
    {
    return new (ELeave) CEvtMgmtUiAppUi;
    }

// End of File  
