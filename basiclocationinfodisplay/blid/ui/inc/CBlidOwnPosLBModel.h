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
* Description:  Provides OwnPosView's details
*
*/

#ifndef CBLIDOWNPOSLBMODEL_H
#define CBLIDOWNPOSLBMODEL_H

// INCLUDES
#include <e32base.h>    // CBase
#include <bamdesca.h>  // MDesCArray


// FORWARD DECLARATIONS
class MBlidLocation;
class MBlidSettings;
class TPosition;
class TCourse;
// CLASS DECLARATION

/**
*  CBlidOwnPosLBModel
*
*/
class CBlidOwnPosLBModel : public CBase, public MDesCArray
    {
    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aLocationModel A pointer to Location model
         * @param aSettingsModel A pointer to Blid settings API
         * @return CBlidEng object.
         */
		static CBlidOwnPosLBModel* NewLC( MBlidLocation* aLocationModel,
                                          MBlidSettings* aSettings );

        /**
         * Destructor.
         */
        ~CBlidOwnPosLBModel();

    private: //
        /**
         * Second phase of construction.
         */
        void ConstructL();

        /**
         * c++ default constructor.
         * @param aLocationModel A pointer to Location model
         * @param aSettingsModel A pointer to Blid settings API
         */
        CBlidOwnPosLBModel( MBlidLocation* aLocationModel,
                            MBlidSettings* aSettings );

    private: // From MDesCArray
        TInt MdcaCount() const;
        TPtrC16 MdcaPoint(TInt aIndex) const;

    private: //new functions
        /**
         * Format latitude string
         * @param aLatitudeString
         * @param aPosition
         */
        void LatitudeFormat( TDes& aLatitudeString,
                             TPosition aPosition ) const;

        /**
         * Format longitude string
         * @param aLongitudeString
         * @param aPosition
         */
        void LongitudeFormat( TDes& aLongitudeString,
                              TPosition aPosition  ) const;

        /**
         * Format accuracy string
         * @param aAccuracyString
         * @param aPosition
         */
        void AccuracyFormat( TDes& aAccuracyString,
                             TPosition aPosition ) const;

        /**
         * Format altitude string
         * @param aAltitudeString
         * @param aPosition
         */
        void AltitudeFormat( TDes& aAltitudeString,
                             TPosition aPosition  ) const;
        /**
         * Format Altitude accuracy string
         * @param aAccuracyString
         * @param aPosition
         */

        void altAccuracyFormat( TDes& aAltAccuracyString ) const;

    private: //data
        /// Own: conversion buffer
        HBufC* iBuffer;

        // Own: connect GPS indicator
        HBufC* iConnectGPS;

        /// Ref: Blid location model API
        MBlidLocation* iLocationModel;

        /// Ref: Blid settings model API
        MBlidSettings* iSettingsModel;

        /// Own: Latitude's title text string
        HBufC* iBlidLatitude;

        /// Own: Longitude's title text string
        HBufC* iBlidLongitude;

        /// Own: Accuracy's title text string
        HBufC* iBlidAccuracy;

        /// Own: Heading's title text string
        HBufC* iBlidAltAccuracy;

        /// Own: Speed's title text string
        HBufC* iBlidSpeed;

        /// Own: Altitude's title text string
        HBufC* iBlidAltitude;
        
        /// Own: Metric accuracy's format text string
        HBufC* iAccuraryFormatM;

        /// Own: Imperial accuracy's format text string
        HBufC* iAccuraryFormatFt;

        /// Own: Metric speed's format text string
        HBufC* iSpeedFormatKmph;

        /// Own: Imperial speed's format text string
        HBufC* iSpeedFormatMph;

        /// Own: Metric altitude's format text string
        HBufC* iAltitudeFormatM;

        /// Own: Imperial altitude's format text string
        HBufC* iAltitudeFormatFt;

        /// Own: Heading's format text string
        HBufC* iDataBeingCalculated;

        /// Own: A Pointer to TLocale
        TLocale* iLocale;
        
        // Own: Not Available indicator
        HBufC* iNotAvailable;
        
        /// Own: Data blank text string
        HBufC* iDataBlank;
        
        /// Own: Waiting GPS data text string
        HBufC* iWaitingGpsData;
    };

#endif // CBLIDOWNPOSLBMODEL_H

// End of File
