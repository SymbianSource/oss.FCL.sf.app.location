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
* Description:    LandmarksUi Content File -    This file has implementation methods for Landmarks
*                individual fields.
*
*/







// INCLUDE FILES
#include <sysutil.h>
#include <e32math.h>
#include <EPos_CPosLandmarkDatabase.h>
#include <AknUtils.h>
#include "landmarks.hrh"
#include "LmkConsts.h"
#include "CLmkDbUtils.h"
#include "CLmkFieldData.h"
#include "CLmkFields.h"

// CONSTANTS
const TInt KMaxFieldLength50( 50 );
const TInt KMaxFieldLength20( 20 );
const TInt KMaxFieldLength10( 10 );
//const TInt KParaChar( 0x2029 );


// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// CLmkFields::CLmkFields
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//

CLmkFields::CLmkFields(
    RPointerArray<HBufC>& aLabelArray,
    CPosLandmarkDatabase& aDb,
    TLmkEditorAttributes aAttributes,
    TPosLmItemId aLandmarkId,
    CPosLandmark* aLandmark,
    TBool aJapaneseInputMode)
    : iDb( aDb ),
      iAttributes( aAttributes ),
      iLandmarkId( aLandmarkId ),
      iLandmark( aLandmark ),
      iFieldsArray(5),
      iLabelArray( &aLabelArray ),
      iJapaneseMode( aJapaneseInputMode )
    {
    }

// ----------------------------------------------------------------------------
// CLmkFields::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CLmkFields::ConstructL()
    {
    iNewLandmark = EFalse;
    if ( !iLandmark )
        {
        if ( iLandmarkId != KPosLmNullItemId )
            {
            iLandmark = iDb.ReadLandmarkLC(iLandmarkId);
            iNewLandmark = ETrue;
            CleanupStack::Pop(); //landmark
            }
        else
            {
            //create new empty landmark
            iLandmark = CPosLandmark::NewL();
            iNewLandmark = ETrue;
            }
        }

    TRealX nan;
    nan.SetNaN();
        
	iLocationData.iLatitude				=nan;
	iLocationData.iLongitude			=nan;
	iLocationData.iAltitude				=nan;
	iLocationData.iVerticalAccuracy		=nan;               
	iLocationData.iHorizontalAccuracy	=nan;
	
    CreateLmkFieldsL();
    }

// ----------------------------------------------------------------------------
// CLmkFields::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CLmkFields* CLmkFields::NewL( RPointerArray<HBufC>& aLabelArray,
                                       CPosLandmarkDatabase& aDb,
                                       TLmkEditorAttributes aAttributes,
                                       TPosLmItemId aLandmarkId,
                                       CPosLandmark* aLandmark,
                                       TBool aJapaneseInputMode )

    {

    CLmkFields* self = new ( ELeave ) CLmkFields( aLabelArray,
                                                  aDb,
                                                  aAttributes,
                                                  aLandmarkId,
                                                  aLandmark,
                                                  aJapaneseInputMode );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// ----------------------------------------------------------------------------
// CLmkFields::~CLmkFields
// ----------------------------------------------------------------------------
//
EXPORT_C CLmkFields::~CLmkFields()
    {
    if ( iNewLandmark )
        {
        delete iLandmark;
        }
    iFieldsArray.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// CLmkFields::Fields
// ----------------------------------------------------------------------------
//
EXPORT_C CArrayPtrFlat<MLmkFieldData>& CLmkFields::Fields()
    {
    return iFieldsArray;
    }

// ----------------------------------------------------------------------------
// CLmkFields::SaveFieldsL
// ----------------------------------------------------------------------------
//
EXPORT_C void CLmkFields::SaveFieldsL()
	{

    TInt count(iFieldsArray.Count());
    TBool isPosFieldPresent = EFalse;
    
    for ( TInt i = 0; i < count; i++ )
        {
        switch( iFieldsArray[i]->FieldType() )
            {
            case EName:
                {
                iLandmark->SetLandmarkNameL( iFieldsArray[i]->TextData() );
                HBufC* path = iFieldsArray[i]->IconPath();
                if ( path && path->Length() > 0 &&
                    iFieldsArray[i]->IconId() != KErrNotFound )
                    {
                    // Get the default Mask index
                    iLandmark->SetIconL( *path, iFieldsArray[i]->IconId() , KLmkDefaultId+1);
                    }

                break;
                }
            case ECategory:
            case ECategories:
                {
                //Add categories to landmark
                RArray<TPosLmItemId>& categories =
                    iFieldsArray[i]->Categories();
                TInt Tcount( categories.Count() );
                for ( TInt j=0; j < Tcount; j++ )
                    {
                    iLandmark->AddCategoryL( categories[j] );
                    }
                break;
                }
			case EDescription:
				{
				iLandmark->SetLandmarkDescriptionL( iFieldsArray[i]->TextData() );
				break;
				}
            //these all are same
            case EStreet:
            case EPostCode:
            case ECity:
            case EStateProvince:
            case ECountry:
			case EPhoneNumber:
			case EWebAddress:
                {
                iLandmark->SetPositionFieldL(
                        iFieldsArray[i]->PositionFieldId(),
                        iFieldsArray[i]->TextData() );
                break;
                }
            case ELatitude:
                {
                isPosFieldPresent = ETrue;
                iLocationData.iLatitude = iFieldsArray[i]->DoubleData();
                break;
                }
            case ELongitude:
                {
                isPosFieldPresent = ETrue;
                iLocationData.iLongitude = iFieldsArray[i]->DoubleData();
                break;
                }
            case EPositionAccuracy:
                {
                iLocationData.iHorizontalAccuracy =
                    iFieldsArray[i]->FloatData();
                break;
                }
            case EAltitude:
                {
                iLocationData.iAltitude = iFieldsArray[i]->FloatData();
                break;
                }
            case EAltitudeAccuracy:
                {
                iLocationData.iVerticalAccuracy = iFieldsArray[i]->FloatData();
                break;
                }
            default:
                {
                break;
                }
            }
        }

    //Position data
    iLocality = TLocality(); // empty locality
    
    if( isPosFieldPresent )
        {
        // remove the old values to set new values       
        iLandmark->RemoveLandmarkAttributes(CPosLandmark::EPosition);

        if ( !Math::IsNaN(iLocationData.iLatitude) && !Math::IsNaN(iLocationData.iLongitude) )         
            {
            if (!Math::IsNaN(iLocationData.iAltitude))
                {
                iLocality.SetCoordinate(
                    iLocationData.iLatitude,
                    iLocationData.iLongitude,
                    iLocationData.iAltitude);

                if (!Math::IsNaN(iLocationData.iVerticalAccuracy))
                    {
                    if( iLocationData.iVerticalAccuracy > 0 )
                        iLocality.SetVerticalAccuracy(iLocationData.iVerticalAccuracy);
                    }
                }
            else
                {
                iLocality.SetCoordinate(
                    iLocationData.iLatitude,
                    iLocationData.iLongitude);
                }

            if (!Math::IsNaN(iLocationData.iHorizontalAccuracy))
                {
                if( iLocationData.iHorizontalAccuracy > 0 )
                    iLocality.SetHorizontalAccuracy(iLocationData.iHorizontalAccuracy);
                }
            // save position if at least lat/lon are entered
            iLandmark->SetPositionL(iLocality);            
            }
        }

    /*
    * Check if this landmark still exists in database
    * (could have been potentially deleted some other app)
    * It it doesn't, then create a new landmark and add it to database
    */

    TBool isLmPresent = ETrue;
    CPosLandmark *landmark = NULL;
    TPosLmItemId lmId = iLandmark->LandmarkId();

    TRAPD(err, landmark = iDb.ReadLandmarkLC(lmId);
		CleanupStack::PopAndDestroy(landmark);
    )

    if (err == KErrNotFound) // Landmark deleted already
	    {
	    isLmPresent = EFalse;
	    }

    if ( (isLmPresent) &&
       (iLandmark->LandmarkId() != KPosLmNullItemId) ) // Landmark exists in db
        {
        iDb.UpdateLandmarkL( *iLandmark );
        }
    else // Lm does not exist in db
        {
        iLandmarkId = iDb.AddLandmarkL( *iLandmark );
        }
	}

// ----------------------------------------------------------------------------
// CLmkFields::DeleteLandmarkL
// ----------------------------------------------------------------------------
//
EXPORT_C void CLmkFields::DeleteLandmarkL()
    {
    if ( iLandmark )
        {
        CLmkDbUtils::DeleteLandmarkL( iLandmark->LandmarkId(), iDb );
        }
    }

// ----------------------------------------------------------------------------
// CLmkFields::GetField
// ----------------------------------------------------------------------------
//
EXPORT_C MLmkFieldData* CLmkFields::GetField( TUint16 aFieldType )
    {
    TInt count( iFieldsArray.Count() );

    for ( TInt i=0; i<count; i++ )
        {
        if ( iFieldsArray[i]->FieldType() == aFieldType )
            {
            return iFieldsArray[i];
            }
        }
    return NULL;
    }

// ----------------------------------------------------------------------------
// CLmkFields::CreateLmkFieldsL
// ----------------------------------------------------------------------------
//
EXPORT_C TPosLmItemId CLmkFields::LandmarkId() const
    {
    return iLandmarkId;
    }

// ----------------------------------------------------------------------------
// CLmkFields::CreateLmkFieldsL
// ----------------------------------------------------------------------------
//
void CLmkFields::CreateLmkFieldsL()
    {
    if ( iJapaneseMode ) // Japanese mode
        {
        CreateLmkFieldsInJapaneseModeL();
        }
    else
        {
        CreateLmkFieldsInNormalModeL();
        }
    }
// ----------------------------------------------------------------------------
// CLmkFields::CreateLmkFieldsInJapaneseModeL()
// ----------------------------------------------------------------------------
//
void CLmkFields::CreateLmkFieldsInJapaneseModeL()
    {
    // Japanese address fields format
    //get location position
    iLandmark->GetPosition( iLocality );
    CreateNameFieldL( ENameJapaneseMode ); // name field is always shown

    if ( ( iAttributes & CLmkEditorDlg::ELmkCategory ) != 0 )
        {
        CreateCategoryFieldL();
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkDescription ) != 0 )
        {
        CreateDescriptionFieldL( EDescriptionJapaneseMode );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkPostalZIP ) != 0 )
        {
        CreatePostCodeFieldL( EPostCodeJapaneseMode );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkStateProvince ) != 0 )
        {
        CreateStateProvinceFieldL( EStateProvinceJapaneseMode );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkCity ) != 0 )
        {
        CreateCityFieldL( ECityJapaneseMode );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkStreet ) != 0 )
        {
        CreateStreetFieldL( EStreetJapaneseMode );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkCountry ) != 0 )
        {
        CreateCountryFieldL( ECountryJapaneseMode );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkPhoneNumber ) != 0 )
        {
        CreatePhoneNumberFieldL( EPhoneNumberJapaneseMode );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkWebAddress ) != 0 )
        {
        CreateWebAddressFieldL(	EWebAddressJapaneseMode );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkLatitude ) != 0 )
        {
        CreateLatitudeFieldsL( ELatitudeJapaneseMode );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkLongitude ) != 0 )
        {
        CreateLongitudeFieldsL( ELongitudeJapaneseMode );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkPositionAccuracy ) != 0 )
        {
        CreatePositionAccuracyFieldL( EPositionAccuracyJapaneseMode );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkAltitude ) != 0 )
        {
        CreateAltitudeFieldL( EAltitudeJapaneseMode );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkAltitudeAccuracy ) != 0 )
        {
        CreateAltitudeAccuracyFieldL( EAltitudeAccuracyJapaneseMode );
        }
    }

// ----------------------------------------------------------------------------
// CLmkFields::CreateLmkFieldsInNormalModeL()
// ----------------------------------------------------------------------------
//
void CLmkFields::CreateLmkFieldsInNormalModeL()
    {
    //get location position
    iLandmark->GetPosition( iLocality );
    CreateNameFieldL( EName ); // name field is always shown

    if ( ( iAttributes & CLmkEditorDlg::ELmkCategory ) != 0 )
        {
        CreateCategoryFieldL( );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkDescription ) != 0 )
        {
        CreateDescriptionFieldL( EDescription );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkStreet ) != 0 )
        {
        CreateStreetFieldL( EStreet );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkPostalZIP ) != 0 )
        {
        CreatePostCodeFieldL( EPostCode );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkCity ) != 0 )
        {
        CreateCityFieldL( ECity );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkStateProvince ) != 0 )
        {
        CreateStateProvinceFieldL( EStateProvince );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkCountry ) != 0 )
        {
        CreateCountryFieldL( ECountry );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkPhoneNumber ) != 0 )
        {
        CreatePhoneNumberFieldL( EPhoneNumberJapaneseMode );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkWebAddress ) != 0 )
        {
        CreateWebAddressFieldL(	EWebAddressJapaneseMode );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkLatitude ) != 0 )
        {
        CreateLatitudeFieldsL( ELatitude );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkLongitude ) != 0 )
        {
        CreateLongitudeFieldsL( ELongitude );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkPositionAccuracy ) != 0 )
        {
        CreatePositionAccuracyFieldL( EPositionAccuracy );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkAltitude ) != 0 )
        {
        CreateAltitudeFieldL( EAltitude );
        }
    if ( ( iAttributes & CLmkEditorDlg::ELmkAltitudeAccuracy ) != 0 )
        {
        CreateAltitudeAccuracyFieldL( EAltitudeAccuracy );
        }
    }

// ----------------------------------------------------------------------------
// CLmkFields::CreateNameFieldL
// ----------------------------------------------------------------------------
//
void CLmkFields::CreateNameFieldL( TInt aPos )
    {
    // Landmark name field includes also icon id and path

    MLmkFieldData* field = CLmkFieldData::NewL(
                            iLabelArray->operator[]( aPos ) );
    CleanupStack::PushL(field);

    TPtrC lmkName;
    if ( iLandmark->GetLandmarkName( lmkName ) == KErrNone )
        {
        field->SetTextL( lmkName );
        field->SetFieldType( EName );
        }

    TPtrC iconPath;
    TInt iconId;
    TInt iconMaskIndex;


    if ( iLandmark->GetIcon(iconPath,iconId,  iconMaskIndex) == KErrNone )
        {
        field->SetIconId( iconId );
        field->SetIconPathL( iconPath );
        }
    else
        {
        field->SetIconId( KErrNotFound );
        }

    field->SetPositionFieldId( EPositionFieldNone );
    field->SetEditorType( ETextGenericFieldEditor );
    field->SetFieldLength( KMaxFieldLength50 );
    field->SetTitleField( ETrue );
    iFieldsArray.AppendL(field);

    CleanupStack::Pop(); //field

    }

// ----------------------------------------------------------------------------
// CLmkFields::CreateCategoryFieldL
// ----------------------------------------------------------------------------
//
void CLmkFields::CreateCategoryFieldL()
    {
    TInt catLabelPos(0);
    if ( iJapaneseMode )
        {
        catLabelPos = ECategoryJapaneseMode;
        }
    else
        {
        catLabelPos = ECategoryJapaneseMode;
        }

    MLmkFieldData* field = CLmkFieldData::NewL(
                             iLabelArray->operator[](catLabelPos) );
    CleanupStack::PushL(field);

    RArray<TPosLmItemId>& categories = field->Categories();
    iLandmark->GetCategoriesL(categories);

    field->SetPositionFieldId( EPositionFieldNone );
    if ( categories.Count() <= 1 )
        {
        field->SetFieldType( ECategory );
        }
    else
        {
        field->SetFieldType( ECategories );
        if ( iJapaneseMode )
            {
            field->SetLabel( iLabelArray->operator[]( ECategoriesJapaneseMode ) );
            }
        else
            {
            field->SetLabel( iLabelArray->operator[]( ECategories ) );
            }
        }
    field->SetEditorType( EListFieldEditor );
    iFieldsArray.AppendL(field);
    CleanupStack::Pop(); //field

    //Remove all categories from landmark
    TInt count( categories.Count() );
    for ( TInt i=0; i < count; i++ )
        {
        iLandmark->RemoveCategory( categories[i] );
        }
    }

// ----------------------------------------------------------------------------
// CLmkFields::CreateStreetFieldL
// ----------------------------------------------------------------------------
//
void CLmkFields::CreateStreetFieldL( TInt aPos )
    {
    MLmkFieldData* field = CLmkFieldData::NewL(
                                iLabelArray->operator[]( aPos ) );
    CleanupStack::PushL(field);

    TPtrC street;
    if ( iLandmark->GetPositionField(
            EPositionFieldStreet, street ) == KErrNone )
        {
        field->SetTextL( street );
        }

    field->SetFieldType( EStreet );
    field->SetPositionFieldId( EPositionFieldStreet );
    field->SetEditorType( ETextGenericFieldEditor );
    field->SetFieldLength( KMaxFieldLength50 );
    iFieldsArray.AppendL(field);

    CleanupStack::Pop(); //field
    }

// ----------------------------------------------------------------------------
// CLmkFields::CreateCityFieldL
// ----------------------------------------------------------------------------
//
void CLmkFields::CreateCityFieldL( TInt aPos )
    {
    MLmkFieldData* field = CLmkFieldData::NewL(
                            iLabelArray->operator[]( aPos ) );
    CleanupStack::PushL(field);

    TPtrC city;
    if ( iLandmark->GetPositionField( EPositionFieldCity, city ) == KErrNone )
        {
        field->SetTextL( city );
        }

    field->SetFieldType( ECity );
    field->SetPositionFieldId( EPositionFieldCity );
    field->SetEditorType( ETextGenericFieldEditor );
    field->SetFieldLength( KMaxFieldLength50 );
    iFieldsArray.AppendL(field);

    CleanupStack::Pop(); //field
    }

// ----------------------------------------------------------------------------
// CLmkFields::CreateStateProvinceFieldL
// ----------------------------------------------------------------------------
//
void CLmkFields::CreateStateProvinceFieldL( TInt aPos )
    {
    MLmkFieldData* field = CLmkFieldData::NewL(
                            iLabelArray->operator[]( aPos ) );
    CleanupStack::PushL(field);

    TPtrC stateProvince;
    if ( iLandmark->GetPositionField(
          EPositionFieldState, stateProvince ) == KErrNone )
        {
        field->SetTextL( stateProvince );
        }

    field->SetFieldType( EStateProvince );
    field->SetPositionFieldId( EPositionFieldState );
    field->SetEditorType( ETextGenericFieldEditor );
    field->SetFieldLength( KMaxFieldLength50 );
    iFieldsArray.AppendL(field);

    CleanupStack::Pop(); //field
    }

// ----------------------------------------------------------------------------
// CLmkFields::CreateCountryFieldL
// ----------------------------------------------------------------------------
//
void CLmkFields::CreateCountryFieldL( TInt aPos )
    {
    MLmkFieldData* field = CLmkFieldData::NewL(
                            iLabelArray->operator[]( aPos ) );
    CleanupStack::PushL(field);

    TPtrC country;
    if ( iLandmark->GetPositionField(
          EPositionFieldCountry, country ) == KErrNone )
        {
        field->SetTextL( country );
        }

    field->SetFieldType( ECountry );
    field->SetPositionFieldId( EPositionFieldCountry );
    field->SetEditorType( ETextGenericFieldEditor );
    field->SetFieldLength( KMaxFieldLength50 );
    iFieldsArray.AppendL(field);

    CleanupStack::Pop(); //field
    }

// ----------------------------------------------------------------------------
// CLmkFields::CreatePostCodeFieldL
// ----------------------------------------------------------------------------
//
void CLmkFields::CreatePostCodeFieldL( TInt aPos )
    {
    MLmkFieldData* field = CLmkFieldData::NewL(
                            iLabelArray->operator[]( aPos) );
    CleanupStack::PushL(field);

    TPtrC postcode;
    if ( iLandmark->GetPositionField(
          EPositionFieldPostalCode, postcode ) == KErrNone )
        {
        field->SetTextL( postcode );
        }

    field->SetFieldType( EPostCode );
    field->SetPositionFieldId( EPositionFieldPostalCode );
    field->SetEditorType( ETextGenericFieldEditor );
    field->SetFieldLength( KMaxFieldLength20 );
    iFieldsArray.AppendL(field);

    CleanupStack::Pop(); //field
    }

// ----------------------------------------------------------------------------
// CLmkFields::CreateLatitudeFieldsL
// ----------------------------------------------------------------------------
//
void CLmkFields::CreateLatitudeFieldsL( TInt aPos )
    {
    MLmkFieldData* field = CLmkFieldData::NewL(
                            iLabelArray->operator[]( aPos ) );
    CleanupStack::PushL(field);

    field->SetFieldType( ELatitude );
    field->SetPositionFieldId( EPositionFieldNone );
    field->SetEditorType( ECoordinateFieldEditor );
    field->SetFieldLength( KMaxFieldLength10 );
    field->SetTReal( iLocality.Latitude() );
    iFieldsArray.AppendL(field);

    CleanupStack::Pop(); //field
    }

// ----------------------------------------------------------------------------
// CLmkFields::CreateLongitudeFieldsL
// ----------------------------------------------------------------------------
//
void CLmkFields::CreateLongitudeFieldsL( TInt aPos )
    {
    MLmkFieldData* field = CLmkFieldData::NewL(
                            iLabelArray->operator[]( aPos ) );
    CleanupStack::PushL(field);

    field->SetFieldType( ELongitude );
    field->SetPositionFieldId( EPositionFieldNone );
    field->SetEditorType( ECoordinateFieldEditor );
    field->SetFieldLength( KMaxFieldLength10 );
    field->SetTReal( iLocality.Longitude() );
    iFieldsArray.AppendL(field);

    CleanupStack::Pop(); //field
    }
// ----------------------------------------------------------------------------
// CLmkFields::CreatePositionAccuracyFieldL
// ----------------------------------------------------------------------------
//
void CLmkFields::CreatePositionAccuracyFieldL( TInt aPos )
    {
    MLmkFieldData* field = CLmkFieldData::NewL(
                             iLabelArray->operator[]( aPos ) );
    CleanupStack::PushL(field);

    field->SetFieldType( EPositionAccuracy );
    field->SetPositionFieldId( EPositionFieldNone );
    field->SetEditorType( ENumberFieldEditor );
    field->SetFieldLength( KMaxFieldLength10 );
    field->SetTReal( iLocality.HorizontalAccuracy() );
    iFieldsArray.AppendL(field);

    CleanupStack::Pop(); //field
    }

// ----------------------------------------------------------------------------
// CLmkFields::CreateAltitudeFieldL
// ----------------------------------------------------------------------------
//
void CLmkFields::CreateAltitudeFieldL( TInt aPos )
    {
    MLmkFieldData* field = CLmkFieldData::NewL(
                            iLabelArray->operator[]( aPos ) );
    CleanupStack::PushL(field);

    field->SetFieldType( EAltitude );
    field->SetPositionFieldId( EPositionFieldNone );
    field->SetEditorType( ENumberFieldEditor );
    field->SetFieldLength( KMaxFieldLength10 );
    field->SetTReal( iLocality.Altitude() );
    iFieldsArray.AppendL(field);

    CleanupStack::Pop(); //field
    }

// ----------------------------------------------------------------------------
// CLmkFields::CreateAltitudeAccuracyFieldL
// ----------------------------------------------------------------------------
//
void CLmkFields::CreateAltitudeAccuracyFieldL( TInt aPos )
    {
    MLmkFieldData* field = CLmkFieldData::NewL(
                            iLabelArray->operator[]( aPos ) );
    CleanupStack::PushL(field);

    field->SetFieldType( EAltitudeAccuracy );
    field->SetPositionFieldId( EPositionFieldNone );
    field->SetEditorType( ENumberFieldEditor );
    field->SetFieldLength( KMaxFieldLength10 );
    field->SetTReal( iLocality.VerticalAccuracy() );
    iFieldsArray.AppendL(field);

    CleanupStack::Pop(); //field
    }

// ----------------------------------------------------------------------------
// CLmkFields::CreateDescriptionFieldL
// ----------------------------------------------------------------------------
//
void CLmkFields::CreateDescriptionFieldL( TInt aPos )
    {
    MLmkFieldData* field = CLmkFieldData::NewL(
                                iLabelArray->operator[]( aPos ) );
    CleanupStack::PushL(field);

    TPtrC lmkDescription;

    // Get the description field info
    if ( iLandmark->GetLandmarkDescription( lmkDescription ) == KErrNone )
        {
        field->SetTextL( lmkDescription );
        field->SetFieldType( EDescription );
        }

    field->SetFieldType( EDescription );
    field->SetPositionFieldId(  EPositionFieldNone );
    field->SetEditorType( ETextGenericFieldEditor );
    field->SetFieldLength( KMaxDescriptionFieldLen );
    iFieldsArray.AppendL(field);
    CleanupStack::Pop(); //field
    }

// ----------------------------------------------------------------------------
// CLmkFields::CreatePhoneNumberFieldL
// ----------------------------------------------------------------------------
//
void CLmkFields::CreatePhoneNumberFieldL( TInt aPos )
    {
    MLmkFieldData* field = CLmkFieldData::NewL(
                                iLabelArray->operator[]( aPos ) );
    CleanupStack::PushL(field);

    TPtrC phoneNumber;
    if ( iLandmark->GetPositionField(
             ELmkPositionFieldPhoneNumber, phoneNumber ) == KErrNone )
        {
        field->SetTextL( phoneNumber );
        }

    field->SetFieldType( EPhoneNumber );
    field->SetPositionFieldId( ELmkPositionFieldPhoneNumber );
    field->SetEditorType( ETextPhoneNumberFieldEditor );
    field->SetFieldLength( KMaxPhoneNumberFieldLen );
    iFieldsArray.AppendL(field);

    CleanupStack::Pop(); //field
    }

// ----------------------------------------------------------------------------
// CLmkFields::CreateWebAddressFieldL
// ----------------------------------------------------------------------------
//
void CLmkFields::CreateWebAddressFieldL( TInt aPos )
    {
    MLmkFieldData* field = CLmkFieldData::NewL( iLabelArray->operator[]( aPos ) );
    CleanupStack::PushL(field);

    TPtrC webAddress;
    if ( iLandmark->GetPositionField(
            	ELmkPositionFieldWebAddress, webAddress ) == KErrNone )
        {
        field->SetTextL( webAddress );
        }

    /*
     * Web Address field is one of the data part of whole media link info
     * Media link -- name + Mime Info/format + URL
     * Right now Mime info not supported.
     * landmark's media link info containing '//' indicates a empty mime info
     * and remove the same before saving the received landmark to database
     */

    HBufC* lmkField =  HBufC::NewL( 256);
    CleanupStack::PushL( lmkField );
    lmkField->Des().Copy( field->TextData() );
    TPtr a = lmkField->Des();
    CLmkDbUtils::RemoveDefaultProtocolL(a);
    field->SetTextL( lmkField->Des());
    CleanupStack::PopAndDestroy( lmkField );

    field->SetFieldType( EWebAddress);
    field->SetPositionFieldId( ELmkPositionFieldWebAddress);
    field->SetEditorType( ETextUriFieldEditor );
    field->SetFieldLength( KMaxUrlFieldLen);
    iFieldsArray.AppendL(field);

    CleanupStack::Pop(); //field
    }
//----------------------------------------------------------------------------
// CLmkFields::CheckDiscSpace()
// ----------------------------------------------------------------------------
//
EXPORT_C TInt CLmkFields::CheckDiscSpaceL()
    {
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL( fs );
    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &fs, sizeof( CPosLandmark ), EDriveC ) )
        {
        CleanupStack::PopAndDestroy(); // fs
        return KErrDiskFull;
        }
    CleanupStack::PopAndDestroy(); // fs
    return KErrNone;
    }
//----------------------------------------------------------------------------
// CLmkFields::RemoveEnterCharacter()
// ----------------------------------------------------------------------------
//
EXPORT_C TPtr  CLmkFields::RemoveEnterCharacter( TPtrC aText )
	{
	_LIT( KReplaceWhitespaceChars, "\x0009\x000A\x000B\x000C\x000D\x2028\x2029" );
	_LIT(KSpace," ");
	TUint16* data = const_cast<TUint16*>(aText.Ptr());
    TPtr des1(data,aText.Length());
    des1.Copy(aText);
	AknTextUtils::ReplaceCharacters(des1,KReplaceWhitespaceChars,TChar(' '));
	AknTextUtils::PackWhiteSpaces(des1, KSpace);
    return des1;
	}
//  End of File
