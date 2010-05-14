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
* Description: Maptile database lookup table source implementation.
*
*/

#include <bautils.h>
#include "mylocationlogger.h"
#include "lookupmaptiledb.h"

_LIT( KSelectAllFrom, "SELECT * FROM " );
_LIT(KQueryToDB,"SELECT * FROM cntmaptilelookuptable WHERE cntuid = %d AND source = %d");

// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::CLookupMapTileDatabase()
// Default constructor.
// -----------------------------------------------------------------------------
//
CLookupMapTileDatabase::CLookupMapTileDatabase()
{
}

// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::~CLookupMapTileDatabase()
// Destructor.
// -----------------------------------------------------------------------------
//
CLookupMapTileDatabase::~CLookupMapTileDatabase()
{
    __TRACE_CALLSTACK;// close the database
    iItemsDatabase.Close();
    // close the file session
    iFsSession.Close();
}

// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::~CLookupMapTileDatabase()
// Creates an object of this class and pushes to cleanup stack.
// -----------------------------------------------------------------------------
//
CLookupMapTileDatabase* CLookupMapTileDatabase::NewLC(
        const TDesC& aLookupTableName)
{
    CLookupMapTileDatabase* self = new (ELeave) CLookupMapTileDatabase;
    CleanupStack::PushL(self);
    self->ConstructL(aLookupTableName);
    return self;
}

// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::NewL()
// Creates an object of this class.
// -----------------------------------------------------------------------------
//
CLookupMapTileDatabase* CLookupMapTileDatabase::NewL(
        const TDesC& aLookupTableName)
{
    CLookupMapTileDatabase* self = CLookupMapTileDatabase::NewLC(
            aLookupTableName);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::ConstructL()
// 2nd phase contructor.
// -----------------------------------------------------------------------------
//
void CLookupMapTileDatabase::ConstructL(const TDesC& aLookupTableName)
{
    __TRACE_CALLSTACK;
    User::LeaveIfError(iFsSession.Connect());

    //create private path
    User::LeaveIfError(iFsSession.CreatePrivatePath(RFs::GetSystemDrive()));
    // private path with no drive on it
    iFsSession.PrivatePath(iDbFileName);

    TFindFile PrivFolder(iFsSession);
    // find out the drive
    if (KErrNone == PrivFolder.FindByDir(iDbFileName, KNullDesC))
    {
        iFsSession.MkDir(KLookupDbPath);
        iDbFileName.Copy(KLookupDbPath);
        iDbFileName.Append(aLookupTableName);

        if (!BaflUtils::FileExists(iFsSession, iDbFileName))
        { // no database exists so we make one
            User::LeaveIfError(iItemsDatabase.Create(iFsSession, iDbFileName));
            // and will create the only table needed for it
            CreateTableL(iItemsDatabase);

            //close the database
            iItemsDatabase.Close();
        }        
    }
}

// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::Open()
// Opens the lookup database.
// -----------------------------------------------------------------------------
//
TInt CLookupMapTileDatabase::Open()
{
    __TRACE_CALLSTACK;
    return iItemsDatabase.Open(iFsSession, iDbFileName);
}

// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::Close()
// Closes the lookup database.
// -----------------------------------------------------------------------------
//
void CLookupMapTileDatabase::Close()
{
    __TRACE_CALLSTACK;
    iItemsDatabase.Close();
}

// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::CreateTableL()
// Creates a lookup table.
// -----------------------------------------------------------------------------
//
void CLookupMapTileDatabase::CreateTableL(RDbDatabase& aDatabase)
{
    __TRACE_CALLSTACK;// Create a table definition
    CDbColSet* columns = CDbColSet::NewLC();

    // Add Columns

    // Add uid column
    columns->AddL(TDbCol(NCntColUid, EDbColUint32));

    columns->AddL(TDbCol(NColSource, EDbColUint32));

    // add file path type column
    columns->AddL(TDbCol(NCntColFilePath, EDbColText16));

    // Create a table
    User::LeaveIfError(aDatabase.CreateTable(KMapTileLookupTable, *columns));

    // cleanup the column set
    CleanupStack::PopAndDestroy(columns);
}

// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::CreateEntryL()
// Creates an entry in the lookup table.
// -----------------------------------------------------------------------------
//
void CLookupMapTileDatabase::CreateEntryL(const TLookupItem& aLookupItem)
{
    __TRACE_CALLSTACK;// create a query for the view
    TFileName queryBuffer;
    queryBuffer.Copy(KSelectAllFrom);
    queryBuffer.Append(KMapTileLookupTable);

    TInt ret = Open();
    if (ret != KErrNone)
    {
		Close();
        Open();
       
    }
	 iItemsDatabase.Begin();
    
    RDbView myView;
    myView.Prepare(iItemsDatabase, TDbQuery(queryBuffer));
    CleanupClosePushL(myView);

    // Inert the item
    myView.InsertL();
    // set the fields
    myView.SetColL(KColumncntUid, aLookupItem.iUid);
    myView.SetColL(KColumnSource, aLookupItem.iSource);
    myView.SetColL(KColumnFilePath, aLookupItem.iFilePath);

    myView.PutL();

    CleanupStack::PopAndDestroy(&myView); // myView
    iItemsDatabase.Commit();
    Close();
}

// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::UpdateEntryL()
// Updates an entry in the lookup table.
// -----------------------------------------------------------------------------
//
void CLookupMapTileDatabase::UpdateEntryL(const TLookupItem& aLookupItem)
{
    __TRACE_CALLSTACK;// Create the query to find the row to be updated.
    TFileName queryBuffer;
    queryBuffer.Format(KQueryToDB, aLookupItem.iUid, aLookupItem.iSource);
    TInt ret = Open();
    if (ret != KErrNone)
    {
		Close();
        Open();
       
    }
	 iItemsDatabase.Begin();

    // Create a view of the table based on the query created.
    RDbView myView;
    myView.Prepare(iItemsDatabase, TDbQuery(queryBuffer));
    CleanupClosePushL(myView);

    myView.EvaluateAll();
    myView.FirstL();

    if (myView.AtRow())
    {
        // found the entry. update it.
        myView.UpdateL();
        myView.SetColL(KColumnFilePath, aLookupItem.iFilePath);
        myView.PutL();
    }

    CleanupStack::PopAndDestroy(&myView); // myView
    iItemsDatabase.Commit();

    Close();

}

// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::DeleteEntryL()
// Deletes an entry from the lookup table.
// -----------------------------------------------------------------------------
//
void CLookupMapTileDatabase::DeleteEntryL(const TLookupItem& aLookupItem)
{
    __TRACE_CALLSTACK;// Create the query to find the row to be deleted.

    TFileName queryBuffer;
    queryBuffer.Format(KQueryToDB, aLookupItem.iUid, aLookupItem.iSource);

    TInt ret = Open();
    if (ret != KErrNone)
    {
		Close();
        Open();
       
    }
	 iItemsDatabase.Begin();

    RDbView myView;
    // query buffer finds only the selected item row.
    myView.Prepare(iItemsDatabase, TDbQuery(queryBuffer));
    CleanupClosePushL(myView);

    myView.EvaluateAll();

    // positions the cursor on the first row of the rowset
    myView.FirstL();

    // Delete the entry found.
    if (myView.AtRow())
    {
        myView.GetL();
        if (aLookupItem.iUid == myView.ColUint(KColumncntUid))
        {
            ret = iFsSession.Delete(myView.ColDes16(KColumnFilePath));
        }
        myView.DeleteL();
    }

    CleanupStack::PopAndDestroy(&myView); // myView
    iItemsDatabase.Commit();
    // compacts the databse, by physicaly removing deleted data.
    iItemsDatabase.Compact();

    Close();
}

// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::FindEntryL()
// Finds an entry in the lookup table.
// -----------------------------------------------------------------------------
//
TBool CLookupMapTileDatabase::FindEntryL(TLookupItem& aLookupItem)
{
    __TRACE_CALLSTACK;// used to store return value
    TBool retVal = EFalse;
    // Create a query to find the item.
    TFileName queryBuffer;
    queryBuffer.Format(KQueryToDB, aLookupItem.iUid, aLookupItem.iSource);

   TInt ret = Open();
    if (ret != KErrNone)
    {
		Close();
        Open();
       
    }
	 iItemsDatabase.Begin();

    // Create a view of the table with the above query.
    RDbView myView;
    myView.Prepare(iItemsDatabase, TDbQuery(queryBuffer));
    CleanupClosePushL(myView);
    myView.EvaluateAll();
    myView.FirstL();

    if (myView.AtRow())
    {
        // Item found. get the details.
        myView.GetL();
        if (aLookupItem.iUid == myView.ColUint(KColumncntUid))
        {
            aLookupItem.iFilePath.Copy(myView.ColDes16(KColumnFilePath));
        }
        retVal = ETrue;
    }

    CleanupStack::PopAndDestroy(&myView); // myView
    Close();
    return retVal;
}
// End of file

