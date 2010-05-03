/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#ifndef __GEOCODEUPDATE_H__
#define __GEOCODEUPDATE_H__

#include <e32def.h> 

// CLASS DECLARATION

/**
 * CntGeocodeUpdate, a class to update latitude and longtude into contact database
 * for a perticular contact.
 */
class GeocodeUpdate 
{
public:    
    /**
     * Request to create contactmanager ,
     * contactmanager will create contact db.    
     */
    IMPORT_C static void CreateContactdb();
    
    /**
     * Request to update latitude and longitude.    *
     * @param aCntId contact unique id
     * @param aLatitude Latitude to be saved
     * @param aLongitude longitude to be saved
     * @return Status code (0 is failure,1 success)
     */

    IMPORT_C static void UpDate(const TInt32 aCntId,
            const TInt32 aCntAddressType, const TReal aLatitude,
            const TReal aLongitude);
};

#endif // __GEOCODEUPDATE_H__ 
