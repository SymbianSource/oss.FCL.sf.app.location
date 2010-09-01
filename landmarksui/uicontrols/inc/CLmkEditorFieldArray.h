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
* Description:    LandmarksUi Content File -    Landmark editor field array.
*
*/







#ifndef __CLMKEDITORFIELDARRAY_H__
#define __CLMKEDITORFIELDARRAY_H__

//  INCLUDES
#include "landmarks.hrh"
#include "CLmkLandMarkCategoriesName.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class MLmkEditorField;
class MLmkEditorUiBuilder;
class CLmkFields;
class CPosLandmark;
class MLmkFieldData;

// CLASS DECLARATION

/**
* Landmark editor field array.
*/
class CLmkEditorFieldArray :
        public CBase
    {
    public:  // Constructors and destructor
        /**
        * Creates an editor for aLmkItem.
        * @param aLmkItem  edited landmark item
        * @param aUiBuilder    reference to the UI creator
        * @return newly instantiated object
        */
		static CLmkEditorFieldArray* NewL
                (CLmkFields& aLmkFields,
                MLmkEditorUiBuilder& aUiBuilder);

        /**
        * Destructor.
        */
        ~CLmkEditorFieldArray();

    public:  // Interface
        /**
        * Update fields
        * @param aFieldType
        */
        void UpdateFieldsL(const TUint16 aFieldType=EAllEditorItems);

        /**
		* 2nd part constructor.
        * Create dialog lines from iLmkFields
        */
		void CreateFieldsFromLmkL();

        /**
        * Returns ETrue if all the fields of this lmk are empty
        * or contain only whitespace characters.
        * @return TBool
        */
        TBool AreAllFieldsEmptyL() const;

		/**
		* Returns the aFieldIndex field.
		* @precond	aFieldIndex >= 0 && aFieldIndex < EditorCount()
		*/
		const MLmkEditorField& FieldAt(TInt aFieldIndex);

        /**
        * Returns the lmk editor field corresponding to aControlId.
		* @return NULL if not found.
        */
        //MLmkEditorField* Find( TInt aControlId, TInt* aPos =NULL ) const;
        MLmkEditorField* Find( TInt aControlId, TInt* aPos=NULL , TInt* aPrevControlId=NULL ) const;

        /**
        * Saves all fields in the editor to the lmk item.
		* @return true if at least one field was changed
        */
        TBool SaveFieldsL();
		void SaveFieldsToBufferL();

        public: //new function
        /**
        * Its a getter method for returning the field array
		* @return iFieldArray member by reference
        */
        CArrayPtrFlat<MLmkEditorField>& GetFieldArray();
	public :// Added for category name support
		void SetCategoryName(const TDesC& aCategoryName);
		TBool CheckIfFieldIsEmpty(MLmkFieldData& a);

    private:  // Implementation
        /**
        * Create CLmkEditorFieldArray object
        * @param aLmkFields
        * @param aUiBuilder
        * @return newly instantiated object
        */
        CLmkEditorFieldArray(
				CLmkFields& aLmkFields,
				MLmkEditorUiBuilder& aUiBuilder);

        void ConstructL();

    private:  // Data
        /// Ref:
        CLmkFields& iLmkFields;

        /// Ref: editor UI creator
        MLmkEditorUiBuilder& iUiBuilder;

        /// Own: Contact editor fields
        CArrayPtrFlat<MLmkEditorField> iFieldArray;
        TCategoryName iCategoryName;
        TBool iCategoryFlag;
    };

#endif // __CLMKEDITORFIELDARRAY_H__

// End of File
