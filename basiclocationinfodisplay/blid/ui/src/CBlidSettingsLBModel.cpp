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
* Description:  Provides Blid request view listbox model.
*
*/


// INCLUDE FILES
#include <StringLoader.h> 
#include <AknUtils.h>
#include <blid.rsg>
#include "CBlidSettingsLBModel.h"
#include "MBlidSettings.h"
#include "bliduiconsts.h" 
#include "Blidutils.h"
#include "Blid.hrh"

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CBlidSettingsLBModel::NewLC
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBlidSettingsLBModel* CBlidSettingsLBModel::NewLC( 
                       MBlidSettings* aSettingsModel )
    {
    CBlidSettingsLBModel* self =
		new (ELeave) CBlidSettingsLBModel( aSettingsModel );
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
    }

// ----------------------------------------------------------------------------
// CBlidSettingsLBModel::CBlidSettingsLBModel
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CBlidSettingsLBModel::CBlidSettingsLBModel(MBlidSettings* aSettingsModel):
    iSettingsModel(aSettingsModel)
    {
    }

// ----------------------------------------------------------------------------
// CBlidSettingsLBModel::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CBlidSettingsLBModel::ConstructL()
    {
    iLocale = new(ELeave)TLocale();

    iEnv = CEikonEnv::Static();
    iBuffer = HBufC::NewL( KBlidLBLineMaxLen );
    
    iBlidArrivalToneName = HBufC::NewL( 257 );
    	    
    iBlidAltitude =  StringLoader::LoadL( R_BLID_ALTITUDE_CALIBRATION, iEnv);
    iAltitudeFormatM =  iEnv->AllocReadResourceL( R_BLID_METRIC_METER );
    iAltitudeFormatFt =  iEnv->AllocReadResourceL( 
                         R_BLID_IMPERIAL_FOOT );
    iBlidBacklight = iEnv->AllocReadResourceL( 
		         R_BLID_BACKLIGHT);
    iBlidBacklightNormal = iEnv->AllocReadResourceL( 
		         R_BLID_BACKLIGHT_NORMAL);
    iBlidBacklightOn = iEnv->AllocReadResourceL( 
		         R_BLID_BACKLIGHT_ON);		         
    iBlidArrivalTone = iEnv->AllocReadResourceL( 
		         R_BLID_ARRIVAL_RINGING_TONE );
    iBlidResetOdom = iEnv->AllocReadResourceL( 
		         R_BLID_RESET_ODOMETER );
    iBlidArrivalToneOff = iEnv->AllocReadResourceL( 
		         R_BLID_ARRIVAL_TONE_OFF );		         		         
    }

// ---------------------------------------------------------
// CBlidSettingsLBModel::~CBlidSettingsLBModel
// ---------------------------------------------------------
//
CBlidSettingsLBModel::~CBlidSettingsLBModel()
    {
    delete iBlidAltitude;
    delete iAltitudeFormatM;
    delete iAltitudeFormatFt;
    delete iBuffer;
    delete iLocale;
    delete iBlidBacklight;
    delete iBlidBacklightNormal;
    delete iBlidBacklightOn;
    delete iBlidArrivalTone;
    delete iBlidResetOdom;
    delete iBlidArrivalToneOff;
    delete iBlidArrivalToneName;
    }

// ----------------------------------------------------------------------------
// CBlidSettingsLBModel::MdcaCount
// ----------------------------------------------------------------------------
//
TInt CBlidSettingsLBModel::MdcaCount() const
    {
    return 4;
    }

// ----------------------------------------------------------------------------
// CBlidSettingsLBModel::MdcaPoint
// ----------------------------------------------------------------------------
//
TPtrC16 CBlidSettingsLBModel::MdcaPoint(TInt aIndex) const
    {
    TPtr ptr( iBuffer->Des() );
    ptr.Zero();
    switch ( aIndex )
        {
        case EBlidSettingsCalibration:
            {
            TBuf<KBlidLBItemMaxLen> altitudeString;
            CalibrationFormat( altitudeString );

            ptr.Append( EKeyTab ); // EKeyTab is same as "\t"
            ptr.Append( *iBlidAltitude );
            ptr.Append( EKeyTab );
            ptr.Append( EKeyTab );            
            ptr.Append( altitudeString );
            break;
            }      
        case EBlidSettingsBacklight:
            {
            TBuf<KBlidLBItemMaxLen> backlightString;
            BacklightFormat( backlightString );

            ptr.Append( EKeyTab ); // EKeyTab is same as "\t"
            ptr.Append( *iBlidBacklight );
            ptr.Append( EKeyTab );
            ptr.Append( EKeyTab );            
            ptr.Append( backlightString );
            break;
            }
        case EBlidSettingsArrivalTone:
            {
            TBuf<KMaxFileName> arrivalTone;
            ArrivalToneFormat( arrivalTone );

            ptr.Append( EKeyTab ); // EKeyTab is same as "\t"
            ptr.Append( *iBlidArrivalTone );
            ptr.Append( EKeyTab );
            ptr.Append( EKeyTab );            
            ptr.Append( arrivalTone );
            break;
            }                 
        case EBlidSettingsResetOdometer:
            {
            ptr.Append( EKeyTab ); // EKeyTab is same as "\t"
            ptr.Append( *iBlidResetOdom );
            ptr.Append( EKeyTab );
            break;
            }                 
        default:
            {
            break;
            }
        }

    AknTextUtils::LanguageSpecificNumberConversion( ptr );
    return iBuffer->Des();
    }

// ----------------------------------------------------------------------------
// CBlidSettingsLBModel::CalibrationFormat
// ----------------------------------------------------------------------------
//
void CBlidSettingsLBModel::CalibrationFormat( TDes& aCalibrationString ) const
    {
    TInt result(0);    
    HBufC* formatter = NULL;
    TInt error( KErrNone );
    if ( iSettingsModel->UnitOfMeasurement() == EUnitsMetric )
        {
        formatter = iAltitudeFormatM;        
        error = BlidUtils::TRealToTInt( 
                    iSettingsModel->AltCalibration(), result );
        }
    else
        {        
        formatter = iAltitudeFormatFt;        
        TReal calibrationFt((iSettingsModel->AltCalibration()*KMetreToFoot));
        error = BlidUtils::TRealToTInt( calibrationFt, result );                
        }
    if(error!=KErrNone)
        {
        iEnv->HandleError( error );
        result = 0;
        }
    StringLoader::Format( aCalibrationString, *formatter, 0, result );
    }

// ----------------------------------------------------------------------------
// CBlidSettingsLBModel::BacklightFormat
// ----------------------------------------------------------------------------
//
void CBlidSettingsLBModel::BacklightFormat( TDes& aBacklightString ) const
    {
    
    if ( iSettingsModel->GetBacklightOption() == EBlidBackLightNormal )
        {
        aBacklightString = *iBlidBacklightNormal;  
        }
    else
        {        
        aBacklightString = *iBlidBacklightOn; 
        }
    
    }
    
// ----------------------------------------------------------------------------
// CBlidSettingsLBModel::ArrivalToneFormat
// ----------------------------------------------------------------------------
//
void CBlidSettingsLBModel::ArrivalToneFormat( TDes& aRrivalTone ) const
    {
    if( iSettingsModel->GetArrivedToneName().Length() > 0 )
	    {
        TParse parseFile;
        TBuf<KMaxFileName> toneFileName;
        toneFileName.Copy( iSettingsModel->GetArrivedToneName() );
        parseFile.Set( toneFileName, NULL, NULL );

	    iBlidArrivalToneName->Des().Copy( parseFile.NameAndExt() );
	    aRrivalTone = *iBlidArrivalToneName;
	    }
    else
	    {
	    aRrivalTone = *iBlidArrivalToneOff;    	
	    }
    }     
// End of File
