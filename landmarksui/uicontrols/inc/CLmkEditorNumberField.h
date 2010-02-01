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
* Description:    LandmarksUi Content File -    Landmark editor distance field.
*
*/







#ifndef __CLMKEDITORNUMBERFIELD_H__
#define __CLMKEDITORNUMBERFIELD_H__

//  INCLUDES
#include "CLmkEditorFieldBase.h"

// FORWARD DECLARATIONS
class MLmkEditorUiBuilder;
class CEikCaptionedControl;
class CLmkDistanceEditor;
class CAknUnitEditor;
class CLmkCentralRepository;


// CLASS DECLARATION

/**
 * Landmark editor distance field.
 */
class CLmkEditorNumberField : public CLmkEditorFieldBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class - number editor to a field.
		 * @param aField the to be edited field
		 * @param aUiBuilder reference to UI builder
		 * //@param aIconInfoContainer reference to icon info container
         */
		static CLmkEditorNumberField* NewL(
                MLmkFieldData& aField,
                MLmkEditorUiBuilder& aUiBuilder);

        /**
         * Destructor.
         */
        ~CLmkEditorNumberField();

    public:  // from MLmkEditorField
    	/**
    	* Saves current value to the database
    	*/
        TBool SaveFieldL();
        void ActivateL();
        /**
        * Updates the new value to the database.
        */
        void UpdateL();
        /**
        * This function handles the unit change, by applying conversion to the current
        * value based on the current unit.
        */
        void HandleUnitChangeL();
       	/**
         * Resets the field
         */
    	void ResetL();

    protected:  // Implementation
        CLmkEditorNumberField(
				MLmkFieldData& aField,
				MLmkEditorUiBuilder& aUiBuilder);

    private:    // implementation
        void ConstructL();
        void CreateAvkonUnitEditorL();
        //from CLmkEditorFieldBase
        TReal FieldValue() const;
        void ResolveUnitL();
        TBool IsSystemUnitMetric();
        void ConvertFromFootToMeters(TReal32& aValue);
        void ConvertFromMetersToFoot(TReal32& aValue);

    protected:  // Data
        /// Ref: Fields editor
       CAknUnitEditor* iControl;
       /// Ref: fields captioned control
       CEikCaptionedControl* iCaptionedCtrl;
       //owns
       CLmkCentralRepository* iRepository;

       //represents the current value in db
       TReal32 iDbData;
       //represents the converted value either metric or imperial.
       TReal32 iConvertedData;
    };

#endif // __CLMKEDITORNUMBERFIELD_H__

// End of File
