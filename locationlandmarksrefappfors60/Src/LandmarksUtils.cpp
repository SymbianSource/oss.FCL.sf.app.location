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
* Description:  Implements the CLandmarksViewDialog class
*
*/



#include <e32std.h>
#include <e32math.h>

#include <aknnotewrappers.h>

#include "LandmarksUtils.h"
#include "LandmarksCommonData.h"

// ============================= STATIC FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void LandmarksUtils::FloatToDes(TReal aFloat, TDes& aDes, TRealFormat aRealFormat)
    {
    if (Math::IsNaN(aFloat))
        {
        aDes = KNullDesC;
        }
    else
        {
        aDes.Num(aFloat, aRealFormat);
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void LandmarksUtils::LatLongToDes(TReal aFloat, TDes& aDes)
    {
    if (Math::IsNaN(aFloat))
        {
        aDes = KNullDesC;
        }
    else
        {
        aDes.Format(KLatLongFormat, aFloat);
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void LandmarksUtils::LatLongFloatToDegMinSecDesL(TReal aLatLong, TDes& aDes)
    {
    if (!Math::IsNaN(aLatLong))
        {
        // Convert to deg min sec
        SDegMinSec degMinSec;

        // Fetch degrees
        User::LeaveIfError(Math::Int(degMinSec.iDegrees, Abs(aLatLong)));

        // Fetch minutes
        TReal minutes = (Abs(aLatLong) - degMinSec.iDegrees) * KMinutesPerDegree;
        User::LeaveIfError(Math::Int(degMinSec.iMinutes, minutes));

        // Fetch seconds
        degMinSec.iSeconds = 
            ((minutes - degMinSec.iMinutes) * KSecondsPerMinute);

        // Check if seconds is going to be rounded to 60
        const TReal KMaxSeconds = 59.999;
        if (degMinSec.iSeconds > KMaxSeconds)
            {
            degMinSec.iSeconds = 0;
            degMinSec.iMinutes++;
            if (degMinSec.iMinutes == KMinutesPerDegree)
                {
                degMinSec.iMinutes = 0;
                degMinSec.iDegrees++;
                // We do not need to check if degress > 90 or if degrees > 180.
                // Landmarks FW takes care of that.
                }
            }

        // Format output descriptor
        if (aLatLong >= 0)
            {
            aDes.Format(KListBoxLatLongFormat, &KPositiveSign, 
                degMinSec.iDegrees, KDegreeSign, degMinSec.iMinutes, 
                degMinSec.iSeconds);
            }
        else
            {
            aDes.Format(KListBoxLatLongFormat, &KNegativeSign, 
                Abs(degMinSec.iDegrees), KDegreeSign, Abs(degMinSec.iMinutes), 
                Abs(degMinSec.iSeconds));
            }
        }
    else
        {
        aDes.Zero();
        }
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void LandmarksUtils::Panic(TInt aReason)
    {
    _LIT(KPanic, "LmRefApp bug");
    User::Panic(KPanic, aReason);
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void LandmarksUtils::ErrorNoteL( const TDesC& aErrorText )
    {
    CAknErrorNote* errorNote = new (ELeave) CAknErrorNote( ETrue );
    errorNote->ExecuteLD( aErrorText );
    }

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//
void LandmarksUtils::InfoNoteL( const TDesC& aInfoText )
    {
    CAknInformationNote* infoNote = new (ELeave) CAknInformationNote( ETrue );
    infoNote->ExecuteLD( aInfoText );
    }
