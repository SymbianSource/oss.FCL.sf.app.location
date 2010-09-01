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
* Description:    Landmark editor dialog.
*
*/




// INCLUDE FILES
#include "CLmkEditorDlg.h"
#include "CLmkEditorImpl.h"
#include "CLmkSender.h"
#include <lmkerrors.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {
#if defined(_DEBUG)

_LIT( KPanicMsg, "CLmkEditorDlg" );

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
#endif
}  // namespace

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
EXPORT_C CLmkEditorDlg* CLmkEditorDlg::NewL( CPosLandmarkDatabase& aDb,
                                    CPosLandmark& aLandmark,
                                    TLmkEditorParams& aParams )
	{
    CLmkEditorDlg* self = new( ELeave ) CLmkEditorDlg();
    CleanupStack::PushL( self );
    self->ConstructL( aDb, aParams, aLandmark.LandmarkId(), &aLandmark );
    CleanupStack::Pop(); //self
    return self;
	}

// -----------------------------------------------------------------------------
// CLmkEditorDlg::~CLmkEditorDlg
// -----------------------------------------------------------------------------
//
EXPORT_C CLmkEditorDlg::~CLmkEditorDlg()
	{
    if ( iDestroyedPtr )
        {
        *iDestroyedPtr = ETrue;
        }
    delete iEditorImpl;
    delete iSender;
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
    CPosLandmarkDatabase& aDb,
    TLmkEditorParams& aParams,
    TPosLmItemId aLandmarkId,
    CPosLandmark* aLandmark )
	{
    iSender = CLmkSender::NewL( aDb );

    if ( aLandmark )
        {
        iEditorImpl = CLmkEditorImpl::NewL( aDb,
                                            *iSender,
                                            aParams.iAttributes,
                                            aParams.iEditorMode,
                                            aLandmark );
        }
    else
        {
        iEditorImpl = CLmkEditorImpl::NewL( aDb,
                                            *iSender,
                                            aParams.iAttributes,
                                            aParams.iEditorMode,
                                            aLandmarkId );
        }
	}

// -----------------------------------------------------------------------------
// CLmkEditorDlg::SetMopParent
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkEditorDlg::SetMopParent( MObjectProvider* aParent )
	{
    __ASSERT_DEBUG( iEditorImpl, Panic( KLmkPanicNullMember ) );
    iEditorImpl->SetMopParent( aParent );
	}

// -----------------------------------------------------------------------------
// CLmkEditorDlg::SetHelpContext
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkEditorDlg::SetHelpContext( TCoeHelpContext aContext )
    {
    __ASSERT_DEBUG( iEditorImpl, Panic( KLmkPanicNullMember ) );
    iEditorImpl->SetHelpContext( aContext );
    }

// -----------------------------------------------------------------------------
// CLmkEditorDlg::ExecuteLD
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CLmkEditorDlg::ExecuteLD()
	{
	CleanupStack::PushL( this );

    TBool thisDestroyed( EFalse );
    iDestroyedPtr = &thisDestroyed;

	TInt ret( iEditorImpl->ExecuteLD() );
    iEditorImpl = NULL;

    if ( thisDestroyed )
        {
        // this object has been deleted already
        CleanupStack::Pop( this );
        }
    else
        {
        CleanupStack::PopAndDestroy(this); //this
        }
    return ret;
	}

// -----------------------------------------------------------------------------
// CLmkEditorDlg::DisableMapAndNavigationMenuOptions
// -----------------------------------------------------------------------------
//
EXPORT_C void CLmkEditorDlg::DisableMapAndNavigationMenuOptions()
	{
	iEditorImpl->DisableMapAndNavigationMenuOptions();
	}

//  End of File
