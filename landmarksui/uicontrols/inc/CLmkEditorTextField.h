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
* Description:    LandmarksUi Content File -    Landmark editor text field.
*
*/







#ifndef __CLMKEDITORTEXTFIELD_H__
#define __CLMKEDITORTEXTFIELD_H__

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
class CLmkEditorTextField : public CLmkEditorFieldBase
    {
    public:  // Constructors and destructor
        /**
         * Creates a new instance of this class - text editor to a field.
		 * @param aField the to be edited field
		 * @param aUiBuilder reference to UI builder
		 * //@param aIconInfoContainer reference to icon info container
         */
		static CLmkEditorTextField* NewL(
                MLmkFieldData& aField,
                MLmkEditorUiBuilder& aUiBuilder );

        /**
         * Destructor.
         */
        ~CLmkEditorTextField();

    public:  // from MLmkEditorField
        TBool SaveFieldL();
        void ActivateL();
        /**
         * Returns the iControl member
         */
        CEikEdwin* Control();

        /**
         * Returns the iCaptionedCtrl member
         */
        CEikCaptionedControl* CaptionedControl();
    	/**
         * Resets the field
         */
    	void ResetL(){}
    	 /**
    	 * Checks, if any prefix-present before the url protocol.
    	 */
    	TInt IsWebUrlProtocolPreFixedL( TPtr& aWebUrl );
    	/**
    	 * Removes the prefixes from the display text
    	 */
		void TrimPrefixesFromWebUrlL( TPtr& aWebUrl,TInt aTrimPos );
		/**
    	 * Sets the display text to the control
    	 */
		void SetControlTextL( TPtrC& aText);
		/*
		* Sets the edit state of the control
		*/
		void SetEditableL( TBool aEditable );

    protected:  // Implementation
        CLmkEditorTextField(
				MLmkFieldData& aField,
				MLmkEditorUiBuilder& aUiBuilder );

    private:    // implementation
        void ConstructL();
    protected:  // Data
        /// Ref: Fields editor
        CEikEdwin* iControl;
        /// Ref: fields captioned control
        CEikCaptionedControl* iCaptionedCtrl;

        // e.g image/jpeg/http://www.google.com
        TBool isUrlPrefixed;
        //own
        HBufC* iUrlPrefix;
        //own
        HBufC* iText;
    };

#endif // __CLMKEDITORTEXTFIELD_H__

// End of File
