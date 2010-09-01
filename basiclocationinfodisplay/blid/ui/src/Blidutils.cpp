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
* Description:  Provides blid utils class methods.
*
*/


// INCLUDE FILES
#include <StringLoader.h> 
#include <e32math.h>
#include <f32file.h>
#include <eikenv.h>
#include <aknViewAppUi.h>
#include <apparc.h>
#include <eikapp.h>
#include "bliduiconsts.h"
#include "Blidutils.h"

//CONSTANTS
_LIT( KTimeFormatString,":%02d%02d%02d." );
_LIT( KDistanceFormatTReal1, "%.1f" );
_LIT( KDistanceFormatInt, "%d" );


// ================= MEMBER FUNCTIONS =======================
// ----------------------------------------------------------------------------
// BlidUtils::TRealToTInt
// ----------------------------------------------------------------------------
//
TInt BlidUtils::TRealToTInt( const TReal aSrc, TInt& aResult )
    {
    TReal resultTReal(0);    
    TInt error(0);

    error = Math::Round( resultTReal, aSrc, 0 );
    if ( error != KErrNone )
    	{
        aResult = 0;
        return error;
        }

    TInt32 resultTInt(0);
    error = Math::Int( resultTInt, resultTReal );  
    if ( error != KErrNone )
        {        
        aResult = 0;
        return error;
        }    
    aResult = resultTInt;
    return error;
    }

// ----------------------------------------------------------------------------
// BlidUtils::FormatStringToTime
// ----------------------------------------------------------------------------
//
TInt BlidUtils::FormatStringToTime( TDes& aTimeString,
                                        const TReal aSeconds )
    {    
    TInt tempInt(0);
    TReal tempTReal(0);
    TInt32 timeHours(0);
    TInt32 timeMinutes(0);
    TInt32 timeSeconds(0);
    TInt error(0);
    TInt seconds;

    error = TRealToTInt( aSeconds, seconds );
    if ( error != KErrNone )
        {
        aTimeString.Zero();
        return error;
        }
    
    if ( (seconds < KDayToSeconds) && (seconds >= 0)) // less than day
        {
        // calculate hours
        error = Math::Int(timeHours,( seconds / KHourToSeconds ));    
        if ( error != KErrNone )
            {
            aTimeString.Zero();
            return error;
            }

        // calculate minutes
        tempInt = seconds - (timeHours*KHourToSeconds);
        error = Math::Int(timeMinutes, ( tempInt / KMinuteToSeconds ));
        if ( error != KErrNone )
            {
            aTimeString.Zero();
            return error;
            }

        // calculate seconds
        error = Math::Mod( tempTReal, seconds, KMinuteToSeconds );
        if ( error != KErrNone )
            {
            aTimeString.Zero();
            return error;
            }
        
        // convert TReal to TInt
        error = Math::Int( timeSeconds, tempTReal );
        if ( error != KErrNone )
            {
            aTimeString.Zero();
            return error;
            }

        }
    
    aTimeString.Format( KTimeFormatString, 
	                    timeHours, 
	                    timeMinutes, 
	                    timeSeconds );    
    return error;
    }

// ----------------------------------------------------------------------------
// BlidUtils::SecondsToDays
// ----------------------------------------------------------------------------
//
TInt BlidUtils::SecondsToDays( const TReal aSrc, TInt& aResult )
    {        
    TInt error(0);
    TInt seconds;
    
    error = TRealToTInt( aSrc, seconds );
    if ( error != KErrNone )
        {
        aResult = 0;
        return error;
        }

    TInt32 days(0);
    error = Math::Int( days, (seconds / KDayToSeconds) );
    if ( error != KErrNone )
    	{
        aResult = 0;
        return error;
        }
    aResult = days;
    return error;
    }

// ----------------------------------------------------------------------------
// BlidUtils::DistanceFormat
// ----------------------------------------------------------------------------
//
void BlidUtils::DistanceFormat( TDes& aDistanceString,
                                const TReal aDistance,
                                TBool aIstobeDecimal )
    {
    TReal distance( aDistance );
    
    if( aIstobeDecimal )
	    {
	    aDistanceString.Format( KDistanceFormatTReal1, distance );	
	    }
	else
		{
		aDistanceString.Format( KDistanceFormatInt, ( TInt )distance );	
		}	    
    }

// ----------------------------------------------------------------------------
// BlidUtils::GetBitmapFilenameL
// ----------------------------------------------------------------------------
//
HBufC* BlidUtils::GetBitmapFilenameLC()
    {
    // Path and file name:
    TFileName* file = new( ELeave ) TFileName;
    CleanupStack::PushL( file );
    file->Append( KBlidSystemMbmPath ); // always safe
    file->Append( KBlidBitmapFileName ); // always safe   
    
    // Drive:    
    TBuf<10> aApp;
    CEikAppUi *appUi = (CEikAppUi *)(CEikonEnv::Static()->AppUi());
    TFileName full = appUi->Application()->AppFullName();
        
    TParsePtr ptr(full); 
    aApp.Copy(ptr.Drive()); 


    // Add drive to path & file name:
    TParse parse;
    User::LeaveIfError( parse.Set( *file, &aApp, NULL ) );
    CleanupStack::PopAndDestroy(); // file

    return parse.FullName().AllocLC();
    }
    
// ----------------------------------------------------------------------------
// BlidUtils::FormatTimeToString
// ----------------------------------------------------------------------------
//
TInt BlidUtils::FormatTimeToString( TDes& aTimeString,
                                        const TReal aSeconds )
    {    
    TInt tempInt(0);
    TReal tempTReal(0);
    TInt32 timeHours(0);
    TInt32 timeMinutes(0);
    TInt32 timeSeconds(0);
    TInt error(0);
    TInt seconds;

    error = TRealToTInt( aSeconds, seconds );
    if ( error != KErrNone )
        {
        aTimeString.Zero();
        return error;
        }
    
    if (seconds >= 0) 
        {
        // calculate hours
        error = Math::Int(timeHours,( seconds / KHourToSeconds ));    
        if ( error != KErrNone )
            {
            aTimeString.Zero();
            return error;
            }

        // calculate minutes
        tempInt = seconds - (timeHours*KHourToSeconds);
        error = Math::Int(timeMinutes, ( tempInt / KMinuteToSeconds ));
        if ( error != KErrNone )
            {
            aTimeString.Zero();
            return error;
            }

        // calculate seconds
        error = Math::Mod( tempTReal, seconds, KMinuteToSeconds );
        if ( error != KErrNone )
            {
            aTimeString.Zero();
            return error;
            }
        
        // convert TReal to TInt
        error = Math::Int( timeSeconds, tempTReal );
        if ( error != KErrNone )
            {
            aTimeString.Zero();
            return error;
            }

        }
    
    aTimeString.Format( KTimeFormatString, 
	                    timeHours, 
	                    timeMinutes, 
	                    timeSeconds );    
    return error;
    }


// End of File
