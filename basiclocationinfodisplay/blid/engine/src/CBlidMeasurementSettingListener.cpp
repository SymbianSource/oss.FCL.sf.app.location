/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Listner for getting the System of measurment settings using cRepository
*
*/



// System Include
#include <centralrepository.h>
#include <locnotprefplugindomaincrkeys.h>

// INCLUDE FILES
#include "CBlidMeasurementSettingListener.h"

// CONSTANTS


// ============================ MEMBER FUNCTIONS ==============================

// ---------------------------------------------------------------------------
// CBlidMeasurementSettingListener::CBlidMeasurementSettingListener
// Overloaded Constructor
//
// ---------------------------------------------------------------------------
//	
CBlidMeasurementSettingListener::CBlidMeasurementSettingListener(  
                                MBlidMeasurementSettings&	aListenerObsrv )
	:CActive(EPriorityStandard),
	iObserver(aListenerObsrv)
	{
	}

// ---------------------------------------------------------------------------
// CBlidMeasurementSettingListener::~CBlidMeasurementSettingListener
// Destructor
//
// ---------------------------------------------------------------------------
//
CBlidMeasurementSettingListener::~CBlidMeasurementSettingListener()
	{
	Cancel();
	
	delete iRepository;
	iRepository = NULL;	
	}
	
// ---------------------------------------------------------------------------
// CBlidMeasurementSettingListener* CBlidMeasurementSettingListener::NewL
// Creates new Systen of Measurement adapter
// ---------------------------------------------------------------------------
//    
CBlidMeasurementSettingListener* CBlidMeasurementSettingListener::NewL( 
                            MBlidMeasurementSettings&	aListenerObsrv )
	{	
    CBlidMeasurementSettingListener* self = 
                        CBlidMeasurementSettingListener::NewLC( aListenerObsrv );
	CleanupStack::Pop(self);
	return self;
	}

// ---------------------------------------------------------------------------
// CBlidMeasurementSettingListener* CBlidMeasurementSettingListener::NewLC
// Creates new Systen of Measurement Listener. Leaves the object on the clean-up
// stack
// ---------------------------------------------------------------------------
// 
CBlidMeasurementSettingListener* CBlidMeasurementSettingListener::NewLC( 
                            MBlidMeasurementSettings&	aListenerObsrv )
	{	
    CBlidMeasurementSettingListener* self = 
                        new(ELeave) CBlidMeasurementSettingListener( aListenerObsrv );
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
// ---------------------------------------------------------------------------
// void CBlidMeasurementSettingListener::ConstructL
// Second Phase Constructor
//
// ---------------------------------------------------------------------------
//
void CBlidMeasurementSettingListener::ConstructL()
	{
	CActiveScheduler::Add( this );
	
	// Create the Central repository object for manipulating Avkon Central
	// repository for System of measurements settings
	iRepository = CRepository::NewL(TUid::Uid( KCRUidLocNotationPref ));
	
	StartNotification();	
	}

// ---------------------------------------------------------------------------
// TReal32 CBlidMeasurementSettingListener::SysofMeasurementL
// Returns the current value of the System of measurements settings
//
// @return TReal32  Current value of the System of measurements settings
// ---------------------------------------------------------------------------
//
TReal32 CBlidMeasurementSettingListener::SysofMeasurementL()
    {
    
    TInt settingsValue = 0;
    User::LeaveIfError( iRepository->Get( KLocSystemofMeasurements,
                                          settingsValue ));  
    return ( static_cast<TReal32>( settingsValue ));
    }

	    
// --------------------------------------------------------------------------
// void CBlidMeasurementSettingListener::RunL
// Inherited from CActive
//
// ---------------------------------------------------------------------------
//
void CBlidMeasurementSettingListener::RunL()
    {
    // Issue Notification
    iObserver.HandleSysMeasureValueL();
    
    // Schedule for listening to change events again
    StartNotification();
    }

// --------------------------------------------------------------------------
// void CBlidMeasurementSettingListener::DoCancel
// Inherited from CActive
//
// ---------------------------------------------------------------------------
//
void CBlidMeasurementSettingListener::DoCancel()
    {
    // Cancel the outstanding CR notification request
    iRepository->NotifyCancel( KLocSystemofMeasurements );
    }
    
// --------------------------------------------------------------------------
// void CBlidMeasurementSettingListener::StartNotification
// Start notification from the Central repository server for any changes in the
// System of measurements key value
//
// ---------------------------------------------------------------------------
//
void CBlidMeasurementSettingListener::StartNotification()
    {
    if( !IsActive())
        {
        iStatus = KRequestPending;
        // Request for notification
        iRepository->NotifyRequest( KLocSystemofMeasurements, iStatus);
        SetActive();          
        }  
    }
	
// End of File

