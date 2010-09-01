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
* Description:  Blid application utils class definition
*
*/


#ifndef BLIDUTILS_H
#define BLIDUTILS_H

// INCLUDES
#include <e32std.h>

// CLASS DECLARATION
/**
* BlidUtils application class.
* Convert seconds to time string
* Convert seconds to days
* Formating distance string
* Find BLID application bitmap file
*/
class BlidUtils
    {

    public: // new functions
        /**
         * Convert TReal to TInt
         * @param aSrc
         * @param aResult
         * @return error code
         */
        static TInt TRealToTInt( const TReal aSrc, TInt& aResult );

        /**
         * Convert seconds to time string
         * If the resulting length of text in this descriptor 
         * exceeds its maximum length, then the function raises
         * a USER 11 panic.
         * @param aTimeString
         * @param aSeconds
         * @return error code
         */
        static TInt FormatStringToTime( TDes& aTimeString,
                                        const TReal aSeconds );

        /** 
         * Convert seconds to days
         * @param aSrc Time in seconsds 
         * @param aResult amount of days
         * @return error code
         */
        static TInt SecondsToDays( const TReal aSrc, TInt& aResult );

        /**
         * Formating distance string
         * If the resulting length of text in this descriptor
         * exceeds its maximum length, then the function raises 
         * a USER 11 panic.
         * @param aDistanceString
         * @param aDistance
         */
        static void DistanceFormat( TDes& aDistanceString,
                                    const TReal aDistance,
                                    TBool aIstobeDecimal = EFalse );

        /** 
         * Find BLID application bitmap file
         * @return full filename ( path + name )
         */
        static HBufC* GetBitmapFilenameLC();
        
        /**
         * Convert seconds to time string
         * If the resulting length of text in this descriptor 
         * exceeds its maximum length, then the function raises
         * a USER 11 panic.
         * @param aTimeString
         * @param aSeconds
         * @return error code
         */
        static TInt FormatTimeToString( TDes& aTimeString,
                                        const TReal aSeconds );


    };

#endif // BLIDUTILS_H

// End of File
