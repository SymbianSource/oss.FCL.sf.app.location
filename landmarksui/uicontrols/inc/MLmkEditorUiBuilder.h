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
* Description:    LandmarksUi Content File -    Landmark editor field abstract callback class.
*
*/







#ifndef __MLMKEDITORUIBUILDER_H__
#define __MLMKEDITORUIBUILDER_H__

//  INCLUDES
#include <e32def.h>

// FORWARD DECLARATIONS
class CCoeControl;
class CEikCaptionedControl;
class CPosLandmarkDatabase;

// CLASS DECLARATION

/**
* Landmark editor field abstract class.
*/
class MLmkEditorUiBuilder
    {
    public:  // destructor
        /**
        * Destructor.
        */
        virtual ~MLmkEditorUiBuilder(){}

    public:  // Interface
		/**
		* Creates a User interface control for a line.
		* @param aCaption	The caption descriptor to the control.
		* @param aControlId	The created controls ID.
		* @param aControlType	The created controls type.
		*/
        virtual CCoeControl* CreateLineL(const TDesC& aCaption,
                TInt aControlId, TInt aControlType) = 0;

        /**
        * Returns the control line corresponding to aControlId.
        * @param aControlId
        * @return CEikCaptionedControl pointer
        */
        virtual CEikCaptionedControl* LineControl(TInt aControlId) const = 0;

        /**
        * Removed the aControlId control from the UI.
        * @param aControlId
        */
        virtual void DeleteControl(TInt aControlId) = 0;

		/**
		* Try to change the focus to aControlId.
		* @param aControlId
		*/
		virtual void TryChangeFocusL(TInt aControlId) = 0;

		/**
		* Change the line controls caption aText in the UI.
        * @param aControlId
        * @param aText
		*/
		virtual void SetCurrentLineCaptionL( TInt aControlId,
                                             const TDesC& aText) = 0;

		/**
		* Return the control object corresponding to aControlId.
        * @param aControlId
		* @return NULL if the control does not exist.
		*/
		virtual CCoeControl* Control(TInt aControlId) const = 0;

		/**
		* Sets the field editable state.
		* @param aState ETrue to set editable, EFalse otherwise
		*/
		virtual void SetEditableL(TBool aState) = 0;

        /**
        * Return landmark's database
        * @return landmark's database
        */
        virtual CPosLandmarkDatabase& Database() = 0;
        /**
        * Return editor's edit state
        */
        virtual TInt EditMode() = 0;
    };

#endif // __MLMKEDITORUIBUILDER_H__

// End of File
