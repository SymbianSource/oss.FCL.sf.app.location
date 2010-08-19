/*
* Copyright (c) 2004-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implements the CLandmarksEditDialog class
*
*/


#include <eikmenub.h>
#include <akntitle.h>
#include <aknnotewrappers.h>
#include <aknappui.h>
#include <e32math.h>
#include <eikcapc.h>
#include <aknselectionlist.h>
#include <aknlocationed.h>
#include <AknWaitDialog.h> 
#include <avkon.rsg>

#include <lbsposition.h>
#include <lbspositioninfo.h>

#include <EPos_CPosLandmark.h>
#include <EPos_CPosLandmarkCategory.h>

#include "LmRefApp.hrh"
#include <lmrefapp.rsg>

#include "LandmarksUtils.h"
#include "LandmarksCommonData.h"
#include "LandmarksEditDialog.h"
#include "LandmarksCategoriesDialog.h"
#include "LandmarksPositionRequest.h"
#include "LandmarksApplicationEngine.h"
#include "LandmarksAppUi.h"
#include "LandmarksLmCategoriesModel.h"

// ============================ LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
//  Gets value from EDWIN and puts to cleanup stack
//  @param aEdwin must be EDWIN control
// -----------------------------------------------------------------------------
//
HBufC* FetchTextLC( CCoeControl* aEdwin )
    {
    CEikEdwin* editor = static_cast <CEikEdwin*> ( aEdwin );
    HBufC* text = editor->GetTextInHBufL();
    if ( text )
        {
        CleanupStack::PushL( text );
        }
    else
        {
        // create empty text
        text = HBufC::NewLC( 0 );
        }
    return text;
    }

// -----------------------------------------------------------------------------
//  Retrieves a long float from am EDWIN editor.
//  @param aDouble double float that is retrieved from editor.
//  @param aEdwin must be EDWIN control
//  @return ETrue if successful, EFalse otherwise
// -----------------------------------------------------------------------------
//
TBool FetchDoubleL( TReal& aDouble, CCoeControl* aEdwin )
    {
    TBool success = EFalse;
    TLocale locale;

    HBufC* text = FetchTextLC( aEdwin );

    if ( text->Length() )
        {
        // Try to extract value
        TLex fieldLex( *text );
        success = ( KErrNone == fieldLex.Val( aDouble, locale.DecimalSeparator() ) );
        }
    else
        {
        // User has indicated that he/she wants this value to be undefined.
        TRealX nan;
        nan.SetNaN();
        aDouble = nan;
        success = ETrue;
        }

    CleanupStack::PopAndDestroy( text );
    return success;
    }

// -----------------------------------------------------------------------------
//  Retrieves a float from am EDWIN editor.
//  @param aFloat float that is retrieved from editor.
//  @param aEdwin must be EDWIN control
//  @return ETrue if successful, EFalse otherwise
// -----------------------------------------------------------------------------
//
TBool FetchFloatL( TReal32& aFloat, CCoeControl* aEdwin )
    {
    TBool success = EFalse;
    TLocale locale;

    HBufC* text = FetchTextLC( aEdwin );

    if ( text->Length() )
        {
        // Try to extract value
        TLex fieldLex( *text );
        success = ( KErrNone == fieldLex.Val( aFloat, locale.DecimalSeparator() ) );
        }
    else
        {
        // User has indicated that he/she wants this value to be undefined.
        TRealX nan;
        nan.SetNaN();
        aFloat = nan;
        success = ETrue;
        }

    CleanupStack::PopAndDestroy( text );
    return success;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksEditDialog::CLandmarksEditDialog(
    CPosLandmark& aLandmark,
    TBool aUseCurrentLocation,
    TBool& aEdited,
    CLandmarksApplicationEngine& aEngine,
    TInt aInitialFocusFieldId )
:   iOriginalLandmark( aLandmark ),
    iUseCurrentLocation( aUseCurrentLocation ),
    iEdited( aEdited ),
    iEngine( aEngine ),
    iInitialFocusFieldId( aInitialFocusFieldId )
    {
    iEdited = EFalse;

    // Configure real format
    const TChar KDecimalChar = '.';
    iRealFormat.iPlaces = KNrOfDecimals;
    iRealFormat.iPoint = KDecimalChar;
    iRealFormat.iTriLen = 0;
    iRealFormat.iWidth = KPosLmMaxTextFieldLength;
    iRealFormat.iType = KRealFormatFixed;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::ConstructL()
    {
    CAknForm::ConstructL( R_LMREFAPP_EDIT_MENU_BAR );

    // Save original title
    CEikStatusPane* statusPane = iAvkonAppUi->StatusPane();
    iTitlePane = ( CAknTitlePane* ) statusPane->ControlL(
        TUid::Uid( EEikStatusPaneUidTitle ) );
    iOriginalTitle = iTitlePane->Text()->AllocL();

    // Copy data of original landmark
    iLandmark = CPosLandmark::NewL( iOriginalLandmark );

    // Create iPositionRequest
    HBufC* appName = iCoeEnv->AllocReadResourceAsDes16LC( R_LMREFAPP_APP_TITLE );
    iPositionRequest = CLandmarksPositionRequest::NewL( *appName, this );
    CleanupStack::PopAndDestroy( appName );

    // Initialize iCategoryIds
    iLandmark->GetCategoriesL( iCategoryIds );

    iLmCategoriesModel =
            CLandmarksLmCategoriesModel::NewL( iEngine, iCategoryIds );

    // Initialise states of real value editors
    for ( TInt i = ELatitudeEditor; i <= ERadiusEditor; i++ )
        {
        User::LeaveIfError( iIsRealValueEdited.Append( EFalse ) );
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksEditDialog* CLandmarksEditDialog::NewLC(
    CPosLandmark& aLandmark,
    TBool aUseCurrentLocation,
    TBool& aEdited,
    CLandmarksApplicationEngine& aEngine,
    TInt aInitialFocusFieldId )
    {
    CLandmarksEditDialog* self = new ( ELeave ) CLandmarksEditDialog(
        aLandmark, aUseCurrentLocation, aEdited, aEngine, aInitialFocusFieldId );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksEditDialog::~CLandmarksEditDialog()
    {
    CloseWaitDialog();

    delete iLandmark;
    delete iAppName;
    delete iPositionRequest;
    iCategoryIds.Close();
    iIsRealValueEdited.Close();
    delete iLmCategoriesModel;

    // Restore title pane. Ownership of iTitlePane transferred.
    if ( iTitlePane && iOriginalTitle )
        {
        iTitlePane->SetText( iOriginalTitle );
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::DynInitMenuPaneL(
    TInt aResourceId,
    CEikMenuPane* aMenuPane )
    {
    CAknForm::DynInitMenuPaneL( aResourceId, aMenuPane );

    if ( aResourceId == R_AVKON_FORM_MENUPANE )
        {
        // We do not want to allow the user to modify our Form therefore
        // Disable the Label, Add, Delete, and Save Field
        aMenuPane->SetItemDimmed( EAknFormCmdLabel, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdAdd, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdDelete, ETrue );
        aMenuPane->SetItemDimmed( EAknFormCmdSave, ETrue );

        aMenuPane->SetItemDimmed( ELandmarksUpdateToCurrentLocation,
            Control( ELandmarkCategoryField )->IsFocused() );
        aMenuPane->SetItemDimmed( ELandmarksEditCategories,
            !Control( ELandmarkCategoryField )->IsFocused() );
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::ProcessCommandL( TInt aCommandId )
	{
	CAknForm::ProcessCommandL( aCommandId );

        switch ( aCommandId )
            {
            case ELandmarksUpdateToCurrentLocation:
                UpdateToCurrentLocationL();
                break;

            case ELandmarksResetCoordinates:
                ResetCoordinatesL();
                break;

            case ELandmarksEditCategories:
                EditCategoriesL();
                break;

            default:
                break;
            }
	}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::NotifyOperationProgressL(
    TOperation aOperation,
    TInt /*aProgress*/,
    TInt aErrorCode )
    {
    if ( aErrorCode == KErrNone &&
        aOperation == EAcquiringLocation )
        {
        // Update location fields with new location
        TPosition position;
        iPositionInfo.GetPosition( position );
        ConfigureLocationFieldsL( position );
        SetChangesPending( ETrue );

        // Indicate that editors have been updated
        for ( TInt i = ELatitudeEditor; i <= EVerticalAccEditor; i++ )
            {
            iIsRealValueEdited[i] = ETrue;
            }
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TBool CLandmarksEditDialog::OkToExitL( TInt aButtonId )
	{
    if ( aButtonId == EAknSoftkeyDone )
        {
        // exit from CAknForm's "edit" mode
        return CAknForm::OkToExitL( EAknSoftkeyBack );
        }
    else
        {
    	return CAknForm::OkToExitL( aButtonId );
        }
	}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::PreLayoutDynInitL()
    {
    SetEditableL( ETrue );
    ConfigureFieldsL();
    ConfigureCategoryFieldL();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::PostLayoutDynInitL()
    {
    SetChangesPending( ETrue );

    UpdateTitleBarL();
    CAknForm::PostLayoutDynInitL();

    if ( iUseCurrentLocation )
        {
        UpdateToCurrentLocationL();
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::SetInitialCurrentLine()
    {
    CAknForm::SetInitialCurrentLine();

    if ( iInitialFocusFieldId >= ELandmarkNameField &&
        iInitialFocusFieldId <= ELandmarkCountryField )
        {
        TInt err = KErrNone;
        TRAP( err, TryChangeFocusToL( iInitialFocusFieldId ) );
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::HandleControlEventL(
    CCoeControl* aControl,
    TCoeEvent aEventType )
	{
    CAknForm::HandleControlEventL( aControl, aEventType );

    if ( aEventType == EEventStateChanged )
        {
        if ( aControl == ControlOrNull( ELandmarkLatitudeField ) )
            {
            iIsRealValueEdited[ELatitudeEditor] = ETrue;
            }
        else if ( aControl == ControlOrNull( ELandmarkLongitudeField ) )
            {
            iIsRealValueEdited[ELongitudeEditor] = ETrue;
            }
        else if ( aControl == ControlOrNull( ELandmarkAltitudeField ) )
            {
            iIsRealValueEdited[EAltitudeEditor] = ETrue;
            }
        else if ( aControl == ControlOrNull( ELandmarkHorAccField ) )
            {
            iIsRealValueEdited[EHorizontalAccEditor] = ETrue;
            }
        else if ( aControl == ControlOrNull( ELandmarkVerAccField ) )
            {
            iIsRealValueEdited[EVerticalAccEditor] = ETrue;
            }
        else if ( aControl == ControlOrNull( ELandmarkRadiusField ) )
            {
            iIsRealValueEdited[ERadiusEditor] = ETrue;
            }
        else if ( aControl == ControlOrNull( ELandmarkNameField ) )
            {
            UpdateTitleBarL( ETrue ); // get it from name field
            }
        }
	}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::HandleResourceChange( TInt aType )
	{
	if ( aType == KEikDynamicLayoutVariantSwitch )
		{
		CAknForm::HandleResourceChange( aType );
		}
	}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TBool CLandmarksEditDialog::SaveFormDataL()
    {
    TErrorInForm error = FetchAndSaveFormDataL();
    if ( error )
        {
        ProcessErrorsInFormL( error );
        // Leave to interrupt saving of data. Other error codes than KErrNone
        // result in an additional error dialog.
        User::Leave( KErrNone );
        }
    else
        {
        // input is valid, save everything to original landmark
        TPtrC text;

        iLandmark->GetLandmarkName( text );
        iOriginalLandmark.SetLandmarkNameL( text );

        iLandmark->GetLandmarkDescription( text );
        iOriginalLandmark.SetLandmarkDescriptionL( text );

        iOriginalLandmark.RemoveLandmarkAttributes( CPosLandmark::ECategoryInfo );
        for ( TInt i = 0; i < iCategoryIds.Count(); i++ )
            {
            iOriginalLandmark.AddCategoryL( iCategoryIds[i] );
            }

        CopyCoordinateDataL( *iLandmark, iOriginalLandmark );
        CopyAddressDataL( *iLandmark, iOriginalLandmark );
        }

    iEdited = ETrue;
    return ETrue;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksEditDialog::TErrorInForm CLandmarksEditDialog::FetchAndSaveFormDataL()
    {
    HBufC* text = NULL;

    // name
    text = FetchTextLC( Control( ELandmarkNameField ) );
    if ( text->Length() )
        {
        iLandmark->SetLandmarkNameL( *text );
        }
    else
        {
        return EEmptyLandmarkName;
        }
    CleanupStack::PopAndDestroy( text );

    // description
    text = FetchTextLC( Control( ELandmarkDescField ) );
    iLandmark->SetLandmarkDescriptionL( *text );
    CleanupStack::PopAndDestroy( text );

    // categories
    iLandmark->RemoveLandmarkAttributes( CPosLandmark::ECategoryInfo );
    for ( TInt i = 0; i < iCategoryIds.Count(); i++ )
        {
        iLandmark->AddCategoryL( iCategoryIds[i] );
        }

    // Address fields
    SaveAddressDataL();

    // coordinate fields
    TErrorInForm error = SaveCoordinateDataL();
    if ( error )
        {
        return error;
        }

    return ENoErrors;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::SaveAddressDataL()
    {
    HBufC* text = NULL;

    iLandmark->RemovePositionField( EPositionFieldStreet );
    iLandmark->RemovePositionField( EPositionFieldPostalCode );
    iLandmark->RemovePositionField( EPositionFieldCity );
    iLandmark->RemovePositionField( EPositionFieldCountry );

    // street
    text = FetchTextLC( Control( ELandmarkStreetField ) );
    if ( text->Length() )
        {
        iLandmark->SetPositionFieldL( EPositionFieldStreet, *text );
        }
    CleanupStack::PopAndDestroy( text );

    // postal code
    text = FetchTextLC( Control( ELandmarkPostalField ) );
    if ( text->Length() )
        {
        iLandmark->SetPositionFieldL( EPositionFieldPostalCode, *text );
        }
    CleanupStack::PopAndDestroy( text );

    // city
    text = FetchTextLC( Control( ELandmarkCityField ) );
    if ( text->Length() )
        {
        iLandmark->SetPositionFieldL( EPositionFieldCity, *text );
        }
    CleanupStack::PopAndDestroy( text );

    // country
    text = FetchTextLC( Control( ELandmarkCountryField ) );
    if ( text->Length() )
        {
        iLandmark->SetPositionFieldL( EPositionFieldCountry, *text );
        }
    CleanupStack::PopAndDestroy( text );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
CLandmarksEditDialog::TErrorInForm CLandmarksEditDialog::SaveCoordinateDataL()
    {
    TLocality existingLocation;
    TBool locationExists = ( iLandmark->GetPosition( existingLocation ) == KErrNone );

    TReal latitude = existingLocation.Latitude();
    TReal longitude = existingLocation.Longitude();
    TReal32 altitude = existingLocation.Altitude();
    TReal32 horAcc = existingLocation.HorizontalAccuracy();
    TReal32 verAcc = existingLocation.VerticalAccuracy();

    TPosition tempPos;

    // latitude
    if ( iIsRealValueEdited[ELatitudeEditor] || !locationExists )
        {
        CAknLocationEditor* editor = static_cast<CAknLocationEditor*>( Control ( ELandmarkLatitudeField ) );
    	editor->Get( tempPos );
    	latitude = tempPos.Latitude();
        }

    // longitude
    if ( iIsRealValueEdited[ELongitudeEditor] || !locationExists )
        {
        CAknLocationEditor* editor = static_cast<CAknLocationEditor*>( Control ( ELandmarkLongitudeField ) );
    	editor->Get( tempPos );
    	longitude = tempPos.Longitude();
        }

    // altitude
    if ( iIsRealValueEdited[EAltitudeEditor] || !locationExists )
        {
        if ( !FetchFloatL( altitude, Control( ELandmarkAltitudeField ) ) )
            {
            return EWrongAltitude;
            }
        }

    // horizontal accuracy
    if ( iIsRealValueEdited[EHorizontalAccEditor] || !locationExists )
        {
        if ( !FetchFloatL( horAcc, Control( ELandmarkHorAccField ) ) )
            {
            return EWrongHorAcc;
            }
        }

    // vertical accuracy
    if ( iIsRealValueEdited[EVerticalAccEditor] || !locationExists )
        {
        if ( !FetchFloatL( verAcc, Control( ELandmarkVerAccField ) ) )
            {
            return EWrongVerAcc;
            }
        }

    // Update location. There are some rules specified by TLocality and
    // TCoordinate how to update a location. These rules are taken into
    // account below when validating the location data.
    if ( Math::IsNaN( latitude ) && Math::IsNaN( longitude ) )
        {
        // If lat long are undefined, then alt and hAcc
        // must be undefined
        if ( !Math::IsNaN( horAcc ) )
            {
            return ENoLatLonButHorAcc;
            }
        else if ( !Math::IsNaN( altitude ) )
            {
            return ENoLatLonButAltitude;
            }
        else if ( !Math::IsNaN( verAcc ) )
            {
            return ENoAltButVerAcc;
            }
        else
            {
            // None position fields are set, Remove position info.
            iLandmark->RemoveLandmarkAttributes( CPosLandmark::EPosition );
            }
        }
    else if ( !Math::IsNaN( latitude ) && !Math::IsNaN( longitude ) )
        {
        if ( !Math::IsNaN( horAcc ) && horAcc < 0 )
            {
            // If lat long are defined, hAcc must be positive if defined
            return EWrongHorAcc;
            }
        else if ( Math::IsNaN( altitude ) && !Math::IsNaN( verAcc ) )
            {
            // if altitude is undefined, then vertical accuracy
            // must be undefined
            return ENoAltButVerAcc;
            }
        else if ( !Math::IsNaN( verAcc ) && verAcc < 0 )
            {
            // vertical accuracy must be positive if defined
            return EWrongVerAcc;
            }
        else
            {
            // It is allowed to set the new locality
            TCoordinate coordinate( latitude, longitude, altitude );
            TLocality newLocation( coordinate, horAcc, verAcc );
            iLandmark->SetPositionL( newLocation );
            }
        }
    else
        {
        // Inform user that no valid lat/long has been entered
        return EOnlyLatOrLon;
        }

    // coverage radius
    TReal32 radius;
    TInt radiusExists = ( iLandmark->GetCoverageRadius( radius ) == KErrNone );
    if ( iIsRealValueEdited[ERadiusEditor] || !radiusExists )
        {
        if ( !FetchFloatL( radius, Control( ELandmarkRadiusField ) ) )
            {
            return EWrongRadius;
            }
        }
    if ( !Math::IsNaN( radius ) && radius < 0 )
        {
        return EWrongRadius;
        }
    iLandmark->SetCoverageRadius( radius );

    return ENoErrors;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::ConfigureFieldsL()
    {
    TPtrC textFieldValue;

    // Configure landmark name
    textFieldValue.Set( KNullDesC );
    iLandmark->GetLandmarkName( textFieldValue );

    SetTextFieldL( ELandmarkNameField, textFieldValue );

    // Configure description
    textFieldValue.Set( KNullDesC );
    iLandmark->GetLandmarkDescription( textFieldValue );

    SetTextFieldL( ELandmarkDescField, textFieldValue );

    // all address fields
    ConfigureAddressFieldsL();

    // Configure location fields
    TLocality location;
    iLandmark->GetPosition( location );
    ConfigureLocationFieldsL( location );

    // Configure coverage Radius
    TReal32 coverageRadius;
    TBuf<KPosLmMaxTextFieldLength> radiusBuf;
    TInt res = iLandmark->GetCoverageRadius( coverageRadius );
    if ( res != KErrNotFound )
        {
        radiusBuf.AppendNum( coverageRadius, iRealFormat );
        }

    SetTextFieldL( ELandmarkRadiusField, radiusBuf );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::ConfigureAddressFieldsL()
    {
    TPtrC textFieldValue;

    // street
    textFieldValue.Set( KNullDesC );
    iLandmark->GetPositionField( EPositionFieldStreet, textFieldValue );

    SetTextFieldL( ELandmarkStreetField, textFieldValue );

    // postalCode
    textFieldValue.Set( KNullDesC );
    iLandmark->GetPositionField( EPositionFieldPostalCode, textFieldValue );

    SetTextFieldL( ELandmarkPostalField, textFieldValue );

    // city
    textFieldValue.Set( KNullDesC );
    iLandmark->GetPositionField( EPositionFieldCity, textFieldValue );

    SetTextFieldL( ELandmarkCityField, textFieldValue );

    // country
    textFieldValue.Set( KNullDesC );
    iLandmark->GetPositionField( EPositionFieldCountry, textFieldValue );

    SetTextFieldL( ELandmarkCountryField, textFieldValue );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::ConfigureLocationFieldsL( TLocality& aLocation )
    {
    TBuf<KPosLmMaxTextFieldLength> textFieldValue;

    TPosition pos( aLocation, TTime(0) );
    // Configure latitude in float format
    SetLocationFieldL( ELandmarkLatitudeField, pos );

    // Configure longitude in float format
    SetLocationFieldL( ELandmarkLongitudeField, pos );

    // Configure altitude
    LandmarksUtils::FloatToDes( aLocation.Altitude(), textFieldValue, iRealFormat );
    SetTextFieldL( ELandmarkAltitudeField, textFieldValue );

    // Configure horizontal accuracy
    LandmarksUtils::FloatToDes( aLocation.HorizontalAccuracy(), textFieldValue, iRealFormat );
    SetTextFieldL( ELandmarkHorAccField, textFieldValue );

    // Configure vertical accuracy
    LandmarksUtils::FloatToDes( aLocation.VerticalAccuracy(), textFieldValue, iRealFormat );
    SetTextFieldL( ELandmarkVerAccField, textFieldValue );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::ConfigureCategoryFieldL()
    {
    TInt nrOfCategories = iCategoryIds.Count();
    const TInt KExtraChars = 2; // CRLF

    HBufC* lmCategoriesBuf = HBufC::NewLC(
        ( KPosLmMaxTextFieldLength + KExtraChars ) * nrOfCategories );

    TPtr lmCategories = lmCategoriesBuf->Des();
    for ( TInt i = 0; i < nrOfCategories; i++ )
        {
        if ( i > 0 )
            {
            lmCategories.Append( CEditableText::ELineBreak );
            }

        CPosLandmarkCategory* category = iEngine.CategoryLC( iCategoryIds[i] );
        TPtrC categoryName;
        category->GetCategoryName( categoryName );
        lmCategories.Append( categoryName );
        CleanupStack::PopAndDestroy( category );
        }

    SetTextFieldL( ELandmarkCategoryField, *lmCategoriesBuf );

    CleanupStack::PopAndDestroy( lmCategoriesBuf );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::SetTextFieldL( TInt aId, const TDesC& aText )
    {
    CEikEdwin* editor = static_cast<CEikEdwin*>( Control( aId ) );
	editor->SetTextL( &aText );
    editor->DrawDeferred();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::SetLocationFieldL( TInt aId, const TPosition& aPosition )
    {
    CAknLocationEditor* editor = static_cast<CAknLocationEditor*>( Control( aId ) );
	editor->Set( aPosition );
    editor->DrawDeferred();
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::UpdateTitleBarL( TBool aFromField )
    {
    if ( aFromField )
        {
        TBuf<KPosLmMaxTextFieldLength> name;

        CEikEdwin* nameEditor =
            static_cast<CEikEdwin*>( Control( ELandmarkNameField ) );
        nameEditor->GetText( name );
        iTitlePane->SetTextL( name );
        }
    else
        {
        TPtrC ptr;
        iLandmark->GetLandmarkName( ptr );
        iTitlePane->SetTextL( ptr );
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::ProcessErrorsInFormL(
    CLandmarksEditDialog::TErrorInForm aErrorId )
    {
    switch ( aErrorId )
        {
        case ENoErrors:
            break;
        case EWrongLatitude:
            NotifyErrorToUserL( R_LMREFAPP_LATITUDE_ERROR );
            break;
        case EWrongLongitude:
            NotifyErrorToUserL( R_LMREFAPP_LONGITUDE_ERROR );
            break;
        case EWrongAltitude:
            NotifyErrorToUserL( R_LMREFAPP_ALTITUDE_ERROR );
            break;
        case EWrongHorAcc:
            NotifyErrorToUserL( R_LMREFAPP_HOR_ACC_ERROR );
            break;
        case EWrongVerAcc:
            NotifyErrorToUserL( R_LMREFAPP_VER_ACC_ERROR );
            break;
        case EWrongRadius:
            NotifyErrorToUserL( R_LMREFAPP_RADIUS_ERROR );
            break;
        case EOnlyLatOrLon:
            NotifyErrorToUserL( R_LMREFAPP_LAT_LONG_ERROR );
            break;
        case ENoLatLonButHorAcc:
            NotifyErrorToUserL( R_LMREFAPP_HACC_LAT_LONG_ERROR );
            break;
        case ENoLatLonButAltitude:
            NotifyErrorToUserL( R_LMREFAPP_ALT_LAT_LONG_ERROR );
            break;
        case ENoAltButVerAcc:
            NotifyErrorToUserL( R_LMREFAPP_VACC_ALT_ERROR );
            break;
        case EEmptyLandmarkName:
            NotifyErrorToUserL( R_LMREFAPP_EMPTY_LANDMARK_NAME_ERROR );
            break;
        default:
            LandmarksUtils::Panic( KErrGeneral );
            break;
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::NotifyErrorToUserL( TInt aResourceId )
    {
    HBufC* msg = iCoeEnv->AllocReadResourceLC( aResourceId );
    LandmarksUtils::ErrorNoteL( *msg );
    CleanupStack::PopAndDestroy( msg );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::EditCategoriesL()
    {
    TInt res = CLandmarksCategoriesDialog::RunDialogL( *iLmCategoriesModel );
    if ( res )
        {
        ConfigureCategoryFieldL();
        DrawDeferred();
        SetChangesPending( ETrue );
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::UpdateToCurrentLocationL()
    {
    // Update location fields to current location
    HBufC* title =
        iCoeEnv->AllocReadResourceAsDes16LC( R_LMREFAPP_ACQUIRING_LOC );
    iPositionRequest->FetchPositionInfoL( *title, iPositionInfo );
    CleanupStack::PopAndDestroy( title );
    }

// ---------------------------------------------------------------------------
//  Copies address information ( address and building position fields ) from
//  source landmark to target.
// ---------------------------------------------------------------------------
//
void CLandmarksEditDialog::CopyAddressDataL( CPosLandmark& aSrc, CPosLandmark& aTrg )
    {
    // clean address fields
    for ( TPositionFieldId fieldId = EPositionFieldAddressCapabilitiesBegin + 1;
          fieldId < EPositionFieldNMEACapabilitiesBegin;
          fieldId++ )
        {
        aTrg.RemovePositionField( fieldId );
        }

    // copy fields
    TPositionFieldId fieldId = aSrc.FirstPositionFieldId();
    while ( fieldId != EPositionFieldNone )
        {
        if ( fieldId > EPositionFieldAddressCapabilitiesBegin &&
             fieldId < EPositionFieldNMEACapabilitiesBegin )
            {
            TPtrC field;
            if ( KErrNone == aSrc.GetPositionField( fieldId, field ) )
                {
                aTrg.SetPositionFieldL( fieldId, field );
                }
            }
        fieldId = aSrc.NextPositionFieldId( fieldId );
        }
    }

// ---------------------------------------------------------------------------
//  Copies coordinate information ( locality and coverage ) from
//  source landmark to target. If some fields are empty in source,
//  they will also be emptied in result.
// ---------------------------------------------------------------------------
//
void CLandmarksEditDialog::CopyCoordinateDataL( CPosLandmark& aSrc, CPosLandmark& aTrg )
    {
    TLocality loc;
    if ( KErrNone == aSrc.GetPosition( loc ) )
        {
        aTrg.SetPositionL( loc );
        }
    else
        {
        aTrg.RemoveLandmarkAttributes( CPosLandmark::EPosition );
        }

    TRealX nan;
    nan.SetNaN();
    TReal32 coverage( nan );

    aSrc.GetCoverageRadius( coverage );
    aTrg.SetCoverageRadius( coverage );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TBool CLandmarksEditDialog::HasAddressData()
    {
    // check only those fields, which are shown to user
    const TPositionFieldId fieldIds[] =
        {
        EPositionFieldCountry,
        EPositionFieldCity,
        EPositionFieldStreet,
        EPositionFieldPostalCode,
        };
    const TInt fields = 4;

    for ( TInt i = 0; i < fields; i++ )
        {
        TPtrC field;
        TInt err = iLandmark->GetPositionField( fieldIds[i], field );
        if ( !err && field.Length() )
            {
            return ETrue;
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
TBool CLandmarksEditDialog::HasCoordinateData()
    {
    TLocality loc;
    return ( KErrNone == iLandmark->GetPosition( loc ) );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::StartWaitDialogL( TInt aResId )
    {
    if ( iWaitDialog )
        {
        User::Leave( KErrInUse );
        }
        
    iWaitDialog = new (ELeave) CAknWaitDialog( 
        ( reinterpret_cast<CEikDialog**>( &iWaitDialog ) ) ); 
        
    iWaitDialog->SetTone( CAknNoteDialog::EConfirmationTone ); 
    iWaitDialog->ExecuteLD( aResId );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::CloseWaitDialog()
    {
    if ( iWaitDialog )
        {
        TRAP_IGNORE( iWaitDialog->ProcessFinishedL() );
        iWaitDialog = NULL;
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void CLandmarksEditDialog::ResetCoordinatesL()
    {
    TPosition position;
    ConfigureLocationFieldsL( position );

    SetChangesPending( ETrue );

    // Indicate that editors have been updated
    for ( TInt i = ELatitudeEditor; i <= EVerticalAccEditor; i++ )
        {
        iIsRealValueEdited[i] = ETrue;
        }
    }

