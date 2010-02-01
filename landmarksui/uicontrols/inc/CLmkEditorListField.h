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
* Description:    LandmarksUi Content File -    Landmark's editor text field.
*
*/







#ifndef __CLMKEDITORLISTFIELD_H__
#define __CLMKEDITORLISTFIELD_H__

//  INCLUDES
#include "CLmkEditorFieldBase.h"

// FORWARD DECLARATIONS
class CEikEdwin;
class MLmkEditorUiBuilder;
class CEikCaptionedControl;


// CLASS DECLARATION

/**
* Landmark editor text field.
*/
class CLmkEditorListField : public CLmkEditorFieldBase
    {
    public:  // Constructors and destructor
        /**
        * Creates a new instance of this class - text editor to a field.
		* @param aField the to be edited field
		* @param aUiBuilder reference to UI builder
        * @return newly instantiated object
        */
		static CLmkEditorListField* NewL(
                MLmkFieldData& aField,
                MLmkEditorUiBuilder& aUiBuilder );

		static CLmkEditorListField* NewL( MLmkFieldData& aField,
				 MLmkEditorUiBuilder& aUiBuilder,
				 const TDesC& aCategoryName  );
        /**
        * Destructor.
        */
        ~CLmkEditorListField();

    public:  // from MLmkEditorField
        TBool SaveFieldL();
        void ActivateL();
        void UpdateL();
        CEikEdwin* Control();
        /**
         * Resets the field
         */
    	void ResetL(){}

    protected:  // Implementation
        /**
        * Create CLmkEditorListField object
        * @param aField field include all item data
        * @param aUiBuilder reference
        * @return newly instantiated object
        */
        CLmkEditorListField(
				MLmkFieldData& aField,
				MLmkEditorUiBuilder& aUiBuilder );

    private:    // implementation
        void ConstructL();
        void ConstructL(const TDesC& aCategoryName);

    protected:  // Data
        /// Ref: Fields editor
        CEikEdwin* iControl;

        /// Ref: fields captioned control
        CEikCaptionedControl* iCaptionedCtrl;
    };

#endif // __CLMKEDITORLISTFIELD_H__

// End of File
