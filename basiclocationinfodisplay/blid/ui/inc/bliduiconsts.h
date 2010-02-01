/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*     Contains common constants
*
*/


#ifndef __BLID_BLIDCONSTS_H__
#define __BLID_BLIDCONSTS_H__

#include <data_caging_path_literals.hrh>

// CONSTANTS

// These own literals are used because platform's literals are insufficient:
_LIT( KBlidSystemMbmPath, "\\resource\\apps\\" ); // KDC_APP_BITMAP_DIR
// Filename of bitmaps
_LIT( KBlidBitmapFileName, "Blid.mbm" );

// Blid UI's constants
const TInt KZeroResourceId(0);

const TUint KAsterixKey( 42 );
const TUint KNegativeSignKey( 45 );

// listbox strings lenght
const TInt KBlidLBItemMaxLen( 50 );
const TInt KBlidLBLineMaxLen( 100 );

// CStreamDictionary ID
const TUid KBlidUI = { 0x101F85A0 };

//distance converters
const TReal KMetreToFoot( 3.280839895013123 );
const TReal KKilometreToMiles( 0.6213711922373338 );

// size of arrays
const TInt KNumberOfTriangePoints( 3 );
const TInt KNumberOfSquarePoints( 4 );
const TInt KNumberOfCardinalPoints( 4 );

// Compass's consts
const TInt KRadiusOfCircle( 47 );
const TInt KRightAngle( 90 );
const TReal KDivisor( 2.0 );
const TInt KSquared( 2 );
const TInt KRound( 1 );

// dataControl's consts
const TInt KDayToSeconds( 86400 );
const TInt KHourToSeconds( 3600 );
const TInt KMinuteToSeconds( 60 );
const TInt KDecimals3( 3 );
const TInt KMaxDestinationDist(10);
const TInt KMaxPostiontAccuracy(50);

// compassControl's consts
const TInt KBlidDirectionPaneWidth( 16 );
const TInt KBlidDirectionPaneHeight( 16 );
const TInt KBlidDirPaneCenterWidth( 8 );
const TInt KBlidDirPaneCenterHeight( 8 );

const TInt KAccuracyLevel1( 1000 );
const TInt KAccuracyLevel2( 500 );
const TInt KAccuracyLevel3( 250 );
const TInt KAccuracyLevel4( 100 );
const TInt KAccuracyLevel5( 50 );
const TInt KAccuracyLevel6( 10 );
const TInt KAccuracyLevel7( 0 );

const TInt KKilometerToMeters( 1000 );
const TInt KMileToFeets( 5280 );
const TInt KDayToHours( 24 );
const TReal KSpeedConvertion( 3.6 ); // m/s => km/h
const TInt KBlidPoorAccuracy(200); // Poor Landmark accuracy of 200m

//////////////////////////////////////////
// Blid engine's constants
//////////////////////////////////////////
const TInt KIntervalTime( 3000000 ); //3s
const TInt KTimeOutRate( 5 );
const TInt KMinArrivalTime(30000000); // Minumum time untill which arrived note to be displayed

const TInt KNotesTimeOut( 30000000 ); //30 seconds

//Coordinate Editors contants
const TInt KMaxLongitude(180);

const TInt KBlidTripSpeedTimeOut(10); //10 seconds

// Identifier for the primary control group.
const TInt KPrimaryTripGroup( 2 );

// Identifier for the primary control group.
const TInt KPrimaryGroup( 1 );

// Identifier for the primary control group.
const TInt KBlidMainGroup( 3 );

// Identifier for the primary control group.
const TInt KBlidMainSatelliteGroup( 4 );

// Path to find the mbm file
_LIT( KBlidSystemIconPath, ":\\resource\\apps\\blid.mif" );

// Number of Icons in Flag Animation
const TInt KNumOfFlagIcon = 5;
const TInt KNumOfStartUpIcon = 8;
const TInt KNumOfSearchIcon = 8;

#endif // __BLID_BLIDCONSTS_H__

// End of File
