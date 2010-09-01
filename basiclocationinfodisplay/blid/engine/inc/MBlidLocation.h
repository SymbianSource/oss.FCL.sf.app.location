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
* Description:  Blid Location can be requested.
*
*/



#ifndef __MBLIDLOCATION_H__
#define __MBLIDLOCATION_H__

// INCLUDES
#include <lbspositioninfo.h>
#include <lbs.h>

// FORWARD DECLARATIONS
class MBlidEngObserver;
class TPosition;
class TCourse;
class RPositionServer;
// CLASS DECLARATION

/**
*  This Blid application engine API is only used by Blid application.
*  Blid location can be requested with help of this API.
*
*/
class MBlidLocation
    {
    public:  // Constructors and destructor
        /**
         * Destructor.
         */
        virtual ~MBlidLocation(){};

    public: // New functions
        /**
         * Sets location observer
		 *
         * @param aObserver A reference to observer
         */
        virtual void SetObserver(MBlidEngObserver& aObserver, TInt aViewId) = 0;

        /**
         * Removes location observer
         *
         */
        virtual void RemoveModelObserver() = 0;

        /**
         * Initialize location request, must call StartRequesting()
         * method after this.
         */
        virtual void InitialLocationRequestL( const TDesC& aSrvName ) = 0;

        /**
         * Get current position
         *
         */
        virtual TPosition& GetCurrentPosition() = 0;

        /**
         * Get position course info
         *
         */
        virtual TCourse& GetCourse() = 0;

        /**
         * Cancel request
         */
        virtual void CancelRequest() = 0;

        /**
         * Stop requesting
         */
        virtual void StopRequesting() = 0;

        /**
         * Start requesting
         */
        virtual void StartRequesting() = 0;

        /**
         * Is application online or offline mode
         * ETrue = online, EFalse = offline
         */
        virtual TBool IsOnline() = 0;

        /**
         * Is PSY capable of returning satellite information
         * ETrue if can, EFalse id can't
         */
        virtual TBool SatelliteCapability() = 0;

        /**
         * Return speed average
         * @return speed average
         */
        virtual TReal SpeedAverage() = 0;

        /**
         * Return status of position query
         * @return istatus
         */
        virtual TInt GetPositionInfoStatus() = 0;

        /**
         * Return status of the PSY
         * @return istatus
         */
        virtual void GetModuleInfoStatus(TPositionModuleStatusBase& aPosModuleStatus) = 0;

        /**
         * Returns number of satellites
         * @return istatus
         */
        virtual void GetNumOfSatellitesL(TInt& anumSatellites) = 0;
        
        /**
         * Return max speed
         * @returns max speed
         */
        virtual TReal MaxSpeed() = 0;

	    /**
         * Resets max speed variable
         * 
         */
        virtual void ResetMaxSpeed() = 0;

	    /**
         * Resets avg speed variable
         * 
         */
        virtual void ResetAvgSpeed()=0;
        
        /**
         * Set the trip start variable
         */
        virtual void SetTripStartL( )=0 ;
        
        /**
         * Return the iIsTripStart variable
         */
        virtual TBool HasTripStarted( )=0 ;
        
        /**
         * Set the trip stop variable
         */
        virtual void SetTripStop()=0 ;
        
        /**
         * Return the iIsTripNotStart variable
         */
        virtual TBool HasTripNotStarted( ) =0 ;

    	/**
    	 * All the display for the current trip are
    	 * reset
    	 */
         virtual void ResetTrip() = 0;

         virtual void GetTripTime(  TTimeIntervalSeconds& aTime )=0;  
            
         virtual void GetTotalTime( TTimeIntervalSeconds& aTime )=0;  
            
         virtual TBool IsTripDistanceTravelled()=0; 
            
         virtual TBool IsTotalDistanceTravelled() = 0; 
            
         virtual void GetTripDistance( TReal32& aDistance ) = 0; 
            
         virtual void GetTotalDistance( TReal32& aDistance ) = 0; 
            
         virtual void SetTripResumeL()=0;     
         
         /**
          * Set the trip clear variable
          */
         virtual void SetTripClear( ) = 0 ;
            
         /**
          * Return the iIsTripClear variable
          */
         virtual TBool IsTripCleared( ) = 0 ;
            
         /**
          * To set the iIsTripClear variable to false
          */
         virtual void ResetTripClear() = 0;  
         
         /**
         * Function to check if trip
         * is stopped and is reset
         */
         virtual TBool IsTripReset() = 0;
         
         /**
         * Function to return the recorded 
         * heading
         *
         */
         virtual TReal32 GetHeading() = 0;
         
         /**
         * Function to check if GPS
         * device is paired and if available
         */
         virtual TInt CheckGPSAvailability() = 0;
         
         /**
         * Function to check if GPS
         * data is available. i.e 
         * if satellites are visible
         */
         virtual TBool IsGPSDataAvailable() = 0;
         
         /**
         * Trip meter function to get the
         * average speed in the trip
         */
         virtual TReal32 GetTripAverageSpeed() = 0;
         
         /**
         * Trip meter function to get the
         * maximum speed in the trip
         */
         virtual TReal32 GetTripMaxSpeed() = 0;
         
         /**
         * Trip meter function to determine
         * if GPS data was lost when trip was running
         */
         virtual TBool WasGPSDataLost() = 0;
        
         /**
         * Set function for trip meter GPS data
         * availability
         */
         virtual void SetGPSDataAvailability(TBool aAvailability) = 0;
         
         /**
         * Function to get the current update
         * interval set
         */
         virtual void GetCurrentUpdateInterval(TInt& aInterval) = 0;
         
         /**
         * Function to determine if GPS data was available
         * when trip was stopped
         */
         virtual TBool WasGPSAvailableOnTripStop() = 0;
         
         /**
         * Function to check if GPS data is awaited.
         * Returns true if connection to the GPS device is 
         * being established or if connection is being established
         * and some satellites are visible but fix not available
         */
         virtual TBool WaitingGPSData() = 0;
         
         /**
         * Function to get the PSY connection status
         */         
         virtual TBool IsGPSDeviceConnected() = 0;
         
         /**
         * Function to get the Registered View ID
         */         
         virtual TInt GetRegisteredViewId() = 0;
         
         /**
         * Function to get the PSY module status
         * notification event to CBlidLocation //added for TSW error.
         */         
         virtual void GetModuleInfoNotificationL() = 0;

        /**
         * Set the Navigation start variable
         */
        virtual void SetNavigationStart( TBool aNavigationStart )=0 ;
        
        /**
         * Reset Odometer Valye
         */
        virtual void ResetOdometerValue()=0 ;
        
        virtual TBool IsSatCapablePsyAvailable() = 0;
        
        virtual TInt NumberOfSatellitesVisible() = 0;
        
        virtual TBool IsSatellitesUsed( TInt aIndex ) = 0;
        
        virtual TInt GetSatelliteSignalStrength( TInt aIndex ) = 0;
        
        virtual TInt GetSatelliteNo( TInt aIndex ) = 0;
         
    };

#endif      // __MBLIDLOCATION_H__

// End of File
