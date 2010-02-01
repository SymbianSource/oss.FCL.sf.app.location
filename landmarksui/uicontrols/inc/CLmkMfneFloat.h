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
* Description:    LandmarksUi Content File -    Floating point field for CEikMFne
*
*/






#ifndef CLmkMfneFloat_H
#define CLmkMfneFloat_H

//  INCLUDES
#include <eikmfne.h>

// CLASS DECLARATION

/**
* CLmkMfneFloat class
* Contain two number editor and one separator.
*/
class CLmkMfneFloat : public CEikMfneField
    {
    public:
        virtual ~CLmkMfneFloat();
        static CLmkMfneFloat* NewL(
            TInt aMaxDigits,
            TInt aMaxDecimalDigits);

        TBool SetLimits(
            TReal aMinimumValue,
            TReal aMaximumValue);

        void GetLimits(
            TReal& aMinimumValue,
            TReal& aMaximumValue) const;

        void SetValue(TReal aValue);
        TReal Value() const;

        void HandleHighlight();

    private:
        CLmkMfneFloat(
            TInt aMaxDigits,
            TInt aMaxDecimalDigits);

        void ConstructL();

    // From CEikMfneField
    public:
        virtual TInt MaximumWidthInPixels(
            const CFont& aFont,
            TBool aShrinkToMinimumSize);

        virtual TCoeInputCapabilities InputCapabilities() const;

        virtual TBool IsEditable() const;

        // these functions may only be called if
        // IsEditable returns ETrue - all TBool&/TInt& parameters
        // in these functions must be first set to EFalse/0 by the caller
        virtual THighlightType HighlightType() const;

        virtual void HandleKey(
            const CFont& aFont,
            const TKeyEvent& aKeyEvent,
            TBool aInterpretLeftAndRightAsEarEvents,
            TBool& aDataAltered,
            TInt& aHighlightIncrement);

        virtual void HandleDeHighlight(
            const CFont& aFont,
            CEikonEnv& aEikonEnv,
            TBool& aDataAltered,
            TBool& aError);

    private:
        virtual const TDesC& Text() const;

    // New internal methods
    private:
        TBool   EnsureValueInLimits(TReal aValue);
        TInt    MaxNumOfChars() const;

        TBool   IsDecimalSeparatorAllowed(const TDesC& aText) const;
        TBool   IsMoreDigitsAllowed(const TDesC& aText) const;

        TReal   ValueFromText(const TDesC& aText) const;
        void    TextFromValue(TReal aValue, TDes& aText) const;

    private:
        TReal   iMinimumValue;
        TReal   iMaximumValue;
        TInt    iMaxDigits;
        TInt    iMaxDecimalDigits;

        HBufC*  iText;

        TBool   iIsBeingEditedWithCursor;
        TBool   iFocus;
    };

#endif

// end of File