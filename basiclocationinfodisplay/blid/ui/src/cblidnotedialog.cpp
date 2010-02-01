/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  execute a information note dialog.
*
*/



// INCLUDE FILES
#include <aknnotedialog.h>
#include <Blid.rsg>
#include "cblidnotedialog.h"

// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// CBlidNoteDialog::CBlidNoteDialog
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CBlidNoteDialog::CBlidNoteDialog()
    :CActive(EPriorityStandard)
    {
    CActiveScheduler::Add( this );    
    }
    
    
// ----------------------------------------------------------------------------
// CBlidNoteDialog::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CBlidNoteDialog::ConstructL( )
    {
    iIsNoteDisplayed = EFalse; 
    }
    
    
// ----------------------------------------------------------------------------
// CBlidNoteDialog::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBlidNoteDialog* CBlidNoteDialog::NewL()
    {
    CBlidNoteDialog* self = new( ELeave ) CBlidNoteDialog();    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }    
    
    
// ----------------------------------------------------------------------------
// CBlidNoteDialog::~CBlidNoteDialog
// ----------------------------------------------------------------------------
//
CBlidNoteDialog::~CBlidNoteDialog()
    {
    Cancel();
    }  
    
    
// ----------------------------------------------------------------------------
// CBlidNoteDialog::RunL
// ----------------------------------------------------------------------------
//
void CBlidNoteDialog::RunL()
    {
    switch( iStatus.Int() ) 
        {
        case KErrNone:
            {
            if(!iIsNoteDisplayed)
                {
                iIsNoteDisplayed = ETrue;
                CAknNoteDialog* dlg;
                dlg = new (ELeave) CAknNoteDialog(CAknNoteDialog::EConfirmationTone,CAknNoteDialog::ENoTimeout);
                dlg->ExecuteLD(R_TRIP_RESUME_CONFIRMATION_NOTE);
                iIsNoteDisplayed = EFalse;
                }
            break;
            }
        default:
            {
            break;
            }            
        }
    }
    

// ----------------------------------------------------------------------------
// CBlidNoteDialog::DoCancel
// ----------------------------------------------------------------------------
//
void CBlidNoteDialog::DoCancel()
    {
    }

// ----------------------------------------------------------------------------
// CBlidNoteDialog::ExecuteNoteDialogL
// ----------------------------------------------------------------------------
//
void CBlidNoteDialog::ExecuteNoteDialog()
    {
    SetActive();  
    TRequestStatus *status = &iStatus;  
    User::RequestComplete(status, KErrNone);
    }
    
//End of File

