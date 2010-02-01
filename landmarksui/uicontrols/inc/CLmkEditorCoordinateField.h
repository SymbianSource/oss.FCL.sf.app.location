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
* Description:    LandmarksUi Content File -    Landmark editor number field.
*
*/







#ifndef __CLmkEditorCoordinateField_H__
#define __CLmkEditorCoordinateField_H__

//  INCLUDES
#include "CLmkEditorFieldBase.h"

// FORWARD DECLARATIONS
class MLmkEditorUiBuilder;
class CEikCaptionedControl;
class CAknLocationEditor;
class TPosition;

// CLASS DECLARATION

/**
* Landmark editor number field.
*/
class CLmkEditorCoordinateField : public CLmkEditorFieldBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class - number editor to a field.
		 * @param aField the to be edited field
		 * @param aUiBuilder reference to UI builder
         */
		static CLmkEditorCoordinateField* NewL(
                MLmkFieldData& aField,
                MLmkEditorUiBuilder& aUiBuilder);

        /**
         * Destructor.
         */
        ~CLmkEditorCoordinateField();

    public:  // from MLmkEditorField
        TBool SaveFieldL();
        void ActivateL();
        void UpdateL();
	    //from CLmkEditorFieldBase
        TReal FieldValue() const;

        /**
         * Resets the field
         */
    	void ResetL();

    protected:  // Implementation
        CLmkEditorCoordinateField(
				MLmkFieldData& aField,
				MLmkEditorUiBuilder& aUiBuilder);

    private:    // implementation
        void ConstructL();
		void CreateAvkonCoordinateEditorL();
		void ConstructEditorFromResourceL();
		TPosition ConstructPosition(const TReal& aValue);

    protected:  // Data
        /// Ref: Fields editor
		CAknLocationEditor* iControl;
		TBool iIsLatitude;
        /// Ref: fields captioned control
        CEikCaptionedControl* iCaptionedCtrl;
        TReal iCurrentValue;
    };

#endif // __CLmkEditorCoordinateField_H__

// End of File
