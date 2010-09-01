/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Responsible for launching positioning settings
*
*/







// INCLUDE FILES
#include "CLmkLocSettingLauncher.h"

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CLmkLocSettingsLauncher::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CLmkLocSettingsLauncher* CLmkLocSettingsLauncher:: NewL()
	{
	CLmkLocSettingsLauncher* self = new (ELeave) CLmkLocSettingsLauncher();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

// ----------------------------------------------------------------------------
// CLmkLocSettingsLauncher::CLmkLocSettingsLauncher
// ----------------------------------------------------------------------------
//
CLmkLocSettingsLauncher::CLmkLocSettingsLauncher()
	: CActive( EPriorityStandard )
	{
	}

// ----------------------------------------------------------------------------
// CLmkLocSettingsLauncher::~CLmkLocSettingsLauncher
// ----------------------------------------------------------------------------
//
CLmkLocSettingsLauncher::~CLmkLocSettingsLauncher()
	{
	Cancel();
    delete iClientLibrary;
	}

// ----------------------------------------------------------------------------
// CLmkLocSettingsLauncher::ConstructL
// ----------------------------------------------------------------------------
//
void CLmkLocSettingsLauncher::ConstructL()
	{
	iClientLibrary  = CLocSettingsUiClient::NewL();
    CActiveScheduler::Add( this );
	}

// ----------------------------------------------------------------------------
// CLmkLocSettingsLauncher::LaunchL
// ----------------------------------------------------------------------------
//
void CLmkLocSettingsLauncher::LaunchL()
	{
	iClientLibrary->LaunchPosSettingsAsEmbeddedAppL( iStatus );
	SetActive();
	}

// ----------------------------------------------------------------------------
// CLmkLocSettingsLauncher::RunL
// ----------------------------------------------------------------------------
//
void CLmkLocSettingsLauncher::RunL()
    {
    switch( iStatus.Int())
        {
        case KErrNone:
            {
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
// CLmkLocSettingsLauncher::DoCancel
// ----------------------------------------------------------------------------
//
void CLmkLocSettingsLauncher::DoCancel()
    {
    iClientLibrary->ClosePosSettings();
    }

// End of File

