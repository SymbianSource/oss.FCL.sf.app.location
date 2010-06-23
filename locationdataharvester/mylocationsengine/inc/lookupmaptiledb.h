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
* Description: Maptile database lookup table header.
*
*/

#ifndef __LOOKUPMAPTILEDB_H__
#define __LOOKUPMAPTILEDB_H__

//Headers needed
#include <f32file.h> //RFs
#include <d32dbms.h>  //RDbNamedDatabase,RDbView 

#include "mylocationsdefines.h"
 
/**
 * CLookupMapTileDatabase class.
 * This class handles all the operations related to maptile lookup database.
 *
 */
 
class CLookupMapTileDatabase : public CBase
{
public:
    // Two phase constructors
 
    /**
    * This is a static function, which creates and returns an instance of this class.
    */
    static CLookupMapTileDatabase* NewL( const TDesC& aLookupTableName );

    /**
    * This is a static function, which creates and returns an instance of this class. Pushes the created object 
    * to the cleanup stack.
    */
    static CLookupMapTileDatabase* NewLC( const TDesC& aLookupTableName );

    /**
    * Destructor
    */
    ~CLookupMapTileDatabase();

public:
    
    /**
    * Opens the lookup database.
    */
    TInt Open();

    /**
    * Closes the lookup database.
    */
    void Close();

    /**
    * Creates an entry in the lookup table.
    * @param[in] aLookupItem The lookup item to be created in the database.
    */
    void CreateEntryL( const TLookupItem& aLookupItem );

    /**
    * Updates an entry in the lookup table.
    * @param[in] aLookupItem The lookup item to be updated in the database.
    */
    void UpdateEntryL( const TLookupItem& aLookupItem );

    /**
    * Deletes an entry from the lookup table.
    * @param[in] aLookupItem The lookup item to be deleted from the database.
    */
    void DeleteEntryL( TLookupItem& aLookupItem );
    
    /**
    * Deletes maptile from the repository if the ref to that maptile in the db reaches one
    * @param[in] aLookupItem The lookup item containing the path to the maptile
    */
    void DeleteMapTileL( const TLookupItem& aLookupItem );

    /**
    * Finds an entry in the lookup table.
    * @param[in/out] aLookupItem The lookup item to be found in the database. The source iUid is passed
    * in the lookup item
    * @return ETrue if found, else EFalse
    */
    TBool FindEntryL( TLookupItem& aLookupItem);
    
    /**
    * Finds an entry in the lookup table for the given maptile name.
    * @param[in] aFilePath The lookup item to be found in the database. maptile file path is passed
    * @return ETrue if found, else EFalse
    */
    TBool FindEntryByFilePathL(const TDesC& aFilePath);

    void FindEntriesByMapTileFetchingStateL(const TUint32 aFetchingState,
            RArray<TLookupItem>& aLookupItemArray);

    /**
    * ReSet the entry in the lookup table , with null value 
    * @param aLookupItem The lookup item entry to be reseted in the database,
    * get the used maptile path as part of lookupItem filepath , to free the resource 
    */
    void ReSetEntryL( TLookupItem &aLookupItem );
private:
    
    // default constructor
    CLookupMapTileDatabase();
    
    // Second phase constructor
    void ConstructL( const TDesC& aLookupTableName );
    
    // Creates a lookup table in the lookup database.
    void CreateTableL(RDbDatabase& aDatabase);
          
private:
    
    // Handle to the items database
    RDbNamedDatabase iItemsDatabase;
    
    // handle to the file session
    RFs iFsSession;
        
    // holds the database file name
    TFileName iDbFileName;

};


#endif  // __LOOKUPMAPTILEDB_H__

// End of file

