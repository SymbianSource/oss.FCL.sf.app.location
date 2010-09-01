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


#ifndef BLID_ENG_CONSTS_H__
#define BLID_ENG_CONSTS_H__

#include <data_caging_path_literals.hrh>
#include "blidcommonconsts.h"

//////////////////////////////////////////
// Blid engine's constants
//////////////////////////////////////////
const TInt KStartIntervalTime( 5000000 ); //1s
const TInt KMinArrivalDist(10); // Minimum arrival distance 10 meters
const TInt KMaxHorAccLimit(50); // Maximum horzontal accuracy 50 meters

const TInt KMaxItemsInArray( 5 );
const TInt KStartUpdateIntervalTimeOut( 5000001 ); //5 seconds

const TInt KManualWaypointIndex(KMaxNumberOfWaypoints + 1);

// Blid's user data document versions.
const TInt KBlidDocVersion1( 1 );
const TInt KBlidHundredMeters( 100 );    //100 m
const TInt KBlidHundredOneMeters( 101 ); //101 m
const TInt KBlidOneKilometer( 1000 );    //1 km

const TReal KBlidCutOffSpeed(0.8); 

//////////////////////////////////////////
// Blid engine's constants for 
// Update Intervals
//////////////////////////////////////////
const TInt KShortIntervalTime( 1000000 ); //1s
const TInt KMidIntervalTime( 3000000 ); //3s
const TInt KMaxIntervalTime( 5000000 ); //3s

const TInt KBlidLeaveExit( -1003 );

const TInt KMaxSatelliteSignalStrength( 40 );

const TInt KMidSatelliteSignalStrength( 20 );

const TInt KMinSatelliteSignalStrength( 5 );


#endif // BLID_ENG_CONSTS_H__

// End of File
