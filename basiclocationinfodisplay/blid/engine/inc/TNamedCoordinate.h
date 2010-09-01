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
* Description:  Provides named coordinates
*
*/



#ifndef TNAMEDCOORDINATE_H
#define TNAMEDCOORDINATE_H

// INCLUDES
#include <lbsposition.h>
#include "blidcommonconsts.h"

// CLASS DECLARATION

/**
*  TNamedCoordinate is used to hold the named coordinates of a location 
*  (latitude, longitude and altitude).
*/
class TNamedCoordinate : public TCoordinate
    {
    public:
    	/**
		 * C++ default constructors
		 */
        IMPORT_C TNamedCoordinate();

		IMPORT_C TNamedCoordinate(const TReal64& aLatitude,
						 const TReal64& aLongitude, const TDesC& aName );
        
		IMPORT_C TNamedCoordinate(const TReal64& aLatitude,
						 const TReal64& aLongitude,
						 TReal32 aAltitude, const TDesC& aName,TReal32 aAccuracy);

    public: //new functions
        /**
         * 
         * @return pointer to iName 
         */
		IMPORT_C HBufC* Name() const;
     
        /**
         * Set Coordinate plot's name
         *
         * @param aName
         */
		IMPORT_C void SetName( const TDesC& aName );
		
		/**
         * Set destination accuracy
         *
         * @param aAccuracy
         */
		IMPORT_C void SetAccuracy( const TReal32& aAccuracy );
		
		/**
         * 
         * @return destination accuracy 
         */
		IMPORT_C TReal32 Accuracy() const;

    private:
        /// Own: Coordinate plot's name
		TBuf<KBlidWaypointNameMaxLen> iName;
        TReal32 iAccuracy;
    };

#endif      // TNAMEDCOORDINATE_H

// End of File
