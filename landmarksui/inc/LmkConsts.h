/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:    LandmarksUi Content File -
*     Contains common constants
*
*/







#ifndef LMKCONSTS_H
#define LMKCONSTS_H

#include <lmkui.mbg>
#include <landmarks.mbg>
#include <data_caging_path_literals.hrh>
#include <landmarks.loc>


// CONSTANTS
_LIT( KLmkTmp_RESOURCE_FILES_DIR, "\\resource\\"); // KDC_RESOURCE_FILES_DIR
// Following own literals are used because platform's literals are insufficient:
_LIT( KLmkSystemMbmPath, "\\resource\\apps\\" ); // == KDC_APP_BITMAP_DIR
_LIT(KLmkEncodedDataFileName, "EncodedLmData" );
_LIT(KLmkEncodedDataFileExtension, ".lmx");
_LIT( KLmkUiIconFileName, "LmkUi.mif" );
_LIT( KLandmarksIconFileName, "Landmarks.mif" );
_LIT( KLmkUiRscFileName, "LmkUi.rsc" );

//used for url field to identify protocol present or not
_LIT( KProtocol, ":" );
//used as default protocol for url field.
_LIT(KHttp, "http://");




//default icon id
const TUint KLmkDefaultId(EMbmLmkuiQgn_prop_lm_gene);
const TUint KCategoryDefaultId(EMbmLmkuiQgn_prop_lm_gene);

// Icon ID defines for generic IconIndex and IconMaskIndex

const TUint KLmkGenericIconIndex( EMbmLmkuiQgn_prop_lm_gene );
const TUint KLmkGenericIconMaskIndex ( 	EMbmLmkuiQgn_prop_lm_gene_mask );

// default granularity for array containing marked item indexes:
const TInt KLmkMultiSelectionGranularity( 10 );

// default granularity of landmark or category array:
const TInt KLmItemArrayGranularity( 100 );

//Zero tab
const TInt KZeroResourceId( 0 );

const TUid KLmkEventListenerSingletonUid = { 0x1 };

//location request timeout
const TInt KLmkLRTimeOut( 30000000 ); //30s

//distance converters
const TInt  KKilometerToMeter( 1000 );
const TReal KMileToMeter( 1609.344 );
const TReal KFeetToMeter( 0.3048 );
const TReal KMetreToFoot( 3.280839895013123 );
const TInt  KMileToFeet( 5280 );

// Max and Min Limits Number Editor values

const TInt KMaxLatitude = 90;
const TInt KMinLatitude = -90;
const TInt KMaxLongitude = 180;
const TInt KMinLongitude = -180;

const TInt KMaxAltitude = 20000; // Max altitude 20000m
const TInt KMinAltitude = -500;  // Min altitude -500m

const TInt KMaxAccuracy = 20000;
const TInt KMinAccuracy = 0;

//Added for Package Name
const TInt KLmkPackageNameMaxLen(127);

// Invalid List Index
const TInt KLmkInValidIndex (-1);

const TInt KMaxCategoryNameFieldLen( 150 );

const TInt KMaxDescriptionFieldLen (250);
const TInt KMaxPhoneNumberFieldLen (48);
const TInt KMaxUrlFieldLen (150);
const TInt KMaxBufferLen (255);
const TInt KMaxUrlFieldLenWithDefaultProtocol(143);
const TInt KMskCommandPos (3);
#endif // LMKCONSTS_H

// End of File
