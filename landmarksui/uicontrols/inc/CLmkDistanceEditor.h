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
* Description:    LandmarksUi Content File -    Distance editor for editor/viewer form
*
*/








#ifndef CLmkDistanceEditor_h
#define CLmkDistanceEditor_h

//  INCLUDES
#include "CLmkFloatEditor.h"
#include "landmarks.hrh"

/// FORWARD DECLARATION
class CEikMfneSymbol;

// CLASS DECLARATION

/**
 *  CLmkDistanceEditor extends CLmkFloatEditor's functionality.
 *  Implements both metric and imperial units.
 *  Underlying floating value is just a value, which user inputs.
 *  Its limits depend on current unit.
 *  Number() method returns distance always in meters.
 */
class CLmkDistanceEditor : public CLmkFloatEditor
    {
    public:
        // Constructors and destructor
        /**
         * Create CLmkDistanceEditor object
         * @return newly instantiated object
         */
        CLmkDistanceEditor();

        /**
         * Destructor.
         */
        virtual ~CLmkDistanceEditor();

    	/**
    	 * By default Symbian 2nd phase constructor
         * @param aMinimumValue
         * @param aMaximumValue
         * @param aInitialValue
         * @param aType
    	 */
    	void ConstructL(TReal32 aMinimumValue,
                        TReal32 aMaximumValue,
                        TReal32 aInitialValue);

    public: // From CLmkFloatEditor

        /** Sets value */
	    void SetNumber(TReal aNumber);

        /** Gets value */
	    TReal Number() const;

    protected: // From CLmkFloatEditor

        void HandleCustomFieldChanged(TInt aCustomFieldIndex);

        void HandleEnter();
        void HandleExit();

    protected: // new functions

        enum TDistanceUnit
        	{
        	EUnitNone,
        	EUnitMeters,
        	EUnitKilometers,
        	EUnitFeet,
        	EUnitMiles
        	};

        /** Converts from meters to given units */
        TReal32 MetersToUnits(
            TReal32 aMeters,
            TDistanceUnit aToUnits) const;

        /** Converts from given units to meters */
        TReal32 UnitsToMeters(
            TReal32 aValue,
            TDistanceUnit aFromUnits) const;


        /** Updates min and max limits in editor field according to
         *  current units
         */
        void UpdateValueLimits();

        void SetUnit(TDistanceUnit aUnit);
        TDistanceUnit Unit() const;
        TDistanceUnit DefaultUnit() const;

        void ConstructUnitFieldL();

        void ShowUnits(TBool aVisible);

    private: //data
        TReal   iMinimumValue;
        TReal   iMaximumValue;

        TDistanceUnit iLastUsedUnit;

        // Ref:
        CEikMfneSymbol* iUnitsField;
    };

#endif      // CLmkDistanceEditor_h

// End of File
