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
* Description: 
*     Implements interface for Blid event
*
*/


// INCLUDE FILES
#include <sysutil.h>
#include <f32file.h>
#include <EPos_CPosLandmark.h>
#include <driveinfo.h>

#include "CBlidEng.h"
#include "BlidModelFactory.h"
#include "DllMain.h" // panic codes
#include "MBlidSettings.h" // MBlidSettings
#include "MBlidLocation.h" //MBlidLocation
#include "MBlidRouter.h" //MBlidRouter
#include "lbsposition.h"
#include "TNamedCoordinate.h"
#include "blidengconsts.h"

// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// CBlidEng::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
EXPORT_C CBlidEng* CBlidEng::NewL()
    {
    CBlidEng* self = new( ELeave ) CBlidEng;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop(self); // self
    return self;
    }

// ---------------------------------------------------------
// CBlidEng::ConstructL
// ---------------------------------------------------------
//
void CBlidEng::ConstructL()
    {
    User::LeaveIfError(iServer.Connect());
    CreateModelL( EBlidSettings );
    CreateModelL( EBlidRouter );
    iAppMode = EAppForeground;
    iHWRMLight = CHWRMLight::NewL();    
    }

// ----------------------------------------------------------------------------
// CBlidEng::CBlidEng
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CBlidEng::CBlidEng()
    {
    }

// ---------------------------------------------------------
// CBlidEng::~CBlidEng
// ---------------------------------------------------------
//
CBlidEng::~CBlidEng()
    {
    delete iSettings;
    delete iLocation;
    delete iRouter;
    delete iHWRMLight;
    iServer.Close();    
    }

// ---------------------------------------------------------
// CBlidEng::StoreL
// ---------------------------------------------------------
//
EXPORT_C TStreamId CBlidEng::StoreL( CStreamStore& aStore,
                                     CStreamStore& aRestore )
    {
    RFs fs;
    User::LeaveIfError(fs.Connect()); 
    CleanupClosePushL( fs ); 

    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &fs, 0, DriveInfo::EDefaultPhoneMemory ) )
        {
        const_cast<CBlidEng*>(this)->RestoreL( aRestore, iStreamId );
        User::Leave( KErrDiskFull );
		}
	CleanupStack::PopAndDestroy(); // fs
    
    RStoreWriteStream stream;
    iStreamId = stream.CreateLC( aStore );
    ExternalizeL( stream );
    stream.CommitL();
    CleanupStack::PopAndDestroy(); //stream
    return iStreamId;
    }

// ---------------------------------------------------------
// CBlidEng::RestoreL
// ---------------------------------------------------------
//
EXPORT_C void CBlidEng::RestoreL( const CStreamStore& aStore,
                TStreamId aStreamId )
    {
    iStreamId = aStreamId;
    RStoreReadStream stream;
    stream.OpenLC(aStore,iStreamId);
    InternalizeL(stream);
    CleanupStack::PopAndDestroy(); //stream
    }

// ----------------------------------------------------------------------------
// CBlidEng::SettingsModel()
// Settings model getter.
// ----------------------------------------------------------------------------
//
EXPORT_C MBlidSettings* CBlidEng::SettingsModel() const
    {
    return iSettings;
    }

// ----------------------------------------------------------------------------
// CBlidEng::LocationModel()
// Location model getter.
// ----------------------------------------------------------------------------
//
EXPORT_C MBlidLocation* CBlidEng::LocationModel() const
    {
    return iLocation;
    }

// ----------------------------------------------------------------------------
// CBlidEng::RouterModel()
// Router model getter.
// ----------------------------------------------------------------------------
//
EXPORT_C MBlidRouter* CBlidEng::RouterModel() const
    {
    return iRouter;
    }

// ----------------------------------------------------------------------------
// CBlidEng::CreateModelL()
// Creates a given model type.
// ----------------------------------------------------------------------------
//
EXPORT_C void CBlidEng::CreateModelL( TBlidModelType aModel )
    {
    switch( aModel )
        {
        case EBlidSettings:
            {
            if ( !iSettings )
                {
                iSettings = BlidModelFactory::SettingsL();
                }
            break;
            }
        case EBlidLocation:
            {
            if ( !iLocation )
                {
                iLocation = BlidModelFactory::LocationL(iServer, *this);                
                }
            break;
            }
        case EBlidRouter:
            {
            if ( !iRouter )
            	{
                iRouter = BlidModelFactory::RouterL();
                }
            break;
            }
        default:
            {
            // Not supported model type
            __ASSERT_DEBUG(EFalse, BlidEngPanic(ELCEModelTypeNotSupported));
            break;
            }            
        }
    }
    
// ---------------------------------------------------------
// CBlidEng::GetDistanceToDestination
// ---------------------------------------------------------
//
EXPORT_C TInt CBlidEng::GetDistanceToDestination(TReal32& aDistance)
    {
    if(iRouter->IsAnyPointSet())
        {
        TInt retVal;
        TPosition& currentPosition = iLocation->GetCurrentPosition();
        if(iRouter->IsWaypointSet())
            {
            TNamedCoordinate* destination = iRouter->Waypoint();
            retVal = destination->Distance(currentPosition, aDistance);
            }
        else
            {
            CPosLandmark* landmark = iRouter->Landmark();
            TPosition destination;
            landmark->GetPosition(destination);
            retVal = currentPosition.Distance(destination, aDistance);            
            }
        
        return retVal;
        }
    else
        {
        return KErrNotFound;
        }        
    }

// ---------------------------------------------------------
// CBlidEng::HasArrivedToDestination
// ---------------------------------------------------------
//
EXPORT_C TBool CBlidEng::HasArrivedToDestination()
	{
	if(iRouter->IsAnyPointSet())
		{
		TPosition& currentPosition = iLocation->GetCurrentPosition();
		TReal32 distanceToDestination(0);
		
		if(iRouter->IsWaypointSet())
	        {
	        TNamedCoordinate* destination = iRouter->Waypoint();
	        if(destination->Distance(currentPosition, distanceToDestination) != KErrNone)
	        	{
	        	return EFalse;
	        	}
	        }
	    else
	        {
	        TPosition destination;
	        CPosLandmark* landmark = iRouter->Landmark();	        
	        landmark->GetPosition(destination);
	        if(currentPosition.Distance(destination, distanceToDestination) != KErrNone)
	        	{
	        	return EFalse;
	        	}
	        }	    
	    
	    if(distanceToDestination <= KMinArrivalDist)
	    	{
	    	iSettings->SetArrivedDistance( KMinArrivalDist );
	    	return ETrue;
	    	}
	    
	    TReal tempHorizontalAccuracy = currentPosition.HorizontalAccuracy();
	    
	    if(tempHorizontalAccuracy > KMaxHorAccLimit)
	    	{
	    	tempHorizontalAccuracy = KMaxHorAccLimit;
	    	}
	    
	    if(distanceToDestination <= tempHorizontalAccuracy)
	    	{
	    	iSettings->SetArrivedDistance( tempHorizontalAccuracy );
	    	return ETrue;
	    	}
		}	
	return EFalse;		
	}
// ----------------------------------------------------------------------------
// CBlidEng::SetAppMode
// ----------------------------------------------------------------------------
//
EXPORT_C void CBlidEng::SetAppMode(TAppMode aMode)
    {
    iAppMode = aMode;
    }

// ----------------------------------------------------------------------------
// CBlidEng::SetBackLightStateL
// It's allocate the Backlight resource for the client and deallocate also depends
// Upon the choise of backlight option from the settings view
// ----------------------------------------------------------------------------
//
EXPORT_C void CBlidEng::SetBackLightStateL(TBacklightState aState)
	{
	if(aState == EBlidBacklightAlwaysOn)
		{
		TRAPD( error, iHWRMLight->ReserveLightL(CHWRMLight::EPrimaryDisplay, EFalse, EFalse); 
		              iHWRMLight->LightOnL(CHWRMLight::EPrimaryDisplay); );
        if( error != KErrNone )
            {
            // do something if necessary.
            }
		}
	else
		{
		iHWRMLight->ReleaseLight(CHWRMLight::EPrimaryDisplay);
		}		
	}	

// ---------------------------------------------------------
// CBlidEng::ExternalizeL
// ---------------------------------------------------------
//
void CBlidEng::ExternalizeL( RWriteStream& aStream ) const
    {
    aStream.WriteInt32L( KBlidDocVersion1 );
	iRouter->ExternalizeL( aStream );
    }

// ---------------------------------------------------------
// CBlidEng::InternalizeL
// ---------------------------------------------------------
//
void CBlidEng::InternalizeL( RReadStream& aStream )
    {
    switch( aStream.ReadInt32L() )
        {
        case KBlidDocVersion1:        
            {
            InternalizeV1L( aStream );
            break;
            }
        default:
            {
            User::Leave( KErrNotSupported );
            break;
            }
        }
    }

// ---------------------------------------------------------
// CBlidEng::InternalizeLV1
// ---------------------------------------------------------
//
void CBlidEng::InternalizeV1L( RReadStream& aStream )
    {    
    if ( iRouter->Count() )
        {
        iRouter->ResetAndDestroy();
        }
    iRouter->InternalizeV1L( aStream );
    }

// ----------------------------------------------------------------------------
// CBlidEng::AppMode
// ----------------------------------------------------------------------------
//
EXPORT_C CBlidEng::TAppMode CBlidEng::AppMode()
    {
    return iAppMode;
    }
// End of File
