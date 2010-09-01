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
#include <e32math.h>
#include <stringloader.h>
#include <eikenv.h>
#include <AknUtils.h>

#include <LmkUi.rsg>
#include "LmkConsts.h"
#include "landmarks.hrh"
#include "CLmkMfneSeparator.h"
#include "CLmkCoordinateEditor.h"
#include <AknBidiTextUtils.h>

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

const TInt KKeyToMatchEmptyCardinal = 0;

const TInt KKeyToMatchNorth = 78;
const TInt KKeyToMatchSouth = 83;
const TInt KKeyToMatchWest = 87;
const TInt KKeyToMatchEast = 69;

const TInt KNumFields = 3; // value, degree, cardinal

const TInt KLatitudeMaxLength = 7; // DD.DDDD
const TInt KLatitudeMaxDecimals = 4;
const TInt KLongitudeMaxLength = 8; // DDD.DDDD
const TInt KLongitudeMaxDecimals = 4;

const TReal KLatitudeMaxLimit = 90;
const TReal KLongitudeMaxLimit = 180;

const TInt KNumCardinalSymbols = 3; // NS / WE + empty

_LIT(KEmptyDegreeMark, "");

#if defined(_DEBUG)
/// Panic codes for CLmkCoordinateEditor
enum TPanicCode
    {
	EPanicUnknownEditorType
    };

void Panic(TPanicCode aReason)
    {
    _LIT(KPanicText, "CLmkCoordinateEditor");
    User::Panic(KPanicText, aReason);
    }
#endif  // _DEBUG

}  // namespace

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CLmkCoordinateEditor::CLmkCoordinateEditor
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CLmkCoordinateEditor::CLmkCoordinateEditor()
    {
    }

// ----------------------------------------------------------------------------
// CLmkCoordinateEditor::~CLmkCoordinateEditor
// ----------------------------------------------------------------------------
//
CLmkCoordinateEditor::~CLmkCoordinateEditor()
    {
    delete iDegreeMark;
    }

// ----------------------------------------------------------------------------
// CLmkCoordinateEditor::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CLmkCoordinateEditor::ConstructL(
    TReal aInitialValue,
    CLmkCoordinateEditor::TEditorType aType)
    {
	iEditorType = aType;

    TInt maxLength = 0, maxDecimals = 0;
    TReal maxLimit = 0;
    switch( iEditorType )
        {
        case ELatitudeEditor:
            {
            maxLength = KLatitudeMaxLength;
            maxDecimals = KLatitudeMaxDecimals;
            maxLimit = KLatitudeMaxLimit;
            break;
            }
        case ELongitudeEditor:
            {
            maxLength = KLongitudeMaxLength;
            maxDecimals = KLongitudeMaxDecimals;
            maxLimit = KLongitudeMaxLimit;
            break;
            }
        default:
            {
            __ASSERT_DEBUG(EFalse, Panic(EPanicUnknownEditorType));
            break;
            }
        }

    // create float field
    BaseConstructL(KNumFields);

    if (!AknLayoutUtils::LayoutMirrored())
        {
        ConstructValueFieldL(maxLength, maxDecimals);
        ConstructDegreeMarkFieldL();
        ConstructCardinalPointFieldL();
        }
    else
        {
        ConstructCardinalPointFieldL();
        ConstructValueFieldL(maxLength, maxDecimals);
        ConstructDegreeMarkFieldL();
        }

    SetCardinal(DefaultCardinal());
    SetLimits(0, maxLimit);
    SetNumber(aInitialValue);
    ShowDegreeAndCardinal(!Math::IsNaN(aInitialValue));
    }

// ----------------------------------------------------------------------------
// CLmkCoordinateEditor::ConstructDegreeMarkFieldL
// ----------------------------------------------------------------------------
//
void CLmkCoordinateEditor::ConstructDegreeMarkFieldL()
    {
    iDegreeMark = StringLoader::LoadL( R_LMK_DEGREE_MARK, iCoeEnv );
    iDegreeField = CLmkMfneSeparator::NewL(iDegreeMark->Des());
    AddField(iDegreeField);
    }

// ----------------------------------------------------------------------------
// CLmkCoordinateEditor::ConstructCardinalPointFieldL
// ----------------------------------------------------------------------------
//
void CLmkCoordinateEditor::ConstructCardinalPointFieldL()
    {
    HBufC* text = NULL;
    iCardinalField = CEikMfneSymbol::NewL(KNumCardinalSymbols);

    // add empty cardinal
    text = HBufC::NewLC(1);
    iCardinalField->AddSymbolicItem(
        CEikMfneSymbol::CItem::NewL(ECardinalNone, KKeyToMatchEmptyCardinal, text),
        ETrue);
    CleanupStack::Pop(text);

    // read and add cardinals
    if ( iEditorType == ELatitudeEditor  )
        {
        text = StringLoader::LoadLC(R_LMK_CARDINAL_POINT_N, iCoeEnv );
        iCardinalField->AddSymbolicItem(
            CEikMfneSymbol::CItem::NewL(ECardinalNorth, KKeyToMatchNorth, text),
            EFalse);
        CleanupStack::Pop(text);

		text = StringLoader::LoadLC(R_LMK_CARDINAL_POINT_S, iCoeEnv );
        iCardinalField->AddSymbolicItem(
            CEikMfneSymbol::CItem::NewL(ECardinalSouth, KKeyToMatchSouth, text),
            EFalse);
        CleanupStack::Pop(text);
        }
    else
        {
		text = StringLoader::LoadLC(R_LMK_CARDINAL_POINT_E, iCoeEnv );
        iCardinalField->AddSymbolicItem(
            CEikMfneSymbol::CItem::NewL(ECardinalEast, KKeyToMatchEast, text),
            EFalse);
        CleanupStack::Pop(text);

        text = StringLoader::LoadLC(R_LMK_CARDINAL_POINT_W, iCoeEnv );
        iCardinalField->AddSymbolicItem(
            CEikMfneSymbol::CItem::NewL(ECardinalWest, KKeyToMatchWest, text),
            EFalse);
        CleanupStack::Pop(text);
        }

    AddField(iCardinalField);
    }

// ----------------------------------------------------
// CLmkCoordinateEditor::Number
// ----------------------------------------------------
//
TReal CLmkCoordinateEditor::Number() const
    {
    TReal coord = CLmkFloatEditor::Number();
    if (Math::IsNaN(coord))
        {
        return coord;
        }

    // South and West coordinates are negative
    switch(Cardinal())
        {
        case ECardinalSouth:
        case ECardinalWest:
            {
            coord = -coord;
            break;
            }
        default:
            break;
        }

    return coord;
    }

// ----------------------------------------------------
// CLmkCoordinateEditor::SetNumber
// ----------------------------------------------------
//
void CLmkCoordinateEditor::SetNumber(TReal aNumber)
    {
    if (!Math::IsNaN(aNumber))
        {
        // set cardinal point according to number sign
        // zero belongs to positive cardinals
        TCardinalPoint cardinalId = ECardinalNone;
        if (iEditorType == ELatitudeEditor)
            {
            cardinalId = (aNumber >= 0) ? ECardinalNorth : ECardinalSouth;
            }
        else // ELongitudeEditor
            {
            cardinalId = (aNumber >= 0) ? ECardinalEast : ECardinalWest;
            }
        SetCardinal(cardinalId);
		ShowDegreeAndCardinal(ETrue);
        CLmkFloatEditor::SetNumber(Abs(aNumber));
        }
    else
        {
        ShowDegreeAndCardinal(EFalse);
        CLmkFloatEditor::SetNumber(aNumber); // nan
        }
    }

// ----------------------------------------------------
// CLmkCoordinateEditor::SetCardinal
// ----------------------------------------------------
//
void CLmkCoordinateEditor::SetCardinal(TCardinalPoint aCardinalId)
    {
    iCardinalField->SetCurrentSymbolicItemToId(aCardinalId);
    CheckEastLongitude();
    }

// ----------------------------------------------------
// CLmkCoordinateEditor::Cardinal
// ----------------------------------------------------
//
CLmkCoordinateEditor::TCardinalPoint CLmkCoordinateEditor::Cardinal() const
    {
    return (TCardinalPoint) iCardinalField->IdOfCurrentSymbolicItem();
    }

// ----------------------------------------------------
// CLmkCoordinateEditor::DefaultCardinal
// ----------------------------------------------------
//
CLmkCoordinateEditor::TCardinalPoint CLmkCoordinateEditor::DefaultCardinal()
    {
    return (iEditorType == ELatitudeEditor) ? ECardinalNorth : ECardinalEast;
    }

// ----------------------------------------------------
// CLmkCoordinateEditor::CheckEastLongitude
// Abs(180) is allowed only for West
// ----------------------------------------------------
//
void CLmkCoordinateEditor::CheckEastLongitude()
    {
    if (Cardinal() == ECardinalEast &&
        CLmkFloatEditor::Number() == KLongitudeMaxLimit)
        {
        SetCardinal(ECardinalWest);
        }
    }

// ----------------------------------------------------
// CLmkCoordinateEditor::ShowDegreeAndCardinal
// ----------------------------------------------------
//
void CLmkCoordinateEditor::ShowDegreeAndCardinal(TBool aVisible)
    {
    if (aVisible)
        {
        if (iDegreeField->Text() == KEmptyDegreeMark)
            {
            TInt err = KErrNone;
            TRAP(err, iDegreeField->SetTextL(*iDegreeMark));
            }

        if (Cardinal() == ECardinalNone)
            {
            SetCardinal(DefaultCardinal());
            }
        }
    else
        {
        TInt err = KErrNone;
        TRAP(err, iDegreeField->SetTextL(KEmptyDegreeMark));
        SetCardinal(ECardinalNone);
        }
    }

// ----------------------------------------------------
// CLmkCoordinateEditor::HandleEnter
// ----------------------------------------------------
//
void CLmkCoordinateEditor::HandleEnter()
    {
    ShowDegreeAndCardinal(ETrue);
    CLmkFloatEditor::HandleEnter();
    }

// ----------------------------------------------------
// CLmkCoordinateEditor::HandleExit
// ----------------------------------------------------
//
void CLmkCoordinateEditor::HandleExit()
    {
    ShowDegreeAndCardinal(!IsNan());
    CLmkFloatEditor::HandleExit();
    }

// ----------------------------------------------------
// CLmkCoordinateEditor::HandleValueChanged
// ----------------------------------------------------
//
void CLmkCoordinateEditor::HandleValueChanged()
    {
    CheckEastLongitude();
    CLmkFloatEditor::HandleValueChanged();
    }

// ----------------------------------------------------
// CLmkCoordinateEditor::HandleCustomFieldChanged
// ----------------------------------------------------
//
void CLmkCoordinateEditor::HandleCustomFieldChanged(TInt aFieldIndex)
    {
    if (Field(aFieldIndex) == iCardinalField)
        {
        if (Cardinal() == ECardinalNone)
            {
            // cycle from empty cardinal
            SetCardinal(DefaultCardinal());
            }
        else
            {
            CheckEastLongitude();
            }
        }
    CLmkFloatEditor::HandleCustomFieldChanged(aFieldIndex);
    }

//  End of File
