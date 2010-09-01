/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Events editor dialog.
*
*/

// INCLUDE FILES
#include "evteventsdlg.h"
#include "evteditor.h"
#include "evtmgmtuiengine.h"

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {
#if defined(_DEBUG)

_LIT( KPanicMsg, "CEvtEventsDlg" );

enum TPanicCode
    {
    KEvtPanicNullMember = 1
    };

void Panic( TPanicCode aReason )
    {
    User::Panic( KPanicMsg, aReason );
    }
#endif
}  // namespace

// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CEvtEventsDlg::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C CEvtEventsDlg* CEvtEventsDlg::NewL(TBool                              aIsEditable, 
                                            CEvtEvent&                         aEvent, 
                                            CEvtMgmtUiEngine&                  aEventEngine,
                                            TBool                              aDisplayExitMenuItem
                                            )                             
	{
    CEvtEventsDlg* self = new( ELeave ) CEvtEventsDlg();
    CleanupStack::PushL( self );
    self->ConstructL( aIsEditable, aEvent, aEventEngine, aDisplayExitMenuItem );
    CleanupStack::Pop(); //self
    return self;
	}

// -----------------------------------------------------------------------------
// CEvtEventsDlg::~CEvtEventsDlg
// -----------------------------------------------------------------------------
//
EXPORT_C CEvtEventsDlg::~CEvtEventsDlg()
	{    
    if ( iDestroyedPtr ) 
        {
        *iDestroyedPtr = ETrue;        
        }
    delete iEvtEditor;
	}

// -----------------------------------------------------------------------------
// CEvtEventsDlg::CEvtEventsDlg
// -----------------------------------------------------------------------------
//
CEvtEventsDlg::CEvtEventsDlg()
	{
	}

// -----------------------------------------------------------------------------
// CEvtEventsDlg::ConstructL
// -----------------------------------------------------------------------------
//
void CEvtEventsDlg::ConstructL( TBool                              aIsEditable, 
                                CEvtEvent&                         aEvent,             
                                CEvtMgmtUiEngine&                  aEventEngine,
                                TBool                              aDisplayExitMenuItem
                                )   
	{    
    iEvtEditor = CEvtEditor::NewL( aIsEditable, aEvent, aEventEngine, aDisplayExitMenuItem );
    
	}

// -----------------------------------------------------------------------------
// CEvtEventsDlg::SetMopParent
// -----------------------------------------------------------------------------
//
EXPORT_C void CEvtEventsDlg::SetMopParent( MObjectProvider* aParent )
	{
    __ASSERT_DEBUG( iEvtEditor, Panic( KEvtPanicNullMember ) );
    iEvtEditor->SetMopParent( aParent );
	}

// -----------------------------------------------------------------------------
// CEvtEventsDlg::SetHelpContext
// -----------------------------------------------------------------------------
//
EXPORT_C void CEvtEventsDlg::SetHelpContext( TCoeHelpContext aContext )
    {
    __ASSERT_DEBUG( iEvtEditor, Panic( KEvtPanicNullMember ) );
    iEvtEditor->SetHelpContext( aContext );
    }

// -----------------------------------------------------------------------------
// CEvtEventsDlg::ExecuteLD
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CEvtEventsDlg::ExecuteLD()
	{
	CleanupStack::PushL( this );
    
    TBool thisDestroyed( EFalse );
    iDestroyedPtr = &thisDestroyed;

	TInt ret( iEvtEditor->ExecuteLD() ); 
    iEvtEditor = NULL;

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

//  End of File
