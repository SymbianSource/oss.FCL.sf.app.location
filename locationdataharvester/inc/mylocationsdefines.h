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


#ifndef __MYLOCATIONSDEFINES_H__
#define __MYLOCATIONSDEFINES_H__

/**
 * TLookupItem struct.
 * This structure defines a single lookup item.
 */

class TLookupItem
{
public:
    // Uid of the source entry
    TUint32 iUid;

    // Source type
    TUint32 iSource;

    // Landmark uid in the landmarks database
    TUint32 iLmId;
   
    // File Path
    TFileName iFilePath;
    
    //MK  map tile fetching status
    TUint32 iFetchingStatus;
};

/** Defines map tile fetching states.
 */

enum TMapTileFetchingState
    {
    /** Map tile fetching completed */
    EMapTileFectchingCompleted,
    /** Map tile fetching in progress */
    EMapTileFetchingInProgress,
    /** Map  tile fetching n/w error */
    EMapTileFetchingNetworkError,
    /** Map tile fetching invalid address */
    EMapTileFetchingInvalidAddress,
    /** Map tile fetching unknown erro */
    EMapTileFetchingUnknownError
    };
// contacts category in landmarks db
_LIT( KContactsCategory, "Contacts" );

// calendar category in landmarks db
_LIT( KCalendarCategory, "Calendar" );

// lookup database folder path
_LIT( KLookupDbPath, "c:\\mylocations\\" );

//Mylocations resource file
_LIT(KMyLocationsResourceFile, "z:\\resource\\apps\\mylocations.rsc");

// My locations landmarks database uri.
_LIT( KMylocationsDatabaseUri, "file://c:MyLocationsLandmarks.ldb" );

// maptile database uri.
_LIT( KMaptileDatabaseUri, "file://c:MyLocationsMaptile.ldb" );

// History landmarks database uri.
_LIT( KMapsHistoryUri, "file://c:MapsHistoryLandmarks.ldb" );

// mylocations lookup database name
_LIT( KMylocationsLookupDatabaseName, "mylocationslookup.db" );

// maptile lookup database name
_LIT( KMapTileLookupDatabaseName, "mylocationsmaptilelookup.db" );

// landmarks lookup database name
_LIT( KLandmarksLookupDatabaseName, "landmarkslookup.db" );

// database column names
_LIT( NColUid, "uid" );
_LIT( NColSource, "source" );
_LIT( NColLmUid, "lmuid" );

// maptile database column names
_LIT( NCntColUid, "cntuid" );
_LIT( NCntColFilePath, "filepath" );
_LIT( MapTileFetchingStatus, "fetchingstatus" );

// database table name
_LIT( KLookupTable, "lookuptable" );

// maptile database table name
_LIT( KMapTileLookupTable, "cntmaptilelookuptable" );

// uid column number
const TInt KColumnUid = 1;
// source type column number
const TInt KColumnSource = 2;
// landmark uid column number
const TInt KColumnLmkUid = 3;

// uid column number
const TInt KColumncntUid = 1;
// source type column number
const TInt KColumnFilePath = 3;
// fetching state  column number
const TInt KColumnMapTileFetchingStatus = 4;

const TInt KBufSize=256;

#endif  // __MYLOCATIONSDEFINES_H__
// End of file

