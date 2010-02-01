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
* Description:  Provides OwnPosView's details.
*
*/


// INCLUDE FILES
#include <StringLoader.h> 
#include <AknUtils.h>
#include <Blid.rsg>
#include <aknlocationed.h>

#include "CBlidOwnPosLBModel.h"
#include "CBlidBaseContainer.h"
#include "MBlidLocation.h"
#include "MBlidSettings.h"
#include "bliduiconsts.h"
#include "Blid.hrh"
#include "Blidutils.h"
#include "lbscommon.h"


//CONSTANTS

// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// CBlidOwnPosLBModel::NewLC
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBlidOwnPosLBModel* CBlidOwnPosLBModel::NewLC( MBlidLocation* aLocationModel,
                                               MBlidSettings* aSettings )
    {
    CBlidOwnPosLBModel* ptr =
		new (ELeave) CBlidOwnPosLBModel( aLocationModel, aSettings );
	CleanupStack::PushL(ptr);
	ptr->ConstructL();
	return ptr;
    }

// ----------------------------------------------------------------------------
// CBlidOwnPosLBModel::CBlidOwnPosLBModel
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CBlidOwnPosLBModel::CBlidOwnPosLBModel(MBlidLocation* aLocationModel,
                                       MBlidSettings* aSettings ):
    iLocationModel( aLocationModel ), iSettingsModel( aSettings )
    {
    }

// ----------------------------------------------------------------------------
// CBlidOwnPosLBModel::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CBlidOwnPosLBModel::ConstructL()
    {
    CEikonEnv* env = CEikonEnv::Static();
    iBuffer = HBufC::NewL( KBlidLBLineMaxLen );

    iLocale = new(ELeave)TLocale();

    /// titles  
    iBlidLatitude = StringLoader::LoadL( R_BLID_POSITION_LATITUDE, env);
    iBlidLongitude = StringLoader::LoadL( R_BLID_POSITION_LONGITUDE, env);
    iBlidAccuracy = StringLoader::LoadL( R_BLID_POSITION_ACCURACY, env);
    iBlidAltAccuracy = StringLoader::LoadL( R_BLID_POSITION_ALTITUDE_ACCURACY, env);
    //iBlidSpeed = StringLoader::LoadL( R_BLID_POSITION_SPEED, env);
    iBlidAltitude = StringLoader::LoadL( R_BLID_POSITION_ALTITUDE, env);

    iAccuraryFormatM = env->AllocReadResourceL( R_BLID_METRIC_METER );
    iAccuraryFormatFt = env->AllocReadResourceL( R_BLID_IMPERIAL_FOOT);
    iSpeedFormatKmph = env->AllocReadResourceL( R_BLID_METRIC_SPEED );
    iSpeedFormatMph = env->AllocReadResourceL( R_BLID_IMPERIAL_SPEED );
    iAltitudeFormatM = env->AllocReadResourceL( R_BLID_METRIC_METER );
    iAltitudeFormatFt = env->AllocReadResourceL( R_BLID_IMPERIAL_FOOT );
    iDataBeingCalculated = StringLoader::LoadL( R_BLID_CALCULATING_LIST, env);
    iConnectGPS = StringLoader::LoadL( R_BLID_CONNECT_GPS_LIST, env);
    iNotAvailable = env->AllocReadResourceL( R_BLID_NOT_AVAILABLE );
    iDataBlank = StringLoader::LoadL( R_BLID_BLANK, env);
    iWaitingGpsData = StringLoader::LoadL( R_BLID_WAITING_GPS_LIST, env);
    }

// ---------------------------------------------------------
// CBlidOwnPosLBModel::~CBlidOwnPosLBModel
// ---------------------------------------------------------
//
CBlidOwnPosLBModel::~CBlidOwnPosLBModel()
    {
    delete iLocale;

    delete iBlidLatitude;
    delete iBlidLongitude;
    delete iBlidAltitude;
    delete iBlidAltAccuracy;
    //delete iBlidSpeed;
    delete iBlidAccuracy;

    delete iBuffer;
    
    delete iAccuraryFormatM;
    delete iAccuraryFormatFt;
    delete iSpeedFormatKmph;
    delete iSpeedFormatMph;
    delete iAltitudeFormatM;
    delete iAltitudeFormatFt;
    delete iDataBeingCalculated;
    delete iConnectGPS;
    delete iNotAvailable;
    delete iDataBlank;
    delete iWaitingGpsData;
    }


// ----------------------------------------------------------------------------
// CBlidOwnPosLBModel::MdcaCount
// ----------------------------------------------------------------------------
//
TInt CBlidOwnPosLBModel::MdcaCount() const
    {
    return 5;
    }

// ----------------------------------------------------------------------------
// CBlidOwnPosLBModel::MdcaPoint
// ----------------------------------------------------------------------------
//
TPtrC16 CBlidOwnPosLBModel::MdcaPoint(TInt aIndex) const
    {
    TPtr ptr( iBuffer->Des() );
    ptr.Zero();
    const TPosition position = iLocationModel->GetCurrentPosition();
    const TCourse course = iLocationModel->GetCourse();
     
    switch ( aIndex )
        {
        case ELocLatitudeItemId:
            {            
            TBuf<KBlidLBItemMaxLen> latitudeString;
            LatitudeFormat( latitudeString, position );

            ptr.Append( *iBlidLatitude );
            ptr.Append( EKeyTab ); 
            ptr.Append( latitudeString );
            break;
            }
        case ELocLongitudeItemId:
            {
            TBuf<KBlidLBItemMaxLen> longitudeString;
            LongitudeFormat( longitudeString, position );

            ptr.Append( *iBlidLongitude );
            ptr.Append( EKeyTab );
            ptr.Append( longitudeString );
            break;
            }
        case ELocAccuracyItemId:
            {
            TBuf<KBlidLBItemMaxLen> accuracyString;
            AccuracyFormat( accuracyString, position );

            ptr.Append( *iBlidAccuracy );
            ptr.Append( EKeyTab );
            ptr.Append( accuracyString );
            break;
            }
        case ELocAltAccuracyItemId:
            {
            TBuf<KBlidLBItemMaxLen> altAccuracyString;
            altAccuracyFormat( altAccuracyString );
                        
            ptr.Append( *iBlidAltAccuracy );
            ptr.Append( EKeyTab );
            ptr.Append( altAccuracyString );
            break;
            }
        case ELocAltitudeItemId:
            {
            TBuf<KBlidLBItemMaxLen> altitudeString;
            AltitudeFormat( altitudeString, position );

            ptr.Append( *iBlidAltitude );
            ptr.Append( EKeyTab );
            ptr.Append( altitudeString );
            break;
            }
        default:
            {
            break;
            }
        } //switch

    AknTextUtils::LanguageSpecificNumberConversion( ptr );
    return iBuffer->Des();
    }

// ----------------------------------------------------------------------------
// CBlidOwnPosLBModel::LatitudeFormat
// ----------------------------------------------------------------------------
//
void CBlidOwnPosLBModel::LatitudeFormat( TDes& aLatitudeString,
                                         TPosition aPosition ) const
    {
    TReal latitude = aPosition.Latitude();

    if(!CBlidBaseContainer::IsGPSAvailable() || 
       !iLocationModel->IsGPSDataAvailable() )
        {
        if(iLocationModel->WaitingGPSData())
        	{
        	aLatitudeString = *iWaitingGpsData;
        	}
        else
        	{
        	aLatitudeString = *iConnectGPS;
        	}	 	
	 	}
	else if ( !Math::IsNaN(latitude) )
        {    
        HBufC* formatter = NULL;                     
        TRAP_IGNORE(formatter = 
                           CAknLocationEditor::DisplayableLocationL(aPosition,CAknLocationEditor::ELatitudeOnly));
        if(formatter)  
            {
            aLatitudeString.Copy(formatter->Des());             
            delete formatter;
            }
        formatter = NULL;
        }
    else
        {   
    	aLatitudeString = *iDataBeingCalculated;
    	}    	
    }

// ----------------------------------------------------------------------------
// CBlidOwnPosLBModel::LongitudeFormat
// ----------------------------------------------------------------------------
//
void CBlidOwnPosLBModel::LongitudeFormat( TDes& aLongitudeString,
                                          TPosition aPosition ) const
    {
    TReal longitude = aPosition.Longitude();
    
    if(!CBlidBaseContainer::IsGPSAvailable() ||
       !iLocationModel->IsGPSDataAvailable() )
        {
        aLongitudeString = *iDataBlank;
        }
    else if ( !Math::IsNaN(longitude) )
        {  
        HBufC* formatter = NULL;             
        TRAP_IGNORE(formatter = 
                           CAknLocationEditor::DisplayableLocationL(aPosition,CAknLocationEditor::ELongitudeOnly));

        if(formatter)  
            {
            aLongitudeString.Copy(formatter->Des());             
            delete formatter;
            }                           
        formatter = NULL;
        }
    else
    	{
   		aLongitudeString = *iDataBeingCalculated;
    	}
    }

// ----------------------------------------------------------------------------
// CBlidOwnPosLBModel::AccuracyFormat
// ----------------------------------------------------------------------------
//
void CBlidOwnPosLBModel::AccuracyFormat( TDes& aAccuracyString,
                                         TPosition aPosition ) const
    {
    TReal accuracy = aPosition.HorizontalAccuracy();
    TInt result(0);
    
    if(!CBlidBaseContainer::IsGPSAvailable() ||
       !iLocationModel->IsGPSDataAvailable() )
        {
        aAccuracyString = *iDataBlank;
        }
    else if ( !Math::IsNaN(accuracy) &&
         BlidUtils::TRealToTInt( accuracy, result) == KErrNone )
        {        
        if ( iSettingsModel->UnitOfMeasurement() == EUnitsMetric )
            {
            StringLoader::Format( aAccuracyString, *iAccuraryFormatM,
                                  0, result );
            }
        else
            {
            if ( BlidUtils::TRealToTInt((accuracy*KMetreToFoot), 
                result) != KErrNone )
                {
                aAccuracyString = *iDataBlank;
                } 
            else
                {
                StringLoader::Format( aAccuracyString, *iAccuraryFormatFt, 
                                  0, result );
                }
            }
        }
    else
    	{
      	aAccuracyString = *iDataBeingCalculated;
    	}
    }

// ----------------------------------------------------------------------------
// CBlidOwnPosLBModel::altAccuracyFormat
// ----------------------------------------------------------------------------
//
void CBlidOwnPosLBModel::altAccuracyFormat( TDes& aAltAccuracyString ) const
    {
    const TPosition position = iLocationModel->GetCurrentPosition();    
    const TReal32 altAccuracy = position.VerticalAccuracy();
    TInt result(0);
    TBuf<KBlidLBItemMaxLen> altitudeAccString;    
    AltitudeFormat(altitudeAccString, iLocationModel->GetCurrentPosition());
    
    if(!CBlidBaseContainer::IsGPSAvailable() ||
       !iLocationModel->IsGPSDataAvailable() ||
       altitudeAccString == *iDataBlank)
        {
        aAltAccuracyString = *iDataBlank;
        }    
    else if ( !Math::IsNaN(altAccuracy) &&
         BlidUtils::TRealToTInt( altAccuracy, result) == KErrNone )
        {        
        if ( iSettingsModel->UnitOfMeasurement() == EUnitsMetric )
            {
            StringLoader::Format( aAltAccuracyString, *iAccuraryFormatM,
                                  0, result );
            }
        else
            {
            if ( BlidUtils::TRealToTInt((altAccuracy*KMetreToFoot), 
                result) != KErrNone )
                {
                aAltAccuracyString = *iDataBlank;
                } 
            else
                {
                StringLoader::Format( aAltAccuracyString, *iAccuraryFormatFt, 
                                  0, result );
                }
            }
        }
    else
        {
        aAltAccuracyString = *iDataBeingCalculated;
        }
    }


// ----------------------------------------------------------------------------
// CBlidOwnPosLBModel::AltitudeFormat
// ----------------------------------------------------------------------------
//
void CBlidOwnPosLBModel::AltitudeFormat( TDes& aAltitudeString,
                                         TPosition aPosition ) const
    {
    TReal altitude = aPosition.Altitude();
    TInt result(0);
    
    if(!CBlidBaseContainer::IsGPSAvailable() || 
       !iLocationModel->IsGPSDataAvailable() )
        {
        aAltitudeString = *iDataBlank;
        }    
    else if ( !Math::IsNaN(altitude) &&
         BlidUtils::TRealToTInt((altitude - iSettingsModel->AltCalibration()),
         result) == KErrNone )
        {
        if ( iSettingsModel->UnitOfMeasurement() == EUnitsMetric )
            {
        
            StringLoader::Format( aAltitudeString, *iAltitudeFormatM, 
                                  0, result );
            }
        else
            {
            TInt imperialAlt(0);
            if ( BlidUtils::TRealToTInt((result*KMetreToFoot),
                imperialAlt) != KErrNone )
                {
                aAltitudeString = *iDataBlank;
                }
            else
                {
                StringLoader::Format( aAltitudeString, *iAltitudeFormatFt,
                                  0, imperialAlt );
                }
            }
        }
    else
        {
        aAltitudeString = *iDataBlank;
        }

    }

// End of File
