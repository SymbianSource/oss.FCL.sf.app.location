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
* Description: Implementation of TNamedCoordinate cllass. This class holds the named coordinates of a location (lat, long and altitude)
*
*/



// INCLUDE FILES
#include "TNamedCoordinate.h"

// ============================ MEMBER FUNCTIONS ==============================
// ----------------------------------------------------------------------------
// TNamedCoordinate::TNamedCoordinate
// ----------------------------------------------------------------------------
//
EXPORT_C TNamedCoordinate::TNamedCoordinate()
    {
    iAccuracy = 0;
    }
// ----------------------------------------------------------------------------
// TNamedCoordinate::TNamedCoordinate
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
EXPORT_C TNamedCoordinate::TNamedCoordinate( const TReal64& aLatitude,
	     const TReal64& aLongitude, const  TDesC& aName ) :
	     TCoordinate( aLatitude, aLongitude ), iName(aName)
	{
	iAccuracy = 0;
	}

// ----------------------------------------------------------------------------
// TNamedCoordinate::TNamedCoordinate
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
EXPORT_C TNamedCoordinate::TNamedCoordinate( const TReal64& aLatitude,
			                                 const TReal64& aLongitude, 
                                                   TReal32 aAltitude, 
                                             const  TDesC& aName,TReal32 aAccuracy) :
			      TCoordinate( aLatitude, aLongitude, aAltitude ), iName(aName),iAccuracy(aAccuracy)
	{

	}

// ----------------------------------------------------------------------------
// TNamedCoordinate::Name
// ----------------------------------------------------------------------------
//
EXPORT_C HBufC* TNamedCoordinate::Name() const
    {
    return iName.Alloc();
    }

// ----------------------------------------------------------------------------
// TNamedCoordinate::SetName
// ----------------------------------------------------------------------------
//
EXPORT_C void TNamedCoordinate::SetName( const TDesC& aName )
    {
    iName = aName;
    }

// ----------------------------------------------------------------------------
// TNamedCoordinate::Accuracy
// ----------------------------------------------------------------------------
//
EXPORT_C TReal32 TNamedCoordinate::Accuracy() const
    {
    return iAccuracy;
    }

// ----------------------------------------------------------------------------
// TNamedCoordinate::SetAccuracy
// ----------------------------------------------------------------------------
//
EXPORT_C void TNamedCoordinate::SetAccuracy( const TReal32& aAccuracy )
    {
    iAccuracy = aAccuracy;
    }
//  End of File
