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
#include <QString>
#include <QFile>

#include <bautils.h>
#include "mylocationlogger.h"
#include "lookupmaptiledb.h"

_LIT( KSelectAllFrom, "SELECT * FROM " );
_LIT(KQueryToDB,"SELECT * FROM cntmaptilelookuptable WHERE cntuid = %d AND source = %d");
_LIT( KSelectfilepathFrom, "SELECT filepath FROM " );
_LIT(KQueryMaptile, "SELECT filepath FROM cntmaptilelookuptable WHERE filepath = '%S'");
// string 'where'
_LIT( KStringWhere, " WHERE " );
// string ' = '
_LIT( KStringEqual, " = " );

_LIT(KQueryByMaptileFetchingState,"SELECT * FROM cntmaptilelookuptable WHERE fetchingstatus = %d");

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
    
    //MK 
    // add map tile fetching status to the db
    columns->AddL(TDbCol(MapTileFetchingStatus, EDbColUint32));
    

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
    myView.SetColL(KColumnMapTileFetchingStatus, aLookupItem.iFetchingStatus); //MK

    myView.PutL();

    CleanupStack::PopAndDestroy(&myView); // myView
    iItemsDatabase.Commit();
    Close();
}

// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::ReSetEntryL()
// Reset the entry with null value and get the used maptile path as part of aLookupItem.
// -----------------------------------------------------------------------------
//
void CLookupMapTileDatabase::ReSetEntryL(TLookupItem &aLookupItem)
{
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
        myView.GetL();
        aLookupItem.iFilePath.Copy(myView.ColDes16(KColumnFilePath));
        // found the entry. update it.
        myView.UpdateL();
        myView.SetColL(KColumnFilePath, KNullDesC);
        myView.SetColL(KColumnMapTileFetchingStatus,
                aLookupItem.iFetchingStatus); //MK
        myView.PutL();
    }

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
        myView.SetColL(KColumnMapTileFetchingStatus, aLookupItem.iFetchingStatus); //MK
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
void CLookupMapTileDatabase::DeleteEntryL(TLookupItem& aLookupItem)
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
            aLookupItem.iFilePath.Copy(myView.ColDes16(KColumnFilePath));
            
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
// CLookupMapTileDatabase::DeleteMapTileL()
// Deletes an maptile if there's no reference to maptile in lookupdb
// -----------------------------------------------------------------------------
//
void CLookupMapTileDatabase::DeleteMapTileL( const TLookupItem& aLookupItem)
{
    __TRACE_CALLSTACK;// Create the query to find the row to be deleted.

    TFileName queryBuffer;    
    queryBuffer.Format(KQueryMaptile, &aLookupItem.iFilePath);
    
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

    // Delete if no reference to maptile
    if (!myView.AtRow())
    {
        QString filePath =  QString::fromUtf16( aLookupItem.iFilePath.Ptr(), aLookupItem.iFilePath.Length() );
        //delete all releted  maptile 
        QString temp=filePath;
        temp.append(MAPTILE_IMAGE_PORTRAIT);       
        QFile file;
        file.remove(temp);
        
        temp=filePath;
        temp.append(MAPTILE_IMAGE_CONTACT);
        temp.append(MAPTILE_IMAGE_LANDSCAPE);
        file.remove(temp);
        
        temp=filePath;
        temp.append(MAPTILE_IMAGE_CALENDAR);
        temp.append(MAPTILE_IMAGE_LANDSCAPE);
        file.remove(temp);
        
        temp=filePath;
        temp.append(MAPTILE_IMAGE_HURRIGANES);         
        file.remove(temp);
         
       // ret = iFsSession.Delete(aLookupItem.iFilePath);     
    }
        
    CleanupStack::PopAndDestroy(&myView); // myView

    Close();
}


// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::FindEntriesByMapTileFetchingStatusL()
// Finds a list of lookup items given a landmark uid.
// -----------------------------------------------------------------------------
//
void CLookupMapTileDatabase::FindEntriesByMapTileFetchingStateL(const TUint32 aFetchingState,
        RArray<TLookupItem>& aLookupItemArray)
{
    __TRACE_CALLSTACK;// Create a query to find the item.
    TFileName queryBuffer;
    queryBuffer.Format(KQueryByMaptileFetchingState,aFetchingState);
    
    TInt ret = Open();
        if (ret != KErrNone)
        {
           Close();
           ret= Open();
           
        }
      iItemsDatabase.Begin();

    
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
        aLookupItemArray.Append(newItem);
        myView.NextL();
    }

    CleanupStack::PopAndDestroy(&myView); // myView
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
        retVal = ETrue;
    }

    CleanupStack::PopAndDestroy(&myView); // myView
    Close();
    return retVal;
}

// -----------------------------------------------------------------------------
// CLookupMapTileDatabase::FindEntryByFIlePathLL()
// Finds an entry in the lookup table for maptile image file
// -----------------------------------------------------------------------------
//
TBool CLookupMapTileDatabase::FindEntryByFilePathL(const TDesC& aFilePath)
{
    __TRACE_CALLSTACK;// used to store return value
    TBool retVal = EFalse;
   
    TInt ret = Open();
    if (ret != KErrNone)
    {
        Close();
        Open();
       
    }
    
    iItemsDatabase.Begin();
    
    // Create a query to find the item.
    TFileName queryBuffer;
    _LIT(KFormatSpec, "%S%S%S%S%S'%S'");
    
    queryBuffer.Format(KFormatSpec, 
        &KSelectfilepathFrom(), 
        &KMapTileLookupTable(),
        &KStringWhere(),
        &NCntColFilePath(),
        &KStringEqual(),
        &aFilePath);
    
    // Create a view of the table with the above query.
    RDbView myView;
    TInt retPrep = myView.Prepare(iItemsDatabase, TDbQuery(queryBuffer));
    CleanupClosePushL(myView);
    myView.EvaluateAll();
    myView.FirstL();

    if (myView.AtRow())
    {
        // Item found, return true
        retVal = ETrue;
    }

    CleanupStack::PopAndDestroy(&myView); // myView
    Close();

    return retVal;
}

// End of file

