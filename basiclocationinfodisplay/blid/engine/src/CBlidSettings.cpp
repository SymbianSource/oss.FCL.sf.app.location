/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of CBlidSettings class which provide access to Blid settings. Blid Setting can be read and modified using this class
*
*/

// INCLUDE FILES
#include <f32file.h> // RFs
#include <centralrepository.h>
#include <Blid.rsg>
#include <eikenv.h>

#include "CBlidSettings.h"
#include "MBlidEngObserver.h"
#include "CBlidMeasurementSettingListener.h"
#include "BlidSettingsCrkeys.h"
#include "Blid.hrh"

// CONSTANTS

// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// CBlidSettings::CBlidSettings
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CBlidSettings::CBlidSettings() 
    : iAltCalibration(0)
    ,iBacklightOption(0)
    ,iIsOdoReset( EFalse )
    {
    iArrivedDistance = -1;
    }

// ----------------------------------------------------------------------------
// CBlidSettings::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CBlidSettings::ConstructL()
    {
    iSysofMeasSettings = CBlidMeasurementSettingListener::NewL( * this );
    iArrivedToneName = HBufC8::NewL( KMaxFileName );
    HandleSysMeasureValueL();
    iRepository = CRepository::NewL( KCRUidBlidOdometerSettings );
    }

// ----------------------------------------------------------------------------
// CBlidSettings::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBlidSettings* CBlidSettings::NewL()
    {
    CBlidSettings* self = new( ELeave ) CBlidSettings;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CBlidSettings::~CBlidSettings
// ----------------------------------------------------------------------------
//
CBlidSettings::~CBlidSettings()
    {
    delete iSysofMeasSettings;
    delete iArrivedToneName;
    delete iRepository;
    }

// ----------------------------------------------------------------------------
// CBlidSettings::SetAltCalibration
// ----------------------------------------------------------------------------
//
void CBlidSettings::SetAltCalibration( const TReal32 aAltCalibration )
    {
    TReal altCalibration = aAltCalibration;
    iRepository->Set( KBlidAltCalibrationValue, altCalibration );
    }

// ----------------------------------------------------------------------------
// CBlidSettings::AltCalibration
// ----------------------------------------------------------------------------
//
TReal32 CBlidSettings::AltCalibration()
    {
    TReal altCalibration;
    TInt error = iRepository->Get( KBlidAltCalibrationValue, altCalibration );
    if( error == KErrNone )
        {
        iAltCalibration = altCalibration;
        }    
    return iAltCalibration;
    }

// ----------------------------------------------------------------------------
// CBlidSettings::UnitOfMeasurement
// ----------------------------------------------------------------------------
//
TReal32 CBlidSettings::UnitOfMeasurement()
    {
    return iMeasurementUnit;
    }
    
// ----------------------------------------------------------------------------
// CBlidSettings::HandleSysMeasureVlueL
// ----------------------------------------------------------------------------
//
void CBlidSettings::HandleSysMeasureValueL()
    {
    iMeasurementUnit = iSysofMeasSettings->SysofMeasurementL();    
    }

// ----------------------------------------------------------------------------
// CBlidSettings::SetBacklightOption
// ----------------------------------------------------------------------------
//
void CBlidSettings::SetBacklightOption( const TReal32 aBacklightOption )
    {
    TReal backlightOption = aBacklightOption;
    iRepository->Set( KBlidBacklightOptionValue, backlightOption );
    }

// ----------------------------------------------------------------------------
// CBlidSettings::GetBacklightOption
// ----------------------------------------------------------------------------
//
TReal32 CBlidSettings::GetBacklightOption()
    {
    TReal backlightOption;
    TInt error = iRepository->Get( KBlidBacklightOptionValue, backlightOption );
    if( error == KErrNone )
        {
        iBacklightOption = backlightOption;
        }    
    return iBacklightOption;
    }

// ----------------------------------------------------------------------------
// CBlidSettings::SetArrivedToneName
// ----------------------------------------------------------------------------
//
void CBlidSettings::SetArrivedToneName( const TDesC8& aArrivedToneName )
    {
    iRepository->Set( KBlidArrivedToneNameString, aArrivedToneName );
    }

// ----------------------------------------------------------------------------
// CBlidSettings::GetArrivedToneName
// ----------------------------------------------------------------------------
//
TDesC8& CBlidSettings::GetArrivedToneName()
    {
    TPtr8 des = iArrivedToneName->Des();    
    TInt error = iRepository->Get( KBlidArrivedToneNameString, des );
    if( error != KErrNone )
        {
	    if( iArrivedToneName->Length() <= 0 )
		    {
		    des.Copy( _L8( " Off " ) );	
		    }
		}
    return ( *iArrivedToneName );
    }

// ----------------------------------------------------------------------------
// CBlidSettings::SetResetOdometerOption
// ----------------------------------------------------------------------------
//
void CBlidSettings::SetResetOdometerOption( const TReal32 aIsOdoReset )
    {    
    TReal isOdoReset = aIsOdoReset;
    iRepository->Set( KBlidDoResetValue, isOdoReset );
    }

// ----------------------------------------------------------------------------
// CBlidSettings::GetResetOdometerOption
// ----------------------------------------------------------------------------
//
TReal32 CBlidSettings::GetResetOdometerOption()
    {
    TReal isOdoReset;
    TInt error = iRepository->Get( KBlidDoResetValue, isOdoReset );
    if( error != KErrNone )
        {
        iIsOdoReset = isOdoReset;
        }
    return iIsOdoReset;
    }

// ----------------------------------------------------------------------------
// CBlidSettings::SetResetOdometerOption
// ----------------------------------------------------------------------------
//
void CBlidSettings::SetArrivedDistance( TReal32 aDistance )
    {
    TReal arrivedDistance = aDistance;
    iRepository->Set( KBlidOdometerSettingsValue, arrivedDistance );
    }

// ----------------------------------------------------------------------------
// CBlidSettings::GetResetOdometerOption
// ----------------------------------------------------------------------------
//
TReal32 CBlidSettings::GetArrivedDistance()
    {
    TReal arrivedDistance;
    TInt error = iRepository->Get( KBlidOdometerSettingsValue, arrivedDistance );
    if( error == KErrNone )
        {
        iArrivedDistance = arrivedDistance;
        }
    return iArrivedDistance;
    }    
    
//  End of File
