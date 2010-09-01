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
* Description:    LandmarksUi Content File -    Include data for form items
*
*/








#ifndef CLMKFIELDDATA_H
#define CLMKFIELDDATA_H

//  INCLUDES
#include "MLmkFieldData.h"
#include <e32base.h>
#include <EPos_Landmarks.h>

// CLASS DECLARATION

/**
*  Landmarks field's data
*/
class CLmkFieldData : public CBase, public MLmkFieldData
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aLabel
        * @return newly instantiated object
        */
        static CLmkFieldData* NewL( HBufC* aLabel );

        /**
        * Destructor.
        */
        ~CLmkFieldData();

    private:
        /**
        * C++ default constructor.
        * @return newly instantiated object
        */
        CLmkFieldData();

        /**
        * By default Symbian 2nd phase constructor is private.
        * @param aLabel
        */
        void ConstructL( HBufC* aLabel );

    private: //From MLmkFieldData
        void SetLabel( HBufC* aLabel );
        const HBufC& Label() const;

        void SetEditorType( const TUint16 aType );
        TUint16 EditorType() const;

        void SetTextL( const TPtrC aText );
        void SetText( HBufC* aText ); //take ownership
        const HBufC& TextData() const;

        void SetInt( const TInt aTInt );
        TInt IntegerData() const;

        void SetTReal( const TReal32 aTReal );
        TReal32 FloatData() const;

        void SetTReal( const TReal64 aTReal );
        TReal64 DoubleData() const;

        RArray<TPosLmItemId>& Categories() ;
        TInt UniqueFieldIdentity() const;

        void SetFieldType( const TUint16 aFieldType );
        TUint16 FieldType() const;

        void SetPositionFieldId( const TUint16 aPositionFieldId );
        TUint16 PositionFieldId() const;

        void SetFieldLength( const TInt aLength );
        TInt FieldLength() const;

        void SetTitleField( const TBool aTitleField );
        TBool IsTitleField() const;

        void SetIconId( const TInt aIconId );
        TInt IconId() const;

        void SetIconPathL( const TDesC& aIconPath );
        HBufC* IconPath();

    private: //data

        TUint16 iFieldType;

        TUint16 iEditorType;

        TUint16 iPositionFieldId;

        TInt iUniqueFieldIdentity;

        TInt iFieldLength;

        ///Ref:
    	HBufC* iLabel;

        ///Own:
    	HBufC* iTextData;

    	RArray<TPosLmItemId> iCategories;

    	TInt iIntData;

    	TReal32 iTRealData32;

        TReal64 iTRealData64;

        TBool iTitleField;

        TInt iIconId;

        ///Own:
        HBufC* iIconPath;

    };

#endif      // CLMKFIELDDATA_H

// End of File
