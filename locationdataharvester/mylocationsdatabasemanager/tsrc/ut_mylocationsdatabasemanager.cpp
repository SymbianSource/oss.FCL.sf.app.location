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
* Description: mylocations database manager unit test cases
*
*/

#include <QtTest/QtTest>
#include <QtGui>
#include <QString>

#include <mylocationsdatabasemanager.h>
#include <locationservicedefines.h>
#include <EPos_CPosLandmark.h>
#include <lbsposition.h>


//Lookup db test interface class
class CMyLocationsDatabaseManagerTest: public QObject
{
    Q_OBJECT
        
private slots:
    
    void testCase1();
    void testCase2();
    void testCase3();
    void testCase4();
};


//Tests basic construction and destruction
void CMyLocationsDatabaseManagerTest::testCase1()
{
    // tests just new and delete operation.
	CMyLocationsDatabaseManager* dbm = new CMyLocationsDatabaseManager();
	QVERIFY( dbm != NULL );
    delete dbm;
    dbm = NULL;

    // tests ConstructL()
    dbm = new CMyLocationsDatabaseManager();
	dbm->ConstructL();
	QVERIFY( dbm->iLocationAppLookupDb != NULL );
	delete dbm;
}

//Tests UpdateDatabaseL()
void CMyLocationsDatabaseManagerTest::testCase2()
{
    // tests just new and delete operation.
    CMyLocationsDatabaseManager* dbm = new CMyLocationsDatabaseManager();
    QVERIFY( dbm != NULL );
    dbm->ConstructL();
    
    CPosLandmark* lm = CPosLandmark::NewL();
    lm->SetLandmarkNameL(_L("John"));
    lm->SetPositionFieldL( EPositionFieldStreet, _L("MG Road") );
    TLocality loc( TCoordinate( 0.1, 0.2 ), 0 );
    lm->SetPositionL(loc);
    
    // test add
    dbm->UpdateDatabaseL( lm, 1, ESourceContactsPref, EEntryAdded );
    QLookupItem item;
    item.mSourceType = ESourceContactsPref;
    item.mSourceUid = 1;
    bool flag = dbm->iLocationAppLookupDb->findEntryBySourceIdAndType( item );
    QVERIFY( flag == true );
    QVERIFY( item.mStreet == "MG Road" );
    
    // test modify
    lm->SetPositionFieldL( EPositionFieldStreet, _L("Brigade Road") );
    dbm->UpdateDatabaseL( lm, 1, ESourceContactsPref, EEntryModified );
    flag = dbm->iLocationAppLookupDb->findEntryBySourceIdAndType( item );
    QVERIFY( flag == true );
    QVERIFY( item.mStreet == "Brigade Road" );

    dbm->UpdateDatabaseL( lm, 1, ESourceContactsPref, EEntryModified );
    flag = dbm->iLocationAppLookupDb->findEntryBySourceIdAndType( item );
    QVERIFY( flag == true );
    QVERIFY( item.mStreet == "Brigade Road" );

    // test delete
    dbm->UpdateDatabaseL( NULL, 1, ESourceContactsPref, EEntryDeleted );
    flag = dbm->iLocationAppLookupDb->findEntryBySourceIdAndType( item );
    QVERIFY( flag == false );

    // check invalid
    dbm->UpdateDatabaseL( NULL, 1, ESourceContactsPref, EEntryUnknown );
    flag = dbm->iLocationAppLookupDb->findEntryBySourceIdAndType( item );
    QVERIFY( flag == false );

    delete lm;
    delete dbm;
}

//Tests GetLandmarkFullAddress() and FillLookupItemAddressDetails()
void CMyLocationsDatabaseManagerTest::testCase3()
{
    CMyLocationsDatabaseManager* dbm = new CMyLocationsDatabaseManager();
    dbm->ConstructL();
    QVERIFY( dbm->iLocationAppLookupDb != NULL );
    
    QLookupItem item;
    
    CPosLandmark* lm = CPosLandmark::NewL();
    TBuf<255> buff;
    dbm->GetLandmarkFullAddress( buff, lm );
    QString str1 = QString( (QChar*)buff.Ptr(), buff.Length());
    QVERIFY( str1 == "" );
    
    dbm->FillLookupItemAddressDetails( lm, item );
    QVERIFY( item.mName == "" );
    QVERIFY( item.mStreet == "" );
    QVERIFY( item.mPostalCode == "" );
    QVERIFY( item.mCity == "" );
    QVERIFY( item.mState == "" );
    QVERIFY( item.mCountry == "" );
    
    lm->SetLandmarkNameL( _L("name") );
    lm->SetPositionFieldL( EPositionFieldCountry, _L("india") );
    dbm->GetLandmarkFullAddress( buff, lm );
    str1 = QString( (QChar*)buff.Ptr(), buff.Length());
    QVERIFY( str1 == "india" );
    dbm->FillLookupItemAddressDetails( lm, item );
    QVERIFY( item.mName == "name" );
    QVERIFY( item.mCountry == "india" );

    lm->SetPositionFieldL( EPositionFieldState, _L("kar") );
    dbm->GetLandmarkFullAddress( buff, lm );
    str1 = QString( (QChar*)buff.Ptr(), buff.Length());
    QVERIFY( str1 == "kar, india" );
    dbm->FillLookupItemAddressDetails( lm, item );
    QVERIFY( item.mState == "kar" );

    lm->SetPositionFieldL( EPositionFieldCity, _L("blr") );
    dbm->GetLandmarkFullAddress( buff, lm );
    str1 = QString( (QChar*)buff.Ptr(), buff.Length());
    QVERIFY( str1 == "blr, kar, india" );
    dbm->FillLookupItemAddressDetails( lm, item );
    QVERIFY( item.mCity == "blr" );

    lm->SetPositionFieldL( EPositionFieldPostalCode, _L("postalcode") );
    lm->SetPositionFieldL( EPositionFieldStreet, _L("mg road") );
    dbm->GetLandmarkFullAddress( buff, lm );
    str1 = QString( (QChar*)buff.Ptr(), buff.Length());
    QVERIFY( str1 == "mg road, blr, kar, india" );
    dbm->FillLookupItemAddressDetails( lm, item );
    QVERIFY( item.mPostalCode == "postalcode" );
    QVERIFY( item.mStreet == "mg road" );
    
    lm->SetLandmarkNameL(_L(""));
    lm->SetPositionFieldL( EPositionFieldStreet, _L("") );
    lm->SetPositionFieldL( EPositionFieldPostalCode, _L("") );
    lm->SetPositionFieldL( EPositionFieldCity, _L("") );
    lm->SetPositionFieldL( EPositionFieldState, _L("") );
    lm->SetPositionFieldL( EPositionFieldCountry, _L("") );
    dbm->GetLandmarkFullAddress( buff, lm );
    str1 = QString( (QChar*)buff.Ptr(), buff.Length());
    dbm->FillLookupItemAddressDetails( lm, item );
    QVERIFY( str1 == "" );
    QVERIFY( item.mName == "" );
    QVERIFY( item.mStreet == "" );
    QVERIFY( item.mPostalCode == "" );
    QVERIFY( item.mCity == "" );
    QVERIFY( item.mState == "" );
    QVERIFY( item.mCountry == "" );
    
    delete lm;
    delete dbm;
}

//Tests UpdateMapTilePath()
void CMyLocationsDatabaseManagerTest::testCase4()
{
    // tests just new and delete operation.
    CMyLocationsDatabaseManager* dbm = new CMyLocationsDatabaseManager();
    dbm->ConstructL();
    QVERIFY( dbm->iLocationAppLookupDb != NULL );
    
    QLookupItem item;
    item.mSourceType = ESourceCalendar;
    item.mSourceUid = 1;
    dbm->iLocationAppLookupDb->createEntry( item );
    dbm->UpdateMapTilePath( item.mSourceUid, item.mSourceType, _L("maptile.png") );
    dbm->iLocationAppLookupDb->findEntryBySourceIdAndType( item );
    
    QVERIFY( item.mMapTilePath == "maptile.png" );
    
    dbm->iLocationAppLookupDb->deleteEntryBySourceIdAndType( item );
    delete dbm;
}

QTEST_MAIN(CMyLocationsDatabaseManagerTest)
#include "ut_mylocationsdatabasemanager.moc"


