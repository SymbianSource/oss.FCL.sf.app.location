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
* Description:    LandmarksUi Content File -    Landmark editor dialog.
*
*/







// INCLUDE FILES
#include "CLmkEditorDlg.h"
#include <coehelp.h>


// ============================= LOCAL FUNCTIONS ===============================
// -----------------------------------------------------------------------------
// TLmkEditorParams::TLmkEditorParams
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkEditorDlg::TLmkEditorParams::TLmkEditorParams()
    : iAttributes( ELmkAll ),
      iEditorMode( ELmkEditor )
	{
	}

// -----------------------------------------------------------------------------
// CLmkEditorDlg::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkEditorDlg* CLmkEditorDlg::NewL( CPosLandmarkDatabase& aDb,
                                    TPosLmItemId aLandmarkId,
                                    TLmkEditorParams& aParams )
	{
    CLmkEditorDlg* self = new( ELeave ) CLmkEditorDlg();
    CleanupStack::PushL( self );
    self->ConstructL( aDb, aParams, aLandmarkId, NULL );
    CleanupStack::Pop(); //self
    return self;
	}

// -----------------------------------------------------------------------------
// CLmkEditorDlg::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkEditorDlg* CLmkEditorDlg::NewL( CPosLandmarkDatabase& /*aDb*/,
                                    CPosLandmark& /*aLandmark*/,
                                    TLmkEditorParams& /*aParams*/)
	{
    CLmkEditorDlg* self = new( ELeave ) CLmkEditorDlg();
    CleanupStack::PushL( self );
    User::Leave(KErrNotSupported);
    //self->ConstructL( aDb, aParams, aLandmark.LandmarkId(), &aLandmark );
    CleanupStack::Pop(); //self
    return self;
	}

// -----------------------------------------------------------------------------
// CLmkEditorDlg::~CLmkEditorDlg
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkEditorDlg::~CLmkEditorDlg()
	{
	}

// -----------------------------------------------------------------------------
// CLmkEditorDlg::CLmkEditorDlg
// -----------------------------------------------------------------------------
//
CLmkEditorDlg::CLmkEditorDlg()
	{
	}

// -----------------------------------------------------------------------------
// CLmkEditorDlg::ConstructL
// -----------------------------------------------------------------------------
//
void CLmkEditorDlg::ConstructL(
    CPosLandmarkDatabase& /*aDb*/,
    TLmkEditorParams& /*aParams*/,
    TPosLmItemId /*aLandmarkId*/,
    CPosLandmark* /*aLandmark*/ )
	{
	User::Leave(KErrNotSupported);
	}

// -----------------------------------------------------------------------------
// CLmkEditorDlg::SetMopParent
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkEditorDlg::SetMopParent( MObjectProvider* /*aParent*/)
	{
  	}

// -----------------------------------------------------------------------------
// CLmkEditorDlg::SetHelpContext
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkEditorDlg::SetHelpContext( TCoeHelpContext /*aContext*/)
    {
    }

// -----------------------------------------------------------------------------
// CLmkEditorDlg::ExecuteLD
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CLmkEditorDlg::ExecuteLD()
	{
	User::Leave(KErrNotSupported);
	return 0;
	}

// -----------------------------------------------------------------------------
// CLmkEditorDlg::DisableMapAndNavigationMenuOptions
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkEditorDlg::DisableMapAndNavigationMenuOptions()
	{
	TRAP_IGNORE (User::Leave(KErrNotSupported));
	}
//  End of File
