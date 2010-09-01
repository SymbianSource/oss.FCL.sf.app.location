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
* Description:  Document class for Events Handler Server.
*
*/


// INCLUDE FILES
#include "evtserverappdocument.h"
#include "evtserverappui.h"

// ----------------- Member funtions for CEvtServerAppDocument class ------------------

// ---------------------------------------------------------------------------
// CEvtServerAppDocument::CEvtServerAppDocument
// ---------------------------------------------------------------------------
//
CEvtServerAppDocument::CEvtServerAppDocument( CEikApplication&  aApp )
	:CAknDocument( aApp )    
    {
    }

// ---------------------------------------------------------------------------
// CEvtServerAppDocument::~CEvtServerAppDocument
// ---------------------------------------------------------------------------
//
CEvtServerAppDocument::~CEvtServerAppDocument()
    { 
    }

// ---------------------------------------------------------------------------
// CEvtServerAppDocument* CEvtServerAppDocument::NewL
// ---------------------------------------------------------------------------
//
CEvtServerAppDocument* CEvtServerAppDocument::NewL( CEikApplication&    aApp )     
    {
    CEvtServerAppDocument* self = new (ELeave) CEvtServerAppDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    return self;
    }
    
// ---------------------------------------------------------------------------
// CEvtServerAppDocument::ConstructL
// ---------------------------------------------------------------------------
//
void CEvtServerAppDocument::ConstructL()
    {
    }
    
// ---------------------------------------------------------------------------
// CEvtServerAppDocument::CEvtServerAppDocument
// ---------------------------------------------------------------------------
//    
CEikAppUi* CEvtServerAppDocument::CreateAppUiL()
    {
    return new (ELeave) CEvtServerAppUi;
    }

// End of File  
