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
* Description:  Blid engine object.
*
*/



// SYSTEM INCLUDE
#include <lbs.h>
#include <lbscommon.h>

//USER INCLUDE
#include "cblidmoduleinfoobserver.h"
#include "MBlidLocation.h"

//CONSTANTS


// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// CBlidModuleinfoObserver::CBlidModuleinfoObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CBlidModuleinfoObserver::CBlidModuleinfoObserver( RPositionServer& 	aServer,
												  MBlidLocation&	aObserver )
    :CActive(EPriorityStandard)
	,iServer( aServer )
	,iObserver( aObserver )
    {
    CActiveScheduler::Add( this );    
    }
    
    
// ----------------------------------------------------------------------------
// CBlidModuleinfoObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CBlidModuleinfoObserver::ConstructL( )
    {

    }
    
    
// ----------------------------------------------------------------------------
// CBlidModuleinfoObserver::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBlidModuleinfoObserver* CBlidModuleinfoObserver::NewL( RPositionServer& aServer,
												  MBlidLocation&	aObserver )
    {
    CBlidModuleinfoObserver* self = new( ELeave ) CBlidModuleinfoObserver( aServer, aObserver);    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }    
    
    
// ----------------------------------------------------------------------------
// CBlidModuleinfoObserver::~CBlidModuleinfoObserver
// ----------------------------------------------------------------------------
//
CBlidModuleinfoObserver::~CBlidModuleinfoObserver()
    {
    Cancel();
    }  
    
    
// ----------------------------------------------------------------------------
// CBlidModuleinfoObserver::RunL
// ----------------------------------------------------------------------------
//
void CBlidModuleinfoObserver::RunL()
    {
    switch( iStatus.Int() ) 
        {
        case KErrNone:
            {
            StartNotification();
            iObserver.GetModuleInfoNotificationL();
            break;
            }
        default:
            {
            break;
            }            
        }
    }
    

// ----------------------------------------------------------------------------
// CBlidModuleinfoObserver::DoCancel
// ----------------------------------------------------------------------------
//
void CBlidModuleinfoObserver::DoCancel()
    {
    iServer.CancelRequest(EPositionServerNotifyModuleStatusEvent);
    }

// ----------------------------------------------------------------------------
// CBlidModuleinfoObserver::StartNotification
// ----------------------------------------------------------------------------
//
void CBlidModuleinfoObserver::StartNotification()
    {
    SetActive();  
    TPositionModuleStatusEvent aStatusEvent;
    iServer.NotifyModuleStatusEvent(aStatusEvent,iStatus);  
    }
    
//End of File

