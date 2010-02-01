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
* Description: Implementation of CBlidRouter class which provides access to blid router. Blid route can be modified and read using this class
*
*/



// INCLUDE FILES
#include <f32file.h> // RFs
#include <e32math.h>
#include <sysutil.h>
#include <driveinfo.h>

#include "CBlidRouter.h"
#include "TNamedCoordinate.h"
#include "blidengconsts.h"

// CONSTANTS
const TInt KBlidInvalidWaypointIndex( -1 );

//Forward declaration. Function for sorted ordering of waypoints
TInt WaypointOrdering(const TNamedCoordinate& aCoordinate1, const TNamedCoordinate& aCoordinate2);

_LIT(KPanicText, "CBlidRouter");
enum TPanicCode
    {
    EBlidReadOutOfRange = 1
    };

// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// CBlidRouter::CBlidRouter
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CBlidRouter::CBlidRouter()
    {
    iWaypointIndex = -1;    
    }

// ----------------------------------------------------------------------------
// CBlidRouter::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CBlidRouter::ConstructL()
    {
    iLandmark = NULL;
    }

// ----------------------------------------------------------------------------
// CBlidRouter::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CBlidRouter* CBlidRouter::NewL()
    {
    CBlidRouter* self = new( ELeave ) CBlidRouter;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ----------------------------------------------------------------------------
// CBlidRouter::~CBlidRouter
// ----------------------------------------------------------------------------
//
CBlidRouter::~CBlidRouter()
    {
    iRoute.ResetAndDestroy();
    iRoute.Close();
    if( iLandmark )
	    {
	    delete iLandmark;
	    }
    }

// ----------------------------------------------------------------------------
// CBlidRouter::At
// ----------------------------------------------------------------------------
//
TNamedCoordinate& CBlidRouter::At( TInt aIndex )
	{
    __ASSERT_ALWAYS(aIndex >= 0 && aIndex < iRoute.Count(),
        			User::Panic(KPanicText,EBlidReadOutOfRange));
    return *iRoute[aIndex];
	}

// ----------------------------------------------------------------------------
// CBlidRouter::AppendL
// ----------------------------------------------------------------------------
//
TInt CBlidRouter::AppendL( TNamedCoordinate* aCoordinate, TBool aCheckForMemory )
	{
	RFs fs;
    User::LeaveIfError(fs.Connect()); 
    CleanupClosePushL( fs ); 
    
    if(aCheckForMemory)
    	{
    	if ( SysUtil::DiskSpaceBelowCriticalLevelL( &fs, sizeof(aCoordinate), DriveInfo::EDefaultPhoneMemory ) )
		    {
		    CleanupStack::PopAndDestroy(); // fs
		    return KErrDiskFull;        
			}
    	}    
	CleanupStack::PopAndDestroy(); // fs
	TInt retVal;
	TLinearOrder<TNamedCoordinate> order(WaypointOrdering);    		
    retVal = iRoute.InsertInOrder(aCoordinate, order);    
    return retVal;
	}


// ----------------------------------------------------------------------------
// CBlidRouter::RemoveL
// ----------------------------------------------------------------------------
//
TInt CBlidRouter::RemoveL( TInt aIndex )
	{
    __ASSERT_ALWAYS(aIndex >= 0 && aIndex < iRoute.Count(),
            User::Panic(KPanicText,EBlidReadOutOfRange));
    RFs fs;
    User::LeaveIfError(fs.Connect()); 
    CleanupClosePushL( fs ); 

    if ( SysUtil::DiskSpaceBelowCriticalLevelL( &fs, 0, DriveInfo::EDefaultPhoneMemory ) )
        {
        CleanupStack::PopAndDestroy(); // fs
        return KErrDiskFull;        
		}
	CleanupStack::PopAndDestroy(); // fs
    TNamedCoordinate* plot = iRoute[aIndex];
    iRoute.Remove( aIndex );
    delete plot;
    return KErrNone;
	}

// ----------------------------------------------------------------------------
// CBlidRouter::ResetAndDestroy
// ----------------------------------------------------------------------------
//
void CBlidRouter::ResetAndDestroy()
    {
    iRoute.ResetAndDestroy();
    }

// ----------------------------------------------------------------------------
// CBlidRouter::Count
// ----------------------------------------------------------------------------
//
TInt CBlidRouter::Count()
	{
    return iRoute.Count();
	}

// ----------------------------------------------------------------------------
// CBlidRouter::Waypoint()
// Waypoint getter
// ----------------------------------------------------------------------------
//
TNamedCoordinate* CBlidRouter::Waypoint()
    {
    if(!iIsWaypoint || iWaypointIndex == KBlidInvalidWaypointIndex)
	    {
	    return NULL;
	    }
	else if( iWaypointIndex == KManualWaypointIndex )
	    {
	    return &iManualWaypoint;
	    }
    else
	    {
	    return &iWaypoint;	
	    }
    }

// ----------------------------------------------------------------------------
// CBlidRouter::IndexOfWaypoint
// ----------------------------------------------------------------------------
//
TInt CBlidRouter::IndexOfWaypoint() const
    {
    return iWaypointIndex;
    }

// ----------------------------------------------------------------------------
// CBlidRouter::SetWaypoint( const TInt aIndex )
// Waypoint setter
// ----------------------------------------------------------------------------
//
void CBlidRouter::SetWaypoint( const TInt aIndex )
    {
    iWaypoint = At( aIndex );  
    iWaypointIndex = aIndex;
    iIsWaypoint = ETrue;
    }

// ----------------------------------------------------------------------------
// CBlidRouter::RemoveWaypoint()
// ----------------------------------------------------------------------------
//
void CBlidRouter::RemoveWaypoint()
    {
    iWaypointIndex = -1;
    }

// ----------------------------------------------------------------------------
// CBlidRouter::ClearSetPoints()
// ----------------------------------------------------------------------------
//
void CBlidRouter::ClearSetPoints()
	{
	RemoveWaypoint();
	if(iLandmark)
		{
		delete iLandmark;
		iLandmark = NULL;			
		}
	}

// ----------------------------------------------------------------------------
// CBlidRouter::ExternalizeL
// ----------------------------------------------------------------------------
//
void CBlidRouter::ExternalizeL( RWriteStream& aStream ) 
    {
    TDesC *name;
    TInt count( Count() );
    aStream.WriteInt32L( count );
    for (TInt i = 0; i < count; i++ )
        {
        name = At( i ).Name();
        if ( name )
            {
            CleanupStack::PushL( name );
            aStream.WriteInt32L( name->Length() );
            aStream.WriteL( *name );
            CleanupStack::PopAndDestroy(); //name
            name = NULL;
            
            if (  !Math::IsNaN(At( i ).Latitude()) )
                {
                aStream.WriteReal64L( At( i ).Latitude() );
                }
            else 
                {
                aStream.WriteReal64L( 0 );
                }

            if (  !Math::IsNaN(At( i ).Longitude()) )
                {
                aStream.WriteReal64L( At( i ).Longitude() );
                }
            else 
                {
                aStream.WriteReal64L( 0 );
                }

            if (  !Math::IsNaN(At( i ).Altitude()) )
                {
                aStream.WriteReal32L( At( i ).Altitude() );
                }
            else 
                {
                aStream.WriteReal32L( 0 );
                }
			if (  !Math::IsNaN(At( i ).Accuracy()) )
                {
                aStream.WriteReal32L( At( i ).Accuracy() );
                }
            else 
                {
                aStream.WriteReal32L( 0 );
                }
            }
        }   
    }

// ----------------------------------------------------------------------------
// CBlidRouter::InternalizeV1L
// ----------------------------------------------------------------------------
//
void CBlidRouter::InternalizeV1L( RReadStream& aStream )
    {
    TInt i;
    TInt count( aStream.ReadInt32L() );    
    TInt length(0);
    TBuf<KBlidWaypointNameMaxLen> name;
    for ( i = 0; i < count; i++ )
        {
        length = aStream.ReadInt32L();
        aStream.ReadL( name, length );
        TReal64 latitude = aStream.ReadReal64L();
        TReal64 longitude = aStream.ReadReal64L();
        TReal32 altitude = aStream.ReadReal32L();
        TReal32 accuracy = aStream.ReadReal32L();
        TNamedCoordinate* plot = new(ELeave)TNamedCoordinate(
                            latitude, 
                            longitude,
                            altitude,
                            name,accuracy);
        CleanupStack::PushL( plot );
        //iRouter takes ownership of plot
        User::LeaveIfError( AppendL( plot, EFalse ) );
        CleanupStack::Pop(); //plot
        name.Zero();
        }
    }
	
// ----------------------------------------------------------------------------
// CBlidRouter::Landmark
// ----------------------------------------------------------------------------
//	
CPosLandmark* CBlidRouter::Landmark() const
	{
	if( iLandmark && !iIsWaypoint  )
		{
		return iLandmark;	
		}		
	else
		{
		return NULL;	
		}	
	}

// ----------------------------------------------------------------------------
// CBlidRouter::SetLandmark
// ----------------------------------------------------------------------------
//    
void CBlidRouter::SetLandmark(CPosLandmark* aLandmark) 
	{
	if( iLandmark )
		{
		delete iLandmark;	
		}
	
	iLandmark = aLandmark;
	iIsWaypoint = EFalse;
	}
	
// ----------------------------------------------------------------------------
// CBlidRouter::IsWaypointSet
// ----------------------------------------------------------------------------
//
TBool CBlidRouter::IsWaypointSet()
	{
	return iIsWaypoint;
	}

// ----------------------------------------------------------------------------
// CBlidRouter::IsAnyPointSet
// ----------------------------------------------------------------------------
//
TBool CBlidRouter::IsAnyPointSet()
	{
	if( iLandmark || (iWaypointIndex != -1) )
		{
		return ETrue;	
		}
	else
		{
		return EFalse;	
		}	
	}
// ----------------------------------------------------------------------------
// CBlidRouter::SetManualWaypoint
// ----------------------------------------------------------------------------
//
void CBlidRouter::SetManualWaypoint(TReal latitude, TReal longitude, TDesC& aName)
    {
    iWaypointIndex = KManualWaypointIndex;
    iIsWaypoint = ETrue;
    iManualWaypoint.SetCoordinate(latitude, longitude);
    iManualWaypoint.SetName(aName);
    }

// ----------------------------------------------------------------------------
// CBlidRouter::FindWaypointIndex
// ----------------------------------------------------------------------------
//
TInt CBlidRouter::FindWaypointIndex( TNamedCoordinate* aCoordinate )
    {
    TLinearOrder<TNamedCoordinate> order(WaypointOrdering); 
    iRoute.Sort( order );
    TInt lReturn = iRoute.Find( aCoordinate );
    return lReturn;
    }
// ----------------------------------------------------------------------------
// WaypointOrdering() : Function used for ordering of waypoints
// ----------------------------------------------------------------------------
//
TInt WaypointOrdering(const TNamedCoordinate& aCoordinate1, const TNamedCoordinate& aCoordinate2)
    {
    HBufC* name1 = aCoordinate1.Name();
    HBufC* name2 = aCoordinate2.Name();
    TInt retVal = name1->Compare(name2->Des());
    delete name1;
    delete name2;
    return retVal;
    }
//  End of File


