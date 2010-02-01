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
#include <barsread.h>
#include <eikenv.h>
#include <NumberConversion.h>
#include <AknUtils.h>
#include <aknsoundsystem.h>
#include <AknAppUi.h>
#include <AknTextDecorationMetrics.h>
#include <eikctl.rsg>
#include <e32math.h>

#include "CLmkDecimalEditor.h"


_LIT(KMinus,"-");

namespace EUtils
	{
	LOCAL_D TBool IsAnyDigit(TChar aCharacter)
		{
		return ( aCharacter.GetNumericValue() != KErrNotFound);
		}
	}


// ---------------------------------------------------------
// CLmkDecimalEditor::CLmkDecimalEditor
// ---------------------------------------------------------
//
CLmkDecimalEditor::CLmkDecimalEditor(TInt aMinimumValue, TInt aMaximumValue, TUint32 aFlags)
									:iMinimumValue(aMinimumValue),
									 iMaximumValue(aMaximumValue),
									 iFlags(aFlags),
									 iDigitType(EDigitTypeWestern)
	{
	TBuf<16> minText;
	minText.AppendNum(aMinimumValue);
	TBuf<16> maxText;
	maxText.AppendNum(aMaximumValue);
	iMaxDigits=Max(minText.Length(),maxText.Length());

	iMaxDigitsMinimumValue=minText.Length();
	iMaxDigitsMaximumValue=maxText.Length();
	iEnv = CEikonEnv::Static();
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::~CLmkDecimalEditor
// ---------------------------------------------------------
//
CLmkDecimalEditor::~CLmkDecimalEditor()
	{
	delete iText;
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::NewL
// ---------------------------------------------------------
//
CLmkDecimalEditor* CLmkDecimalEditor::NewL(const CFont& aFont, TResourceReader& aResourceReader)
	{
	TInt minimumValue=aResourceReader.ReadInt32();
	TInt maximumValue=aResourceReader.ReadInt32();
	TUint32 flags=aResourceReader.ReadUint8();
	return NewL(aFont, minimumValue, maximumValue, maximumValue, flags, 0);
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::NewL
// ---------------------------------------------------------
//
CLmkDecimalEditor* CLmkDecimalEditor::NewL(const CFont& aFont, TInt aMinimumValue, TInt aMaximumValue, TInt aInitialValue, TUint32 aFlags, TInt aNumberOfZeros)
	{
	CLmkDecimalEditor* number=new(ELeave) CLmkDecimalEditor(aMinimumValue, aMaximumValue, aFlags);
	CleanupStack::PushL(number);
	number->iText=HBufC::NewL(number->MaximumNumberOfDigits()+((aMinimumValue<0)? 1: 0));
	number->ConstructL(aNumberOfZeros);
	number->SetValue(aInitialValue, aFont);
	CleanupStack::Pop();
	return number;
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::InvalidFieldAlert
// ---------------------------------------------------------
//
void CLmkDecimalEditor::InvalidFieldAlert() const
	{
    CAknKeySoundSystem* soundPlayer = (static_cast<CAknAppUi*>(iEnv->AppUi()))->KeySounds();
    if(soundPlayer)
        {
        soundPlayer->PlaySound(EAvkonSIDWarningTone);
        }
    }

// ---------------------------------------------------------
// CLmkDecimalEditor::AdditionalWidthForHighlights
// ---------------------------------------------------------
//
TInt CLmkDecimalEditor::AdditionalWidthForHighlights(const CFont& aFont) const
    {
    TInt addedWidth(0);
    if ( IsEditable() )
        {
        TInt rightHighlight(0);
        TAknTextDecorationMetrics metrics( &aFont );
        // Width is increased only by leftHighlight.  This is a compromise in that glyphs already have
        // spacing within them to achieve character spacing. This spacing is generally (for numbers) on the right.
        metrics.GetLeftAndRightMargins( addedWidth, rightHighlight ); // rightHighlight is not used
        }
    return addedWidth;
    }

// ---------------------------------------------------------
// CLmkDecimalEditor::WidthInPixels
// ---------------------------------------------------------
//
TInt CLmkDecimalEditor::WidthInPixels(const CFont& aFont) const
	{
	return Max( iMinimumWidthInPixels, aFont.TextWidthInPixels(Text()) + AdditionalWidthForHighlights(aFont) );
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::ConstructL
// ---------------------------------------------------------
//
void CLmkDecimalEditor::ConstructL(TInt aNumberOfZeros)
	{
	iNudgeCharMinus=iEnv->NudgeCharMinus();
	iNudgeCharPlus=iEnv->NudgeCharPlus();
	iNumberOfZeros = aNumberOfZeros;
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::SetMinimumAndMaximum
// ---------------------------------------------------------
//
void CLmkDecimalEditor::SetMinimumAndMaximum(TInt aMinimumValue, TInt aMaximumValue, const CFont& aFont)
	{
	TBuf<16> minText;
	minText.AppendNum(aMinimumValue);
	TBuf<16> maxText;
	maxText.AppendNum(aMaximumValue);
	TInt minTextLength=minText.Length();
	TInt maxTextLength=maxText.Length();
	iMinimumValue=aMinimumValue;
	iMaximumValue=aMaximumValue;
	TInt value=ValueFromText();
	if (value<aMinimumValue||!NumberOfDigits())
		SetTextToValue(aMinimumValue, aFont);
	else if (value>aMaximumValue)
		SetTextToValue(aMaximumValue, aFont);
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::GetMinimumAndMaximum
// ---------------------------------------------------------
//
void CLmkDecimalEditor::GetMinimumAndMaximum(TInt& aMinimumValue, TInt& aMaximumValue) const
	{
	aMinimumValue=iMinimumValue;
	aMaximumValue=iMaximumValue;
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::SetValue
// ---------------------------------------------------------
//
void CLmkDecimalEditor::SetValue(TInt aValue, const CFont& aFont)
	{
	SetTextToValue(aValue, aFont);
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::Value
// ---------------------------------------------------------
//
TReal CLmkDecimalEditor::Value() const
	{
	TInt numberOfDigits = NumberOfDigits();
	TReal retVal(0);
	TReal temp(0);

    if (numberOfDigits == 0)
        {
        InvalidFieldAlert();
        }

    TInt value = ValueFromText();
    if (value < iMinimumValue)
        {
		InvalidFieldAlert();
	    iEnv->LeaveWithInfoMsg(R_EIK_TBUF_NUMBER_BELOW_MIN, iMinimumValue);
        }
    if (value > iMaximumValue)
		{
	    iEnv->LeaveWithInfoMsg(R_EIK_TBUF_NUMBER_ABOVE_MAX, iMaximumValue);
        }
    Math::Pow10(temp, numberOfDigits * (-1));
    retVal = value * temp;
    return retVal;
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::IsValid
// ---------------------------------------------------------
//
TBool CLmkDecimalEditor::IsValid() const
	{
	return (NumberOfDigits() != 0);
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::NormalizeDigit
// ---------------------------------------------------------
//
TChar CLmkDecimalEditor::NormalizeDigit(TChar aChar)
	{
	TBuf<1> buf;
	buf.Append(aChar);
	NumberConversion::ConvertDigits(buf, iDigitType);
	return buf[0];
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::ZeroCharacter
// ---------------------------------------------------------
//
TText CLmkDecimalEditor::ZeroCharacter() const
		{
		return TText(iDigitType);
		}

// ---------------------------------------------------------
// CLmkDecimalEditor::NegativeCharacter
// ---------------------------------------------------------
//
TText CLmkDecimalEditor::NegativeCharacter() const
		{
		return TText('-');
		}

// ---------------------------------------------------------
// CLmkDecimalEditor::IsTextNull
// ---------------------------------------------------------
//
TBool CLmkDecimalEditor::IsTextNull() const
	{
	return ( Text().Length() < 1 );
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::MaximumWidthInPixels
// ---------------------------------------------------------
//
TInt CLmkDecimalEditor::MaximumWidthInPixels(const CFont& aFont, TBool aShrinkToMinimumSize)
	{
	if (aShrinkToMinimumSize)
		{
		TBuf<16> minText;
		minText.AppendNum(iMinimumValue);
		TBuf<16> maxText;
		maxText.AppendNum(iMaximumValue);

		iMaxDigits=Max(2,Max(minText.Length(),maxText.Length()));
		}
	return (MaximumNumberOfDigits()*TFindWidthOfWidestDigitType(DigitType()).MaximumWidthInPixels(aFont)) + AdditionalWidthForHighlights(aFont) /*+((iMinimumValueCateredForInFieldWidth<0)? aFont.TextWidthInPixels(_L("-")): 0)*/;
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::InputCapabilities
// ---------------------------------------------------------
//
TCoeInputCapabilities CLmkDecimalEditor::InputCapabilities() const
	{
	TUint inputCapabilities=TCoeInputCapabilities::ENone;
	if (iMinimumValue<0)
		{
		inputCapabilities|=TCoeInputCapabilities::EWesternNumericIntegerNegative;
		}
	if (iMaximumValue>0)
		{
		inputCapabilities|=TCoeInputCapabilities::EWesternNumericIntegerPositive;
		}
	return TCoeInputCapabilities(inputCapabilities);
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::Text
// ---------------------------------------------------------
//
const TDesC& CLmkDecimalEditor::Text() const
	{
	if (IsUninitialised())
		return KNullDesC();
	else
		return *iText;
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::IsEditable
// ---------------------------------------------------------
//
TBool CLmkDecimalEditor::IsEditable() const
	{
	return ETrue;
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::HighlightType
// ---------------------------------------------------------
//
CEikMfneField::THighlightType CLmkDecimalEditor::HighlightType() const
	{
	return (iFlags&EIsBeingEditedWithCursor)? ECursor: EInverseVideo;
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::HandleKey
// ---------------------------------------------------------
//
void CLmkDecimalEditor::HandleKey(const CFont& aFont, const TKeyEvent& aKeyEvent, TBool aInterpretLeftAndRightAsEarEvents, TBool& aDataAltered, TInt& aHighlightIncrement)
	{
	TChar ch=aKeyEvent.iCode;

	TPtr text=iText->Des();
	TInt textLength=text.Length();
	TBool nudgeTen=EFalse;
	switch (ch)
		{
	case EKeyLeftArrow:
	case EKeyRightArrow:
		if (!aInterpretLeftAndRightAsEarEvents)
			HandleLeftOrRightArrow(ch, aDataAltered, aHighlightIncrement);
		else
			{
		doAdjust:
			const TInt nudge=(nudgeTen? 10 : 1);
			TInt newValue;


			if ((textLength==0) || ! EUtils::IsAnyDigit( TChar(text[textLength-1]) ) )
				newValue=0;
			else
				newValue=ValueFromText()+((ch==EKeyLeftArrow)? -nudge: nudge);
			if (newValue>=iMaximumValue)
				newValue=iMaximumValue;
			else if (newValue<=iMinimumValue)
				newValue=iMinimumValue;
			SetTextToValue(newValue, aFont);
			aHighlightIncrement=0;// Always highlight the field being nudged
			aDataAltered=ETrue;
			SetUninitialised(EFalse);
			iFlags&=~EIsBeingEditedWithCursor;
			}
		break;
    case EKeyDownArrow:
    case EKeyUpArrow:
        {
		if (IsUninitialised())
			{
			SetUninitialised(EFalse);
			}
		else
			{
		        const TInt nudge=(nudgeTen? 10 : 1);
        		TInt newValue;


		        if ((textLength==0) || ! EUtils::IsAnyDigit( TChar(text[textLength-1])) )
        		    newValue=0;
        		else
            			newValue=ValueFromText()+((ch==EKeyDownArrow)? -nudge: nudge);

	 		if (newValue>iMaximumValue)
            			newValue=iMinimumValue;
        		else if (newValue<iMinimumValue)
		            newValue=iMaximumValue;
       			SetTextToValue(newValue, aFont);
        		aHighlightIncrement=0;// Always highlight the field being nudged
        		aDataAltered=ETrue;
        		iFlags&=~EIsBeingEditedWithCursor;
			}
		aDataAltered=ETrue;
        }
        break;
    case EKeyBackspace:
		if (textLength)
			{
			iFlags|=EIsBeingEditedWithCursor;
			text.SetLength(textLength-1);
			if (text==KMinus)
				text.SetLength(0);
			aDataAltered=ETrue;
			SetUninitialised(EFalse);
			}
		break;
	case '+':
		break;
	case '-':
        if (iMinimumValue < 0)
            {
			if (~iFlags&EIsBeingEditedWithCursor)
				{
				iFlags|=EIsBeingEditedWithCursor;
				text.SetLength(0);
				}
			if (text.Length()==0)
				{
				text.Append('-');
				aDataAltered=ETrue;
				SetUninitialised(EFalse);
				}
			}
		break;
	default:
		TBuf<2> nudgeTenChars;
		CCoeEnv::Static()->ReadResource(nudgeTenChars,R_EIK_TBUF_NUDGE_TEN_CHARS);

		if ( EUtils::IsAnyDigit(ch) )
			{
			if (~iFlags&EIsBeingEditedWithCursor)
				{
				iFlags|=EIsBeingEditedWithCursor;
				text.SetLength(0);
				}
			text.Append(NormalizeDigit(ch) );
			aDataAltered=ETrue;
			SetUninitialised(EFalse);
			if (NumberOfDigits()>=MaximumNumberOfDigits())
				aHighlightIncrement=1;
			}
		if (ch==iNudgeCharMinus)
			{
			ch=EKeyLeftArrow;
			goto doAdjust;
			}
		else if (ch==iNudgeCharPlus)
			{
			ch=EKeyRightArrow;
			goto doAdjust;
			}
		else if (ch==((TUint)nudgeTenChars[0]))
			{
			ch=EKeyLeftArrow;
			nudgeTen=ETrue;
			goto doAdjust;
			}
		else if (ch==((TUint)nudgeTenChars[1]))
			{
			ch=EKeyRightArrow;
			nudgeTen=ETrue;
			goto doAdjust;
			}
		break;
		}
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::HandleDeHighlight
// ---------------------------------------------------------
//
void CLmkDecimalEditor::HandleDeHighlight(const CFont& aFont, CEikonEnv& aEikonEnv, TBool& aDataAltered, TBool& aError)
	{
	iFlags&=~EIsBeingEditedWithCursor;
	if (NumberOfDigits()==0)
		{
		SetTextToValue(iMaximumValue, aFont);
		aDataAltered=ETrue;
		aError=ETrue;
		aEikonEnv.InfoMsg(R_EIK_TBUF_NO_NUMBER_ENTERED);
		return;
		}
	TInt value=ValueFromText();
    if ((value<iMinimumValue) || (value>iMaximumValue))
        {
        TBool succeeded = ConvertsIntoValidValue(value);
        if ( !succeeded )
            {
            aDataAltered=ETrue;
            aError=ETrue;
            if (value<iMinimumValue)
                {
                SetTextToValue(iMinimumValue, aFont);
				InvalidFieldAlert();
                aEikonEnv.InfoMsg(R_EIK_TBUF_NUMBER_BELOW_MIN, iMinimumValue);
                return;
                }
            if (value>iMaximumValue)
                {
				InvalidFieldAlert();
                SetTextToValue(iMaximumValue, aFont);
                aEikonEnv.InfoMsg(R_EIK_TBUF_NUMBER_ABOVE_MAX, iMaximumValue);
                return;
                }
            }
        }
	TBuf<128> oldText=*iText;
	TBool unInit = IsUninitialised();
	//SetTextToValue(value, aFont);
	if (oldText!=*iText)
		aDataAltered=ETrue;
	else
		SetUninitialised(unInit);
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::MaximumNumberOfDigits
// ---------------------------------------------------------
//
TInt CLmkDecimalEditor::MaximumNumberOfDigits() const
	{
	return Max(2,iMaxDigits);
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::NumberOfDigits
// ---------------------------------------------------------
//
TInt CLmkDecimalEditor::NumberOfDigits() const
	{
	return iText->Length();
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::SetTextToValue
// ---------------------------------------------------------
//
void CLmkDecimalEditor::SetTextToValue(TInt aValue, const CFont& aFont)
	{
	SetUninitialised(EFalse);
	TPtr text=iText->Des();
	text.SetLength(0);
	if (aValue<0)
		{
		text.Append('-');
		aValue=-aValue;
		}
	TInt firstUnsetCharacter=text.Length();
	text.SetLength(firstUnsetCharacter+MaximumNumberOfDigits());
	TInt lastUnsetCharacter=text.Length()-1;
	if (aValue==0)
		text[lastUnsetCharacter--]=ZeroCharacter();
	else
		{
		for (; aValue; aValue/=10)
			text[lastUnsetCharacter--]=(TText)(ZeroCharacter()+(aValue%10));
		}
	while(iNumberOfZeros--)
		{
		text[lastUnsetCharacter--]=(TText)(ZeroCharacter());
		}
	iNumberOfZeros = 0;

	if (lastUnsetCharacter>=firstUnsetCharacter)
		{
		if (iFlags&EFillWithLeadingZeros)
			while (lastUnsetCharacter>=firstUnsetCharacter) text[lastUnsetCharacter--]=ZeroCharacter();
		else
			text.Delete(firstUnsetCharacter, (lastUnsetCharacter+1)-firstUnsetCharacter);
		}
	if (iFlags&EPreserveOldWidthBeforeEditing)
		iMinimumWidthInPixels = WidthInPixels(aFont);
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::ValueFromText
// ---------------------------------------------------------
//
TInt CLmkDecimalEditor::ValueFromText() const
	{
	if (!NumberOfDigits())
		return iMinimumValue;
	TInt i=0;
	TInt valueFromText=0;
	TBool isNegative=EFalse;
	switch ((*iText)[i])
		{
	case '-':
		++i;
		isNegative=ETrue;
		break;
		}
	TInt textLength=iText->Length();
	for (; i<textLength; ++i)
		{
		TText digit=(*iText)[i];
		if (i>=textLength-2)
			{
			if (!isNegative && valueFromText>(KMaxTInt32/10))
				return KMaxTInt32;
			else if (isNegative && (-valueFromText)<(KMinTInt32/10))
				return KMinTInt32;
			}
		valueFromText=(valueFromText*10)+(TInt)(digit-ZeroCharacter());
		}
	if (isNegative==EFalse && valueFromText<0)
		return KMaxTInt32;// Deals with overflow
	if (isNegative)
		valueFromText=-valueFromText;
	return valueFromText;
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::ConvertsIntoValidValue
// ---------------------------------------------------------
//
TBool CLmkDecimalEditor::ConvertsIntoValidValue(TInt& aValue) const
	{
	if ((iFlags&ERepresentsYear) && (aValue>=0) && (aValue<100))
		{
		TTime homeTime;
		homeTime.HomeTime();
		TInt currentYear=homeTime.DateTime().Year();
		if (currentYear>0)
			{
			TInt yearsSinceStartOfCurrentCentury=currentYear%100;
			TInt newValue=(currentYear-yearsSinceStartOfCurrentCentury)+aValue;
            // If two digit year is given, 2000 + aValue is expected.
    		aValue=newValue;
			if ((newValue>=iMinimumValue) && (newValue<=iMaximumValue))
				{
				return ETrue;
				}
			}
		}
	return EFalse;
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::SetUninitialised
// ---------------------------------------------------------
//
void CLmkDecimalEditor::SetUninitialised(TBool aUninitialised)
	{
	if (aUninitialised)
		iFlags |= EIsUninitialised;
	else
		iFlags &= ~EIsUninitialised;
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::IsUninitialised
// ---------------------------------------------------------
//
TBool CLmkDecimalEditor::IsUninitialised() const
	{
	return iFlags & EIsUninitialised;
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::RefreshDigitType
// ---------------------------------------------------------
//
void CLmkDecimalEditor::RefreshDigitType( const CFont& aFont)
	{
	SetDigitType( AknTextUtils::NumericEditorDigitType(), aFont);
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::SetDigitType
// ---------------------------------------------------------
//
void CLmkDecimalEditor::SetDigitType(TDigitType aDigitType, const CFont& aFont)
	{
	TInt value = Value();
	iDigitType = aDigitType;
	SetTextToValue(value, aFont);
	}

// ---------------------------------------------------------
// CLmkDecimalEditor::DigitType
// ---------------------------------------------------------
//
TDigitType CLmkDecimalEditor::DigitType() const
	{
	return iDigitType;
	}

