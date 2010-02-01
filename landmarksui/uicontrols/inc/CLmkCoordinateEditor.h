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
* Description:    LandmarksUi Content File -    Coordinate editor for editor/viewer form
*
*/








#ifndef CLmkCoordinateEditor_h
#define CLmkCoordinateEditor_h

//  INCLUDES
#include "CLmkFloatEditor.h"

/// FORWARD DECLARATION
class CEikMfneSymbol;
class CLmkMfneSeparator;

// CLASS DECLARATION

/**
 *  CLmkCoordinateEditor extend ClmkFloatEditor's functionality
 *  Implements both latitude and longitude editors.
 *  Underlying floating value is just a value, which user inputs.
 *  Its upper limit depends on editor type: latitude or longitude.
 *  Lower limit is always 0, because coordinate sign is represented
 *  by cardinal point.
 *  Number() always returns coordinate in degrees. Cardinal points
 *  are represented by number's sign.
 */
class CLmkCoordinateEditor : public CLmkFloatEditor
    {
    public:
        enum TEditorType
            {
            ELatitudeEditor,
            ELongitudeEditor
            };

    public:  // Constructors and destructor
        /**
        * Constructor
        * @return newly instantiated object
        */
        CLmkCoordinateEditor();

        /**
        * Destructor.
        */
        virtual ~CLmkCoordinateEditor();

    	/**
    	* By default Symbian 2nd phase constructor
    	*/
    	void ConstructL(TReal aInitialValue,
                        TEditorType aType);

    public: // From CLmkFloatEditor

        /** Sets value */
	    void SetNumber(TReal aNumber);

        /** Gets value */
	    TReal Number() const;

    protected: // From CLmkFloatEditor

        void HandleValueChanged();
        void HandleCustomFieldChanged(TInt aCustomFieldIndex);

        void HandleEnter();
        void HandleExit();

    protected: // new functions

        // only NS or WE may be shown
        enum TCardinalPoint
        	{
        	ECardinalNone,
        	ECardinalNorth,
        	ECardinalSouth,
        	ECardinalEast,
        	ECardinalWest
        	};

        /** Abs(180) is allowed only for West */
        void CheckEastLongitude();

        void SetCardinal(TCardinalPoint aUnit);
        TCardinalPoint Cardinal() const;
        TCardinalPoint DefaultCardinal();

        void ConstructDegreeMarkFieldL();
        void ConstructCardinalPointFieldL();

        void ShowDegreeAndCardinal(TBool aVisible);

    private: //data

        /// Ref:
        CLmkMfneSeparator* iDegreeField;

        /// Ref:
        CEikMfneSymbol* iCardinalField;

        TEditorType iEditorType;

        HBufC* iDegreeMark;
    };

#endif      // CLmkCoordinateEditor_h

// End of File
