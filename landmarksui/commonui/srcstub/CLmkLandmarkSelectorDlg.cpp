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
* Description:    LandmarksUi Content File -
*
*/









// INCLUDE FILES
#include "CLmkLandmarkSelectorDlg.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkLandmarkSelectorDlg::CLmkLandmarkSelectorDlg
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkLandmarkSelectorDlg::CLmkLandmarkSelectorDlg()
    : iIsMultiSelector( ETrue )
    {
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkSelectorDlg::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//

void CLmkLandmarkSelectorDlg::ConstructL()
    {
    }


// -----------------------------------------------------------------------------
// CLmkLandmarkSelectorDlg::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//

// Changed for supporting multiple databases
EXPORT_C CLmkLandmarkSelectorDlg* CLmkLandmarkSelectorDlg::NewL( )

    {

    CLmkLandmarkSelectorDlg* self = new( ELeave ) CLmkLandmarkSelectorDlg();

    CleanupStack::PushL( self );

    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkSelectorDlg::~CLmkLandmarkSelectorDlg
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkLandmarkSelectorDlg::~CLmkLandmarkSelectorDlg()
    {
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkSelectorDlg::SetMopParent
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkLandmarkSelectorDlg::SetMopParent( MObjectProvider* /*aParent*/)
    {
    }

// -----------------------------------------------------------------------------
// CLmkLandmarkSelectorDlg::ExecuteLD
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
// For multiple database support

EXPORT_C TInt CLmkLandmarkSelectorDlg::ExecuteLD( TLmkItemIdDbCombiInfo& /*aSelected*/)
    {
    User::Leave(KErrNotSupported);
    return 0;
    }


// -----------------------------------------------------------------------------
// CLmkLandmarkSelectorDlg::ExecuteLD
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
 // For multiple database support
EXPORT_C TInt CLmkLandmarkSelectorDlg::ExecuteLD(
    RArray<TLmkItemIdDbCombiInfo>& /*aSelectedItems*/ )
    {
    User::Leave(KErrNotSupported);
    return 0;
    }
//  End of File
