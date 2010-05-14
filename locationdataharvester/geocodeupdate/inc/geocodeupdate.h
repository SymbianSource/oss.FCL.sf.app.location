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
#include <QtCore/qglobal.h>

// CLASS DECLARATION

/**
 * GeocodeUpdate, a class to update latitude and longtude into contact and calender database
 */
class GeocodeUpdate 
{
public:    
    /**
     Create contact database   
     */
    IMPORT_C static void createContactdb();
    
    /**
     * Request to update latitude and longitude into contact db. 
     * @param contactId contact unique id.
     * @param addressType contact address type.
     * @param latitude Latitude to be updated.
     * @param longitude longitude to be updated.
     */

    IMPORT_C static void updateGeocodeToContactDB(const quint32 contactId,
            const int addressType, const double latitude,
            const double longitude);
    /**
     * Request to update latitude and longitude into calender db.    
     * @param calEntryId calender entry unique id
     * @param latitude Latitude to be updated.
     * @param longitude longitude to be updated.
     */

    IMPORT_C static void updateGeocodeToCalenderDB(const ulong calEntryId,
            const double latitude, const double longitude);
};

#endif // __GEOCODEUPDATE_H__ 
