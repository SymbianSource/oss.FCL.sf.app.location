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
* Description:  Responsible for launching positioning settings
*
*/


// INCLUDE FILES
#include "CBlidLocSettingLauncher.h"
#include "CBlidMainControl.h"

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CBlidLocSettingsLauncher::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBlidLocSettingsLauncher* CBlidLocSettingsLauncher:: NewL()
	{
	CBlidLocSettingsLauncher* self = new (ELeave) CBlidLocSettingsLauncher();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop( self ); // self
	return self;
	}

// ----------------------------------------------------------------------------
// CBlidLocSettingsLauncher::CBlidLocSettingsLauncher
// ----------------------------------------------------------------------------
//
CBlidLocSettingsLauncher::CBlidLocSettingsLauncher()
	: CActive( EPriorityStandard )
	{
	iControl = NULL;
	}

// ----------------------------------------------------------------------------
// CBlidLocSettingsLauncher::~CBlidLocSettingsLauncher
// ----------------------------------------------------------------------------
//
CBlidLocSettingsLauncher::~CBlidLocSettingsLauncher()
	{
	Cancel();
    delete iClientLibrary;
	}

// ----------------------------------------------------------------------------
// CBlidLocSettingsLauncher::ConstructL
// ----------------------------------------------------------------------------
//
void CBlidLocSettingsLauncher::ConstructL()
	{
	iClientLibrary  = CLocSettingsUiClient::NewL();    
    CActiveScheduler::Add( this );
	}

// ----------------------------------------------------------------------------
// CBlidLocSettingsLauncher::LaunchL
// ----------------------------------------------------------------------------
//
void CBlidLocSettingsLauncher::LaunchL()
	{
	iClientLibrary->LaunchPosSettingsAsEmbeddedAppL( iStatus );
	SetActive();
	}

// ----------------------------------------------------------------------------
// CBlidLocSettingsLauncher::RunL
// ----------------------------------------------------------------------------
//
void CBlidLocSettingsLauncher::RunL()
    {
    switch( iStatus.Int())
        {
        case KErrNone:
            {
            if( iControl )
	            {
	            iControl->UpdateL();	
	            }
            break;
            }
        case KErrCancel:
            {
            break;
            }
        default:
            {
            break;    
            }
        }
    }

// ----------------------------------------------------------------------------
// CBlidLocSettingsLauncher::DoCancel
// ----------------------------------------------------------------------------
//
void CBlidLocSettingsLauncher::DoCancel()
    {
    iClientLibrary->ClosePosSettings();
    }
    
// ----------------------------------------------------------------------------
// CBlidLocSettingsLauncher::SetControl
// ----------------------------------------------------------------------------
//
void CBlidLocSettingsLauncher::SetControl( CBlidMainControl* aControl )
    {
    iControl = aControl;
    }    
// End of File

