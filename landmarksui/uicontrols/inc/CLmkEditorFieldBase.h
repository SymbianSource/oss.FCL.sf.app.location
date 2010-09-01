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
* Description:    LandmarksUi Content File -    Base class for fields of form
*
*/







#ifndef __CLMKEDITORFIELDBASE_H__
#define __CLMKEDITORFIELDBASE_H__

//  INCLUDES
#include "MLmkEditorField.h"
#include "MLmkFieldData.h"
#include <e32std.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class MLmkEditorUiBuilder;
class CLmkDistanceEditor;
class CEikonEnv;


// CLASS DECLARATION

/**
* Landmark editor field base class.
*/
class CLmkEditorFieldBase :
        public CBase,
        public MLmkEditorField
    {
    protected:  // Construction
        /**
        * Constructor.
		* @param aField reference to the field to edit
		* @param aUiBuilder reference to UI builder
        * @return newly instantiated object
        */
        CLmkEditorFieldBase( MLmkFieldData& aField,
				             MLmkEditorUiBuilder& aUiBuilder );

        /**
        * Destructor.
        */
		~CLmkEditorFieldBase();

        /**
        * Base constructor.
        */
        void BaseConstructL();

    public:  // from MLmkEditorField
        TInt ControlId() const;
        TInt FieldId() const;
        TPtrC FieldLabel() const;
        void SetFieldLabelL(HBufC* aLabel);
        MLmkFieldData& LandmarkItemField() const;
		HBufC* ControlTextL() const;
        void ActivateL() = 0;
        //code added
        virtual TReal FieldValue() const;
		TReal GetNumberFieldValue() const;
		//-end-

		void LoadBitmapToFieldL	(MLmkEditorUiBuilder& aUiBuilder);
    protected:  // Data
        /// Own: Contact item field associated with this editor field
        MLmkFieldData& iLmkItemField;

		/// Ref: UI builder
		MLmkEditorUiBuilder& iUiBuilder;

		// Ref : Eikon Env
		CEikonEnv*     iEnv;
    };

#endif // __CLMKEDITORFIELDBASE_H__

// End of File
