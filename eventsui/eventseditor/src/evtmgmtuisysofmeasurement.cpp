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
* Description:  An observer for change in System of Measurement
*
*/


// System Include
#include <centralrepository.h>

// User Include
#include "evtmgmtuisysofmeasurement.h"


// ========================= MEMBER FUNCTIONS ================================


// ---------------------------------------------------------------------------
// CEvtMgmtUiSysOfMeasurement::CEvtMgmtUiSysOfMeasurement
// Overloaded Constructor
//
// ---------------------------------------------------------------------------
//  
CEvtMgmtUiSysOfMeasurement::CEvtMgmtUiSysOfMeasurement(  
                                MEvtMgmtUiSOMObserver& aAdapterObsrv )
    :CActive(EPriorityStandard),
    iObserver(aAdapterObsrv)
    {
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiSysOfMeasurement::~CEvtMgmtUiSysOfMeasurement
// Destructor
//
// ---------------------------------------------------------------------------
//
CEvtMgmtUiSysOfMeasurement::~CEvtMgmtUiSysOfMeasurement()
    {
    Cancel();
    
    delete iRepository; 
    }
    
// ---------------------------------------------------------------------------
// CEvtMgmtUiSysOfMeasurement* CEvtMgmtUiSysOfMeasurement::NewL
// Creates new Systen of Measurement adapter
//
// @param  aAdapterObsrv                        Observer to the Settings adapter
// @return CEvtMgmtUiSysOfMeasurement&  Reference to the application view
// ---------------------------------------------------------------------------
//    
CEvtMgmtUiSysOfMeasurement* CEvtMgmtUiSysOfMeasurement::NewL( 
                            MEvtMgmtUiSOMObserver& aAdapterObsrv )
    {   
    CEvtMgmtUiSysOfMeasurement* self = 
                        CEvtMgmtUiSysOfMeasurement::NewLC( aAdapterObsrv );
    CleanupStack::Pop(self);
    return self;
    }

// ---------------------------------------------------------------------------
// CEvtMgmtUiSysOfMeasurement* CEvtMgmtUiSysOfMeasurement::NewLC
// Creates new Systen of Measurement adapter. Leaves the object on the clean-up
// stack
//
// @param  aAdapterObsrv                        Observer to the Settings adapter
// @return CEvtMgmtUiSysOfMeasurement&  Reference to the application view
// ---------------------------------------------------------------------------
// 
CEvtMgmtUiSysOfMeasurement* CEvtMgmtUiSysOfMeasurement::NewLC( 
                            MEvtMgmtUiSOMObserver& aAdapterObsrv )
    {   
    CEvtMgmtUiSysOfMeasurement* self = 
                        new(ELeave) CEvtMgmtUiSysOfMeasurement( aAdapterObsrv );
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
    
// ---------------------------------------------------------------------------
// void CEvtMgmtUiSysOfMeasurement::ConstructL
// Second Phase Constructor
//
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiSysOfMeasurement::ConstructL()
    {
    CActiveScheduler::Add( this );
    
    // Create the Central repository object for manipulating Avkon Central
    // repository for System of measurements settings
    iRepository = CRepository::NewL(TUid::Uid( KCRUidLocNotationPref ));
    
    StartNotification();    
    }

// ---------------------------------------------------------------------------
// TLocSystemofMeasurementValues CEvtMgmtUiSysOfMeasurement::GetCurrentSysofMeasL
// Returns the current value of the System of measurements settings
//
// @return TLocSystemofMeasurementValues  Current value of the System of 
//                                        measurements settings
// ---------------------------------------------------------------------------
//
TLocSystemofMeasurementValues CEvtMgmtUiSysOfMeasurement::GetCurrentSysofMeasL()
    {
    
    TInt settingsValue = 0;
    User::LeaveIfError( iRepository->Get( KLocSystemofMeasurements,
                                          settingsValue ));  
    return ( static_cast<TLocSystemofMeasurementValues>( settingsValue ));
    }
           
// --------------------------------------------------------------------------
// void CEvtMgmtUiSysOfMeasurement::RunL
// Inherited from CActive
//
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiSysOfMeasurement::RunL()
    {
    // Issue Notification
    iObserver.NotifySOMChangeL();
    
    // Schedule for listening to change events again
    StartNotification();
    }

// --------------------------------------------------------------------------
// void CEvtMgmtUiSysOfMeasurement::DoCancel
// Inherited from CActive
//
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiSysOfMeasurement::DoCancel()
    {
    // Cancel the outstanding CR notification request
    if( iRepository )
        {
        iRepository->NotifyCancel( KLocSystemofMeasurements );        
        }
    }
    
// --------------------------------------------------------------------------
// void CEvtMgmtUiSysOfMeasurement::StartNotification
// Start notification from the Central repository server for any changes in the
// System of measurements key value
//
// ---------------------------------------------------------------------------
//
void CEvtMgmtUiSysOfMeasurement::StartNotification()
    {
    if( !IsActive())
        {
        iStatus = KRequestPending;
        // Request for notification
        iRepository->NotifyRequest( KLocSystemofMeasurements, iStatus);
        SetActive();          
        }  
    }
