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
* Description:    LandmarksUi Content File -
*
*/








// INCLUDE FILES
#include <e32math.h>
#include "CLmkMfneFloat.h"
#include <aknutils.h>

namespace {

const TInt KNumOtherSymbols = 2; // +/- and decimal point

_LIT(KMinus,"-");

const TInt KEventCodeStarSign(42);
const TInt KEventCodeMinusSign(45);
const TInt KKeyToMatchPoint = 35; // Shift/Hash
const TInt KPointKeyFromQwerty = 46;

#if defined(_DEBUG)
void Panic(TInt aReason)
    {
    _LIT(KPanicCategory, "LMK_MFNE_FLT");
    User::Panic(KPanicCategory, aReason);
    }
#endif

TReal Nan()
    {
    TRealX nan;
    nan.SetNaN();
    return nan;
    }

TBool IsAnyDigit(TChar aCharacter)
	{
	return (aCharacter.GetNumericValue() != KErrNotFound);
	}

void TrimRightmostChar(TDes& aText)
    {
    if (aText.Length())
        {
        aText.SetLength(aText.Length() - 1);
        }
    }
}

// ---------------------------------------------------------
// CLmkMfneFloat::CLmkMfneFloat
// ---------------------------------------------------------
//
CLmkMfneFloat::CLmkMfneFloat(
    TInt aMaxDigits,
    TInt aMaxDecimalDigits)
    :
    iMaxDigits(aMaxDigits),
    iMaxDecimalDigits(aMaxDecimalDigits)
    {
    __ASSERT_DEBUG(aMaxDigits > 0, Panic(KErrArgument));
    __ASSERT_DEBUG(aMaxDecimalDigits > 0, Panic(KErrArgument));
    __ASSERT_DEBUG(aMaxDecimalDigits <= aMaxDigits, Panic(KErrArgument));
    }

// ---------------------------------------------------------
// CLmkMfneFloat::~CLmkMfneFloat
// ---------------------------------------------------------
//
CLmkMfneFloat::~CLmkMfneFloat()
    {
    delete iText;
    }

// ---------------------------------------------------------
// CLmkMfneFloat::NewL
// ---------------------------------------------------------
//
CLmkMfneFloat* CLmkMfneFloat::NewL(
    TInt aMaxDigits,
    TInt aMaxDecimalDigits)
    {
    CLmkMfneFloat* number = new(ELeave)
        CLmkMfneFloat(aMaxDigits, aMaxDecimalDigits);

    CleanupStack::PushL(number);
    number->ConstructL();
    CleanupStack::Pop();

    return number;
    }

// ---------------------------------------------------------
// CLmkMfneFloat::ConstructL
// ---------------------------------------------------------
//
void CLmkMfneFloat::ConstructL()
    {
    iText = HBufC::NewL(MaxNumOfChars());
    }

// ---------------------------------------------------------
// CLmkMfneFloat::SetLimits
// ---------------------------------------------------------
//
TBool CLmkMfneFloat::SetLimits(
    TReal aMinimumValue,
    TReal aMaximumValue)
    {
    __ASSERT_DEBUG(
        Math::IsNaN(aMinimumValue) ||
        Math::IsNaN(aMaximumValue) ||
        (aMinimumValue < aMaximumValue), Panic(KErrArgument));

    iMinimumValue = aMinimumValue;
    iMaximumValue = aMaximumValue;

    return EnsureValueInLimits(ValueFromText(*iText));
    }

// ---------------------------------------------------------
// CLmkMfneFloat::GetLimits
// ---------------------------------------------------------
//
void CLmkMfneFloat::GetLimits(
    TReal& aMinimumValue,
    TReal& aMaximumValue) const
    {
    aMinimumValue = iMinimumValue;
    aMaximumValue = iMaximumValue;
    }

// ---------------------------------------------------------
// CLmkMfneFloat::SetValue
// ---------------------------------------------------------
//
void CLmkMfneFloat::SetValue(TReal aValue)
    {
    TPtr ptr(iText->Des());
    TextFromValue(aValue, ptr);
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );
    EnsureValueInLimits(aValue);
    iIsBeingEditedWithCursor = EFalse;
    }

// ---------------------------------------------------------
// CLmkMfneFloat::Value
// ---------------------------------------------------------
//
TReal CLmkMfneFloat::Value() const
    {
    return ValueFromText(*iText);
    }

// ---------------------------------------------------------
// CLmkMfneFloat::MaximumWidthInPixels
// ---------------------------------------------------------
//
TInt CLmkMfneFloat::MaximumWidthInPixels(
    const CFont& aFont,
    TBool /*aShrinkToMinimumSize*/)
    {
    TLocale locale;
    TInt digitWidth =
        TFindWidthOfWidestDigitType(locale.DigitType()).MaximumWidthInPixels(aFont);

    return MaxNumOfChars() * digitWidth;
    }

// ---------------------------------------------------------
// CLmkMfneFloat::InputCapabilities
// ---------------------------------------------------------
//
TCoeInputCapabilities CLmkMfneFloat::InputCapabilities() const
    {
    return TCoeInputCapabilities(TCoeInputCapabilities::EWesternNumericReal);
    }

// ---------------------------------------------------------
// CLmkMfneFloat::IsEditable
// ---------------------------------------------------------
//
TBool CLmkMfneFloat::IsEditable() const
    {
    return ETrue;
    }

// ---------------------------------------------------------
// CLmkMfneFloat::HandleHighlight
// ---------------------------------------------------------
//
void CLmkMfneFloat::HandleHighlight()
    {
    iFocus = ETrue;
    if ((iText->Length() == 0))
        {
        iIsBeingEditedWithCursor = ETrue;
        }
    }

// ---------------------------------------------------------
// CLmkMfneFloat::HighlightType
// ---------------------------------------------------------
//
CEikMfneField::THighlightType CLmkMfneFloat::HighlightType() const
    {
    if (iFocus)
        {
        return iIsBeingEditedWithCursor ? ECursor : EInverseVideo;
        }
    else
        {
        return EInverseVideo;
        }
    }

// ---------------------------------------------------------
// CLmkMfneFloat::HandleDeHighlight
// ---------------------------------------------------------
//
void CLmkMfneFloat::HandleDeHighlight(
    const CFont& /*aFont*/,
    CEikonEnv& /*aEikonEnv*/,
    TBool& aDataAltered,
    TBool& /*aError*/)
    {
    iIsBeingEditedWithCursor = EFalse;
    iFocus = EFalse;

    TPtr text(iText->Des());
    if (Math::IsNaN(ValueFromText(text)))
        {
        text.Zero();
        aDataAltered = ETrue;
        }
    }

// ---------------------------------------------------------
// CLmkMfneFloat::HandleKey
// ---------------------------------------------------------
//
void CLmkMfneFloat::HandleKey(
    const CFont& /*aFont*/,
    const TKeyEvent& aKeyEvent,
    TBool /*aInterpretLeftAndRightAsEarEvents*/,
    TBool& aDataAltered,
    TInt& aHighlightIncrement)
    {
    TChar ch = aKeyEvent.iCode;
    TPtr text = iText->Des();

    switch (ch)
        {
        case EKeyLeftArrow:
        case EKeyRightArrow:
            HandleLeftOrRightArrow(ch, aDataAltered, aHighlightIncrement);
            break;

        case EKeyBackspace:
            if (text.Length())
                {
                iIsBeingEditedWithCursor = ETrue;
                TrimRightmostChar(text);
                if (text == KMinus)
                    {
                    text.Zero();
                    }
                aDataAltered=ETrue;
                }
            break;

        case KEventCodeStarSign:
        case KEventCodeMinusSign:
            // if negative values allowed
            if (Math::IsNaN(iMinimumValue) || iMinimumValue < 0)
                {
                if (!iIsBeingEditedWithCursor)
                    {
                    iIsBeingEditedWithCursor = ETrue;
                    text.Zero();
                    }

                if (text.Length() == 0)
                    {
                    text.Append(KMinus);
                    aDataAltered=ETrue;
                    }
                }
            break;

        default:
            TLocale locale;
            TChar point = locale.DecimalSeparator();

            if ((ch == KKeyToMatchPoint || ch == KPointKeyFromQwerty ) && (iIsBeingEditedWithCursor))
                {
                if (IsDecimalSeparatorAllowed(text))
                    {
                    text.Append(point);
                    aDataAltered = ETrue;
                    }
                }

            if (IsAnyDigit(ch))
                {
                if (!iIsBeingEditedWithCursor)
                    {
                    iIsBeingEditedWithCursor = ETrue;
                    text.Zero();
                    aDataAltered=ETrue;
                    }

                if (IsMoreDigitsAllowed(text))
                    {
                    text.Append(ch);
                    aDataAltered = ETrue;

                    // check that value is within limits
                    if (EnsureValueInLimits(ValueFromText(text)))
                        {
                        // value is changed, make whole field selected
                        iIsBeingEditedWithCursor = EFalse;
                        }
                    }
                else
                    {
                    aHighlightIncrement = 1;
                    }
                }
            break;
            }
    }

// ---------------------------------------------------------
// CLmkMfneFloat::EnsureValueInLimits
// ---------------------------------------------------------
//
TBool CLmkMfneFloat::EnsureValueInLimits(TReal aValue)
    {
    if ( !Math::IsNaN(aValue))
        {
        TPtr ptr(iText->Des());
        if (aValue < iMinimumValue)
            {
            TextFromValue(iMinimumValue, ptr);
            return ETrue;
            }

        if (aValue > iMaximumValue)
            {
            TextFromValue(iMaximumValue, ptr);
            return ETrue;
            }
        }
    return EFalse;
    }

// ---------------------------------------------------------
// CLmkMfneFloat::Text
// ---------------------------------------------------------
//
const TDesC& CLmkMfneFloat::Text() const
    {
    TPtr ptr = (iText->Des());
    AknTextUtils::DisplayTextLanguageSpecificNumberConversion( ptr );
    return *iText;
    }

// ---------------------------------------------------------
// CLmkMfneFloat::MaxNumOfChars
// ---------------------------------------------------------
//
TInt CLmkMfneFloat::MaxNumOfChars() const
    {
    return iMaxDigits + KNumOtherSymbols;
    }

// ---------------------------------------------------------
// CLmkMfneFloat::TextHasDecimalSeparator
// ---------------------------------------------------------
//
TBool CLmkMfneFloat::IsDecimalSeparatorAllowed(const TDesC& aText) const
    {
    if (Math::IsNaN(ValueFromText(aText)))
        {
        return EFalse;
        }

    TLocale locale;
    TChar point = locale.DecimalSeparator();
    TInt i = 0;
    while (i < aText.Length())
        {
        if (TChar(aText[i++]) == point)
            {
            return EFalse;
            }
        }

    return (aText.Length() < MaxNumOfChars());
    }

// ---------------------------------------------------------
// CLmkMfneFloat::ValueFromText
// ---------------------------------------------------------
//
TBool CLmkMfneFloat::IsMoreDigitsAllowed(const TDesC& aText) const
    {
    // check that number of allowed digits is less then iMaxDigits
    TInt digits = 0, decimalDigits = 0;
    TInt isDecimalPart = EFalse;
    TLocale locale;
    TChar point = locale.DecimalSeparator();

    // if string starts with 0, then only decimal point is allowed
    _LIT(KSingleZero, "0");
    _LIT(KMinusZero, "-0");

    if (aText == KSingleZero || aText == KMinusZero)
        {
        return EFalse;
        }

    // other cases - check total amount of digits
    // and amount of decimal digits
    for (TInt i = 0; i < aText.Length(); i++)
        {
        if (IsAnyDigit(aText[i]))
            {
            digits++;
            }
        if (isDecimalPart)
            {
            decimalDigits++;
            }
        if (aText[i] == point)
            {
            isDecimalPart = ETrue;
            }
        }
    return ((digits < iMaxDigits) && (decimalDigits < iMaxDecimalDigits));
    }

// ---------------------------------------------------------
// CLmkMfneFloat::ValueFromText
// ---------------------------------------------------------
//
TReal CLmkMfneFloat::ValueFromText(const TDesC& aText) const
    {
    if (aText.Length() == 0)
        {
        return Nan();
        }

    TBool isNegative(EFalse);
    TInt integer(0);
    TInt integerDigits(0);
    TBool fractionPart(EFalse);
    TInt fraction(0);
    TInt fractionDigits(0);
    TLocale locale;

    for (TInt i = 0; i < aText.Length(); i++)
        {
        TChar ch(aText[i]);
        if (ch == '-')
            {
            __ASSERT_DEBUG(i == 0, Panic(KErrGeneral));
            isNegative = ETrue;
            continue;
            }

        if (ch == locale.DecimalSeparator())
            {
            __ASSERT_DEBUG(fractionPart == EFalse, Panic(KErrGeneral));
            fractionPart = ETrue;
            continue;
            }

        TInt val = ch.GetNumericValue();
        if (val >= 0)
            {
            if (!fractionPart)
                {
                integer = integer * 10 + val;
                integerDigits++;
                }
            else
                {
                fraction = fraction * 10 + val;
                fractionDigits++;
                }
            }
        }

    // integer part must contain at least "0".
    if (integerDigits < 1)
        {
        return Nan();
        }

    TReal value(fraction);
    for (TInt i = 0; i < fractionDigits; i++)
        {
        value /= 10;
        }

    value += TReal(integer);
    return (isNegative) ? -value : value;
    }

// ---------------------------------------------------------
// CLmkMfneFloat::TextFromValue
// ---------------------------------------------------------
//
void CLmkMfneFloat::TextFromValue(TReal aValue, TDes& aText) const
    {
    aText.SetLength(0);

    if (Math::IsNaN(aValue))
        {
        return;
        }

    TRealFormat format;
    format.iType = KRealFormatFixed;
    format.iTriLen = 0; // no triad separations
    format.iWidth = MaxNumOfChars();
    format.iPlaces = (aValue == 0) ? 0 : iMaxDecimalDigits;

    // convert real value to a string
    aText.Num(aValue, format);

    // cut trailing zeroes from decimal part
    TLocale locale;
    TInt pointIndex = aText.Locate(locale.DecimalSeparator());
    if (pointIndex != KErrNotFound)
        {
        TInt newLength = aText.Length();
        for (TInt i = aText.Length() - 1; i > pointIndex; i--)
            {
            if (aText[i] == '0')
                {
                newLength--;
                }
            else
                {
                break;
                }
            }

        // if only decimal point left, cut it also
        if (aText[newLength-1] == locale.DecimalSeparator())
            {
            newLength--;
            }

        aText.SetLength(newLength);
        }
    }
