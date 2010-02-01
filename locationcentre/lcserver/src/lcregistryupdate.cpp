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
* Description:  Location Centre Server object.
*
*/


// SYSTEM INCLUDES

// USER INCLUDES
#include "lcregistryupdate.h"
#include "lcregistrationupdatenotifier.h"
#include "lcdebug.h"

// CONSTANT DEFINTIONS
_LIT(KFileName,"?:\\private\\10283138\\import\\");

const TInt KLCUpdateDelay = 2000000;


// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// CLcRegistryUpdate::CLcRegistryUpdate
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CLcRegistryUpdate::CLcRegistryUpdate(MLcRegistryUpdateNotifier& aFileNotifier,
    								 RFs& aFs)
    :CActive(EPriorityStandard),
    iFileNotifier( aFileNotifier ),
    iFs( aFs )
    {
    TInt err = iTimer.CreateLocal();
    CActiveScheduler::Add( this );    
    }
    
// ----------------------------------------------------------------------------
// CLcRegistryUpdate::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CLcRegistryUpdate* CLcRegistryUpdate::NewL(MLcRegistryUpdateNotifier& aFileNotifier,
    								   	   RFs& aFs)
    {
    CLcRegistryUpdate* self = new( ELeave ) CLcRegistryUpdate( aFileNotifier, aFs );    
    return self;
    }    
    
    
// ----------------------------------------------------------------------------
// CLcRegistryUpdate::~CLcRegistryUpdate
// ----------------------------------------------------------------------------
//
CLcRegistryUpdate::~CLcRegistryUpdate()
    {
    Cancel();
    }  

// ----------------------------------------------------------------------------
// CLcRegistryUpdate::ConstructL
// ----------------------------------------------------------------------------
//
void CLcRegistryUpdate::ConstructL()
    {    
    }
    
// ----------------------------------------------------------------------------
// CLcRegistryUpdate::RunL
// ----------------------------------------------------------------------------
//
void CLcRegistryUpdate::RunL()
    { 
    if ( iFileNotifyPending )
        {    
        StartFileNotifation();
        iFileNotifier.HandleDynamicRegistrationL();
        iFileNotifyPending = EFalse;       
        }
    else
        {
        iTimer.After( iStatus, KLCUpdateDelay );
        SetActive();
        iFileNotifyPending = ETrue;
        }
    }
    

// ----------------------------------------------------------------------------
// CLcRegistryUpdate::DoCancel
// ----------------------------------------------------------------------------
//
void CLcRegistryUpdate::DoCancel()
    {
    iTimer.Cancel();
    iFs.NotifyChangeCancel();
    }

// ---------------------------------------------------------------------------
// CLcRegistryUpdate::RunError
// ---------------------------------------------------------------------------
//        
TInt CLcRegistryUpdate::RunError( TInt aError )
    {
    return aError;
    }


// ----------------------------------------------------------------------------
// CLcRegistryUpdate::StartFileNotifation
// ----------------------------------------------------------------------------
//
void CLcRegistryUpdate::StartFileNotifation()
    {
    SetActive();  
    iFs.NotifyChange(ENotifyEntry,iStatus,KFileName);
    }
    
//End of File
