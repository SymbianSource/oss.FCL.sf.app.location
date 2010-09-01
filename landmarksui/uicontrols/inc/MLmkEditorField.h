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
* Description:    LandmarksUi Content File -    Landmark editor field abstract class.
*
*/







#ifndef __MLMKEDITORFIELD_H__
#define __MLMKEDITORFIELD_H__

//  INCLUDES
#include <e32def.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class MLmkFieldData;

// CLASS DECLARATION

/**
 * Landmark editor field abstract class.
 */
class MLmkEditorField
    {
    public:  // destructor
        /**
         * Destructor.
         */
        virtual ~MLmkEditorField(){}

    public:  // Interface
        /**
         * Return the dialog control id associated to this field.
         */
        virtual TInt ControlId() const = 0;

        /**
         * Returns the id of this field.
         */
        virtual TInt FieldId() const = 0;

        /**
         * Save editor control data to contact item field.
		 * @return ETrue if success, EFalse otherwise
         */
        virtual TBool SaveFieldL() = 0;

        /**
         * Returns the landmark item fields label.
         */
        virtual TPtrC FieldLabel() const = 0;

        /**
         * Sets aLabel as the landmark item fields label
         * Takes ownership of the aLabel.
         */
        virtual void SetFieldLabelL(HBufC* aLabel) = 0;

        /**
         * Returns the landmark item field.
         */
        virtual MLmkFieldData& LandmarkItemField() const = 0;

		/**
		 * Returns the text in the fields control.
		 */
		virtual HBufC* ControlTextL() const = 0;
		// code added -start-
		virtual TReal FieldValue() const = 0;
		//-end-
        /**
         * Activates the control.
         */
        virtual void ActivateL() = 0;

        /**
         * Update field data. Default implementation is empty
         */
        virtual void UpdateL(){}
        /**
         * Resets the field
         */
        virtual void ResetL()=0;
        /**
        * if the control is added when form is created, its state is set
        * by the form, but, when the control is added dynamically
        * its state must be set at the time of creation,hence this
        * function is used to set the state.
        */
        virtual void SetEditableL( TBool /*aEditable*/ ){}
    };

#endif // __MLMKEDITORFIELD_H__

// End of File
