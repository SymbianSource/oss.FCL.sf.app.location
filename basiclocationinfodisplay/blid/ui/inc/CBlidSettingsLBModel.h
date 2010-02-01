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
* Description:  Provides Blid request detail view listbox model.
*
*/

#ifndef CBLIDSETTINGSLBMODEL_H
#define CBLIDSETTINGSLBMODEL_H

// INCLUDES
#include <e32base.h>    // CBase
#include <bamdesca.h>  // MDesCArray


// FORWARD DECLARATIONS
class MBlidSettings;
// CLASS DECLARATION

/**
*  CBlidSettingsLBModel
*
*/
class CBlidSettingsLBModel : public CBase, public MDesCArray
    {
    public: // Constructors and destructor
        /**
         * Two-phased constructor.
         * @param aSettingsModel A pointer to Blid settings API
         * @return created object
         */
		static CBlidSettingsLBModel* NewLC( MBlidSettings* aSettingsModel);

        /**
         * Destructor.
         */
        ~CBlidSettingsLBModel();

    private: 
        /**
         * Second phase of construction.
         */
        void ConstructL();

        /**
         * c++ default constructor.
         * @param aSettingsModel A pointer to Blid settings API
         */
        CBlidSettingsLBModel( MBlidSettings* aSettingsModel );

    private: // From MDesCArray
        TInt MdcaCount() const;
        TPtrC16 MdcaPoint(TInt aIndex) const;

    private: // new functions
        /**
         * Format calibration string
         * @param aCalibrationString contains the calibration value given by user
         */
        void CalibrationFormat( TDes& aCalibrationString ) const;

        /**
         * Format Backlight string
         * @param aBacklightString Contains the selected option string
         */
        void BacklightFormat( TDes& aBacklightString ) const;
        
        void ArrivalToneFormat( TDes& aRrivalTone ) const;

    private: //data
        /// Own: conversion buffer
        HBufC* iBuffer;

        /// Ref: Blid settings model API
        MBlidSettings* iSettingsModel;

        /// Own: Latitude functionality text string
        HBufC* iBlidAltitude;

        /// Own: Altitude functionality text string
        HBufC* iAltitudeFormatM;

        /// Own: Altitude functionality text string
        HBufC* iAltitudeFormatFt;

        /// Own: A pointer to TLocale
        TLocale* iLocale;
        
        /// Own: Settings view backlight text string
        HBufC* iBlidBacklight;

        /// Own: Settings view backlight selection normal text 
	    HBufC* iBlidBacklightNormal;
	
    	/// Own: Settings view backlight selection On text 
    	HBufC* iBlidBacklightOn;

	    HBufC* iBlidArrivalTone;
	
    	HBufC* iBlidResetOdom;
    	
    	HBufC* iBlidArrivalToneOff;
        
        HBufC* iBlidArrivalToneName;
        //Environment Variable
        CEikonEnv* iEnv;
    };

#endif //CBLIDSETTINGSLBMODEL_H 
// End of File

