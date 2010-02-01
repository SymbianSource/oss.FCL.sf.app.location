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
* Description: Implemenation of CBlidLocation class which provides access to blid location and satellite information
*
*/



// INCLUDE FILES
#include <lbs.h>
#include <lbscommon.h>
#include <inetprottextutils.h>
#include <lbsposition.h>
#include <lbssatellite.h>

#include "CBlidLocation.h"
#include "MBlidEngObserver.h"
#include "blidengconsts.h"
#include "blidcommonconsts.h"
#include "Blid.hrh"
#include "cblidmoduleinfoobserver.h"

#ifdef _DEBUG
#include "Debug.h"
#endif

// FORWARD DECLARATION
TInt TimerCallBackFunctionL(TAny* aPtr);
TInt SortCriteria(const TSatelliteData& aCurrent, const TSatelliteData& aNext);

// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// CBlidLocation::CBlidLocation
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CBlidLocation::CBlidLocation(RPositionServer& aServer, CBlidEng& aEngine )
                            : CActive(EPriorityStandard)
							,iServer( aServer ), iRequesting( EFalse ),
							iOnlineMode( EFalse ),iIsDistance(ETrue),
							iDistanceTravelled(0),iIsTripStart(EFalse),
							iIsTripNotStart(ETrue),iIsTripClear(EFalse),
							iIsTripReset(EFalse),iEngine(aEngine)

    {
    CActiveScheduler::Add( this );    
    }
   

// ----------------------------------------------------------------------------
// CBlidLocation::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CBlidLocation::ConstructL( )
    {
    iSatelliteCapability = IsSatelliteCapabilityL();
    //User::LeaveIfError(iPositioner.Open( iServer ));
    // get the number of modules
    GetModuleInfoNotificationL();
    // and registered for notification
	iModuleInfoObserv = CBlidModuleinfoObserver::NewL( iServer, *this );
	iModuleInfoObserv->StartNotification();    
    
    //initialize iSpeedAverage
    TRealX temp;
    temp.SetNaN();
    iSpeedAverage = temp;
    iHeading = 0;    
    iGPSDataAvailable = EFalse;
    iTripAverageSpeed = 0.0;
    iTripMaxSpeed = 0.0;
    iIsPreviousPositionUpdated = EFalse;
    iWaitingGpsData = ETrue;
    iIsDistanceTravelled = ETrue;
    iRegisteredViewId = EBlidMainView;
    iUpdateReceived = EFalse;
    iDeviceStatus = EDeviceUnderConnection;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBlidLocation* CBlidLocation::NewL( RPositionServer& aServer, CBlidEng& aEngine )
    {
    CBlidLocation* self = new( ELeave ) CBlidLocation(aServer, aEngine);    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self ); //self
    return self;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::~CBlidLocation
// ----------------------------------------------------------------------------
//
CBlidLocation::~CBlidLocation()
    {
    Cancel();
    iPositioner.Close();    
    iSpeedArray.Close();
    if(iTimer)
        {
        delete iTimer;
        iTimer = NULL;
        }
    iModuleInfoArray.Reset();        
    delete iModuleInfoObserv;
    iSortedSatData.Reset();
    }

// ----------------------------------------------------------------------------
// CBlidLocation::SetObserver
// ----------------------------------------------------------------------------
//
void CBlidLocation::SetObserver(MBlidEngObserver& aObserver, TInt aViewId)
    {
    iObserver = &aObserver;
    iRegisteredViewId = aViewId;
    // added to fix TSW error 
    iLastGoodFixTime.UniversalTime();

    if(iUpdateReceived)
        {
        SetUpdateOptions();
        }    
    }

// ----------------------------------------------------------------------------
// CBlidLocation::RemoveModelObserver
// ----------------------------------------------------------------------------
//
void CBlidLocation::RemoveModelObserver()
    {
    iObserver = NULL;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::InitialLocationRequestL
// ----------------------------------------------------------------------------
//
void CBlidLocation::InitialLocationRequestL( const TDesC& aSrvName )
    {
    User::LeaveIfError(iPositioner.Open( iServer ));
    
    User::LeaveIfError(iPositioner.SetRequestor( 
                             CRequestor::ERequestorService,
                             CRequestor::EFormatApplication,
                             aSrvName ));
	SetUpdateOptions();
    iRequesting = ETrue;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::CurrentPosition
// ----------------------------------------------------------------------------
//
TPosition& CBlidLocation::GetCurrentPosition()
    {
    return iPosition;  
    }

// ----------------------------------------------------------------------------
// CBlidLocation::IsSatelliteCapability
// ----------------------------------------------------------------------------
//
TBool CBlidLocation::IsSatelliteCapabilityL()
    {  
    TUint count = 0;  
    User::LeaveIfError( iServer.GetNumModules(count) );
    TPositionModuleInfo moduleInfo;
    
    for( TUint i = 0; i < count; i++ )
        {
        User::LeaveIfError( iServer.GetModuleInfoByIndex( i, moduleInfo ) );
        if ( moduleInfo.Capabilities() &  
             TPositionModuleInfo::ECapabilitySatellite )
            {
            return ETrue;
            }
        }
   
    return EFalse;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::GetCourseInfo
// ----------------------------------------------------------------------------
//
TCourse& CBlidLocation::GetCourse()
    {
    return iCourse;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::CancelRequest
// ----------------------------------------------------------------------------
//
void CBlidLocation::CancelRequest()
    {
    Cancel();
    }

// ----------------------------------------------------------------------------
// CBlidLocation::StopRequesting
// ----------------------------------------------------------------------------
//
void CBlidLocation::StopRequesting()
    {
    iOnlineMode = EFalse;
    iRequesting = EFalse;
    }
    

// ----------------------------------------------------------------------------
// CBlidLocation::StartRequesting
// ----------------------------------------------------------------------------
// 
void CBlidLocation::StartRequesting()
    {
    if ( !IsActive() )
        {
        SetUpdateOptions();        
        }
    iRequesting = ETrue;    
    }

// ----------------------------------------------------------------------------
// CBlidLocation::IsOnline
// ----------------------------------------------------------------------------
//
TBool CBlidLocation::IsOnline()
    {
    return iOnlineMode;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::SatelliteCapability
// ----------------------------------------------------------------------------
//
TBool CBlidLocation::SatelliteCapability()
    {
    return iSatelliteCapability;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::SpeedAverage
// ----------------------------------------------------------------------------
//
TReal CBlidLocation::SpeedAverage()
    {
    return iSpeedAverage;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::MaxSpeed
// ----------------------------------------------------------------------------
//
TReal CBlidLocation::MaxSpeed()
    {
    return iMaxSpeed;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::WaitingGPSData
// ----------------------------------------------------------------------------
//
TBool CBlidLocation::WaitingGPSData()
	{
	return iWaitingGpsData;
	}

// ----------------------------------------------------------------------------
// CBlidLocation::IsGPSDeviceConnected
// ----------------------------------------------------------------------------
//
TBool CBlidLocation::IsGPSDeviceConnected()
    {
    return (iDeviceStatus == EDeviceConnected);
    }

// ----------------------------------------------------------------------------
// CBlidLocation::RunL
// ----------------------------------------------------------------------------
//
void CBlidLocation::RunL()
    {
    iPosStatus = iStatus;

    iDeviceStatus = EDeviceConnected;
    
    /// Check if we have satellite data
    if(!(iStatus.Int() == KErrNone ||
         iStatus.Int() == KPositionPartialUpdate) )
	    {	    
	    iPositionInfo.ClearSatellitesInView();
		}	
    SortSatelliteDataL();
        
    switch ( iStatus.Int() )
        {
        //case KPositionPartialUpdate:
        case KErrNone: // Success
            {
            iOnlineMode = ETrue;
            iUpdateReceived = ETrue;
            iGPSDataAvailable = ETrue;            
            iPositionInfo.GetPosition( iPosition );
            iPositionInfo.GetCourse( iCourse );
            iLastGoodFixTime.UniversalTime();
    	    
    	    CalculateCourse();
    	        	        	   
			if(iIsTripStart)
				{
				if(!iIsPreviousPositionUpdated)
					{
					iIsPreviousPositionUpdated = ETrue;
					iPrevPosition = GetCurrentPosition();            	
					}
				UpdateTripData();	       
				}
    	    
    	    //SortSatelliteDataL();
    	    
    	    if(iObserver)    	    
    	        {
    	        iObserver->NotifyL( iOnlineMode );
    	        }            
    	      if ( iRequesting )
            {
            //DEBUG("KErrNone : NotifyPositionUpdate");
            iPositioner.NotifyPositionUpdate( iPositionInfo, iStatus );
            SetActive();
            }     
            break;
            }
        case KErrAccessDenied:
        /*
          Happens if we don't specify requestor information.
          This condition should not be encountered.
        */
            {
            iRequesting = EFalse;
            User::Leave( KErrAccessDenied );
            break;
            }
        case KErrCancel: // Postion update request cancelled
            {
            // We canceled the request so do nothing
            break;
            }
        case KPositionPartialUpdate: // Incomplete position information   
        {
        if ( iRequesting )
            {
            //DEBUG("KPositionPartialUpdate : NotifyPositionUpdate");
            iPositioner.NotifyPositionUpdate( iPositionInfo, iStatus );
            SetActive();
            }
        	break;
        }     
        case KErrTimedOut: // Position update timed out
        case KPositionQualityLoss: //GPS not connected
            {
            iUpdateReceived = ETrue;
            iGPSDataAvailable = EFalse;           
            if(iIsTripStart && !iIsTripClear)
                {
                iTripGPSDataLost = ETrue;
                }
            // Leaving break is intentional
            }
            //-fallthrough
        case KErrNotFound: // No PSY selected.
        case KErrUnknown:  // This will be returned by MLFW        
        case KErrArgument:
        /*
         PSY does not support the information type specified
         by BLID.
        */
        default:
            {  
            if(iStatus.Int() != KPositionPartialUpdate )
                {
                iDeviceStatus = EDeviceNotFound;
                iPositionInfo.ClearSatellitesInView();
                iWaitingGpsData = EFalse;                
                }
            // added for TSW error.    
            if(iStatus.Int() == KPositionPartialUpdate )
                {
                //SortSatelliteDataL();
                TTime now;
                now.UniversalTime();
                TTimeIntervalSeconds secondsSinceLastGoodFix;
                now.SecondsFrom( iLastGoodFixTime , secondsSinceLastGoodFix );
                if ( secondsSinceLastGoodFix.Int() > 15 )
                    {
                    iLastGoodFixTime.UniversalTime();
                    if(iObserver)
                        {
                        iObserver->NotifyErrorL( KErrTimedOut );
                        }  
                    }
                }
                else
                {
                iLastGoodFixTime.UniversalTime();
                }
            // end of addition for TSW error
                            
			iGPSDataAvailable = EFalse;
			iUpdateReceived = ETrue;
            
            // All cases are errors and hence go to offline mode
            iOnlineMode = EFalse;
            if(iObserver)
                {
                iObserver->NotifyErrorL(iStatus.Int());
                }            
            if ( iRequesting )
            {
            
            iPositioner.NotifyPositionUpdate( iPositionInfo, iStatus );
            SetActive();
            }         
            break;
            }
        }
        
    /*if( iStatus.Int() != KErrCancel )
        {
        //DEBUG("RUNL  : SetUpdateOptions");
        SetUpdateOptions();
        }*/
    if(iStatus.Int() != KPositionPartialUpdate )
    {
    	iWaitingGpsData = CheckIfWaitingGPSData();
    }
    }

// ----------------------------------------------------------------------------
// CBlidLocation::SetUpdateOptions
// ----------------------------------------------------------------------------
//
void CBlidLocation::SetUpdateOptions()
    {
    if(IsActive())
        {
        Cancel();
        }
    // update interval
    TInt intervalTime = KMidIntervalTime;
    
    if(iEngine.AppMode() == CBlidEng::EAppBackground) // If application is in background mode
        {
        TRAP_IGNORE(UpdateIntervalL( KMaxIntervalTime ));
        if ( iRequesting )
            {
            iPositioner.NotifyPositionUpdate( iPositionInfo, iStatus );
            SetActive();
            }
        return;
        }
    
    switch(iRegisteredViewId)
        {
        case EBlidNavigationView:
            {
            TReal32 distance;
            TInt retVal;
            retVal = iEngine.GetDistanceToDestination(distance);
            if(retVal == KErrNone)
                {
                if(distance <= KBlidHundredMeters)
                    {
                    intervalTime = KShortIntervalTime;
                    }
                else if(distance > KBlidHundredMeters && 
                        distance < KBlidOneKilometer)
                    {
                    intervalTime = KMidIntervalTime;
                    }
                else
                    {
                    intervalTime = KMaxIntervalTime;
                    }
                }
            else
                {
                intervalTime = KShortIntervalTime;
                }
            break;
            }
        case EBlidTripMeterView:
            {
            if(iDistanceTravelled < KBlidHundredMeters)
                {
                intervalTime = KShortIntervalTime;
                }
            else if(iDistanceTravelled > KBlidHundredMeters && 
                    iDistanceTravelled < KBlidOneKilometer)
                {
                intervalTime = KMidIntervalTime;
                }
            else
                {
                intervalTime = KMaxIntervalTime;
                }
            break;
            }        
        case EBlidMainView: 
        default:
            {
            intervalTime = KMidIntervalTime;
            break;
            }                        
        }
    TRAP_IGNORE(UpdateIntervalL( intervalTime ));
    if ( iRequesting )
        {
        iPositioner.NotifyPositionUpdate( iPositionInfo, iStatus );
        SetActive();
        }
    }

// ----------------------------------------------------------------------------
// CBlidLocation::DoCancel
// ----------------------------------------------------------------------------
//
void CBlidLocation::DoCancel()
    {
    iPositioner.CancelRequest( EPositionerNotifyPositionUpdate );
    }

// ----------------------------------------------------------------------------
// CBlidLocation::RunError
// ----------------------------------------------------------------------------
//
TInt CBlidLocation::RunError(TInt aError)
    {
    if ( iObserver && aError != KBlidLeaveExit )
        {
        TRAPD(error,iObserver->NotifyErrorL( aError ));
        if(error != KErrNone)
        	{
        	return error;
        	}
        }
    return aError;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::UpdateIntervalL
// ----------------------------------------------------------------------------
//
void CBlidLocation::UpdateIntervalL( const TInt aIntervalTime )
    {
    iCurrentUpdateInterval = aIntervalTime;
    TPositionUpdateOptions updateOptions;    
    updateOptions.SetUpdateInterval(TTimeIntervalMicroSeconds(aIntervalTime));    
    updateOptions.SetAcceptPartialUpdates( ETrue );
    if(iUpdateReceived)
    	{
    	updateOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(KUpdateIntervalTimeOut));
    	}
    else
    	{
    	updateOptions.SetUpdateTimeOut( TTimeIntervalMicroSeconds(KStartUpdateIntervalTimeOut));
    	}    
    User::LeaveIfError( iPositioner.SetUpdateOptions(updateOptions) );    
    }


// ----------------------------------------------------------------------------
// CBlidLocation::GetCurrentUpdateInterval
// ----------------------------------------------------------------------------
//
void CBlidLocation:: GetCurrentUpdateInterval(TInt& aInterval)
	{
	aInterval = iCurrentUpdateInterval;
	}

// ----------------------------------------------------------------------------
// CBlidLocation::GetPositionInfoStatus
// ----------------------------------------------------------------------------
//
TInt CBlidLocation::GetPositionInfoStatus()
    {
    return iPosStatus.Int();
    }

// ----------------------------------------------------------------------------
// CBlidLocation::CheckIfWaitingGPSData
// ----------------------------------------------------------------------------
//
TBool CBlidLocation::CheckIfWaitingGPSData()
	{
	if(iDeviceStatus == EDeviceUnderConnection)
	    {
	    return ETrue;
	    }
	    
	// If none of the above then check satellite status
    TInt num = iPositionInfo.NumSatellitesInView();
    if(num > 0)
    	{
    	return ETrue;
    	}
    return EFalse;
	}

// ----------------------------------------------------------------------------
// CBlidLocation::GetRegisteredViewId
// ----------------------------------------------------------------------------
//
TInt CBlidLocation::GetRegisteredViewId()
    {
    return iRegisteredViewId;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::GetModuleInfoStatus
// ----------------------------------------------------------------------------
//
void CBlidLocation::GetModuleInfoStatus(TPositionModuleStatusBase& aPosModuleStatus)
	{
	// This function scan through the all satellite and selected PSY
	// and check the status of those PSY's if those are in Device err state.
	//
	TUint numberOfModules;    
	TPositionModuleInfo moduleInfo;
	numberOfModules = iModuleInfoArray.Count();

	if( numberOfModules > 0)
		{
		for( TInt i = 0; i < numberOfModules; i++ )
			{
			moduleInfo = iModuleInfoArray[i];
			if ( (moduleInfo.Capabilities() &  
			TPositionModuleInfo::ECapabilitySatellite )  && 
			moduleInfo.IsAvailable())
				{
				TPositionModuleId moduleId = moduleInfo.ModuleId();
				iServer.GetModuleStatus(aPosModuleStatus, moduleId);

				TPositionModuleStatus lPosModuleStatus;
				iServer.GetModuleStatus(lPosModuleStatus, moduleId);
				TInt resulterr = lPosModuleStatus.DeviceStatus();

				if( (resulterr == TPositionModuleStatus::EDeviceError ) ||
				(resulterr == TPositionModuleStatus::EDeviceInactive ) ||
				(resulterr == TPositionModuleStatus::EDeviceStandBy ) )
					{
					continue;
					}
				else
					{
					break;
					}                        
				}
			}
		}
	}

// ----------------------------------------------------------------------------
// CBlidLocation::GetNumOfSatellitesL
// ----------------------------------------------------------------------------
//
void CBlidLocation::GetNumOfSatellitesL(TInt& aNumSatellites)
    {
    TSatelliteData tempSatelliteData;
    TPosition CurrentPosition;
    TInt totalStrength = 0;
    TInt noOfSatUsed = iPositionInfo.NumSatellitesUsed();
    TInt noOfSatInView = iPositionInfo.NumSatellitesInView();
    iPositionInfo.GetPosition( CurrentPosition );
    TBool isLat = Math::IsNaN( CurrentPosition.Latitude() );
    TBool isLong = Math::IsNaN( CurrentPosition.Longitude () );
    TBool isAlt = Math::IsNaN( CurrentPosition.Altitude () );
    
    if( noOfSatUsed >= 4 && noOfSatInView > 4 )
	    {
		for(TInt i = 0;i < noOfSatUsed ;i++ )
			{
			User::LeaveIfError(iPositionInfo.GetSatelliteData(i,tempSatelliteData));
			totalStrength += tempSatelliteData.SignalStrength();
			}
		TInt avgStrength = 	totalStrength / noOfSatUsed;
		if( avgStrength >= 40 &&
			!isLat &&
			!isLong &&
			!isAlt  )
			{
			aNumSatellites = 4;				
			}
		else
			{
			aNumSatellites = 3;				
			}			
	    }
	else if ( noOfSatUsed < 4  &&  noOfSatInView >= 4 )	    
		{
		aNumSatellites = 2;	
		}
	else if ( noOfSatUsed < 4  &&  noOfSatInView < 4 )
		{
		aNumSatellites = 1;			
		}
	else
		{
		aNumSatellites = 0;
		}		
    }

// ----------------------------------------------------------------------------
// CBlidLocation::ResetMaxSpeed
// ----------------------------------------------------------------------------
//
void CBlidLocation::ResetMaxSpeed()
	{
    iMaxSpeed = 0.0;
    }


// ----------------------------------------------------------------------------
// CBlidLocation::ResetAvgSpeed
// ----------------------------------------------------------------------------
//
void CBlidLocation::ResetAvgSpeed()
	{
    iSpeedAverage= 0.0;
    }
    

// ----------------------------------------------------------------------------
// CBlidLocation::CalculateTripTimeL
// ----------------------------------------------------------------------------
//
void CBlidLocation::CalculateTripTimeL()
    {
    TTime curTime;
    curTime.UniversalTime();
    curTime.SecondsFrom(iInitTripTime,iTripInterval);    
    }

// ----------------------------------------------------------------------------
// CBlidLocation::CalculateTotalTimeL
// ----------------------------------------------------------------------------
//
void CBlidLocation::CalculateTotalTimeL()
    {
    TTime curTime;
    curTime.UniversalTime();
    if( iIsTripStart )
        {
        curTime.SecondsFrom(iInitViewTime,iTotalInterval);
        }
    }


// ----------------------------------------------------------------------------
// CBlidLocation::UpdateTripData
// ----------------------------------------------------------------------------
//
void CBlidLocation::UpdateTripData( )
    {
    if( iIsTripStart )
    	{
    	TReal32 distance;
      	TPosition& pos = GetCurrentPosition();
      	TReal32 max = Max(pos.HorizontalAccuracy(), iPrevPosition.HorizontalAccuracy());
      		
   	  	if(iPrevPosition.Distance(pos, distance) == KErrNone)
    		{
    		TBool fixConsidered = EFalse;
    		if( distance > max )
    		    {
    		    iDistanceTravelled += distance;
    		    iTotalDistanceTravelled += distance;
    		    iPrevPosition = TPosition(pos);
    		    fixConsidered = ETrue;
    		    }
    		else
    			{
    			TReal32 speed = iCourse.Speed();
    			if(speed > KBlidCutOffSpeed)
    				{
    				iDistanceTravelled += distance;
    				iTotalDistanceTravelled += distance;
    				iPrevPosition = TPosition(pos);
    				fixConsidered = ETrue;
    				}
    			}
    	    if(fixConsidered)
    	        {
    	        // Evaluated Trip Speeds if fix considered
    	        TReal32 currentSpeed = iCourse.Speed();
                if( currentSpeed > iTripMaxSpeed)
                    {
                    iTripMaxSpeed = currentSpeed;
                    }
                
                TInt time = iTripInterval.Int();
            	iTripAverageSpeed = iDistanceTravelled / time;
            	
            	if(iTripAverageSpeed > iTripMaxSpeed)
            		{
            		iTripAverageSpeed = iTripMaxSpeed;
            		}
    	        }
	        }
    	else
    		{
        	iDistanceTravelled = 0;
        	iTotalDistanceTravelled = 0;
           	} 
    	
	    iIsDistanceTravelled = ETrue;
    	}    
    }
   
// ----------------------------------------------------------------------------
// CBlidLocation::GetTripTime
// ----------------------------------------------------------------------------
//
void CBlidLocation::GetTripTime( TTimeIntervalSeconds& aTime )
    {
    aTime = iTripInterval;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::GetTripTime
// ----------------------------------------------------------------------------
//
void CBlidLocation::GetTotalTime( TTimeIntervalSeconds& aTime )
    {
    aTime = iTotalInterval;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::IsTripDistanceTravelled
// ----------------------------------------------------------------------------
//
TBool CBlidLocation::IsTripDistanceTravelled( )
    {
    return iIsDistanceTravelled;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::GetTripDistance
// ----------------------------------------------------------------------------
//
void CBlidLocation::GetTripDistance(TReal32& aDistance )
    {
    aDistance = iDistanceTravelled;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::GetTotalDistance
// ----------------------------------------------------------------------------
//
void CBlidLocation::GetTotalDistance(TReal32& aDistance )
    {
    aDistance = iTotalDistanceTravelled;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::IsTotalDistanceTravelled
// ----------------------------------------------------------------------------
//
TBool CBlidLocation::IsTotalDistanceTravelled( )
    {
    return iIsDistance;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::SetTripStartL
// ----------------------------------------------------------------------------
//
void CBlidLocation::SetTripStartL()
	{
	if( !iIsTripStart )
		{
		iIsTripStart = ETrue;
		iIsTripNotStart = EFalse;
		iIsTripClear = EFalse;
		iInitTripTime.UniversalTime();
		iInitViewTime.UniversalTime();
		iDistanceTravelled = 0.0;
		if(iOnlineMode)
			{
			iPrevPosition = GetCurrentPosition();
			iIsPreviousPositionUpdated = ETrue;
			}
		else
			{
			iIsPreviousPositionUpdated = EFalse;
			}	
		
		if(iTimer)
		    {
		    delete iTimer;
		    iTimer = NULL;
		    }
		
		iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
		TCallBack callback(TimerCallBackFunctionL, this);
		iTimer->Start( TTimeIntervalMicroSeconds32(1000000),
		               TTimeIntervalMicroSeconds32(1000000),
		               callback );
		}
	}

// ----------------------------------------------------------------------------
// CBlidLocation::HasTripStarted
// ----------------------------------------------------------------------------
//
TBool CBlidLocation::HasTripStarted()
	{
	return iIsTripStart;
	}

// ----------------------------------------------------------------------------
// CBlidLocation::SetTripStop
// ----------------------------------------------------------------------------
//
void CBlidLocation::SetTripStop()
	{
	iIsTripStart = EFalse;
	//iIsTripNotStart = EFalse;
	if(iOnlineMode)
		{
		iWasGPSDataAvailableOnTripStop = ETrue;
		}	
    else
    	{
    	iWasGPSDataAvailableOnTripStop = EFalse;
    	}
	iStoppedTime.UniversalTime();
	if(iTimer)
	    {
	    delete iTimer;
	    iTimer = NULL;
	    }
	}

// ----------------------------------------------------------------------------
// CBlidLocation::HasTripStarted
// ----------------------------------------------------------------------------
//
TBool CBlidLocation::HasTripNotStarted()
	{
	return iIsTripNotStart;
	}

// ----------------------------------------------------------------------------
// CBlidLocation::HasTripStarted
// ----------------------------------------------------------------------------
//
TBool CBlidLocation::WasGPSAvailableOnTripStop()
	{
	return iWasGPSDataAvailableOnTripStop;
	}

// ----------------------------------------------------------------------------
// CBlidLocation::ResetTrip
// ----------------------------------------------------------------------------
//
void CBlidLocation::ResetTrip()
	{
	if(iIsTripStart == EFalse)
	    {
	    iIsTripReset = ETrue;	    
	    }
	else
	    {
	    iIsTripReset = EFalse;
	    }
	
	iDurationInSeconds = 0.0;	
	iDistanceTravelled = 0.0;
	iTripTime = 0;
	iTripInterval = 0;
	iInitTripTime.UniversalTime();
	iTripAverageSpeed = 0.0;
	iTripMaxSpeed = 0.0;	
	}

// ----------------------------------------------------------------------------
// CBlidLocation::Restart
// ----------------------------------------------------------------------------
//
/*void CBlidLocation::Restart()
	{
	iDurationInSeconds = 0.0;
	iDistanceTravelled = 0.0;
	//iTotalDistanceTravelled = 0.0;
	iTotalDurationInSeconds = 0.0;
	iWasGPSDataAvailableOnTripStop = EFalse;
	iTripTime = 0;
	iTotalTime = 0;
	iTripInterval = 0;
	iTotalInterval = 0;
	iInitViewTime.UniversalTime();
	iInitTripTime.UniversalTime();
	iIsTripNotStart = EFalse;
    iTripAverageSpeed = 0.0;
	iTripMaxSpeed = 0.0;	
	}*/
	
// ----------------------------------------------------------------------------
// CBlidLocation::SetTripResumeL
// ----------------------------------------------------------------------------
//
void CBlidLocation::SetTripResumeL()
	{
	iIsTripStart = ETrue;
	iIsTripNotStart = EFalse;
	iWasGPSDataAvailableOnTripStop = EFalse;
	iTimer = CPeriodic::NewL(CActive::EPriorityHigh);
	TCallBack callback(TimerCallBackFunctionL, this);
	iTimer->Start( TTimeIntervalMicroSeconds32(1000000),
	               TTimeIntervalMicroSeconds32(1000000),
	               callback );
	TTime curTime;
	curTime.UniversalTime();    
	TTimeIntervalMicroSeconds interval;
    interval = curTime.MicroSecondsFrom(iStoppedTime);
    
	if(iStoppedTime > iInitViewTime )
	    {	    	    
    	iInitViewTime = iInitViewTime + interval;  	    	
	    }
	
	if(iIsTripReset)
	    {
	    iInitTripTime.UniversalTime();
	    iIsTripReset = EFalse;
	    }
	else if(iStoppedTime > iInitTripTime )
	    {
	    iInitTripTime = iInitTripTime + interval;
	    }	

    if(iIsTripStart)
        {       
        CalculateTripTimeL();
        }
    CalculateTotalTimeL();
    
    if(iObserver)
        {
        if(iOnlineMode)
            {
            iObserver->NotifyL(iOnlineMode);
            }
        else
            {
            iObserver->NotifyErrorL(KErrUnknown);
            }        
        }    
	}
	
// ----------------------------------------------------------------------------
// CBlidLocation::SetTripClear
// ----------------------------------------------------------------------------
//
void CBlidLocation::SetTripClear()
	{
	iIsTripClear = ETrue;
	iIsTripStart = EFalse;
    iIsTripReset = EFalse;
    iIsDistance = ETrue;    
    iWasGPSDataAvailableOnTripStop = EFalse;
    iTripAverageSpeed = 0.0;
    iTripMaxSpeed = 0.0;
	iTripGPSDataLost = EFalse;
	iIsTripNotStart = ETrue;

	iDurationInSeconds = 0.0;
	iDistanceTravelled = 0.0;
	iTotalDurationInSeconds = 0.0;
	iTripTime = 0;
	iTotalTime = 0;
	iTripInterval = 0;
	iTotalInterval = 0;
	iInitViewTime.UniversalTime();
	iInitTripTime.UniversalTime();
	}
	
// ----------------------------------------------------------------------------
// CBlidLocation::IsTripCleared
// ----------------------------------------------------------------------------
//
TBool CBlidLocation::IsTripCleared()
	{
	return iIsTripClear;
	}

// ----------------------------------------------------------------------------
// CBlidLocation::ResetTripClear
// ----------------------------------------------------------------------------
//
void CBlidLocation::ResetTripClear()
	{
	iIsTripClear = EFalse;
	iInitTripTime.UniversalTime();
	iInitViewTime.UniversalTime();
	}


// ----------------------------------------------------------------------------
// CBlidLocation::ResetTripClear
// ----------------------------------------------------------------------------
//
TBool CBlidLocation::IsTripReset()
    {
    return iIsTripReset;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::TimerCallBackL
// ----------------------------------------------------------------------------
//    
void CBlidLocation::TimerCallBackL()
    {
    if(iIsTripStart)
       {       
       CalculateTripTimeL();
       }
    CalculateTotalTimeL();
    
    if(iObserver)
        {
        if(iOnlineMode)
            {
            iObserver->NotifyL(iOnlineMode);
            }
        else
            {
            iObserver->NotifyErrorL(KErrUnknown);
            }        
        }    
    }

// ----------------------------------------------------------------------------
// CBlidLocation::GetHeading
// ----------------------------------------------------------------------------
//
TReal32 CBlidLocation::GetHeading()
    {
    return iHeading;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::CalculateCourse
// ----------------------------------------------------------------------------
//
void CBlidLocation::CalculateCourse()
    {
    TReal32 distance;
    TPosition& pos = GetCurrentPosition();	
   	TReal32 max = Max(pos.HorizontalAccuracy(), iPrevPosition.HorizontalAccuracy());
   	   	
   	if(iPrevCoursePosition.Distance(pos, distance) == KErrNone)
    	{    	
    	if( distance > 0.2 * max )
    	    {	        
	        iHeading = iCourse.Heading();
	        iPrevCoursePosition = TPosition(pos);	     
    	    }
	    }
	else
	    {
	    iPrevCoursePosition = TPosition(pos);	
	    }
    }

// ----------------------------------------------------------------------------
// CBlidLocation::CheckGPSAvailability
// ----------------------------------------------------------------------------
//
TInt CBlidLocation::CheckGPSAvailability()
    {
    TPositionModuleStatus lPosModuleStatus;
    GetModuleInfoStatus(lPosModuleStatus);
    TInt retVal = lPosModuleStatus.DeviceStatus();    
    
    
    if ((retVal == TPositionModuleStatus::EDeviceError) ||
        (retVal == TPositionModuleStatus::EDeviceStandBy) ||
        (retVal == TPositionModuleStatus::EDeviceUnknown) )
        {
        // GPS device unavailable        
        return 0;
        }
    else if( (retVal == TPositionModuleStatus::EDeviceReady) ||
             (retVal == TPositionModuleStatus::EDeviceActive) ||
             (retVal == TPositionModuleStatus::EDeviceInactive) )
        {        
        // GPS device is paired and available
        return 1;
        }
    // GPS device connections is being established
    return -1;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::IsGPSDataAvailable
// ----------------------------------------------------------------------------
//
TBool CBlidLocation::IsGPSDataAvailable()
    {
    return iGPSDataAvailable;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::WasGPSDataLost
// ----------------------------------------------------------------------------
//
TBool CBlidLocation::WasGPSDataLost()
    {
    return iTripGPSDataLost;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::SetGPSDataAvailability
// ----------------------------------------------------------------------------
//
void CBlidLocation::SetGPSDataAvailability(TBool aAvailability)
    {
    iTripGPSDataLost = aAvailability;
    }

// ----------------------------------------------------------------------------
// CBlidLocation::GetTripAverageSpeed
// ----------------------------------------------------------------------------
//
TReal32 CBlidLocation::GetTripAverageSpeed()
    {
    return iTripAverageSpeed;    
    }

// ----------------------------------------------------------------------------
// CBlidLocation::GetTripAverageSpeed
// ----------------------------------------------------------------------------
//
TReal32 CBlidLocation::GetTripMaxSpeed()
    {
    TReal32 currentSpeed = iCourse.Speed();
    return currentSpeed;    
    }

// ----------------------------------------------------------------------------
// CBlidLocation::GetModuleInfoNotification
// ----------------------------------------------------------------------------
//
void CBlidLocation::GetModuleInfoNotificationL()
	{
	iModuleInfoArray.Reset();
    User::LeaveIfError( iServer.GetNumModules(iModuleCount) );
    TPositionModuleInfo moduleInfo;
	    
    for( TUint i = 0; i < iModuleCount; i++ )
        {
        User::LeaveIfError( iServer.GetModuleInfoByIndex( i, moduleInfo ) );
        iModuleInfoArray.Append(moduleInfo);
        }
	}

// ----------------------------------------------------------------------------
// CBlidLocation::SetNavigationStart
// ----------------------------------------------------------------------------
//
void CBlidLocation::SetNavigationStart( TBool aNavigationStart )
	{
	if( !aNavigationStart )
		{
		iIsTripStart = EFalse;
		}
	else
		{
		iIsTripStart = ETrue;		
		}
	
	if( !iIsTripStart )
		{
		// delete the timer because the stop trip will not be get called
		// or else it will leads to memory leak
		if(iTimer)
		    {
		    delete iTimer;
		    iTimer = NULL;
		    }				
		}
	}	

// ----------------------------------------------------------------------------
// CBlidLocation::ResetOdometerValue
// ----------------------------------------------------------------------------
//
void CBlidLocation::ResetOdometerValue()
	{
	iTotalDistanceTravelled = 0.0;	
	}


// ----------------------------------------------------------------------------
// CBlidLocation::IsSatCapablePsyAvailable
// ----------------------------------------------------------------------------
//
TBool CBlidLocation::IsSatCapablePsyAvailable()
	{
	TUint numberOfModules;    
	TPositionModuleInfo moduleInfo;
	numberOfModules = iModuleInfoArray.Count();

	if( numberOfModules > 0)
		{
		moduleInfo = iModuleInfoArray[ 1 ];
		if ( (moduleInfo.Capabilities() &  
		TPositionModuleInfo::ECapabilitySatellite )  && 
		!moduleInfo.IsAvailable() )
			{
			return ETrue;	
			}
		}
	return EFalse;		
	}

// ----------------------------------------------------------------------------
// CBlidLocation::NumberOfSatellitesVisible
// ----------------------------------------------------------------------------
//
TInt CBlidLocation::NumberOfSatellitesVisible()
    {
	return iPositionInfo.NumSatellitesInView();	
    }

// ----------------------------------------------------------------------------
// CBlidLocation::IsSatellitesUsed
// ----------------------------------------------------------------------------
//
TBool CBlidLocation::IsSatellitesUsed( TInt aIndex )
    {
    TBool lResult = EFalse;
    TSatelliteData satData;
    
	if( GetSatelliteData( aIndex, satData ) != KErrNotFound)
		{
		lResult = satData.IsUsed();	
		}
	return lResult;		
    }

// ----------------------------------------------------------------------------
// CBlidLocation::SortSatelliteDataL
// ----------------------------------------------------------------------------
//
void CBlidLocation::SortSatelliteDataL()
	{
	TSatelliteData tempSatelliteData;
	TLinearOrder<TSatelliteData> order(SortCriteria);
	iSortedSatData.Reset();
	TInt satellitesInView = iPositionInfo.NumSatellitesInView();
	for(TInt i = 0;i < satellitesInView ;i++ )
		{
		User::LeaveIfError(iPositionInfo.GetSatelliteData(i,tempSatelliteData));
		TInt signalsth = tempSatelliteData.SignalStrength();
    	iSortedSatData.InsertInOrderAllowRepeats(tempSatelliteData, order);                
		}
	}
	
// ----------------------------------------------------------------------------
// CBlidLocation::GetSatelliteSignalStrength
// ----------------------------------------------------------------------------
//
TInt CBlidLocation::GetSatelliteSignalStrength( TInt aIndex )
    {
	TSatelliteData satData;
	
	if( GetSatelliteData( aIndex, satData ) != KErrNotFound )
		{
		if( satData.SignalStrength() < KMinSatelliteSignalStrength )
        	{
        	return 0;
        	}
		else if( satData.SignalStrength() == KMinSatelliteSignalStrength )
        	{
        	return 1;
        	}        
		else if( satData.SignalStrength() > KMinSatelliteSignalStrength &&
				 satData.SignalStrength() < KMidSatelliteSignalStrength )
        	{
        	return 2;
        	}        
		else if( satData.SignalStrength() > KMidSatelliteSignalStrength &&
				 satData.SignalStrength() < KMaxSatelliteSignalStrength )
        	{
        	return 3;
        	}        
		else if( satData.SignalStrength() > KMaxSatelliteSignalStrength )
        	{
        	return 4;
        	}        
		}
	return 0;		
    }
    
// ----------------------------------------------------------------------------
// CBlidLocation::GetSatelliteData
// ----------------------------------------------------------------------------
//
TInt CBlidLocation::GetSatelliteData( TUint aIndex, 
                                      TSatelliteData& aSatelliteData)
    {
    if( aIndex >= iSortedSatData.Count() )
    	{
    	return KErrNotFound;
    	}    
    aSatelliteData = iSortedSatData[aIndex];
    return KErrNone;
    }    

// ----------------------------------------------------------------------------
// CBlidLocation::GetSatelliteNo
// ----------------------------------------------------------------------------
//
TInt CBlidLocation::GetSatelliteNo( TInt aIndex )
    {
	TSatelliteData satData;
	
	if( GetSatelliteData( aIndex, satData ) != KErrNotFound)
		{
		return satData.SatelliteId();
		}
	return 0;		
    }    

// ----------------------------------------------------------------------------
// SortCriteria  
// ----------------------------------------------------------------------------
//	
TInt SortCriteria(const TSatelliteData& aFirst, const TSatelliteData& aNext)
	{
	if(aFirst.SatelliteId() > aNext.SatelliteId())
		{
		return 1;
		}
	else if(aFirst.SatelliteId() < aNext.SatelliteId())
		{
		return -1;
		}
	return 0;
	}
	        
// ----------------------------------------------------------------------------
// TimerCallBackFunctionL
// ----------------------------------------------------------------------------
//
TInt TimerCallBackFunctionL(TAny* aPtr)
    {
    CBlidLocation* ptr = static_cast<CBlidLocation*>(aPtr);
    ptr->TimerCallBackL();
    return 1;
    }

//  End of File

