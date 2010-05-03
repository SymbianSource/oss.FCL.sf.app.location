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
*     maptile service interface
*
*/

#ifndef _MAPTILESERVICE_H_
#define _MAPTILESERVICE_H_


#include <QString>
#include <QtGlobal>


#ifdef  MAPTILESERVICEDLL
#define MAPTILESERVICE_EXPORT Q_DECL_EXPORT
#else
#define MAPTILESERVICE_EXPORT Q_DECL_IMPORT
#endif



// CLASS DECLARATION

/**
*  Maptile service class,  provides interface to get map tile image associated with 
*  contact. Also provides interface to check whether location feature is enabled or disabled.
*
*  Note: Location feature can be enabled or disabled by modifying conf\cntmaptileservice.confml file.
*/
class MAPTILESERVICE_EXPORT MapTileService 
{

public:

    /**
     * Application types      
     */
    enum AppType
    {
        /** Contacts application */
        AppTypeContacts,
        /** Calendar application */
        AppTypeCalendar
    };
    /** 
     * Address types
     */
    enum AddressType      
    {
        /** Plain Address Type */
        AddressPlain,
        /** Address Type Pref */
        AddressPreference,
        /** Address type Work */
        AddressWork,
        /** Address type Home */
        AddressHome
    };
      
   /**
    * Checks whether location feature enabled or disabled for specific application.
    * 
    * @return Returns true or false based on location feature setting.
    */
    static bool isLocationFeatureEnabled(AppType appType);
            
   /**
    * Gets a maptile image associated with a id(contact id/calendar id). 
    * Returns a maptile image path if it is available otherwise returns NULL.
    * 
    * @param contactId  Contact id     
    * @param sourceType Source address type( Preferred, Home , Work address )
    *      
    * @return Returns maptile image path if it is available, else NULL.
    */
    static QString getMapTileImage( int Id, AddressType sourceType  );  
     	
};

#endif //MAPTILESERVICE_H_

