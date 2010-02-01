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
* Description:  Provides SavedWayptsView's details
*
*/

#ifndef CBLIDSAVEDWAYPTSLBMODEL_H
#define CBLIDSAVEDWAYPTSLBMODEL_H

// INCLUDES
#include <e32base.h>    // CBase
#include <bamdesca.h>  // MDesCArray

// FORWARD DECLARATIONS
class MBlidLocation;
class MBlidSettings;
class MBlidRouter;
class TPosition;
class TCourse;
// CLASS DECLARATION

/**
*  CBlidSavedWayptsLBModel
*
*/
class CBlidSavedWayptsLBModel : public CBase, public MDesCArray
    {
    public: // Constructors and destructor

        /**
         * Two-phased constructor.
         * @param aLocationModel A pointer to Location model
         * @param aSettingsModel A pointer to Blid settings API
         * @return CBlidEng object.
         */
		static CBlidSavedWayptsLBModel* NewLC( MBlidLocation* aLocationModel,
                                          MBlidSettings* aSettings,MBlidRouter* aRouter );

        /**
         * Destructor.
         */
        ~CBlidSavedWayptsLBModel();

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
        CBlidSavedWayptsLBModel( MBlidLocation* aLocationModel,
                            MBlidSettings* aSettings,MBlidRouter* aRouter );

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
         * Format speed string
         * @param aSpeedString
         * @param aCourse
         */
        void SpeedFormat( TDes& aSpeedString,
                          TCourse aCourse ) const;

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

        // Own: No Data indicator
        HBufC* iNoData;

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
        
        /// Own: Latitude's format text string
        HBufC* iLatitudeFormatNorth;

        /// Own: Latitude's format text string
        HBufC* iLatitudeFormatSouth;

        /// Own: Longitude's format text string
        HBufC* iLongitudeFormatWest;

        /// Own: Longitude's format text string
        HBufC* iLongitudeFormatEast;

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
        HBufC* iAltAccuracyFormat;

        /// Own: Heading's format text string
        HBufC* iDataBeingCalculated;

        /// Own: A Pointer to TLocale
        TLocale* iLocale;

        ///Pointer to router model
        MBlidRouter* iRouter;
    };

#endif // CBLIDSAVEDWAYPTSLBMODEL_H

// End of File
