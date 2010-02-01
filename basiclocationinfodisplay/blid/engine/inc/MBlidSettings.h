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
* Description:  Blid settings can be modified
*                and read with help of this API.
*
*/



#ifndef __MBLIDSETTINGS_H__
#define __MBLIDSETTINGS_H__

// CLASS DECLARATION

/**
*  This Blid application engine API is only used by Blid application.
*  Blid settings can be modified and read with help of this API.
*
*/
class MBlidSettings
    {
    public:  // Constructors and destructor
        /**
         * Destructor.
         */
        virtual ~MBlidSettings(){};

    public: // New functions
        /**
         * Set altitude calibration 
         *
         * @param aAltCalibration
         */
        virtual void SetAltCalibration( const TReal32 aAltCalibration ) = 0;
        
        /**
         * Get altitude calibration
         *
         * @return altitude calibration
         */
        virtual TReal32 AltCalibration() = 0;
        
        /**
         * Get Unit of Measurement 
         *
         * @return measurement unit 
         */
        virtual TReal32 UnitOfMeasurement() = 0;
        
        /**
         * Set Index of selected Backlight settings option
         *
         * @param aBacklightOption
         */
        virtual void SetBacklightOption( const TReal32 aBacklightOption ) = 0;
        
        /**
         * Get backlight option selected index
         *
         * @return backlight option selected index 
         */
        virtual TReal32 GetBacklightOption() = 0;        
        
        virtual void SetArrivedToneName( const TDesC8& aArrivedToneName ) = 0;
        virtual TDesC8& GetArrivedToneName() = 0;
        virtual void SetResetOdometerOption( const TReal32 aIsOdoReset ) = 0;
        virtual TReal32 GetResetOdometerOption() = 0;
        

        /**
         * Set the arrived distance
         */
        virtual void SetArrivedDistance( TReal32 aDistance )=0 ;

        /**
         * Get the arrived distance
         */
        virtual TReal32 GetArrivedDistance( ) = 0 ;
        
        
    };

#endif      // __MBLIDSETTINGS_H__

// End of File
