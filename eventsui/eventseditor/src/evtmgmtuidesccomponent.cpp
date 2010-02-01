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
* Description:   Description component for retrieving place.
*
*/

#include <evtmgmteditorui.rsg>
#include <NpdApi.h> 
#include <StringLoader.h>
#include <eikappui.h>
#include <eikenv.h>

#include "evtmgmteditorui.hrh"
#include "evtmgmtuidesccomponent.h"

// ---------------------------------------------------------------------------
// CEvtEditor ::NewL()
// ---------------------------------------------------------------------------
//
CEvtMgmtUiDescComponent* CEvtMgmtUiDescComponent::NewL(const TPtrC aPrevDesc)
    {
    CEvtMgmtUiDescComponent * self = CEvtMgmtUiDescComponent::NewLC(aPrevDesc);
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CEvtEditor ::NewLC()
// ---------------------------------------------------------------------------
//
CEvtMgmtUiDescComponent* CEvtMgmtUiDescComponent::NewLC(const TPtrC aPrevDesc)
    {
    CEvtMgmtUiDescComponent * self = new (ELeave) CEvtMgmtUiDescComponent();
    CleanupStack::PushL(self);
    self->ConstructL(aPrevDesc);
    return self;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiDescComponent ::~CEvtMgmtUiDescComponent()
// ---------------------------------------------------------------------------
//
CEvtMgmtUiDescComponent::~CEvtMgmtUiDescComponent()
    {    
    delete iNotepad;
    iNotepad = NULL;
    
    delete iDesc;
    iDesc = NULL;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiDescComponent ::ConstructL()
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiDescComponent ::ConstructL(const TPtrC aPrevDesc)
    {
    iDesc = aPrevDesc.AllocL();
    iNotepad = CNotepadApi::NewL();
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiDescComponent ::CEvtMgmtUiDescComponent()
// ---------------------------------------------------------------------------
//
CEvtMgmtUiDescComponent::CEvtMgmtUiDescComponent() 
    {   
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiDescComponent::DisplayNotepadEditorL()
// It displays notepad editor and saves in descriptor variable
// @return KErrCancel if deleted
//         KErrAlreadyExists if not modified
//         KErrNone if modified
// -----------------------------------------------------------------------------
//
TInt CEvtMgmtUiDescComponent::DisplayNotepadEditorL()
   {
   TInt status = KErrNone;       
   
   //Launch notepad application
   HBufC* desc = iNotepad->ExecTextEditorL(status,*iDesc);    
   
   if(!desc)
       desc = KNullDesC().AllocL();
   
   CleanupStack::PushL(desc);
   
   if(status != KErrNone )//if deleted
       {
       CleanupStack::PopAndDestroy(desc);  
       delete iDesc;
       iDesc = NULL;
       iDesc = KNullDesC().AllocL();
       return KErrCancel;
       }
   
   desc->Des().Trim();
   if(desc->Length()==0) //if no text
       {
       delete iDesc;
       iDesc = NULL;
       iDesc = KNullDesC().AllocL();
       CleanupStack::PopAndDestroy(desc);
       return KErrCancel;
       }
   
   //set text to description
   delete iDesc;
   iDesc = desc; //ownership is transfered   
   CleanupStack::Pop(desc);
   return KErrNone;
   }

// -----------------------------------------------------------------------------
// CEvtMgmtUiDescComponent::DescFromExistingNotesL()
// It shows Memo dialog and saves in descriptor variable
// @return KErrCancel if cancel or no note
//         KErrNone if note is selected
// -----------------------------------------------------------------------------
//
TInt CEvtMgmtUiDescComponent::DescFromExistingNotesL()
    {
    HBufC* title = StringLoader::LoadLC(R_EVTUI_EDITOR_ADDDESC_TITLE);
    
    //Display memo dialog
    HBufC* desc = iNotepad->FetchMemoL(title);
    CleanupStack::PopAndDestroy(title);    
        
    if(desc) //if memo is selected
        {            
        CleanupStack::PushL(desc);
        delete iDesc;
        iDesc = desc; //ownership is transferred       
        CleanupStack::Pop(desc);
        return KErrNone;
        }       
    return KErrCancel;
    }

// -----------------------------------------------------------------------------
// CEvtMgmtUiDescComponent::Desc()
// returns description value
// -----------------------------------------------------------------------------

TPtrC CEvtMgmtUiDescComponent::Desc() const
    {
    //if description, return it, else return empty string
    if(iDesc)
        return *iDesc;
    else
        return KNullDesC();
    }
//end of file
