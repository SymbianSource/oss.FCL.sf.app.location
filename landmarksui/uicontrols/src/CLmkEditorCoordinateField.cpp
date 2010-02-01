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
#include "CLmkEditorCoordinateField.h"
#include "MLmkEditorUiBuilder.h"
#include "LmkConsts.h"
#include "landmarks.hrh"
#include <eikcapc.h>
#include <e32math.h>
#include <lbsposition.h>
#include <aknlocationed.h>
#include <barsread.h>
#include <lmkui.rsg>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {
const TReal KDefaultHorizontalAccuracy = 0.1;
}  // namespace

// ================= MEMBER FUNCTIONS =======================

// ----------------------------------------------------
// CLmkEditorCoordinateField::CLmkEditorCoordinateField
// ----------------------------------------------------
//
CLmkEditorCoordinateField::CLmkEditorCoordinateField
        ( MLmkFieldData& aField,
		  MLmkEditorUiBuilder& aUiBuilder ) :
    CLmkEditorFieldBase( aField, aUiBuilder )
    {
    }

// ----------------------------------------------------
// CLmkEditorCoordinateField::ConstructL
// ----------------------------------------------------
//
void CLmkEditorCoordinateField::ConstructL()
    {
    // Create and insert a line in the dialog
	//using AVKON's coordinate editor
	CreateAvkonCoordinateEditorL();
    iCaptionedCtrl = iUiBuilder.LineControl( ControlId() );
    iCaptionedCtrl->SetTakesEnterKey( ETrue );
    TPosition position;
	iControl->Get(position);
	if (iIsLatitude)
		{
		iCurrentValue = position.Latitude();
		}
	else
		{
		iCurrentValue = position.Longitude();
		}
    }

// ----------------------------------------------------
// CLmkEditorCoordinateField::NewL
// ----------------------------------------------------
//
CLmkEditorCoordinateField* CLmkEditorCoordinateField::NewL
        ( MLmkFieldData& aField,
		 MLmkEditorUiBuilder& aUiBuilder )
    {
    CLmkEditorCoordinateField* self =
        new(ELeave) CLmkEditorCoordinateField( aField, aUiBuilder );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------
// CLmkEditorCoordinateField::~CLmkEditorCoordinateField
// ----------------------------------------------------
//
CLmkEditorCoordinateField::~CLmkEditorCoordinateField()
    {
    }

// ----------------------------------------------------
// CLmkEditorCoordinateField::SaveFieldL
// ----------------------------------------------------
//
TBool CLmkEditorCoordinateField::SaveFieldL()
    {
    TPosition position;
	iControl->Get(position);
	TReal field = LandmarkItemField().DoubleData();
	TReal controlData;
	if (iIsLatitude)
		{
		controlData = position.Latitude();
		}
	else
		{
		controlData = position.Longitude();
		}

	if( iCurrentValue != controlData)
		{
		LandmarkItemField().SetTReal(controlData);
		}
   	return ETrue;
    }

// ----------------------------------------------------
// CLmkEditorCoordinateField::ActivateL
// ----------------------------------------------------
//
void CLmkEditorCoordinateField::ActivateL()
    {
    iCaptionedCtrl->ActivateL();
    }

// ----------------------------------------------------
// CLmkEditorCoordinateField::UpdateL
// ----------------------------------------------------
//
void CLmkEditorCoordinateField::UpdateL()
    {
    TReal fieldData = LandmarkItemField().DoubleData();
	TPosition position = ConstructPosition(fieldData);
	iControl->Set(position);
	iControl->DrawDeferred();
    }

// ----------------------------------------------------
// CLmkEditorCoordinateField::CreateAvkonCoordinateEditorL
// ----------------------------------------------------
//
void CLmkEditorCoordinateField::CreateAvkonCoordinateEditorL()
	{
		//using AVKON's coordinate editor
	iControl = static_cast<CAknLocationEditor*>( iUiBuilder.CreateLineL(
            FieldLabel(), LandmarkItemField().UniqueFieldIdentity(),
            EAknCtLocationEditor));
    ConstructEditorFromResourceL();

	}
// ----------------------------------------------------
// CLmkEditorCoordinateField::ConstructEditorFromResourceL
// ----------------------------------------------------
//
void CLmkEditorCoordinateField::ConstructEditorFromResourceL()
	{

	TRealX fieldData;
	fieldData.SetNaN();
	TInt resId = R_LMK_LONG_EDITOR;
	fieldData = LandmarkItemField().DoubleData();
	if	(LandmarkItemField().FieldType() == ELatitude )
		{
		resId = R_LMK_LAT_EDITOR;
		iIsLatitude = ETrue;
		}
	TPosition position = ConstructPosition(fieldData);
	TResourceReader reader;
	iEnv->CreateResourceReaderLC(reader, resId);
	iControl->ConstructFromResourceL(reader);
	iControl->Set(position);
	CleanupStack::PopAndDestroy(); // reader
	}
// ----------------------------------------------------
// CLmkEditorCoordinateField::ConstructPosition
// ----------------------------------------------------
//
TPosition CLmkEditorCoordinateField::ConstructPosition(const TReal& aValue)
	{
	TRealX latData;
	latData.SetNaN();
	TRealX longData;
	longData.SetNaN();
	TTime time;
	time.HomeTime();
	if (iIsLatitude)
		{
		latData = aValue;
		}
	else
		{
		longData = aValue;
		}
	TCoordinate coord(latData, longData);
	TLocality loc(coord, KDefaultHorizontalAccuracy);
	TPosition position(loc, time);
	return position;
	}

// -----------------------------------------------------------------------------
//CLmkEditorFieldBase::FieldValue()
// -----------------------------------------------------------------------------
//
TReal CLmkEditorCoordinateField::FieldValue() const
	{
	TReal64 number = 0;
	if (iControl)
		{
		TPosition pos;
        iControl->Get(pos);
        if (iIsLatitude)
        	{
        	number = pos.Latitude();
        	}
        else
        	{
        	number = pos.Longitude();
        	}
		}
	return number;
	}

// -----------------------------------------------------------------------------
//CLmkEditorFieldBase::ResetL()
// -----------------------------------------------------------------------------
//
void CLmkEditorCoordinateField::ResetL()
	{
	TRealX latData;
	latData.SetNaN();
	TPosition position = ConstructPosition(latData);
	iControl->Set(position);
	}
// End of File
