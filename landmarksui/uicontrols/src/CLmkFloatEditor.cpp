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
#include <LmkUi.rsg>
#include <e32math.h>
#include <eikmfne.h>
#include <AknUtils.h>
#include <AknEditStateIndicator.h>
#include <eikenv.h>
#include <stringloader.h>

#include "landmarks.hrh"
#include "LmkConsts.h"

#include "CLmkMfneFloat.h"
#include "CLmkFloatEditor.h"

// CONSTANTS
/// Unnamed namespace for local definitions
namespace {

#if defined(_DEBUG)
void Panic(TInt aReason)
    {
    _LIT(KPanicCategory, "CLmkFloatEditor");
    User::Panic(KPanicCategory, aReason);
    }
#endif

}  // namespace

// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CLmkFloatEditor::CLmkFloatEditor
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CLmkFloatEditor::CLmkFloatEditor()
    {
    iIsMirroredLayout = AknLayoutUtils::LayoutMirrored();
    }

// ----------------------------------------------------
// CLmkFloatEditor::BaseConstructL
// ----------------------------------------------------
//
 void CLmkFloatEditor::BaseConstructL(TInt aMaxNumFields)
	{
    __ASSERT_DEBUG(aMaxNumFields > 0, Panic(KErrArgument));
    CreateFieldArrayL(aMaxNumFields);
	}

// ----------------------------------------------------
// CLmkFloatEditor::ConstructValueFieldL
// ----------------------------------------------------
//
 void CLmkFloatEditor::ConstructValueFieldL(
    TInt aMaxNumSignificantDigits,
    TInt aMaxDecimals)
    {
    __ASSERT_DEBUG(iValueField == NULL, Panic(KErrGeneral));

	iValueField =
	    CLmkMfneFloat::NewL(
	        aMaxNumSignificantDigits,
	        aMaxDecimals);

	AddField(iValueField);
	}

// ----------------------------------------------------------------------------
// CLmkFloatEditor::~CLmkFloatEditor
// ----------------------------------------------------------------------------
//
CLmkFloatEditor::~CLmkFloatEditor()
    {
    }

// ----------------------------------------------------
// CLmkDistanceEditor::SetNumberL
// ----------------------------------------------------
//
void CLmkFloatEditor::SetNumber(TReal aNumber)
    {
    __ASSERT_DEBUG(iValueField, Panic(KErrGeneral));
    iValueField->SetValue(aNumber);
    HandleValueChanged();
    }

// ----------------------------------------------------
// CLmkFloatEditor::Number
// ----------------------------------------------------
//
TReal CLmkFloatEditor::Number() const
    {
    __ASSERT_DEBUG(iValueField, Panic(KErrGeneral));
    return iValueField->Value();
    }

// ----------------------------------------------------
// CLmkFloatEditor::IsModified
// ----------------------------------------------------
//
TBool CLmkFloatEditor::IsModified()
    {
    return iIsModified;
    }

// ----------------------------------------------------
// CLmkFloatEditor::IsNan
// ----------------------------------------------------
//
TBool CLmkFloatEditor::IsNan()
    {
    __ASSERT_DEBUG(iValueField, Panic(KErrGeneral));
    return Math::IsNaN(iValueField->Value());
    }

// ----------------------------------------------------
// CLmkFloatEditor::SetLimits
// ----------------------------------------------------
//
TBool CLmkFloatEditor::SetLimits(TReal aMinimumValue, TReal aMaximumValue)
    {
    __ASSERT_DEBUG(iValueField, Panic(KErrGeneral));
    if (iValueField->SetLimits(aMinimumValue, aMaximumValue))
        {
        HandleValueChanged();
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

// ----------------------------------------------------
// CLmkFloatEditor::GetLimits
// ----------------------------------------------------
//
void CLmkFloatEditor::GetLimits(TReal& aMinimum, TReal& aMaximum)
    {
    __ASSERT_DEBUG(iValueField, Panic(KErrGeneral));
    iValueField->GetLimits(aMinimum, aMaximum);
    }

// ----------------------------------------------------
// CLmkFloatEditor::HandleValueChanged
// ----------------------------------------------------
//
void CLmkFloatEditor::HandleValueChanged()
    {
    if (IsNan())
        {
        // switch to iValueField,
        // otherwise it won't be possible to enter value anymore
        if (Field(CurrentField()) != iValueField)
            {
            HighlightField(FindValueField());
            }
        }
    DrawDeferred();
    }

// ----------------------------------------------------
// CLmkFloatEditor::HandleCustomFieldChanged
// ----------------------------------------------------
//
void CLmkFloatEditor::HandleCustomFieldChanged(TInt /*aCustomFieldIndex*/)
    {
    DrawDeferred();
    }

void CLmkFloatEditor::HandleEnter()
    {
    DrawDeferred();
    }

void CLmkFloatEditor::HandleExit()
    {
    DrawDeferred();
    }

// ----------------------------------------------------
// CLmkFloatEditor::FindValueField
// ----------------------------------------------------
//
TInt CLmkFloatEditor::FindValueField()
    {
    for (TInt i = 0; i < NumFields(); i++)
        {
        if (Field(i) == iValueField)
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// ----------------------------------------------------
// CLmkFloatEditor::HighlightField
// ----------------------------------------------------
//
void CLmkFloatEditor::HighlightField(TInt aFieldPosition)
    {
    if (CurrentField() != aFieldPosition)
        {
        TBool dataAltered = EFalse;
        TBool error = EFalse;
        const CFont& font=*Font();

        CEikMfne::HandleInteraction(
            ETrue,
            aFieldPosition,
            Field(CurrentField())->MaximumWidthInPixels(font, EFalse),
            Field(CurrentField())->HighlightType(),
            dataAltered,
            error);
        }
    }

// ----------------------------------------------------
// CLmkFloatEditor::FocusChanged
// ----------------------------------------------------
//
void CLmkFloatEditor::FocusChanged(TDrawNow aDrawNow)
    {
    if (!IsReadyToDraw())
        {
        // this is workaround for MFNE's bug.
        // otherwise it will panic hiding cursor
        return;
        }

    if (IsFocused())
        {
        HandleEnter();

        // need to inform CLmkMfneFloat that it is highlighted
        // because CEikMfne does not do it
        if (iValueField &&
            iValueField == Field(CurrentField()))
            {
            iValueField->HandleHighlight();
            }
        }
    else
        {
        HandleExit();
        }

    CEikMfne::FocusChanged(aDrawNow);
    }

// ----------------------------------------------------
// CLmkFloatEditor::OfferKeyEventL
// ----------------------------------------------------
//
TKeyResponse CLmkFloatEditor::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
    {

    // disallow switch to other fields if value is nan,
    // because it's hard to inform CLmkMfneFloat
    // that focus has changed to it from other field.
    // If it is not done, then CEikMfne will panic when
    // hiding cursor
    if (IsNan() &&
       (aKeyEvent.iCode == EKeyLeftArrow ||
        aKeyEvent.iCode == EKeyRightArrow))
        {
        return EKeyWasConsumed;
        }

    TInt oldField = CurrentField();
    TKeyResponse response = CEikMfne::OfferKeyEventL(aKeyEvent, aType);

    if (response == EKeyWasConsumed)
        {
        // update focus state in CLmkMfneFloat
        if (oldField != CurrentField() &&
            Field(CurrentField()) == iValueField)
            {
            iValueField->HandleHighlight();
            }

        // inform about changes
        if (aKeyEvent.iCode != EKeyLeftArrow &&
            aKeyEvent.iCode != EKeyRightArrow)
            {
            iIsModified = ETrue;
            if (Field(oldField) == iValueField)
                {
                HandleValueChanged();
                }
            else
                {
                HandleCustomFieldChanged(oldField);
                }
            }
        }

    DrawNow();
    return response;
    }

//  End of File
