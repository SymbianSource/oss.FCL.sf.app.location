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
* Description:    LandmarksUi Content File -    Float editor for editor/viewer form
*
*/






#ifndef CLMKDECIMALEDITOR_H
#define CLMKDECIMALEDITOR_H

//  INCLUDES
#include <eikmfne.h>

// CLASS DECLARATION

/**
* CLmkDecimalEditor class
* Contain two number editor and one separator.
*/
class CLmkDecimalEditor : public CEikMfneField
	{
public:
	enum
		{
		EFillWithLeadingZeros			=0x1,
		EPreserveOldWidthBeforeEditing	=0x2,
		ERepresentsYear					=0x4,
		EPublicallySettableFlags		=EFillWithLeadingZeros|EPreserveOldWidthBeforeEditing|ERepresentsYear
		};
private:
	enum
		{
		EIsBeingEditedWithCursor		=0x8,
		EIsUninitialised				=0x10
		};
private:
	CLmkDecimalEditor(TInt aMinimumValue, TInt aMaximumValue, TUint32 aFlags);
	void ConstructL(TInt aNumberOfZeros);
public:
	virtual ~CLmkDecimalEditor();
	static CLmkDecimalEditor* NewL(const CFont& aFont, TResourceReader& aResourceReader);
	static CLmkDecimalEditor* NewL(const CFont& aFont, TInt aMinimumValue, TInt aMaximumValue, TInt aInitialValue, TUint32 aFlags, TInt aNumberOfZeros);
	void SetMinimumAndMaximum(TInt aMinimumValue, TInt aMaximumValue, const CFont& aFont); // only values inside the initial minimum and maximum are permitted
	void GetMinimumAndMaximum(TInt& aMinimumValue, TInt& aMaximumValue) const;
	void SetValue(TInt aValue, const CFont& aFont);
	TReal Value() const;
	virtual TBool IsValid() const;

	void SetUninitialised(TBool aUninitialised);
	TBool IsUninitialised() const;

public:	// International digit support is only public to to eikstd
	void SetDigitType(TDigitType aDigitType, const CFont& aFont);
	TDigitType DigitType() const;

	// Derive digit display type from locale information
	void RefreshDigitType(const CFont& aFont);

public:
	TBool IsTextNull() const;
private: // framework
	virtual TInt MaximumWidthInPixels(const CFont& aFont, TBool aShrinkToMinimumSize);
	virtual TCoeInputCapabilities InputCapabilities() const;
	virtual TBool IsEditable() const;
	virtual THighlightType HighlightType() const;
	virtual void HandleKey(const CFont& aFont, const TKeyEvent& aKeyEvent, TBool aInterpretLeftAndRightAsEarEvents, TBool& aDataAltered, TInt& aHighlightIncrement);
	virtual void HandleDeHighlight(const CFont& aFont, CEikonEnv& aEikonEnv, TBool& aDataAltered, TBool& aError);
	virtual const TDesC& Text() const;
	void InvalidFieldAlert() const;
	TInt AdditionalWidthForHighlights(const CFont& aFont) const;
	TInt WidthInPixels(const CFont& aFont) const;

private:
	TInt MaximumNumberOfDigits() const;
	TInt NumberOfDigits() const;
	void SetTextToValue(TInt aValue, const CFont& aFont);
	TInt ValueFromText() const;
	TBool ConvertsIntoValidValue(TInt& aValue) const;

private: // International digit support
	TChar NormalizeDigit(TChar aChar);
	TText ZeroCharacter() const;
	TText NegativeCharacter() const;

private:
	TInt iMinimumValue;
	TInt iMaximumValue;
	TInt iMaxDigits;
	TUint32 iFlags;
	HBufC* iText;
	TChar iNudgeCharMinus;
	TChar iNudgeCharPlus;
	TInt iMaxDigitsMinimumValue;
    TInt iMaxDigitsMaximumValue;
	TDigitType	iDigitType;
	TInt iNumberOfZeros;

protected:
    // Ref
    CEikonEnv*	iEnv;
	};

#endif