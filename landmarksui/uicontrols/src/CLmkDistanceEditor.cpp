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
* Description:    LandmarksUi Content File -
*
*/








// INCLUDE FILES
#include <eikmfne.h>

#include <eikenv.h>
#include <stringloader.h>
#include <e32math.h>
#include <AknUtils.h>

#include <LmkUi.rsg>
#include "landmarks.hrh"
#include "LmkConsts.h"

#include "CLmkMfneFloat.h"
#include "CLmkDistanceEditor.h"
#include <AknBidiTextUtils.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

const TInt KKeyToMatchEmptyUnits = 0;

const TInt KKeyToMatchMeters = 109;
const TInt KKeyToMatchKilometers = 75;
const TInt KKeyToMatchFeet = 87;
const TInt KKeyToMatchMiles = 77;

const TInt KNumMetricUnitSymbols = 2;
const TInt KNumImperialUnitSymbols = 2;

const TInt KMaxNumSignificantDigits = 7;  // DDDDD.DD
const TInt KMaxNumDecimalDigits = 2;
const TInt KNumFields = 2; // float, Units
}  // namespace

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CLmkDistanceEditor::CLmkDistanceEditor
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CLmkDistanceEditor::CLmkDistanceEditor()
    {
    }

// ----------------------------------------------------------------------------
// CLmkDistanceEditor::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CLmkDistanceEditor::ConstructL(
    TReal32 aMinimumValue,
    TReal32 aMaximumValue,
    TReal32 aInitialValue)
    {
    iMinimumValue = aMinimumValue;
    iMaximumValue = aMaximumValue;

    // create float field
    BaseConstructL(KNumFields);

    if (!AknLayoutUtils::LayoutMirrored())
        {
        ConstructValueFieldL(KMaxNumSignificantDigits, KMaxNumDecimalDigits);
        ConstructUnitFieldL();
        }
    else
        {
        ConstructUnitFieldL();
        ConstructValueFieldL(KMaxNumSignificantDigits, KMaxNumDecimalDigits);
        }

    // Init value
    SetUnit(DefaultUnit());
    UpdateValueLimits();
    SetNumber(aInitialValue);
    ShowUnits(!Math::IsNaN(aInitialValue));
    }

// ----------------------------------------------------------------------------
// CLmkDistanceEditor::ConstructUnitFieldL
// ----------------------------------------------------------------------------
//
void CLmkDistanceEditor::ConstructUnitFieldL()
    {
    TLocale locale;

    //create distance types
    TInt numUnitsymbols = KNumMetricUnitSymbols + 1; // +empty Units
    if (locale.UnitsGeneral() == EUnitsImperial)
        {
        numUnitsymbols += KNumImperialUnitSymbols;
        }

    iUnitsField = CEikMfneSymbol::NewL(numUnitsymbols);

    // create Units field
    HBufC* type = NULL;

    // add empty Units
    type = HBufC::NewLC(1);
    iUnitsField->AddSymbolicItem(
        CEikMfneSymbol::CItem::NewL(EUnitNone, KKeyToMatchEmptyUnits, type ),
        ETrue);
    CleanupStack::Pop(type);

    // add metric Units
    type = StringLoader::LoadLC( R_LMK_DISTANCE_EDITOR_TYPE_M, iCoeEnv );
    iUnitsField->AddSymbolicItem(
        CEikMfneSymbol::CItem::NewL( EUnitMeters, KKeyToMatchMeters, type ),
        EFalse);
    CleanupStack::Pop(type);

    type = StringLoader::LoadLC( R_LMK_DISTANCE_EDITOR_TYPE_KM, iCoeEnv );
    iUnitsField->AddSymbolicItem(
        CEikMfneSymbol::CItem::NewL( EUnitKilometers, KKeyToMatchKilometers, type ),
        EFalse);
    CleanupStack::Pop(type);

    // Add imperial Units for those who use them
    if ( locale.UnitsGeneral() == EUnitsImperial )
        {
        type = StringLoader::LoadLC( R_LMK_DISTANCE_EDITOR_TYPE_FT, iCoeEnv );
        iUnitsField->AddSymbolicItem(
            CEikMfneSymbol::CItem::NewL( EUnitFeet, KKeyToMatchFeet, type ),
            EFalse);
        CleanupStack::Pop(type);

        type = StringLoader::LoadLC( R_LMK_DISTANCE_EDITOR_TYPE_MI, iCoeEnv );
        iUnitsField->AddSymbolicItem(
            CEikMfneSymbol::CItem::NewL( EUnitMiles, KKeyToMatchMiles, type ),
            EFalse);
        CleanupStack::Pop(type);
        }

    AddField(iUnitsField);
    }

// ----------------------------------------------------------------------------
// CLmkDistanceEditor::~CLmkDistanceEditor
// ----------------------------------------------------------------------------
//
CLmkDistanceEditor::~CLmkDistanceEditor()
    {
    }

// ----------------------------------------------------
// CLmkDistanceEditor::Number
// ----------------------------------------------------
//
TReal CLmkDistanceEditor::Number() const
    {
    return UnitsToMeters(CLmkFloatEditor::Number(), Unit());
    }

// ----------------------------------------------------
// CLmkDistanceEditor::SetNumber
// ----------------------------------------------------
//
void CLmkDistanceEditor::SetNumber(TReal aNumber)
    {
    ShowUnits(!Math::IsNaN(aNumber));
    CLmkFloatEditor::SetNumber(MetersToUnits(aNumber, Unit()));
    }

// ----------------------------------------------------
// CLmkDistanceEditor::SetUnit
// ----------------------------------------------------
//
void CLmkDistanceEditor::SetUnit(TDistanceUnit aUnitId)
    {
    iUnitsField->SetCurrentSymbolicItemToId(aUnitId);
    UpdateValueLimits();
    }

// ----------------------------------------------------
// CLmkDistanceEditor::Unit
// ----------------------------------------------------
//
CLmkDistanceEditor::TDistanceUnit CLmkDistanceEditor::Unit() const
    {
    return (TDistanceUnit) iUnitsField->IdOfCurrentSymbolicItem();
    }

// ----------------------------------------------------
// CLmkDistanceEditor::DefaultUnit
// ----------------------------------------------------
//
CLmkDistanceEditor::TDistanceUnit CLmkDistanceEditor::DefaultUnit() const
    {
    return EUnitMeters;
    }

// ----------------------------------------------------
// CLmkDistanceEditor::UpdateValueLimits
// ----------------------------------------------------
//
void CLmkDistanceEditor::UpdateValueLimits()
    {
    if (Unit() != EUnitNone)
        {
        SetLimits(
            MetersToUnits(iMinimumValue, Unit()),
            MetersToUnits(iMaximumValue, Unit()));
        }
    }

// ----------------------------------------------------
// CLmkDistanceEditor::MetersToUnits
// ----------------------------------------------------
//
TReal32 CLmkDistanceEditor::MetersToUnits(
    TReal32 aMeters,
    TDistanceUnit aToUnits) const
    {
    TRealX nan;
    nan.SetNaN();

    if (Math::IsNaN(aMeters))
        {
        return nan;
        }

    switch (aToUnits)
        {
        case EUnitMeters:
            return aMeters;
        case EUnitKilometers:
            return aMeters / KKilometerToMeter;
        case EUnitMiles:
            return aMeters / KMileToMeter;
        case EUnitFeet:
            return aMeters * KMetreToFoot;
        default:
            return nan;
        }
    }

// ----------------------------------------------------
// CLmkDistanceEditor::UnitsToMeters
// ----------------------------------------------------
//
TReal32 CLmkDistanceEditor::UnitsToMeters(
    TReal32 aValue,
    TDistanceUnit aFromUnits) const
    {
    TRealX nan;
    nan.SetNaN();

    if (Math::IsNaN(aValue))
        {
        return nan;
        }

    switch (aFromUnits)
        {
        case EUnitMeters:
            return aValue;
        case EUnitKilometers:
            return aValue * KKilometerToMeter;
        case EUnitMiles:
            return aValue * KMileToMeter;
        case EUnitFeet:
            return aValue / KMetreToFoot;
        default:
            return nan;
        }
    }

// ----------------------------------------------------
// CLmkDistanceEditor::ShowUnits
// ----------------------------------------------------
//
void CLmkDistanceEditor::ShowUnits(TBool aVisible)
    {
    if (aVisible)
        {
        if (Unit() == EUnitNone)
            {
            SetUnit(DefaultUnit());
            }
        }
    else
        {
        SetUnit(EUnitNone);
        }
    }

// ----------------------------------------------------
// CLmkDistanceEditor::HandleEnter
// ----------------------------------------------------
//
void CLmkDistanceEditor::HandleEnter()
    {
    ShowUnits(ETrue);
    CLmkFloatEditor::HandleEnter();
    }

// ----------------------------------------------------
// CLmkDistanceEditor::HandleExit
// ----------------------------------------------------
//
void CLmkDistanceEditor::HandleExit()
    {
    ShowUnits(!IsNan());
    CLmkFloatEditor::HandleExit();
    }

// ----------------------------------------------------
// CLmkDistanceEditor::HandleCustomFieldChanged
// ----------------------------------------------------
//
void CLmkDistanceEditor::HandleCustomFieldChanged(TInt aFieldIndex)
    {
    if (Field(aFieldIndex) == iUnitsField)
        {
        if (Unit() == EUnitNone)
            {
            // cycle from empty unit
            SetUnit(DefaultUnit());
            }
        else
            {
            UpdateValueLimits();
            }
        }
    }

//  End of File
