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
* Description:  Provides access to Blid location.
*
*/



#ifndef CBLIDLOCATION_H
#define CBLIDLOCATION_H

//  INCLUDES
#include <e32base.h>
#include <lbspositioninfo.h>
#include <lbs.h>
#include <lbssatellite.h>
#include "MBlidLocation.h"
#include "CBlidEng.h"

// CLASS DECLARATION
class CBlidModuleinfoObserver;


/**
*  A Blid engine API for blid application.
*  Provides access to blid location.
*  This class implements the MBlidLocation interface provided by this dll.
*
*/
class CBlidLocation : public CActive, public MBlidLocation
    {
    public:  // Constructors and destructor
        /**
         * Two-phased constructor.
         * @param aServer RPositionServer variable
         */
        static CBlidLocation* NewL( RPositionServer& aServer, CBlidEng& aEngine );

        /**
         * Destructor.
         */
        virtual ~CBlidLocation();
        
        void TimerCallBackL();

    private:
        /**
         * C++ default constructor.
         * @param aServer RPosition variable
         */
        CBlidLocation( RPositionServer& aServer,CBlidEng& aEngine );

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private: // From MBlidLocation
        //Current view Id needed to set the update options accordingly.
        void SetObserver(MBlidEngObserver& aObserver, TInt aViewId);
        void RemoveModelObserver();
        void InitialLocationRequestL( const TDesC& aSrvName );
        TPosition& GetCurrentPosition();
        TCourse& GetCourse();
        void CancelRequest();
        void StopRequesting();
        void StartRequesting();
        TBool IsOnline();
        TBool SatelliteCapability();
        TReal SpeedAverage();
        TInt GetPositionInfoStatus() ;
        TReal MaxSpeed();
        void ResetMaxSpeed();
        void ResetAvgSpeed();
        void GetModuleInfoStatus(TPositionModuleStatusBase& aPosModuleStatus);
        void GetNumOfSatellitesL(TInt& anumSatellites);
        TReal32 GetHeading();
        void CalculateCourse();        

        TBool WasGPSAvailableOnTripStop();
        TBool CheckIfWaitingGPSData();
        TInt GetRegisteredViewId();
        void GetModuleInfoNotificationL();
        void SetNavigationStart( TBool aNavigationStart );
        void ResetOdometerValue();
        TBool IsSatCapablePsyAvailable();
        TInt NumberOfSatellitesVisible();
        TBool IsSatellitesUsed( TInt aIndex );
        TInt GetSatelliteSignalStrength( TInt aIndex );
        TInt GetSatelliteNo( TInt aIndex );

    private: // From CActive
        void RunL();
        void DoCancel();
        TInt RunError(TInt aError);
        CPeriodic *iTimer;
        
    private: //New functions
    
        // Indicates the connection status to GPS device
        enum TDeviceConStatus
            {
            EDeviceConnected,
            EDeviceNotFound,
            EDeviceUnderConnection
            };
        /**
         * Update request interval time
         * @param aIntervalTime time in microseconds
         */
        void UpdateIntervalL( const TInt aIntervalTime );
        
        /**
         * Sets the update options based 
         * on the view and its conditions
         */
        void SetUpdateOptions();

        /**
         * Check if satellite information is available
         * @return TBool, ETrue if is capability and EFalse if not
         */
        TBool IsSatelliteCapabilityL();  
        
        /**
         * Calculates all the data for current trip
         *
         */
        void UpdateTripData();  

        /**
         * Calculate trip time
         * 
         */

        void CalculateTripTimeL( );
        
        /**
         * Calculate total time
         * 
         */

        void CalculateTotalTimeL( ); 
        
        /**
         * Gets the trip time
         */        
        void GetTripTime(  TTimeIntervalSeconds& aTime );  
        
        /**
         * Gets the total view time 
         */
        void GetTotalTime( TTimeIntervalSeconds& aTime );  
        
        /**
         * Check to see if any distance
         * is travelled
         */
        TBool IsTripDistanceTravelled(); 
        
        /**
         * Check to see if any distance
         * is travelled
         */
        TBool IsTotalDistanceTravelled(); 
        
        /**
         * Gets the trip
         * distance
         */
        void GetTripDistance( TReal32& aDistance ); 
        
        /**
         * Gets the total distance
         */
        void GetTotalDistance( TReal32& aDistance ); 
        
        /**
         * Set the trip start variable
         */
        void SetTripStartL() ;
        
        /**
         * Return the iIsTripStart variable
         */
        TBool HasTripStarted() ;
        
        /**
         * Set the trip stop variable
         */
        void SetTripStop() ;
        
        /**
         * Return the iIsTripNotStart variable
         */
        TBool HasTripNotStarted() ;

    	/**
    	 * All the display for the current trip are
    	 * reset
    	 */
        void ResetTrip();

        /**
    	 * Resets only the trip 
    	 *
    	 */
        void SetTripResumeL();
        
        /**
         * Set the trip clear variable
         */
        void SetTripClear( ) ;
        
        /**
         * Return the iIsTripClear variable
         */
        TBool IsTripCleared( ) ;
        
        /**
         * To set the iIsTripClear variable to false
         */
        void ResetTripClear();
        
        /**
         * Function to check if trip
         * is stopped and is reset
         */
        TBool IsTripReset();
        
        /**
         * Function to check if GPS
         * device is paired 
         */
        TInt CheckGPSAvailability();
        
        /**
         * Function to check if GPS
         * data is available. i.e 
         * if satellites are visible
         */
        TBool IsGPSDataAvailable();
        
        /**
         * Trip meter function to get the
         * average speed in the trip
         */
        TReal32 GetTripAverageSpeed();
        
        /**
         * Trip meter function to get the
         * maximum speed in the trip
         */
        TReal32 GetTripMaxSpeed();
        
        /**
         * Trip meter function to determine
         * if GPS data was lost when trip was running
         */
        TBool WasGPSDataLost();
        
        /**
         * Set function for trip meter GPS data
         * availability
         */
        void SetGPSDataAvailability(TBool aAvailability);
        
        /**
         * Function to get the current update
         * interval set
         */
        void GetCurrentUpdateInterval(TInt& aInterval);
        
        /**
         * Function to check if GPS data is awaited.
         * Returns true if connection to the GPS device is 
         * being established or if connection is being established
         * and some satellites are visible but fix not available
         */
        TBool WaitingGPSData();
        
        /**
         * Function to get the current PSY status
         * @return TBool, ETrue if is connected and EFalse if not
         */        
        TBool IsGPSDeviceConnected();
        
		TInt GetSatelliteData( TUint aIndex, TSatelliteData& aSatelliteData); 
		
		void SortSatelliteDataL();       
                
    private: // Data
        /// Ref: Observer interface to Blid application engine
        MBlidEngObserver* iObserver;

        ///  RPositioner
        RPositioner iPositioner;

        ///  TPosition
        TPosition iPosition;

        ///  TCource
        TCourse iCourse;

        ///  TPositionCourseInfo
        TPositionSatelliteInfo iPositionInfo;
                
        /// Ref: iServer
        RPositionServer& iServer;

        ///  Update interval time
        TBool iUpdateInterval;

        ///  Start and stop requesting
        TBool iRequesting;

        ///  Online / Offline mode
        TBool iOnlineMode;

        ///  Capable of returning satellite information
        TBool iSatelliteCapability;

        /// TReal variable
        TReal iSpeedAverage;

        /// TReal variable
        TReal iMaxSpeed;

        /// TReal array
        RArray<TReal> iSpeedArray;

        /// TInt variable 
        TInt iCount;
        
        /// Bearing variable
        TInt iBearing;

        /// Is bearing data available
        TBool iIsBearing;

        /// Is Heading data available
        TBool iIsHeading;

        /// distance variable
        TReal32 iDistance;

        /// Is Distance data available
        TBool iIsDistanceTravelled;

        /// Is Distance data available
        TBool iIsDistance;

        /// duration variable
        TTime iDuration;

        /// duration in seconds
        TReal64 iDurationInSeconds;

         /// duration in seconds
        TReal64 iTotalDurationInSeconds;
        
        /// Speed variable
        TReal iSpeed;

        /// Is speed data available
        TBool iIsSpeed;

        /// Distance travelled variable
        TReal32 iDistanceTravelled;

        ///Total Distance travelled variable
        TReal32 iTotalDistanceTravelled;

        /// Trip time variable
        TTime iTripTime;

        /// Trip time variable
        TTime iTotalTime;

        /// Own: A pointer to TLocale object
        TLocale* iLocale;
        
        /// Holds the previously recorded position
        TPosition iPrevPosition; 
        
        /// Holds the previous position for course calculations
        TPosition iPrevCoursePosition;
        
        /// Holds the time at which trip meter was started
        TTime iInitTripTime;
        
         /// Is Trip Started available
        TBool iIsTripStart;

		/// Is Trip Not Started available
        TBool iIsTripNotStart;
        
        /// Holds the time at which trip meter has started
        TTime iInitViewTime;
        
        /// Holds the trip meter timing
        TTimeIntervalSeconds iTripInterval;
        
        /// Holds the total time
        TTimeIntervalSeconds iTotalInterval;
        
        /// Is speed data available
        TBool iIsTripClear;
        
        /// Holds the time when Trip has stopped
        TTime iStoppedTime;
        
        // Boolean to determine if trip has been reset       
        TBool iIsTripReset;
        
        /// Request status from Location FW
        TRequestStatus iPosStatus;
        
        /// Holds the heading angle
        TReal32 iHeading;
        
        /// Boolean to detrmine if GPS data is available
        TBool iGPSDataAvailable;
        
        /// Boolean to determine if GPS data was available when
        /// trip meter was stopped
        TBool iWasGPSDataAvailableOnTripStop;
        
        // Average speed of the trip
        TReal32 iTripAverageSpeed;
        
        // Maximum speed in the trip
        TReal32 iTripMaxSpeed;
        
        // Boolean to determine if GPS data was lost when
        // trip meter was running
        TBool iTripGPSDataLost;
        
        // Id of the view registered by the
        // SetObserver API
        TInt iRegisteredViewId;
                
        // Pointer to the engine. Needed to 
        // get pointer to the router
        CBlidEng& iEngine;
        
        // Value of the current
        // update interval set
        TInt iCurrentUpdateInterval;
        
        /// Boolean to detrmine if variable iPrevPosition 
        /// was updated to hold the appropriate previous position
        TBool iIsPreviousPositionUpdated;
        
        /// Boolean to determine if the system is waiting for GPS data
        TBool iWaitingGpsData;
        
        // Boolean to determine if the first update has been received or not
        TBool iUpdateReceived;
        
        TUint iModuleCount;
        
        RArray<TPositionModuleInfo> iModuleInfoArray;
    	
    	CBlidModuleinfoObserver*	iModuleInfoObserv;
    	
    	
    #ifdef _DEBUG
    private:
        // for testing
        friend class T_CBLIDLOCATION;
    #endif
        
        //Variable holds the PSY status.
        TDeviceConStatus iDeviceStatus;
        
        TTime            iLastGoodFixTime;

    	RArray<TSatelliteData> iSortedSatData;        
    };


#endif      // CBLIDLOCATION_H

// End of File
