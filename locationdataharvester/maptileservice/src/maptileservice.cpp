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
*     Maptile service implementation
*
*/


#include <centralrepository.h> 

#include "maptileservice.h"
#include "maptiledblookuptable.h"

// CONSTANTS
// Maptile interface uid
const TUid KUidMapTileInterface = { 0x2002E6E8 };
// Central Repository Key IDs
const TInt KEnableLocationFeature  = 0x1;

// -----------------------------------------------------------------------------
// MapTileService::isLocationFeatureEnabled()
// Checks whether location feature is enabled or disabled
// -----------------------------------------------------------------------------
//
bool MapTileService::isLocationFeatureEnabled(AppType appType)
{ 
   
    //Create the centrep with uid 0x2002C3A8
    bool iLocationFeatureEnabled = false;
    
    CRepository* centralRepository = NULL;  
     
    TRAPD( err, centralRepository = CRepository::NewL( KUidMapTileInterface ) );
    if ( KErrNone == err )
    {
        TInt repValue;
      
        //Get the Location feature flag
        int ret=0;
        if ( appType == AppTypeContacts )
        {
            ret = centralRepository->Get( KEnableLocationFeature , repValue );
        }
        else if ( appType == AppTypeCalendar )
        {
             ret = centralRepository->Get( KEnableLocationFeature , repValue );
        }
      
        if ( ret == KErrNone && repValue == 1 )
        {
            iLocationFeatureEnabled  = true;
        }
           
         delete centralRepository;
    }
           
    return iLocationFeatureEnabled;
    
}


// -----------------------------------------------------------------------------
// MapTileService::getMapTileImage()
// Gets the maptile image path associated with a contact.
// -----------------------------------------------------------------------------
//
QString MapTileService::getMapTileImage( int id, AddressType sourceType )    
{
   
    //Image file to return;
   
    //Maptile database  instance
    CLookupMapTileDatabase* mapTileDatabase = NULL;
   
    TRAPD( err, mapTileDatabase = CLookupMapTileDatabase::NewL(
            KMapTileLookupDatabaseName ) );
    if ( KErrNone == err )
    {
        TLookupItem lookupItem;
        lookupItem.iUid = id;
        switch( sourceType )
        {
            case AddressPlain:
                lookupItem.iSource = ESourceCalendar;
                break;
           case AddressPreference:
               lookupItem.iSource = ESourceContactsPref;
               break;
           case AddressWork:
               lookupItem.iSource = ESourceContactsWork;
               break;
           case AddressHome:
               lookupItem.iSource = ESourceContactsHome;
               break;
           default: 
               break;
        }
       
        TRAP( err , mapTileDatabase->FindEntryL( lookupItem ) );
       
        //delet the database instance
        delete mapTileDatabase;
       
        //if entry available returns the file path otherwise NULL. 
        if ( KErrNone == err  )
        {
           //Get the image path
            QString imageFile((QChar*)lookupItem.iFilePath.Ptr(),
                    lookupItem.iFilePath.Length());
            return imageFile;
        }
    }
    
    return QString();
}

// End of file

