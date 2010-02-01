/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -    This has the class which implements all the
*                landmarks editor/viewer fields
*
*/







#ifndef CLMKFIELDS_H
#define CLMKFIELDS_H

//  INCLUDES
#include "MLmkFieldData.h"
#include <eikenv.h>
#include <EPos_Landmarks.h>
#include <lbsposition.h>
#include <CLmkEditorDlg.h> // TLmkEditorAttributes

/// FORWARD DECLARATION
class CPosLandmarkDatabase;
class CPosLandmark;
class TLocality;

// Define Generic Landmark fields
enum TLmkGenericPoistionId
	{
	ELmkPositionFieldPhoneNumber = 	EPositionFieldBuildingTelephone,
	ELmkPositionFieldWebAddress	 =  EPositionFieldMediaLinksStart
	};

struct TLocationData
    {
    TReal64 iLatitude;
    TReal64 iLongitude;
    TReal32 iAltitude;
    TReal32 iHorizontalAccuracy;
    TReal32 iVerticalAccuracy;
    };

// CLASS DECLARATION

/**
*  Landmarks fields class, include all form's fields
*/
class CLmkFields : public CBase
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        * @param aLabelArray
        * @param aDb
        * @param aAttributes
        * @param aLandmarkId
        * @param aLandmark
        * @return newly instantiated object
        */

        IMPORT_C static CLmkFields* NewL( RPointerArray<HBufC>& aLabelArray,
                                          CPosLandmarkDatabase& aDb,
                                          TLmkEditorAttributes aAttributes,
                                          TPosLmItemId aLandmarkId,
                                          CPosLandmark* aLandmark,
                                          TBool aJapaneseInputMode );

        /**
        * Destructor.
        */
        IMPORT_C virtual ~CLmkFields();

    private:
        /**
        * C++ default constructor.
        * @param aLabelArray
        * @param aDb
        * @param aAttributes
        * @param aLandmarkId
        * @param aLandmark
        * @return newly instantiated object
        */
        CLmkFields( RPointerArray<HBufC>& aLabelArray,
                             CPosLandmarkDatabase& aDb,
                             TLmkEditorAttributes aAttributes,
                             TPosLmItemId aLandmarkId,
                             CPosLandmark* aLandmark,
                             TBool aJapaneseInputMode );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    public: //new functions
        /**
        * Return reference to fieldData array
        * @return MLmkFieldData array
        */
        IMPORT_C CArrayPtrFlat<MLmkFieldData>& Fields();

        /**
        * Save fields
        */
        IMPORT_C void SaveFieldsL();

        /**
        * Delete landmark
        */
        IMPORT_C void DeleteLandmarkL();

        /**
        * Get one specific field from array
        * Not give Ownership
        * @param aFieldType
        * @return Pointer to MLmkFieldData
        */
        IMPORT_C MLmkFieldData* GetField( TUint16 aFieldType );

        /**
        * Get landmark id. If called before landmark is stored
        * into database it will be KPosLmNullItemId.
        * @return landmark id
        */
        IMPORT_C TPosLmItemId LandmarkId() const;
        /**
        * Checks the disc space level whether is is enough to store a landmark or not
        * @return KErrDiscFull if disc is full or KErrNone otherwise
        */
        IMPORT_C TInt CheckDiscSpaceL();

        /**
        * Replace character 0x2029(suppose to be enter char) with space char
        * @param aText source string
        * @return modified string
        */
        IMPORT_C static TPtr  RemoveEnterCharacter( TPtrC aText );

    private: //new functions
        /**
        * Create all fields
        */
        void CreateLmkFieldsL();
        // Create fields for Japanese style
        void CreateLmkFieldsInJapaneseModeL();

        // Create fields for normal mode
        void CreateLmkFieldsInNormalModeL();

        /**
        * Create specific fields
        */


        void CreateNameFieldL( TInt aPos );
        void CreateCategoryFieldL();
        void CreateStreetFieldL( TInt aPos );
        void CreateCityFieldL( TInt aPos );
        void CreateStateProvinceFieldL( TInt aPos );
        void CreateCountryFieldL( TInt aPos );
        void CreatePostCodeFieldL( TInt aPos );
        void CreateLatitudeFieldsL( TInt aPos );
        void CreateLongitudeFieldsL( TInt aPos );
        void CreatePositionAccuracyFieldL( TInt aPos );
        void CreateAltitudeFieldL( TInt aPos );
        void CreateAltitudeAccuracyFieldL( TInt aPos );
        void CreateDescriptionFieldL( TInt aPos );
        void CreatePhoneNumberFieldL( TInt aPos );
        void CreateWebAddressFieldL( TInt aPos );
    private: //data
        ///
        TLocationData iLocationData;

    	/// Reference to database
        CPosLandmarkDatabase& iDb;

        // editors attributes
        TLmkEditorAttributes iAttributes;

        /// TPosLmItemId
		TPosLmItemId iLandmarkId;

		///Ref: Landmark
        CPosLandmark* iLandmark;

        /// TLocality variable
        TLocality iLocality;

    	/// Own: fields array
		CArrayPtrFlat<MLmkFieldData> iFieldsArray;

        ///Ref:
        RPointerArray<HBufC>* iLabelArray;

        /// Is new landmark created
        TBool iNewLandmark;
        // Japanese Input Mode
        TBool iJapaneseMode;
    };

#endif      // CLMKFIELDS_H

// End of File
