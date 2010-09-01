/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of CLandmarksDocument
*
*/



#include "LandmarksAppUi.h"
#include "LandmarksDocument.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksDocument::CLandmarksDocument(CEikApplication& aApp) : CAknDocument(aApp) 
    {
    // No implementation required
    }   

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksDocument::ConstructL()
    {
    // No implementation required
    }    

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksDocument* CLandmarksDocument::NewL(CEikApplication& aApp)
    {
    CLandmarksDocument* self = new (ELeave) CLandmarksDocument(aApp);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksDocument::~CLandmarksDocument()
    {
    // No implementation required
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CEikAppUi* CLandmarksDocument::CreateAppUiL()
    {
    // Create the application user interface, and return a pointer to it,
    // the framework takes ownership of this object
    CEikAppUi* appUi = new (ELeave) CLandmarksAppUi;
    return appUi;
    }


