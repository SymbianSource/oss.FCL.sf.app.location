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
* Description:  Document class for Events Handler Application.
*
*/


// INCLUDE FILES
#include "evthandlerappdocument.h"
#include "evthandlerappui.h"

// ----------------- Member funtions for CEvtHandlerAppDocument class ---------

// ---------------------------------------------------------------------------
// CEvtHandlerAppDocument::CEvtHandlerAppDocument
// ---------------------------------------------------------------------------
//
CEvtHandlerAppDocument::CEvtHandlerAppDocument( CEikApplication&  aApp )
	:CAknDocument( aApp )    
    {
    }

// ---------------------------------------------------------------------------
// CEvtHandlerAppDocument::~CEvtHandlerAppDocument
// ---------------------------------------------------------------------------
//
CEvtHandlerAppDocument::~CEvtHandlerAppDocument()
    { 
    }

// ---------------------------------------------------------------------------
// CEvtHandlerAppDocument* CEvtHandlerAppDocument::NewL
// ---------------------------------------------------------------------------
//
CEvtHandlerAppDocument* CEvtHandlerAppDocument::NewL( CEikApplication&    aApp )     
    {
    CEvtHandlerAppDocument* self = new (ELeave) CEvtHandlerAppDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    return self;
    }
    
// ---------------------------------------------------------------------------
// CEvtHandlerAppDocument::ConstructL
// ---------------------------------------------------------------------------
//
void CEvtHandlerAppDocument::ConstructL()
    {
    }
    
// ---------------------------------------------------------------------------
// CEvtHandlerAppDocument::CEvtHandlerAppDocument
// ---------------------------------------------------------------------------
//    
CEikAppUi* CEvtHandlerAppDocument::CreateAppUiL()
    {
    return new (ELeave) CEvtHandlerAppUi;
    }

// End of File  
