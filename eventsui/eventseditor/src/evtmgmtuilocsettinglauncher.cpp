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
* Description:   Responsible for launching positioning settings
*
*/

// INCLUDE FILES
#include "evtmgmtuilocsettinglauncher.h"
#include "evtmgmtuilocsettinglauncherobserver.h"

// ----------------------------------------------------------------------------
// CLmkLocSettingsLauncher::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CEvtMgmtUiLocSettingsLauncher* CEvtMgmtUiLocSettingsLauncher:: NewL()
	{
	CEvtMgmtUiLocSettingsLauncher* self = new (ELeave) CEvtMgmtUiLocSettingsLauncher();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

// ----------------------------------------------------------------------------
// CLmkLocSettingsLauncher::CLmkLocSettingsLauncher
// ----------------------------------------------------------------------------
//
CEvtMgmtUiLocSettingsLauncher::CEvtMgmtUiLocSettingsLauncher()
	: CActive( EPriorityStandard )
	{
	}

// ----------------------------------------------------------------------------
// CEvtMgmtUiLocSettingsLauncher::~CEvtMgmtUiLocSettingsLauncher
// ----------------------------------------------------------------------------
//
CEvtMgmtUiLocSettingsLauncher::~CEvtMgmtUiLocSettingsLauncher()
	{
	Cancel();
    delete iClientLibrary;
	}

// ----------------------------------------------------------------------------
// CEvtMgmtUiLocSettingsLauncher::ConstructL
// ----------------------------------------------------------------------------
//
void CEvtMgmtUiLocSettingsLauncher::ConstructL()
	{
	iClientLibrary  = CLocSettingsUiClient::NewL();    
    CActiveScheduler::Add( this );
	}

// ----------------------------------------------------------------------------
// CEvtMgmtUiLocSettingsLauncher::LaunchL
// It is used to cancel launching of position settings.
// ----------------------------------------------------------------------------
//
void CEvtMgmtUiLocSettingsLauncher::LaunchL()
    {
    iClientLibrary->LaunchPosSettingsAsEmbeddedAppL( iStatus );
    SetActive();
    }

// ----------------------------------------------------------------------------
// CEvtMgmtUiLocSettingsLauncher::SetObserver
// It is used to set observer which will get notification 
// after completion of operation.
// ----------------------------------------------------------------------------
//
void CEvtMgmtUiLocSettingsLauncher::SetObserver( MEvtMgmtUiLocSettingsLauncherObserver* aObserver )
    { 
    iObserver = aObserver;
    }
    
// ----------------------------------------------------------------------------
// CEvtMgmtUiLocSettingsLauncher::RunL
// Derived from CActive
// ----------------------------------------------------------------------------
//
void CEvtMgmtUiLocSettingsLauncher::RunL()
    {
    //notify observer completion of operation.
    if(iObserver)
        iObserver->NotifyLocSettingLaucherChangeL( iStatus.Int() );
    }

// ----------------------------------------------------------------------------
// CEvtMgmtUiLocSettingsLauncher::DoCancel
// Derived from CActive
// ----------------------------------------------------------------------------
//
void CEvtMgmtUiLocSettingsLauncher::DoCancel()
    {
    iClientLibrary->ClosePosSettings();
    }

// End of File

