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
* Description:  Provides access to Blid router
*
*/


#ifndef CBLIDROUTER_H
#define CBLIDROUTER_H

//  INCLUDES
#include <e32base.h>
#include <e32std.h>
#include <f32file.h> // RFs
#include <EPos_CPosLandmark.h>

#include "MBlidRouter.h"


// CLASS DECLARATION

/**
*  A Blid engine API for blid application.
*  Provides access to blid router.
*  This class implements the MBlidRouter interface provided by this dll.
*
*/
class CBlidRouter : public CBase, public MBlidRouter
    {
    public:  // Constructors and destructor
        /**
         * Two-phased constructor.
         */
        static CBlidRouter* NewL();

        /**
         * Destructor.
         */
        virtual ~CBlidRouter();

    private:
        /**
         * C++ default constructor.
         */
        CBlidRouter();

        /**
         * By default Symbian 2nd phase constructor is private.
         */
        void ConstructL();

    private: // From MBlidRouter
    	TNamedCoordinate& At( TInt aIndex );
		TInt AppendL( TNamedCoordinate* aCoordinate, TBool aCheckForMemory );
		TInt RemoveL( TInt aIndex );
        void ResetAndDestroy();
		TInt Count();
        TNamedCoordinate* Waypoint();
        TInt IndexOfWaypoint() const;
        void SetWaypoint( const TInt aIndex );
        void RemoveWaypoint();
        void ExternalizeL( RWriteStream& aStream );
        void InternalizeV1L( RReadStream& aStream );
        CPosLandmark* Landmark() const;
        void SetLandmark(CPosLandmark* aLandmark);
        TBool IsWaypointSet();
        TBool IsAnyPointSet();
        void  ClearSetPoints();
        void SetManualWaypoint(TReal latitude, TReal longitude, TDesC& aName);        
        TInt FindWaypointIndex( TNamedCoordinate* aCoordinate );
        
    private:
        ///Own: array variable
      	RPointerArray<TNamedCoordinate> iRoute;

        ///Ref: Current set waypoint
        TNamedCoordinate iWaypoint;

        ///index of waypoint
        TInt iWaypointIndex;
        
        ///to determine waypoint, landmark or none set
        TBool iIsWaypoint;
        
        ///Own:Landmark
        CPosLandmark* iLandmark;
        
        ///Own: manual waypoint set
        TNamedCoordinate iManualWaypoint;
        
    };


#endif      // CBLIDROUTER_H

// End of File
