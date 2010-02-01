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
* Description:  Events Handler Server App Ui.
*
*/
 
//#include <internaleventdispatcher.h>
#include "evtserverappui.h"
#include "evtdebug.h"


// -----------------------------------------------------------------------------
// CEvtServerAppUi::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CEvtServerAppUi::ConstructL()
{
    // Initialise app UI with no resource file.
    BaseConstructL( ENoAppResourceFile );

    // Hide S60 signal strength icons etc.
    //StatusPane()->MakeVisible( EFalse );
    
    // Disable Focus
    //iCoeEnv->RootWin().EnableReceiptOfFocus(EFalse);

    EVTUIDEBUG( "= before HideApplicationFromFSW( ETrue )" );
    // Send to background
    TApaTask task(iEikonEnv->WsSession());
    task.SetWgId( CEikonEnv::Static()->RootWin().Identifier());
    task.SendToBackground();
    
    // Hide from Fast swap
    HideApplicationFromFSW( ETrue );
    EVTUIDEBUG( "= after HideApplicationFromFSW( ETrue )" );
}

// -----------------------------------------------------------------------------
// CEvtServerAppUi::CEvtServerAppUi()
// C++ default constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CEvtServerAppUi::CEvtServerAppUi() : iEventReceiver( NULL )
{
  
}

// -----------------------------------------------------------------------------
// CEvtServerAppUi::~CEvtServerAppUi()
// Destructor.
// -----------------------------------------------------------------------------
//
CEvtServerAppUi::~CEvtServerAppUi()
{
    
}

// -----------------------------------------------------------------------------
// CEvtServerAppUi::HandleWsEventL
// Takes care of WsSwssion events
// -----------------------------------------------------------------------------
//
void CEvtServerAppUi::HandleWsEventL( const TWsEvent &aEvent, CCoeControl *aDestination )
{
    CAknAppUi::HandleWsEventL( aEvent,aDestination );
}

// -----------------------------------------------------------------------------
// CEvtServerAppUi::HandleCommandL()
// Sets the event reciever
// -----------------------------------------------------------------------------
//
void CEvtServerAppUi::SetEventReceiver( CEventReceiver& aEventReceiver )
{
    iEventReceiver = &aEventReceiver;
}
//end of file
