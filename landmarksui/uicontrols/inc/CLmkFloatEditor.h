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
* Description:    LandmarksUi Content File -    Implements float editor's functionality.
*
*/








#ifndef CLmkFloatEditor_h
#define CLmkFloatEditor_h

//  INCLUDES
#include <EikMfne.h>
class CLmkMfneFloat;

// CLASS DECLARATION

/**
* CLmkFloatEditor class
* Contain two number editor and one separator.
*/
class CLmkFloatEditor : public CEikMfne
    {
    public:  // Constructors and destructor
        /**
         * Create CLmkFloatEditor object
         * @return newly instantiated object
         */
        CLmkFloatEditor();

        /**
         * Destructor.
         */
        virtual ~CLmkFloatEditor();

    protected:

		/**
		* By default Symbian 2nd phase constructor
        * @param aNumberOfFields field array's size
		*/
		void BaseConstructL(TInt aNumberOfFields = 1);

        /** Creates floating point value field
         *  and adds it to the array of Mfne fields
         *  @param aMaxNumSignificantDigits number of digits in float editor
         */
        void ConstructValueFieldL(
            TInt aMaxNumSignificantDigits,
            TInt aMaxDecimals);

    public:

        /** Sets value */
	    virtual void SetNumber(TReal aNumber);

        /** Gets value */
	    virtual TReal Number() const;

	    /** Whether value in this editor was modified by user */
	    TBool IsModified();

        /** Shortcut to know if current value is NaN */
        TBool IsNan();

    public: // From CCoeControl;

        void FocusChanged(TDrawNow aDrawNow);
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

    protected: // from CEikMfne

	protected: //new functions

        TBool SetLimits(TReal aMinimumValue, TReal aMaximumValue);
        void GetLimits(TReal& aMinimumValue, TReal& aMaximumValue);

        virtual void HandleValueChanged();
        virtual void HandleCustomFieldChanged(TInt aCustomFieldIndex);

        virtual void HandleEnter();
        virtual void HandleExit();

        TInt FindValueField();
        void HighlightField(TInt aFieldPosition);

    private: //data
        // Ref:
        CLmkMfneFloat*  iValueField;

        // Whether value in this editor was modified by user
        TBool           iIsModified;

        TBool           iIsMirroredLayout;
    };

#endif      // CLmkFloatEditor_h

// End of File
