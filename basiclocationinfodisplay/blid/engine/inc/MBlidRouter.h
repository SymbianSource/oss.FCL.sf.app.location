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
* Description:  Blid route can be modified
*                and read with help of this API.
*
*/



#ifndef __MBLIDROUTER_H__
#define __MBLIDROUTER_H__

// INCLUDES
#include <lbsposition.h> // TPositionModuleId
#include <s32strm.h>
#include <EPos_CPosLandmark.h>
#include "TNamedCoordinate.h"

// FORWARD DECLARATIONS
class TNamedCoordinate;

// CLASS DECLARATION

/**
*  This Blid application engine API is only used by Blid application.
*  Blid route can be modified and read with help of this API.
*
*/
class MBlidRouter
    {
    public:  // Constructors and destructor
        /**
         * Destructor.
         */
        virtual ~MBlidRouter(){};

    public: // New functions
		/**
		 * Get coordinates
		 *
		 * @param aIndex of coordinates
		 * @return reference to coordinates
		 */
		virtual TNamedCoordinate& At( TInt aIndex ) = 0;

		/**
		 * Append coordinates
		 *
		 * @param aCoordinates
         * @return Error value
		 */
		virtual TInt AppendL( TNamedCoordinate* aCoordinate, TBool aCheckForMemory ) = 0;

		/**
		 * Delete coordinates
		 *
		 * @param aIndex of coordinates
		 */
		virtual TInt RemoveL( TInt aIndex ) = 0;

        /**
         * Delete all item from coordinate array
         */
        virtual void ResetAndDestroy() = 0;

		/**
		 * Get count of coordinates
		 *
		 * @return number of objects
		 */
		virtual TInt Count() = 0;

        /**
         * Waypoint getter
         * Ownership not transferred
         * 
         * @return reference to the waypoint
         */
        virtual TNamedCoordinate* Waypoint() = 0;

        /**
         * Get waypoint's index in waypoint array
         * @return index
         */
        virtual TInt IndexOfWaypoint() const = 0;
            
        /**
         * Set waypoint 
         *
         * @param index of RouterModel array
         */
        virtual void SetWaypoint( const TInt aIndex ) = 0;
        
        /**
         * Remove waypoint
         *
         */
        virtual void RemoveWaypoint() = 0;

        /**
         * Save engine's data to file
         * @param aStream A reference to RWriteStream
         */ 
        virtual void ExternalizeL( RWriteStream& aStream ) = 0;

        /**
         * Load initial data from file 
         * @param aSteam A reference to RReadStream
         */
        virtual void InternalizeV1L( RReadStream& aStream ) = 0;
        
        /**
         * Landmark data getter
         * Ownership not transferred
         * 
         * @return reference to CPosLandmark
         */
        virtual CPosLandmark* Landmark() const = 0;
        
        /**
         * Set Landmark. Router takes ownership
         * 
         * @param of type CPosLandmark
         */
        virtual void SetLandmark(CPosLandmark* aLandmark) = 0;

        /**
         * To check if waypoint or landmark is currently set
         * 
         * @return ETrue if set EFalse otherwise
         */
		virtual TBool IsWaypointSet() = 0;
		
		/**
         * To check if any one waypoint or landmark is set
         * 
         * @return ETrue if set EFalse otherwise
         */
		virtual TBool IsAnyPointSet() = 0;
		
		/**
         * Sets the latitude and longitude for manual waypoint
         *          
         */
		virtual void SetManualWaypoint(TReal latitude, TReal longitude, TDesC& aName) = 0;
		
		/**
         * Clears if Landmark or waypoint is set
         * 
         */
		virtual void ClearSetPoints() = 0;
		
		/**
         * Give waypoints index
         * 
         */
        virtual TInt FindWaypointIndex( TNamedCoordinate* aCoordinate ) = 0;
    };

#endif      // __MBLIDROUTER_H__

// End of File
