/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    Methods for Landmark editor number field.
*
*/







// INCLUDE FILES
#include "CLmkEditorNumberField.h"
#include "MLmkEditorUiBuilder.h"
#include "LmkConsts.h"
#include "landmarks.hrh"
#include <eikcapc.h>
#include <e32math.h>
#include <aknuniteditor.h>
#include <barsread.h>
#include <lmkui.rsg>
#include "clmkcentralrepository.h"
#include <locnotprefplugindomaincrkeys.h>
#include <centralrepository.h>
#include <CLmkEditorDlg.h>

//================== CONSTANTS===============================
const TReal KFootToMeterFraction = 0.3048;
// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CLmkEditorNumberField::CLmkEditorNumberField
// ----------------------------------------------------
//
CLmkEditorNumberField::CLmkEditorNumberField
        ( MLmkFieldData& aField,
		  MLmkEditorUiBuilder& aUiBuilder ) :
    CLmkEditorFieldBase( aField, aUiBuilder )
    {
    }

// ----------------------------------------------------
// CLmkEditorNumberField::ConstructL
// ----------------------------------------------------
//
void CLmkEditorNumberField::ConstructL()
    {
    iRepository = CLmkCentralRepository::NewL();
    CreateAvkonUnitEditorL();
    iCaptionedCtrl = iUiBuilder.LineControl( ControlId() );
    iCaptionedCtrl->SetTakesEnterKey( ETrue );
    }

// ----------------------------------------------------
// CLmkEditorNumberField::NewL
// ----------------------------------------------------
//
CLmkEditorNumberField* CLmkEditorNumberField::NewL
        ( MLmkFieldData& aField,
		 MLmkEditorUiBuilder& aUiBuilder )
    {
    CLmkEditorNumberField* self =
        new(ELeave) CLmkEditorNumberField( aField, aUiBuilder );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------
// CLmkEditorNumberField::~CLmkEditorNumberField
// ----------------------------------------------------
//
CLmkEditorNumberField::~CLmkEditorNumberField()
    {
    delete iRepository;
    }

// ----------------------------------------------------
// CLmkEditorNumberField::SaveFieldL
// ----------------------------------------------------
//
TBool CLmkEditorNumberField::SaveFieldL()
    {
    TBool result = EFalse;
	TReal32 oldData = LandmarkItemField().FloatData();
	TReal32 currData = iControl->Value();
	if( oldData != currData)
        {
        if (!IsSystemUnitMetric())
            {
            if (LandmarkItemField().FieldType() == EPositionAccuracy
                    || LandmarkItemField().FieldType() == EAltitudeAccuracy)
                {
                if (currData < (KMinAccuracy / KFootToMeterFraction))
                    {
                    currData = (KMinAccuracy / KFootToMeterFraction);
                    }
                else if (currData > (KMaxAccuracy / KFootToMeterFraction))
                    {
                    currData = (KMaxAccuracy / KFootToMeterFraction);
                    }
                }
            else if (LandmarkItemField().FieldType() == EAltitude)
                {
                if (currData < (KMinAltitude / KFootToMeterFraction))
                    {
                    currData = (KMinAltitude / KFootToMeterFraction);
                    }
                else if (currData > (KMaxAltitude / KFootToMeterFraction))
                    {
                    currData = (KMaxAltitude / KFootToMeterFraction);
                    }
                }

            ConvertFromFootToMeters(currData);
            }
        else
            {
            if (LandmarkItemField().FieldType() == EPositionAccuracy
                    || LandmarkItemField().FieldType() == EAltitudeAccuracy)
                {
                if (currData < KMinAccuracy)
                    {
                    currData = KMinAccuracy;
                    }
                else if (currData > KMaxAccuracy)
                    {
                    currData = KMaxAccuracy;
                    }
                }

            else if (LandmarkItemField().FieldType() == EAltitude)
                {
                if (currData < KMinAltitude)
                    {
                    currData = KMinAltitude;
                    }
                else if (currData > KMaxAltitude)
                    {
                    currData = KMaxAltitude;
                    }
                }
            }

        LandmarkItemField().SetTReal(currData);
        result = ETrue;
        }
   	return result;
    }

// ----------------------------------------------------
// CLmkEditorNumberField::ActivateL
// ----------------------------------------------------
//
void CLmkEditorNumberField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }

// ----------------------------------------------------
// CLmkEditorNumberField::UpdateL
// ----------------------------------------------------
//
void CLmkEditorNumberField::UpdateL()
    {
    TReal32 fieldData = LandmarkItemField().FloatData();
    TReal32 dispData = fieldData;
    if( fieldData == iDbData)
	    {
	    dispData = 	iConvertedData;
	    }
	else
		{
		if( !IsSystemUnitMetric())
			{
			//Imperial
			iDbData = fieldData;
			ConvertFromMetersToFoot( fieldData);
			iConvertedData = fieldData;
			dispData = 	iConvertedData;
			}
		}

	ResolveUnitL();
    iControl->SetValue( dispData );
    }

// ----------------------------------------------------
// CLmkEditorNumberField::CreateAvkonUnitEditorL
// ----------------------------------------------------
//
void CLmkEditorNumberField::CreateAvkonUnitEditorL()
	{
	 // Create Distance editor using AVKON's unit editor
    // and insert a line in the dialog
   iControl = static_cast<CAknUnitEditor*>( iUiBuilder.CreateLineL(
            FieldLabel(), LandmarkItemField().UniqueFieldIdentity(),
            EAknCtUnitEditor ) );
    //read control from resource file.
    TResourceReader reader;
	iEnv->CreateResourceReaderLC(reader, R_LMK_UNIT_EDITOR);
	iControl->ConstructFromResourceL(reader);
	CleanupStack::PopAndDestroy();// reader
	ResolveUnitL();

	// Obtain the current system of measurement
	TInt sysOfMeasurement = EAknUnitEditorMeter;
	if( !IsSystemUnitMetric())
		{
		sysOfMeasurement = EAknUnitEditorFoot;
		}

    /*
     if ( LandmarkItemField().FieldType() == EPositionAccuracy ||
     LandmarkItemField().FieldType() == EAltitudeAccuracy )
     {
     if( EAknUnitEditorMeter == sysOfMeasurement )
     {
     iControl->SetMinimumAndMaximum(KMinAccuracy, KMaxAccuracy);
     }
     else
     {
     iControl->SetMinimumAndMaximum( KMinAccuracy / KFootToMeterFraction,
     KMaxAccuracy / KFootToMeterFraction );
     }

     }
     else if ( LandmarkItemField().FieldType() == EAltitude )
     {
     if( EAknUnitEditorMeter == sysOfMeasurement )
     {
     iControl->SetMinimumAndMaximum( KMinAltitude, KMaxAltitude );
     }
     else
     {
     iControl->SetMinimumAndMaximum( KMinAltitude / KFootToMeterFraction,
     KMaxAltitude / KFootToMeterFraction );
     }
     }
     */

    TReal32 fieldData = LandmarkItemField().FloatData();
    iDbData = fieldData;
    iConvertedData = fieldData;

	if( EAknUnitEditorFoot == sysOfMeasurement )
		{
		//Imperial
		ConvertFromMetersToFoot( fieldData);
		iConvertedData = fieldData;
		}
	iControl->SetValue(iConvertedData);
	}

// -----------------------------------------------------------------------------
//CLmkEditorNumberField::FieldValue()
// -----------------------------------------------------------------------------
//
TReal CLmkEditorNumberField::FieldValue() const
	{
	return iControl->Value();
	}

// -----------------------------------------------------------------------------
//CLmkEditorNumberField::ResolveUnitL()
// -----------------------------------------------------------------------------
//
void CLmkEditorNumberField::ResolveUnitL()
	{
    TReal fieldData = LandmarkItemField().FloatData();

    if( IsSystemUnitMetric())
		{
		TInt fieldUnit = EAknUnitEditorMeter;
		iControl->SetUnitL( fieldUnit );
		}
	else
		{
		TInt fieldUnit = EAknUnitEditorFoot;
		iControl->SetUnitL( fieldUnit );
		}
	}

// -----------------------------------------------------------------------------
//CLmkEditorNumberField::ConvertFromFootToMeters()
// -----------------------------------------------------------------------------
//
void CLmkEditorNumberField::ConvertFromFootToMeters(TReal32& aValue)
	{

	aValue *= KFootToMeterFraction;

	}

// -----------------------------------------------------------------------------
//CLmkEditorNumberField::ConvertFromMetersToFoot()
// -----------------------------------------------------------------------------
//
void CLmkEditorNumberField::ConvertFromMetersToFoot(TReal32& aValue)
	{

	aValue /= KFootToMeterFraction;

	}

// -----------------------------------------------------------------------------
//CLmkEditorNumberField::IsSystemUnitMetric()
// -----------------------------------------------------------------------------
//
TBool CLmkEditorNumberField::IsSystemUnitMetric()
	{
	TBool result = EFalse;
	TInt keyValue;
	TInt err = iRepository->GetKeyValue( KLocSystemofMeasurements, keyValue );
	if( keyValue == EUnitsMetric )
		{
		result = ETrue;
		}
	return result;
	}


// -----------------------------------------------------------------------------
//CLmkEditorNumberField::HandleUnitChangeL()
// -----------------------------------------------------------------------------
//
void CLmkEditorNumberField::HandleUnitChangeL()
	{
	// Obtain the current system of measurement
	TInt sysOfMeasurement = EAknUnitEditorMeter;
	if( !IsSystemUnitMetric())
		{
		sysOfMeasurement = EAknUnitEditorFoot;
		}

	TReal32 controlData = iControl->Value();
	if( controlData != iConvertedData)
		{
		iConvertedData = controlData;
		}

    if( EAknUnitEditorMeter == sysOfMeasurement )
	    {

	    //Imperial to metric
	    ConvertFromFootToMeters( iConvertedData );
	    }
	else
		{
		//metric to imperial
		ConvertFromMetersToFoot( iConvertedData );
		}

    if ( LandmarkItemField().FieldType() == EPositionAccuracy ||
    	 LandmarkItemField().FieldType() == EAltitudeAccuracy )
        {
        if( EAknUnitEditorMeter == sysOfMeasurement )
        	{
        	iControl->SetMinimumAndMaximum(KMinAccuracy, KMaxAccuracy);
        	}
		else
			{
			iControl->SetMinimumAndMaximum( KMinAccuracy / KFootToMeterFraction,
							 			    KMaxAccuracy / KFootToMeterFraction );
			}
        }
    else if ( LandmarkItemField().FieldType() == EAltitude )
        {
        if( EAknUnitEditorMeter == sysOfMeasurement )
        	{
        	iControl->SetMinimumAndMaximum( KMinAltitude, KMaxAltitude );
        	}
		else
			{
        	iControl->SetMinimumAndMaximum( KMinAltitude / KFootToMeterFraction,
        									KMaxAltitude / KFootToMeterFraction );
			}
        }
	ResolveUnitL();
    iControl->SetValue( iConvertedData );
	}

// -----------------------------------------------------------------------------
//CLmkEditorFieldBase::ResetL()
// -----------------------------------------------------------------------------
//
void CLmkEditorNumberField::ResetL()
	{
	TRealX temp;
    temp.SetNaN();
    iControl->SetValue(temp);
	}
// End of File
