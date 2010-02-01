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
#include "CLmkCategorySelectorDlg.h"



// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CLmkCategorySelectorDlg::CLmkCategorySelectorDlg
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CLmkCategorySelectorDlg::CLmkCategorySelectorDlg()
    : iIsMultiSelector( ETrue )
    {
    }

// -----------------------------------------------------------------------------
// CLmkCategorySelectorDlg::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CLmkCategorySelectorDlg::ConstructL(
    TBool /*aShowEmptyCategories*/ )
    {
    }

// -----------------------------------------------------------------------------
// CLmkCategorySelectorDlg::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkCategorySelectorDlg* CLmkCategorySelectorDlg::NewL(
 								 TBool aShowEmptyCategories )
    {

    CLmkCategorySelectorDlg* self = new ( ELeave ) CLmkCategorySelectorDlg();

    CleanupStack::PushL( self );
    self->ConstructL(aShowEmptyCategories );
    CleanupStack::Pop();

    return self;
    }


// -----------------------------------------------------------------------------
// CLmkCategorySelectorDlg::~CLmkCategorySelectorDlg
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkCategorySelectorDlg::~CLmkCategorySelectorDlg()
    {

    }

// -----------------------------------------------------------------------------
// CLmkCategorySelectorDlg::SetMopParent
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkCategorySelectorDlg::SetMopParent( MObjectProvider* /*aParent*/)
    {

    }

// -----------------------------------------------------------------------------
// CLmkCategorySelectorDlg::ExecuteLD
// -----------------------------------------------------------------------------
//
// For multiple database support
EXPORT_C TInt CLmkCategorySelectorDlg::ExecuteLD( TLmkItemIdDbCombiInfo& /*aSelected*/ )
    {
    User::Leave(KErrNotSupported);
    return 0;
    }

// -----------------------------------------------------------------------------
// CLmkCategorySelectorDlg::ExecuteLD
// -----------------------------------------------------------------------------
//
// For multiple database support
  EXPORT_C TInt CLmkCategorySelectorDlg::ExecuteLD(
    RArray<TLmkItemIdDbCombiInfo>& /*aSelectedItems*/ )
    {
    User::Leave(KErrNotSupported);
    return 0;
 		}


//  End of File
