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
* Description:  Document class for Location Centre Application UI.
*
*/


// INCLUDE FILES
#include "lcdocument.h"
#include "lcappui.h"

// ----------------- Member funtions for CLcDocument class ------------------

// ---------------------------------------------------------------------------
// CLcDocument::CLcDocument
// ---------------------------------------------------------------------------
//
CLcDocument::CLcDocument( CEikApplication&  aApp )
	:CAknDocument( aApp )    
    {
    }

// ---------------------------------------------------------------------------
// CLcDocument::~CLcDocument
// ---------------------------------------------------------------------------
//
CLcDocument::~CLcDocument()
    { 
    }

// ---------------------------------------------------------------------------
// CLcDocument* CLcDocument::NewL
// ---------------------------------------------------------------------------
//
CLcDocument* CLcDocument::NewL( CEikApplication&    aApp )     
    {
    CLcDocument* self = new (ELeave) CLcDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    
    return self;
    }
    
// ---------------------------------------------------------------------------
// CLcDocument::ConstructL
// ---------------------------------------------------------------------------
//
void CLcDocument::ConstructL()
    {
    }
    
// ---------------------------------------------------------------------------
// CLcDocument::CLcDocument
// ---------------------------------------------------------------------------
//    
CEikAppUi* CLcDocument::CreateAppUiL()
    {
    return new (ELeave) CLcAppUi;
    }

// End of File  
