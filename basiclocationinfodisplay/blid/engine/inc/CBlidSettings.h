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
* Description:  Provides access to Blid settings.
*
*/



#ifndef CBLIDSETTINGS_H
#define CBLIDSETTINGS_H

//  INCLUDES
#include <e32base.h>
#include "MBlidSettings.h"
#include "MBlidMeasurementSettings.h"

/// FORWARD DECLARATION
class MBlidEngObserver;
class CBlidMeasurementSettingListener;
class CRepository;

// CLASS DECLARATION

/**
*  A Blid engine API for blid application.
*  Provides access to blid settings.
*  This class implements the MBlidSettings interface provided by this dll.
*
*/
class CBlidSettings : public CBase, public MBlidSettings,public MBlidMeasurementSettings
    {
    public:  // Constructors and destructor
        /**
         * Two-phased constructor.
         */
        static CBlidSettings* NewL();

        /**
         * Destructor.
         */
        virtual ~CBlidSettings();

    private:
        /**
         * C++ default constructor.
         */
        CBlidSettings();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private: // From MBlidSettings
        void SetAltCalibration( const TReal32 aAltCalibration );
        TReal32 AltCalibration();
        TReal32 UnitOfMeasurement();
        void SetBacklightOption( const TReal32 aBacklightOption );
        TReal32 GetBacklightOption();
        void SetArrivedToneName( const TDesC8& aArrivedToneName );
        TDesC8& GetArrivedToneName();
        void SetResetOdometerOption( const TReal32 aIsOdoReset );
        TReal32 GetResetOdometerOption();
        void SetArrivedDistance( TReal32 aDistance );
        TReal32 GetArrivedDistance( );
        
    private: //From MBlidMeasurementSettings
        void HandleSysMeasureValueL( );        

    private:        
        // Own: altitude calibration
        TReal32 iAltCalibration;
        
        //Own:  Measurement Unit
        TReal32 iMeasurementUnit;
        
        //Own:  Backlight Settings Option
        TReal32 iBacklightOption;

		TReal32	iIsOdoReset;
		
		HBufC8*	iArrivedToneName;
		/**
		 * System of measurements settings handle
		 * Owns
		 */ 
        CBlidMeasurementSettingListener*    iSysofMeasSettings;
        
        TReal32 iArrivedDistance;
        
        CRepository* iRepository;
    };

#endif      // CBLIDSETTINGS_H

// End of File
