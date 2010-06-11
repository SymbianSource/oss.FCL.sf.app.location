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
* Description: location data lookup db unit test cases
*
*/

#include <QtTest/QtTest>
#include <QtGui>
#include <QString>

#include <locationdatalookupdb.h>
#include <locationservicedefines.h>


//Lookup db test interface class
class LocationDataLookupDbTest: public QObject
{
    Q_OBJECT
    
public: 
    void fillLookupItem( QLookupItem &aItem );
    
private slots:
    
    void testLocationDataLookupDb();
    void testNegative(); 
    void testCase1(); 
    void testCase2();
    void testFindEntriesByLandmarkId();
    void testGetEntries();
};


//Checks whether location data lookup db created.
void LocationDataLookupDbTest::testLocationDataLookupDb()
{
    LocationDataLookupDb* lDb = new LocationDataLookupDb();   
    QVERIFY( lDb != NULL );
    delete lDb;
}

//Checks the create entry function
void LocationDataLookupDbTest::testNegative()
{
    LocationDataLookupDb* lDb = new LocationDataLookupDb();
    QLookupItem item;
    QList<QLookupItem> itemArray;
    lDb->createEntry( item );
    lDb->updateEntryById( item );
    lDb->updateEntryBySourceIdAndType( item );
    lDb->updateMaptileBySourceIdAndType( item.mSourceUid, item.mSourceType, "" );
    lDb->deleteEntryBySourceIdAndType( item );
    lDb->getEntries( itemArray );
    QVERIFY( itemArray.count() == 0 );
    lDb->findEntriesByLandmarkId( item.mDestId, itemArray );
    QVERIFY( itemArray.count() == 0 );
    bool returnFlag = lDb->findEntryBySourceIdAndType( item );
    QVERIFY( returnFlag == false );
    returnFlag = lDb->findEntryById( item );
    QVERIFY( returnFlag == false );
    lDb->close();
    delete lDb;
}

// tests open(), createEntry(), findEntryById(), findEntryBySourceIdAndType(),
// deleteEntryBySourceIdAndType(), close() apis
void LocationDataLookupDbTest::testCase1()
{
    LocationDataLookupDb* lDb = new LocationDataLookupDb();
    bool flag = lDb->open();
    QVERIFY( flag == true );
    
    flag == false;
    flag = lDb->open();
    QVERIFY( flag == true );
    
    QLookupItem item1, item2;    

    // find a lookup item with invalid id. return value should be false
    item2.mId = 0;
    flag = true;
    flag = lDb->findEntryById( item2 );
    QVERIFY( flag == false );

    // find a lookup item using source id and type. Item not found condition verified
    item2.mSourceType = ESourceCalendar;
    item2.mSourceUid = 0;
    flag = true;
    flag = lDb->findEntryBySourceIdAndType( item2 );
    QVERIFY( flag == false );

    // add an item1, check the mId is valid
    fillLookupItem( item1 );
    lDb->createEntry( item1 );
    QVERIFY( item1.mId != 0 );

    // Find entry by id. Try to find the entry just added.
    item2.mId = item1.mId;
    flag = false;
    flag = lDb->findEntryById( item2 );
    QVERIFY( flag == true );
    
    // Find entry by source id and type. Try to find the entry just added.
    item2.mSourceType = item1.mSourceType;
    item2.mSourceUid = item1.mSourceUid;
    flag = false;
    flag = lDb->findEntryBySourceIdAndType( item2 );
    QVERIFY( flag == true );    
    
    // delete the entry just added
    item2.mSourceType = item1.mSourceType;
    item2.mSourceUid = item1.mSourceUid;
    lDb->deleteEntryBySourceIdAndType( item2 );
    
    // verify that the deleted entry is not found.
    flag = true;
    flag = lDb->findEntryById( item1 );
    QVERIFY( flag == false );
    
    lDb->close();
    delete lDb;    
}

// tests updateEntryBySourceIdAndType(), updateEntryById(),
// updateMaptileBySourceIdAndType() apis
void LocationDataLookupDbTest::testCase2()
{
    LocationDataLookupDb* lDb = new LocationDataLookupDb();
    bool flag = lDb->open();
    QVERIFY( flag == true );
    
    QLookupItem item1, item2;    

    // add an item1, check the mId is valid
    fillLookupItem( item1 );
    lDb->createEntry( item1 );
    QVERIFY( item1.mId != 0 );

    // Find entry by id. Try to find the entry just added.
    item2.mId = item1.mId;
    item2.mName = "new name";
    lDb->updateEntryById( item2 );
    flag = false;
    flag = lDb->findEntryById( item1 );
    QVERIFY( flag == true );
    QVERIFY( item1.mName == "new name" );
    
    item1.mName = "new name2";
    lDb->updateEntryBySourceIdAndType( item1 );
    lDb->updateMaptileBySourceIdAndType( item1.mSourceUid,
            item1.mSourceType, "new maptile" );
    
    flag = false;
    flag = lDb->findEntryById( item2 );
    QVERIFY( flag == true );
    QVERIFY( item2.mName == "new name2" );
    QVERIFY( item2.mMapTilePath == "new maptile" );
    
    // delete the entry just added
    lDb->deleteEntryBySourceIdAndType( item2 );    
    
    lDb->close();
    delete lDb;
}

// tests findEntriesByLandmarkId()
void LocationDataLookupDbTest::testFindEntriesByLandmarkId()
{
    LocationDataLookupDb* lDb = new LocationDataLookupDb();
    bool flag = lDb->open();
    QVERIFY( flag == true );
    
    QLookupItem item1, item2;    

    // add an item1, check the mId is valid
    item1.mDestId = 10;
    item1.mName = "lm1";
    item1.mSourceType = ESourceCalendar;
    item1.mSourceUid = 1;
    
    item2.mDestId = 10;
    item2.mName = "lm2";
    item2.mSourceType = ESourceCalendar;
    item2.mSourceUid = 2;
    
    lDb->createEntry( item1 );
    lDb->createEntry( item2 );
    QVERIFY( item1.mId != 0 );
    QVERIFY( item2.mId != 0 );

    // Find entry by id. Try to find the entry just added.
    QList<QLookupItem> itemArray;
    lDb->findEntriesByLandmarkId( 10, itemArray );
    QVERIFY( itemArray.count() == 2 );
    QVERIFY( itemArray[0].mName == "lm1" );
    QVERIFY( itemArray[1].mName == "lm2" );

    // delete the entry just added
    lDb->deleteEntryBySourceIdAndType( item1 );    
    lDb->deleteEntryBySourceIdAndType( item2 );    
    
    lDb->close();
    delete lDb;
}

// tests getEntries()
void LocationDataLookupDbTest::testGetEntries()
{
    LocationDataLookupDb* lDb = new LocationDataLookupDb();
    bool flag = lDb->open();
    QVERIFY( flag == true );

    // Find entry by id. Try to find the entry just added.
    QList<QLookupItem> itemArray;
    lDb->getEntries( itemArray );
    QVERIFY( itemArray.count() == 0 );
    
    itemArray.clear();

    QLookupItem item;    

    // add an item1, check the mId is valid
    item.mDestId = 10;
    item.mName = "contact1";
    item.mSourceType = ESourceContactsHome;
    item.mSourceUid = 1;
    lDb->createEntry( item );

    item.mDestId = 11;
    item.mName = "contact2";
    item.mSourceType = ESourceContactsHome;
    item.mSourceUid = 2;
    lDb->createEntry( item );

    item.mDestId = 12;
    item.mName = "calendar entry";
    item.mSourceType = ESourceCalendar;
    item.mSourceUid = 2;
    lDb->createEntry( item );


    item.mDestId = 13;
    item.mName = "landmark";
    item.mSourceType = ESourceLandmarks;
    item.mSourceUid = 13;
    lDb->createEntry( item );

    lDb->getEntries( itemArray, ESourceLandmarksContactsCat );
    QVERIFY( itemArray.count() == 2 );

    itemArray.clear();
    lDb->getEntries( itemArray, ESourceLandmarksCalendarCat );
    QVERIFY( itemArray.count() == 1 );
    QVERIFY( itemArray[0].mName == "calendar entry" );

    itemArray.clear();
    lDb->getEntries( itemArray, ESourceLandmarks );
    QVERIFY( itemArray.count() == 1 );
    QVERIFY( itemArray[0].mName == "landmark" );

    itemArray.clear();
    lDb->getEntries( itemArray );
    QVERIFY( itemArray.count() == 4 );

    for( int i = 0; i < itemArray.count(); i++ )
        lDb->deleteEntryBySourceIdAndType( itemArray[i] );    
    
    lDb->close();
    delete lDb;
}


void LocationDataLookupDbTest::fillLookupItem( QLookupItem &aItem )
{
    aItem.mId = 0;
    aItem.mCity = "city";
    aItem.mCountry = "country";
    aItem.mDestId = 2;
    aItem.mIconPath = "iconpath";
    aItem.mIconType = QLookupItem::EIconTypeDefault;
    aItem.mIsDuplicate = 0;
    aItem.mLatitude = 0.1;
    aItem.mLongitude = 0.2;
    aItem.mMapTilePath = "maptilepath";
    aItem.mName = "name";
    aItem.mPostalCode = "postalcode";
    aItem.mSourceType = ESourceCalendar;
    aItem.mSourceUid = 3;
    aItem.mState = "state";
    aItem.mStreet = "street";
}

QTEST_MAIN(LocationDataLookupDbTest)
#include "ut_locationdatalookupdb.moc"


