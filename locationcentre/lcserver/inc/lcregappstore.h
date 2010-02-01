/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Location Centre Server object.
*
*/


#ifndef C_LCREGAPPSTORE_H
#define C_LCREGAPPSTORE_H

// SYSTEM INCLUDES
#include <e32base.h>
#include <f32file.h>
#include <d32dbms.h>    // RDbStoreDatabase

// USER INCLUDES


// FORWARD DECLARATIONS
class CFileStore;

/**
 *  Implementation of Location Centre private storage info class.
 *
 *  This class provides the implementation of private 
 *  storage of Location Centre Registration info(services) using 
 *  symbian DBMS API.
 */
NONSHARABLE_CLASS( CLcRegAppStore )  : public CBase
    {
public:
   /**
    * Enum to detect wheather Db is created or not
    */
    enum TLcDbStatus
        {
        // Db is successfully created
        ELcDbCreate = 0,
        // Db is successfully opened
        ELcDbOpen
        };
    
    /**
     * Constructs a new instance of CLcRegAppStore.
     *
     * @return The new instance of CLcRegAppStore object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcRegAppStore* NewL( RFs&  aFs );
    
    /**
     * Constructs a new instance of CLcRegAppStore.
     * Leaves the created instance on the cleanup stack.
     *
     * @return The new instance of CLcRegAppStore object.
     * @leave System wide error code if the object creation fails.         
     */
    static CLcRegAppStore* NewLC( RFs&  aFs );        

    /**
     * C++ Destructor.
     * Frees all the resources associated with this Location Centre
     * server private file handler
     */
    virtual ~CLcRegAppStore();
    

public:    
    /**
     * This function return the first column and first row 
     * value which is Uuid.After externalize this we extract 
     * all the seperate Uuid's of dynamically registered services
     *
     * @since S60 v5.0
     * @param aUuid[ out ] contains the packed Uuid value.
     */
	void GetUuidValueL( TDes8& aUuid );

    /**
     * This function insert this packed Uuid value at first column
     * and first row of the app info database for first time and then do Update
     *
     * @since S60 v5.0
     * @param aAppUid[ in ] Packed Uuid value.
     * @return 
     */
	void AddDataToTableL( const TDesC8& aAppUid );
	
private:
    /**
     * C++ Default constructor
     */
    CLcRegAppStore( RFs&  aFs );
    
    /**
     * Second phase of the two phase constructor
     */
    void ConstructL();

private: // private API for database operations
    /**
     * Open existing app info database in exclusive
     * (non-shared) mode.
     * 
     * @since S60 v5.0
     * @param aRegAppInfoFile	is full path to app info
     *              			database file.
     */
    void OpenDbL( const TFileName aRegAppInfoFile );

    /**
     *  Creates and opens a new app info database. Creates a
     *  database file, table structure and an index. The database
     *  will be open in exclusive (non-shareable) mode. The
     *  database must be closed, when not used any more. If the
     *  database exists, it is replaced.
     * 
     * @since S60 v5.0
     * @param aRegAppInfoFile	is full path to app info
     *              			database file.
     */
    void CreateDbL( const TFileName aRegAppInfoFile );

    /**
	 * Closes the database opened with either OpenDbL or
     * CreateDbL. It is safe to close the database even if it
     * is closed.
     * 
     * @since S60 v5.0
     * @param None
     * @return None.
     */
    void Close();    

    /**
     * This function creates the app info table.
     *
     * @since S60 v5.0
     * @param None
     */
    void CreateAppInfoTableL();


private: // Member data
	// File server reference comming from the lcregistry
	// used for iFileStore
    RFs&              	iFsSession;
    
    // For database operations
    RDbStoreDatabase 	iRegAppInfoDb;
    
    //own: For creating and opening database files 
    CFileStore*      	iFileStore;
    
    // holds the database file name
    TFileName			iLcRegdbFile;
    
    // holds the db creation status
    TLcDbStatus         iDbStatus;
    };
    
#endif // C_LCREGAPPSTORE_H    