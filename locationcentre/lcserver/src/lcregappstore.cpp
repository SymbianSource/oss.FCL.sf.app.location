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


// SYSTEM INCLUDES
#include <badesca.h>    // CDesCArrayFlat
#include <s32file.h>    // CFileStore & CPermanentFileStore
#include <bautils.h>    // file helpers

// USER INCLUDES
#include "lcregappstore.h"

// CONSTANT DEFINTION
_LIT(KLcPrivateDbName,"lcregappinfo.db"); // Name of the Database file for storing registered app info.
_LIT(KAppInfoTable, "AppInfo");            // Name of the one table
_LIT(KAppUUIdCol, "UUID");           	   // First column

const TInt 	KMaxColumnLength = 32*1024;

// ----- Member funtions for CLcRegAppStore ---------------------------------

// ---------------------------------------------------------------------------
// CLcRegAppStore::CLcRegAppStore
// ---------------------------------------------------------------------------
//
CLcRegAppStore::CLcRegAppStore( RFs&  aFs )
	:iFsSession( aFs )
    {
    // C++ Default constructor. No allocations or functions which can Leave
    // should be called from here.Initiallize all the variable here
    }
         
// ---------------------------------------------------------------------------
// CLcRegAppStore::~CLcRegAppStore
// ---------------------------------------------------------------------------
//
CLcRegAppStore::~CLcRegAppStore()
    {
    // C++ Destructor. Free all resources associated with this class.
    Close();
    }
        
// ---------------------------------------------------------------------------
// CLcRegAppStore* CLcRegAppStore::NewL
// ---------------------------------------------------------------------------
//
CLcRegAppStore* CLcRegAppStore::NewL( RFs&  aFs )
    {
    CLcRegAppStore* self = NewLC( aFs );
    CleanupStack::Pop( self );
    return self;         
    }

// ---------------------------------------------------------------------------
// CLcRegAppStore* CLcRegAppStore::NewLC
// ---------------------------------------------------------------------------
//
CLcRegAppStore* CLcRegAppStore::NewLC( RFs&  aFs )
    {
    // Symbian Two phased constructor. Leaves the object on the Clean-up
    // stack.
    CLcRegAppStore* self = new ( ELeave )CLcRegAppStore( aFs );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;         
    }

// ---------------------------------------------------------------------------
// void CLcRegAppStore::ConstructL
// ---------------------------------------------------------------------------
//
void CLcRegAppStore::ConstructL()
    {
    // Obtain the Db file path
    User::LeaveIfError( iFsSession.SessionPath( iLcRegdbFile ) );

    // Create the Database Directory ie the private directory of the process
    if( !BaflUtils::PathExists( iFsSession, iLcRegdbFile ) )
	    {
    	User::LeaveIfError( iFsSession.MkDirAll( iLcRegdbFile ) );	    	
	    }
    
    // Generate the Db file
    iLcRegdbFile.Append( KLcPrivateDbName );

	// Check if the db file is exists or not.
    if( BaflUtils::FileExists( iFsSession, iLcRegdbFile ) )
        {
        OpenDbL( iLcRegdbFile );
        }
	else
		{
		CreateDbL( iLcRegdbFile );	
		}        
    }  

// ---------------------------------------------------------------------------
// void CLcRegAppStore::OpenDbL
// ---------------------------------------------------------------------------
//
void CLcRegAppStore::OpenDbL( const TFileName aRegAppInfoFile )
    {  	
    // if it is already open then close it.
    Close();
	// Check if the db file is exists or not.
    if( BaflUtils::FileExists( iFsSession, aRegAppInfoFile ) )
        {
		TRAPD(error, 
			iFileStore = CPermanentFileStore::OpenL( iFsSession, aRegAppInfoFile, 
				EFileRead|EFileWrite );
			//Set file store type	
			iFileStore->SetTypeL( iFileStore->Layout() );
			iRegAppInfoDb.OpenL( iFileStore,iFileStore->Root() )
			);

	    if( !error )
	    	{
	    	iDbStatus = ELcDbOpen;
	    	}
	    else
		    {
		    User::Leave( error );	
		    }
        }
    }

// ---------------------------------------------------------------------------
// void CLcRegAppStore::CreateDbL()
//
// Create a new database. The database will be in exclusive access mode.
// ---------------------------------------------------------------------------
void CLcRegAppStore::CreateDbL( const TFileName aRegAppInfoFile )
    {
    // if it is already open then close it.
    Close();

    // Create empty database file.
    TRAPD(error,
    	iFileStore = CPermanentFileStore::ReplaceL(iFsSession, aRegAppInfoFile, 
    		EFileRead|EFileWrite);
    	iFileStore->SetTypeL( iFileStore->Layout() );// Set file store type
    	TStreamId id = iRegAppInfoDb.CreateL( iFileStore );// Create stream object
    	iFileStore->SetRootL( id );// Keep database id as root of store
    	iFileStore->CommitL();// Complete creation by commiting
    	// Create appinfo tables
    	CreateAppInfoTableL();
    	);
    
    if( !error )
    	{
    	iDbStatus = ELcDbCreate;
    	}
    else
	    {
	    User::Leave( error );	
	    }
    }

// ---------------------------------------------------------------------------
// void CLcRegAppStore::Close()
//
// Close the database.
// ---------------------------------------------------------------------------
void CLcRegAppStore::Close()
    {
    iRegAppInfoDb.Close();
    delete iFileStore;
    iFileStore = NULL;
    }

// ---------------------------------------------------------------------------
// void CLcRegAppStore::CreateAppInfoTableL()
//
// Creates Appinfo table. Leaves, if the table cannot be created.
// ---------------------------------------------------------------------------
void CLcRegAppStore::CreateAppInfoTableL()
    {
    // Specify columns for appinfo table
    TDbCol uuidCol( KAppUUIdCol, EDbColLongText8,KMaxColumnLength );   // Using default length

    // Add the columns to column set
    CDbColSet* appInfoColSet = CDbColSet::NewLC();
    appInfoColSet->AddL( uuidCol );

    // Create the appinfo table
    User::LeaveIfError( iRegAppInfoDb.CreateTable(KAppInfoTable,
        *appInfoColSet) );
    CleanupStack::PopAndDestroy( appInfoColSet );
    }

// ---------------------------------------------------------------------------
// void CLcRegAppStore::AddDataToTable()
//
// Add item to database using RDbTable API
// ---------------------------------------------------------------------------
void CLcRegAppStore::AddDataToTableL( const TDesC8& aAppUid )
    {
    // Create an updateable database table object
    RDbTable table;
    CleanupClosePushL( table );
    
    User::LeaveIfError( table.Open( iRegAppInfoDb, KAppInfoTable, table.EUpdatable ));
    
	CDbColSet* dbColSet = table.ColSetL();
    CleanupStack::PushL( dbColSet );
	    
    //table.Reset();
    TBool lRowPos = table.FirstL();
    
    TDbColNo colNo = dbColSet->ColNo(KAppUUIdCol);
	    
    // Do first time insert and then do Update
    if( iDbStatus == ELcDbOpen && lRowPos )
        {
    	table.UpdateL();
       	table.SetColL( colNo, aAppUid ); // col = 1
        }
    else
        {
    	table.InsertL();
       	table.SetColL( colNo, aAppUid ); // col = 1
        iDbStatus = ELcDbOpen;    	    	
        }	        

    table.PutL();    // Complete changes (the insertion)

    CleanupStack::PopAndDestroy( dbColSet );    	
    CleanupStack::PopAndDestroy();
    }

// ---------------------------------------------------------------------------
// void CLcRegAppStore::GetUuidValueL()
//
// Get packed Uuid from database according to column name 
// ---------------------------------------------------------------------------
void CLcRegAppStore::GetUuidValueL(TDes8& aUuid)
    {
    aUuid.Zero();
    if(!BaflUtils::FileExists( iFsSession, iLcRegdbFile ) )
        {
        return;
        }

	_LIT(KSelectAll,"SELECT * FROM AppInfo" );

    // Create a view on the database
    RDbView view;
    CleanupClosePushL( view );
    TInt lResult = view.Prepare( iRegAppInfoDb, TDbQuery( KSelectAll ), view.EReadOnly );
    if( lResult == KErrNone )
	    {
	    lResult = view.EvaluateAll();

	    CDbColSet* colSet = view.ColSetL();
	    CleanupStack::PushL( colSet );

	    // Append each result row
	    if ( view.FirstL() )
	        {
	        view.GetL();
	    	TDbColNo colNo = colSet->ColNo( KAppUUIdCol );	        
	        RDbColReadStream readStream;       // A stream object for long columns
	        readStream.OpenLC( view, colNo );
	        readStream.ReadL( aUuid, view.ColLength( colNo ) );
	        CleanupStack::Pop(); //readStream
	        readStream.Close();
	        }
	    CleanupStack::PopAndDestroy( colSet );
	    }
    CleanupStack::PopAndDestroy( &view );
    }
// End of file

