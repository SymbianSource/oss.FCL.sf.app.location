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
* Description:    LandmarksUi Content File -    Provides LM document class methods.
*
*/







// INCLUDE FILES
#include "CLmkDocument.h"
#include "CLmkAppUi.h"
#include "landmarks.hrh"
#include "Debug.h"
#include <EPos_CPosLandmarkDatabase.h>


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CLmkDocument::CLmkDocument
// ----------------------------------------------------------------------------
//
CLmkDocument::CLmkDocument( CEikApplication& aApp )
    : CEikDocument( aApp )
    {
    }

// ----------------------------------------------------------------------------
// CLmkDocument::~CLmkDocument
// ----------------------------------------------------------------------------
//
CLmkDocument::~CLmkDocument()
    {
    delete iDb;
    }

// ----------------------------------------------------------------------------
// CLmkDocument::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CLmkDocument::ConstructL()
    {
    DEBUG( CLmkDocument::ConstructL entered );
    TRAP(iError,iDb = CPosLandmarkDatabase::OpenL());
    DEBUG1( CLmkDocument::ConstructL iError=%d,iError );
    }

// ----------------------------------------------------------------------------
// CLmkDocument::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CLmkDocument* CLmkDocument::NewL(
    CEikApplication& aApp )     // CLocApp reference
    {
    CLmkDocument* self = new ( ELeave ) CLmkDocument( aApp );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------
// CLmkDocument::CreateAppUiL
// constructs CLmkAppUi
// ----------------------------------------------------
//
CEikAppUi* CLmkDocument::CreateAppUiL()
    {
    return new ( ELeave ) CLmkAppUi;
    }

// ----------------------------------------------------------------------------
// CLmkDocument::LmDb
// ----------------------------------------------------------------------------
//
CPosLandmarkDatabase& CLmkDocument::LmDbL() const
    {
    if ( iError != KErrNone)
    	{
    	User::Leave(iError);
    	}
    return *iDb;
    }

// End of File
