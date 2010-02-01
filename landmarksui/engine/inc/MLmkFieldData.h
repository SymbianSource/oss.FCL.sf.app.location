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
* Description:    LandmarksUi Content File -    Interface for landmark's editor items
*
*/







#ifndef __MLMKFIELDDATA_H__
#define __MLMKFIELDDATA_H__

//  INCLUDES
#include <e32base.h>
#include <EPos_Landmarks.h>

// CLASS DECLARATION

/**
* Landmark editor field data abstract class.
*/
class MLmkFieldData
    {
    public:  // destructor
        /**
        * Destructor.
        */
        virtual ~MLmkFieldData(){}

    public:  // Interface
        /**
        * Set label
        * Not take ownership
        * @param aLabel
        */
        virtual void SetLabel( HBufC* aLabel ) = 0;

        /**
        * Return field's label
        * @return HBufC
        */
        virtual const HBufC& Label() const = 0;

        /**
        * Set editor type
        * @param aType
        */
        virtual void SetEditorType( const TUint16 aType ) = 0;

        /**
        * Return field's editor type
        * @return TInt
        */
        virtual TUint16 EditorType() const = 0;

        /**
        * Set text data
        * @param aText
        */
        virtual void SetTextL( const TPtrC aText ) = 0;

        /**
        * Set text data, take ownership
        * @param aText
        */
        virtual void SetText( HBufC* aText ) = 0;

        /**
        * Return text data
        * @return TBufC
        */
        virtual const HBufC& TextData() const = 0;

        /**
        * Set Integer value
        * @param aTInt
        */
        virtual void SetInt( const TInt aTInt ) = 0;

        /**
        * Return field's integer value
        * @return TInt
        */
        virtual TInt IntegerData() const = 0;

        /**
        * Set TReal32 value
        * @param aTReal
        */
        virtual void SetTReal( const TReal32 aTReal ) = 0;

        /**
        * Return field's TReal32 value
        * @return TReal
        */
        virtual TReal32 FloatData() const = 0;

        /**
        * Set TReal64 value
        * @param aTReal64
        */
        virtual void SetTReal( const TReal64 aTReal ) = 0;

        /**
        * Return field's TReal64 value
        * @return TReal64
        */
        virtual TReal64 DoubleData() const = 0;

        /**
        * Return field's categories
        * @return TPosLmItemId
        */
        virtual RArray<TPosLmItemId>& Categories() = 0;

        /**
        * Return TInt
        * @return TInt
        */
        virtual TInt UniqueFieldIdentity() const = 0;

        /**
        * Set Field type
        * @param aFieldType
        */
        virtual void SetFieldType( const TUint16 aFieldType ) = 0;

        /**
        * Return field type
        * @return TUint16
        */
        virtual TUint16 FieldType() const = 0;

        /**
        * Set PositionField id
        * @param aPositionFieldId
        */
        virtual void SetPositionFieldId( const TUint16 aPositionFieldId ) = 0;

        /**
        * Return PositionField id
        * @return TUint16
        */
        virtual TUint16 PositionFieldId() const = 0;

        /**
        * Set Field length
        * @param aLength
        */
        virtual void SetFieldLength( const TInt aLength ) = 0;

        /**
        * Return field length
        * @return TInt
        */
        virtual TInt FieldLength() const = 0;

        /**
        * Set fields title
        * @param aTitleField
        */
        virtual void SetTitleField( const TBool aTitleField ) = 0;

        /**
        * Check if field is title field
        * @return TBool
        */
        virtual TBool IsTitleField() const = 0;

        /**
        * Set icon id to landmark
        * @param aIconId
        */
        virtual void SetIconId( const TInt aIconId ) = 0;

        /**
        * Get landmarks icon id
        * @return TInt
        */
        virtual TInt IconId() const = 0;

        /**
        * Set landmark icon path
        * @param aIconPath
        */
        virtual void SetIconPathL( const TDesC& aIconPath ) = 0;

        /**
        * Get landmarks icon path
        * @return HBufC*
        */
        virtual HBufC* IconPath() = 0;
    };

#endif // __MLMKFIELDDATA_H__

// End of File
