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
* Description: Mylocation database lookup table implementation.
*
*/

#include <BAUTILS.H>
#include <EPos_CPosLmCategoryManager.h>
#include "mylocationslookupdb.h"
#include "mylocationlogger.h"
// select all from
_LIT( KSelectAllFrom, "SELECT * FROM " );
// string 'where'
_LIT( KStringWhere, " WHERE " );
// string ' = '
_LIT( KStringEqual, " = " );
// string 'And'
_LIT( KStringAnd, " AND " );

// -----------------------------------------------------------------------------
// CLookupDatabase::CLookupDatabase()
// Default constructor.
// -----------------------------------------------------------------------------
//
CLookupDatabase::CLookupDatabase()
{
}

// -----------------------------------------------------------------------------
// CLookupDatabase::~CLookupDatabase()
// Destructor.
// -----------------------------------------------------------------------------
//
CLookupDatabase::~CLookupDatabase()
{
    __TRACE_CALLSTACK;// close the database
    iItemsDatabase.Close();
    // close the file session
    iFsSession.Close();
}

// -----------------------------------------------------------------------------
// CLookupDatabase::~CLookupDatabase()
// Creates an object of this class and pushes to cleanup stack.
// -----------------------------------------------------------------------------
//
CLookupDatabase* CLookupDatabase::NewLC(const TDesC& aLookupTableName)
{
    CLookupDatabase* self = new (ELeave) CLookupDatabase;
    CleanupStack::PushL(self);
    self->ConstructL(aLookupTableName);
    return self;
}

// -----------------------------------------------------------------------------
// CLookupDatabase::NewL()
// Creates an object of this class.
// -----------------------------------------------------------------------------
//
CLookupDatabase* CLookupDatabase::NewL(const TDesC& aLookupTableName)
{
    CLookupDatabase* self = CLookupDatabase::NewLC(aLookupTableName);
    CleanupStack::Pop(self);
    return self;
}

// -----------------------------------------------------------------------------
// CLookupDatabase::ConstructL()
// 2nd phase contructor.
// -----------------------------------------------------------------------------
//
void CLookupDatabase::ConstructL(const TDesC& aLookupTableName)
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
// CLookupDatabase::Open()
// Opens the lookup database.
// -----------------------------------------------------------------------------
//
TInt CLookupDatabase::Open()
{
    __TRACE_CALLSTACK;
    return iItemsDatabase.Open(iFsSession, iDbFileName);
}

// -----------------------------------------------------------------------------
// CLookupDatabase::Close()
// Closes the lookup database.
// -----------------------------------------------------------------------------
//
void CLookupDatabase::Close()
{
    __TRACE_CALLSTACK;
    iItemsDatabase.Close();
}

// -----------------------------------------------------------------------------
// CLookupDatabase::CreateTableL()
// Creates a lookup table.
// -----------------------------------------------------------------------------
//
void CLookupDatabase::CreateTableL(RDbDatabase& aDatabase)
{
    __TRACE_CALLSTACK;// Create a table definition
    CDbColSet* columns = CDbColSet::NewLC();

    // Add Columns

    // Add uid column
    columns->AddL(TDbCol(NColUid, EDbColUint32));

    // add source type column
    columns->AddL(TDbCol(NColSource, EDbColUint32));

    // add landmark uid column
    columns->AddL(TDbCol(NColLmUid, EDbColUint32));

    // Create a table
    User::LeaveIfError(aDatabase.CreateTable(KLookupTable, *columns));

    // cleanup the column set
    CleanupStack::PopAndDestroy(columns);
}


// -----------------------------------------------------------------------------
// CLookupDatabase::CreateEntryL()
// Creates an entry in the lookup table.
// -----------------------------------------------------------------------------
//
void CLookupDatabase::CreateEntryL(const TLookupItem& aLookupItem)
{
    __TRACE_CALLSTACK;// create a query for the view
    TFileName queryBuffer;
    queryBuffer.Copy(KSelectAllFrom);
    queryBuffer.Append(KLookupTable);

    iItemsDatabase.Begin();

    RDbView myView;
    myView.Prepare(iItemsDatabase, TDbQuery(queryBuffer));
    CleanupClosePushL(myView);

    // Inert the item
    myView.InsertL();

    // set the fields
    myView.SetColL(KColumnUid, aLookupItem.iUid);
    myView.SetColL(KColumnSource, aLookupItem.iSource);
    myView.SetColL(KColumnLmkUid, aLookupItem.iLmId);

    myView.PutL();

    CleanupStack::PopAndDestroy(&myView); // myView
    iItemsDatabase.Commit();
}

// -----------------------------------------------------------------------------
// CLookupDatabase::UpdateEntryL()
// Updates an entry in the lookup table.
// -----------------------------------------------------------------------------
//
void CLookupDatabase::UpdateEntryL(const TLookupItem& aLookupItem)
{
    __TRACE_CALLSTACK;// Create the query to find the row to be updated.
    TFileName queryBuffer;
    queryBuffer.Copy(KSelectAllFrom);
    queryBuffer.Append(KLookupTable);
    queryBuffer.Append(KStringWhere);
    queryBuffer.Append(NColUid);
    queryBuffer.Append(KStringEqual);
    queryBuffer.AppendNum(aLookupItem.iUid);
    queryBuffer.Append(KStringAnd);
    queryBuffer.Append(NColSource);
    queryBuffer.Append(KStringEqual);
    queryBuffer.AppendNum(aLookupItem.iSource);

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
        myView.SetColL(KColumnLmkUid, aLookupItem.iLmId);
        myView.PutL();
    }

    CleanupStack::PopAndDestroy(&myView); // myView
    iItemsDatabase.Commit();

}

// -----------------------------------------------------------------------------
// CLookupDatabase::DeleteEntryL()
// Deletes an entry from the lookup table.
// -----------------------------------------------------------------------------
//
void CLookupDatabase::DeleteEntryL(const TLookupItem& aLookupItem)
{
    __TRACE_CALLSTACK;// Create the query to find the row to be deleted.
    TFileName queryBuffer;
    queryBuffer.Copy(KSelectAllFrom);
    queryBuffer.Append(KLookupTable);
    queryBuffer.Append(KStringWhere);
    queryBuffer.Append(NColUid);
    queryBuffer.Append(KStringEqual);
    queryBuffer.AppendNum(aLookupItem.iUid);
    queryBuffer.Append(KStringAnd);
    queryBuffer.Append(NColSource);
    queryBuffer.Append(KStringEqual);
    queryBuffer.AppendNum(aLookupItem.iSource);

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
        myView.DeleteL();
    }

    CleanupStack::PopAndDestroy(&myView); // myView
    iItemsDatabase.Commit();
    // compacts the databse, by physicaly removing deleted data.
    iItemsDatabase.Compact();
}

// -----------------------------------------------------------------------------
// CLookupDatabase::FindEntryL()
// Finds an entry in the lookup table.
// -----------------------------------------------------------------------------
//
TBool CLookupDatabase::FindEntryL(TLookupItem& aLookupItem)
{
    __TRACE_CALLSTACK;// used to store return value
    TBool retVal = EFalse;

    // Create a query to find the item.
    TFileName queryBuffer;
    queryBuffer.Copy(KSelectAllFrom);
    queryBuffer.Append(KLookupTable);
    queryBuffer.Append(KStringWhere);
    queryBuffer.Append(NColUid);
    queryBuffer.Append(KStringEqual);
    queryBuffer.AppendNum(aLookupItem.iUid);
    queryBuffer.Append(KStringAnd);
    queryBuffer.Append(NColSource);
    queryBuffer.Append(KStringEqual);
    queryBuffer.AppendNum(aLookupItem.iSource);

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

        aLookupItem.iUid = myView.ColUint(KColumnUid);
        aLookupItem.iSource = myView.ColUint(KColumnSource);
        aLookupItem.iLmId = myView.ColUint(KColumnLmkUid);
        retVal = ETrue;
    }

    CleanupStack::PopAndDestroy(&myView); // myView

    return retVal;
}

// -----------------------------------------------------------------------------
// CLookupDatabase::FindEntriesByLandmarkIdL()
// Finds a list of lookup items given a landmark uid.
// -----------------------------------------------------------------------------
//
void CLookupDatabase::FindEntriesByLandmarkIdL(const TUint32 aLandmarkId,
        RArray<TLookupItem>& aLookupItemArray)
{
    __TRACE_CALLSTACK;// Create a query to find the item.
    TFileName queryBuffer;
    queryBuffer.Copy(KSelectAllFrom);
    queryBuffer.Append(KLookupTable);
    queryBuffer.Append(KStringWhere);
    queryBuffer.Append(NColLmUid);
    queryBuffer.Append(KStringEqual);
    queryBuffer.AppendNum(aLandmarkId);

    // Create a view of the table with the above query.
    RDbView myView;
    myView.Prepare(iItemsDatabase, TDbQuery(queryBuffer));
    CleanupClosePushL(myView);
    myView.EvaluateAll();
    myView.FirstL();

    while (myView.AtRow())
    {
        // Item found. get the details.
        myView.GetL();
        TLookupItem newItem;
        newItem.iUid = myView.ColUint(KColumnUid);
        newItem.iSource = myView.ColUint(KColumnSource);
        newItem.iLmId = myView.ColUint(KColumnLmkUid);
        aLookupItemArray.Append(newItem);
        myView.NextL();
    }

    CleanupStack::PopAndDestroy(&myView); // myView
}

// -----------------------------------------------------------------------------
// CLookupDatabase::FindEntriesByLandmarkIdL()
// Finds a list of lookup items given a landmark uid.
// -----------------------------------------------------------------------------
//
void CLookupDatabase::FindEntriesBySourceTypeL(const TUint32 aSourceType,
        RArray<TLookupItem>& aLookupItemArray)
{
    __TRACE_CALLSTACK;// Create a query to find the item.
    TFileName queryBuffer;
    queryBuffer.Copy(KSelectAllFrom);
    queryBuffer.Append(KLookupTable);
    queryBuffer.Append(KStringWhere);
    queryBuffer.Append(NColSource);
    queryBuffer.Append(KStringEqual);
    queryBuffer.AppendNum(aSourceType);

    // Create a view of the table with the above query.
    RDbView myView;
    myView.Prepare(iItemsDatabase, TDbQuery(queryBuffer));
    CleanupClosePushL(myView);
    myView.EvaluateAll();
    myView.FirstL();

    while (myView.AtRow())
    {
        // Item found. get the details.
        myView.GetL();

        TLookupItem newItem;
        newItem.iUid = myView.ColUint(KColumnUid);
        newItem.iSource = myView.ColUint(KColumnSource);
        newItem.iLmId = myView.ColUint(KColumnLmkUid);
        aLookupItemArray.Append(newItem);
        myView.NextL();
    }

    CleanupStack::PopAndDestroy(&myView); // myView
}

// End of file

