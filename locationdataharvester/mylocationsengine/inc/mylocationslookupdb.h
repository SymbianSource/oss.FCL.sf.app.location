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
* Description: Header file for mylocation database lookup table.
*
*/

#ifndef __MYLOCATIONSLOOKUPDB_H__
#define __MYLOCATIONSLOOKUPDB_H__

//Headers needed
#include <f32file.h> //RFs
#include <d32dbms.h>  //RDbNamedDatabase,RDbView 

#include "mylocationsdefines.h"
 
/**
 * CLookupDatabase class.
 * This class handles all the operations related to mylocations lookup database.
 *
 */
 
class CLookupDatabase : public CBase
{
public:
    // Two phase constructors
 
    /**
    * This is a static function, which creates and returns an instance of this class.
    */
    static CLookupDatabase* NewL( const TDesC& aLookupTableName );

    /**
    * This is a static function, which creates and returns an instance of this class. Pushes the created object 
    * to the cleanup stack.
    */
    static CLookupDatabase* NewLC( const TDesC& aLookupTableName );

    /**
    * Destructor
    */
    ~CLookupDatabase();

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
    void DeleteEntryL( const TLookupItem& aLookupItem );

    /**
    * Finds an entry in the lookup table.
    * @param[in/out] aLookupItem The lookup item to be found in the database. The source iUid and source type 
    * iSource is passed in the lookup item, if the entry is found, then the iLmId is copied to the lookup item
    * @return ETrue if found, else EFalse
    */
    TBool FindEntryL( TLookupItem& aLookupItem );

    /**
    * Finds list of lookup items given a landmark id.
    * @param[in] aLandmarkId The landmark id to be found in the lookup database.  
    * @param[out] aLookupItemArray List of lookup entried found.  
    */
    void FindEntriesByLandmarkIdL( const TUint32 aLandmarkId, 
            RArray<TLookupItem>& aLookupItemArray );

    /**
    * Finds list of lookup items given a source type.
    * @param[in] aSourceType The source type to be found in the lookup database.  
    * @param[out] aLookupItemArray List of lookup entried found.  
    */
    void FindEntriesBySourceTypeL( const TUint32 aSourceType, 
            RArray<TLookupItem>& aLookupItemArray );



private:
    
    // default constructor
    CLookupDatabase();
    
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


#endif  // __MYLOCATIONSLOOKUP_H__

// End of file

