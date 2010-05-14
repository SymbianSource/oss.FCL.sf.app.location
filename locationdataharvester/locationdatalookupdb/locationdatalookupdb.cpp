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
* Description: Location data lookup db implementation
*
*/

// INCLUDE FILES
#include "locationdatalookupdb.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QVariant>
#include<locationservicedefines.h>

// database name
const QString KLocationDataLookupDbName = "c:\\locationdatalookupdb.db"; 

// ================= MEMBER FUNCTIONS =======================
//
// ---------------------------------------------------------
// LocationDataLookupDb::LocationDataLookupDb()
// Constructor
// ---------------------------------------------------------
//
LocationDataLookupDb::LocationDataLookupDb( QObject *parent) : QObject(parent)
{
    mDb = new QSqlDatabase();
    *mDb = QSqlDatabase::addDatabase( "QSQLITE" );
    mDb->setDatabaseName( KLocationDataLookupDbName );
    if (!mDb->open())
    {
        delete mDb;
        mDb = NULL;
        return;
    }

    // create lookup table if doesnot exist
    QSqlQuery query(*mDb);
    query.exec("create table if not exists lplookup (id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                             "duplicate int,"
                                             "sourceid int,"
                                             "sourcetype int,"
                                             "destid int,"
                                             "name varchar(255),"
                                             "street varchar(255),"
                                             "postal varchar(50),"
                                             "city varchar(50),"
                                             "state varchar(50),"
                                             "country varchar(50),"
                                             "latitude double,"
                                             "longitude double,"
                                             "icontype int,"
                                             "iconpath varchar(255),"
                                             "maptile varchar(255))");

    mDb->close();
}

// ---------------------------------------------------------
// LocationDataLookupDb::~LocationDataLookupDb()
// destructor
// ---------------------------------------------------------
LocationDataLookupDb::~LocationDataLookupDb()
{
    if( mDb )
        delete mDb;
}

// ---------------------------------------------------------
// LocationDataLookupDb::open()
// ---------------------------------------------------------
bool LocationDataLookupDb::open()
{
    return mDb->open();
}

// ---------------------------------------------------------
// LocationDataLookupDb::close()
// ---------------------------------------------------------
void LocationDataLookupDb::close()
{
    if( mDb )
        mDb->close();
}

// ---------------------------------------------------------
// LocationDataLookupDb::createEntry()
// ---------------------------------------------------------
void LocationDataLookupDb::createEntry( const QLookupItem& aLookupItem )
{
    if( mDb )
    {
        QSqlQuery query(*mDb);
        query.prepare("INSERT INTO lplookup ("
                              "id, "
                              "duplicate, "
                              "sourceid, "
                              "sourcetype, "
                              "destid, "
                              "name, "
                              "street, "
                              "postal, "
                              "city, "
                              "state, "
                              "country, "
                              "latitude, "
                              "longitude, "
                              "icontype, "
                              "iconpath, "
                              "maptile) "
                      "VALUES ("
                              "NULL, "
                              ":duplicate, "
                              ":sourceid, "
                              ":sourcetype, "
                              ":destid, "
                              ":name, "
                              ":street, "
                              ":postal, "
                              ":city, "
                              ":state, "
                              ":country, "
                              ":latitude, "
                              ":longitude, "
                              ":icontype, "
                              ":iconpath, "
                              ":maptile)" );
        query.bindValue(":duplicate", aLookupItem.mIsDuplicate);
        query.bindValue(":sourceid", aLookupItem.mSourceUid);
        query.bindValue(":sourcetype", aLookupItem.mSourceType);
        query.bindValue(":destid", aLookupItem.mDestId);
        query.bindValue(":name", aLookupItem.mName);
        query.bindValue(":street", aLookupItem.mStreet);
        query.bindValue(":postal", aLookupItem.mPostalCode);
        query.bindValue(":city", aLookupItem.mCity);
        query.bindValue(":state", aLookupItem.mState);
        query.bindValue(":country", aLookupItem.mCountry);
        query.bindValue(":latitude", aLookupItem.mLatitude);
        query.bindValue(":longitude", aLookupItem.mLongitude);
        query.bindValue(":icontype", aLookupItem.mIconType);
        query.bindValue(":iconpath", aLookupItem.mIconPath);
        query.bindValue(":maptile", aLookupItem.mMapTilePath);
        query.exec();
    }
}

// ---------------------------------------------------------
// LocationDataLookupDb::updateEntryBySourceIdAndType()
// ---------------------------------------------------------
void LocationDataLookupDb::updateEntryBySourceIdAndType( const QLookupItem& aLookupItem )
{
    if( mDb )
    {
        QSqlQuery query(*mDb);
        query.prepare("UPDATE lplookup SET "
                        "id = ?, "
                        "duplicate = ?, "
                        "sourceid = ?, "
                        "sourcetype = ?, "
                        "destid = ?, "
                        "name = ?, "
                        "street = ?, "
                        "postal = ?, "
                        "city = ?, "
                        "state = ?, "
                        "country = ?, "
                        "latitude = ?, "
                        "longitude = ?, "
                        "icontype = ?, "
                        "iconpath = ?, "
                        "maptile = ? "
                      "WHERE sourceid = ? AND sourcetype = ?");
    
        query.addBindValue( aLookupItem.mId );
        query.addBindValue( aLookupItem.mIsDuplicate );
        query.addBindValue( aLookupItem.mSourceUid );
        query.addBindValue( aLookupItem.mSourceType );
        query.addBindValue( aLookupItem.mDestId );
        query.addBindValue( aLookupItem.mName );
        query.addBindValue( aLookupItem.mStreet );
        query.addBindValue( aLookupItem.mPostalCode );
        query.addBindValue( aLookupItem.mCity );
        query.addBindValue( aLookupItem.mState );
        query.addBindValue( aLookupItem.mCountry );
        query.addBindValue( aLookupItem.mLatitude );
        query.addBindValue( aLookupItem.mLongitude );
        query.addBindValue( aLookupItem.mIconType );
        query.addBindValue( aLookupItem.mIconPath );
        query.addBindValue( aLookupItem.mMapTilePath );
    
        query.addBindValue( aLookupItem.mSourceUid );
        query.addBindValue( aLookupItem.mSourceType );
    
        query.exec();
    }
}

// ---------------------------------------------------------
// LocationDataLookupDb::updateMaptileBySourceIdAndType()
// ---------------------------------------------------------
void LocationDataLookupDb::updateMaptileBySourceIdAndType( quint32 aSourceId, 
        quint32 aSourceType, QString aImagePath )
{
    if( mDb )
    {
        QSqlQuery query(*mDb);
        query.prepare("UPDATE lplookup SET "
                        "maptile = ? "
                      "WHERE sourceid = ? AND sourcetype = ?");
    
        query.addBindValue( aImagePath );
        query.addBindValue( aSourceId );
        query.addBindValue( aSourceType );
    
        query.exec();
    }
}

// ---------------------------------------------------------
// LocationDataLookupDb::updateEntryById()
// ---------------------------------------------------------
void LocationDataLookupDb::updateEntryById( const QLookupItem& aLookupItem )
{
    if( mDb )
    {
        QSqlQuery query(*mDb);
        query.prepare("UPDATE lplookup SET "
                        "id = ?, "
                        "duplicate = ?, "
                        "sourceid = ?, "
                        "sourcetype = ?, "
                        "destid = ?, "
                        "name = ?, "
                        "street = ?, "
                        "postal = ?, "
                        "city = ?, "
                        "state = ?, "
                        "country = ?, "
                        "latitude = ?, "
                        "longitude = ?, "
                        "icontype = ?, "
                        "iconpath = ?, "
                        "maptile = ? "
                      "WHERE id = ?");
    
        query.addBindValue( aLookupItem.mId );
        query.addBindValue( aLookupItem.mIsDuplicate );
        query.addBindValue( aLookupItem.mSourceUid );
        query.addBindValue( aLookupItem.mSourceType );
        query.addBindValue( aLookupItem.mDestId );
        query.addBindValue( aLookupItem.mName );
        query.addBindValue( aLookupItem.mStreet );
        query.addBindValue( aLookupItem.mPostalCode );
        query.addBindValue( aLookupItem.mCity );
        query.addBindValue( aLookupItem.mState );
        query.addBindValue( aLookupItem.mCountry );
        query.addBindValue( aLookupItem.mLatitude );
        query.addBindValue( aLookupItem.mLongitude );
        query.addBindValue( aLookupItem.mIconType );
        query.addBindValue( aLookupItem.mIconPath );
        query.addBindValue( aLookupItem.mMapTilePath );
    
        query.addBindValue( aLookupItem.mId );
    
        query.exec();
    }
}

// ---------------------------------------------------------
// LocationDataLookupDb::deleteEntryBySourceIdAndType()
// ---------------------------------------------------------
void LocationDataLookupDb::deleteEntryBySourceIdAndType( const QLookupItem& aLookupItem )
{
    if( mDb )
    {
        QSqlQuery query(*mDb);
        query.prepare( "DELETE FROM lplookup "
                       "WHERE sourceid = ? AND sourcetype = ?" );
    
        query.addBindValue( aLookupItem.mSourceUid );
        query.addBindValue( aLookupItem.mSourceType );
    
        query.exec();
    }
}

// ---------------------------------------------------------
// LocationDataLookupDb::deleteEntryById()
// ---------------------------------------------------------
void LocationDataLookupDb::deleteEntryById( const QLookupItem& aLookupItem )
{
    if( mDb )
    {
        QSqlQuery query(*mDb);
        query.prepare( "DELETE FROM lplookup "
                       "WHERE id = ?" );
    
        query.addBindValue( aLookupItem.mId );
        query.exec();
    }
}

// ---------------------------------------------------------
// LocationDataLookupDb::findEntryBySourceIdAndType()
// ---------------------------------------------------------
bool LocationDataLookupDb::findEntryBySourceIdAndType( QLookupItem& aLookupItem )
{
    if( mDb )
    {
        QSqlQuery query(*mDb);
        query.prepare( "SELECT * FROM lplookup "    
                    "WHERE sourceid = ? AND sourcetype = ?" );
        query.addBindValue( aLookupItem.mSourceUid );
        query.addBindValue( aLookupItem.mSourceType );
    
        query.exec();
        
        if( !query.first() )
        {
            return false;
        }
    
        fillLookupEntry( query, aLookupItem );
        return true;
    }
    return false;
}

// ---------------------------------------------------------
// LocationDataLookupDb::findEntryById()
// ---------------------------------------------------------
bool LocationDataLookupDb::findEntryById( QLookupItem& aLookupItem )
{
    if( mDb )
    {
        QSqlQuery query(*mDb);
        query.prepare( "SELECT * FROM lplookup "    
                    "WHERE id = ?" );
        query.addBindValue( aLookupItem.mId );
    
        query.exec();
        
        if( !query.first() )
        {
            return false;
        }
    
        fillLookupEntry( query, aLookupItem );    
        return true;
    }
    
    return false;
}


// ---------------------------------------------------------
// LocationDataLookupDb::findEntriesByLandmarkId()
// ---------------------------------------------------------
void LocationDataLookupDb::findEntriesByLandmarkId( const quint32 aLandmarkId, 
        QList<QLookupItem>& aLookupItemArray )
{
    if( mDb )
    {

        QSqlQuery query(*mDb);
        query.prepare( "SELECT * FROM lplookup "    
                       "WHERE destid = ?" );
    
        query.addBindValue( aLandmarkId );
        query.exec();
        
        while( query.next() )
        {    
            QLookupItem lookupItem;
            fillLookupEntry( query, lookupItem );        
            aLookupItemArray.append( lookupItem );
        } 
    }   
}

// ---------------------------------------------------------
// LocationDataLookupDb::findEntriesBySourceType()
// ---------------------------------------------------------
void LocationDataLookupDb::findEntriesBySourceType( const quint32 aSourceType, 
        QList<QLookupItem>& aLookupItemArray )
{
    if( mDb )
    {
        QSqlQuery query(*mDb);
        query.prepare( "SELECT * FROM lplookup "    
                       "WHERE sourceType = ?" );
        query.addBindValue( aSourceType );
        query.exec();
       
        while( query.next() )
        {    
            QLookupItem lookupItem;
            fillLookupEntry( query, lookupItem );
            aLookupItemArray.append( lookupItem );
        }
    }
}

// ---------------------------------------------------------
// LocationDataLookupDb::getEntries()
// ---------------------------------------------------------
void LocationDataLookupDb::getEntries( QList<QLookupItem>& aLookupItemArray, const quint32 aCollectionId )
{
    if( mDb )
    {
        QSqlQuery query(*mDb);
        if( aCollectionId == ESourceLandmarksContactsCat )
        {
            query.prepare( "SELECT * FROM lplookup " 
                    "WHERE sourcetype = ? OR sourcetype = ? OR sourcetype = ?" );
            query.addBindValue( ESourceContactsPref );
            query.addBindValue( ESourceContactsWork );
            query.addBindValue( ESourceContactsHome );
            query.exec();
        }
        else if( aCollectionId == ESourceLandmarksCalendarCat )
        {
            query.prepare( "SELECT * FROM lplookup " 
                    "WHERE sourcetype = ?" );
            query.addBindValue( ESourceCalendar );
            query.exec();
           
        }
        else if(  aCollectionId == ESourceLandmarks )
        {
            query.prepare( "SELECT * FROM lplookup " 
                    "WHERE sourcetype = ?" );
            query.addBindValue( ESourceLandmarks );
            query.exec();
           
        }
        else // all contents
        {
            query.prepare( "SELECT * FROM lplookup" );
            query.exec();
        }
        while( query.next() )
        {    
            QLookupItem lookupItem;            
            fillLookupEntry( query, lookupItem );
            aLookupItemArray.append( lookupItem );
        }    
    }
}

void LocationDataLookupDb::fillLookupEntry( QSqlQuery &aQuery, QLookupItem &aLookupItem )
{
    QSqlRecord rec = aQuery.record();
    aLookupItem.mId = aQuery.value( rec.indexOf("id") ).toUInt();
    aLookupItem.mIsDuplicate = aQuery.value( rec.indexOf("duplicate") ).toUInt();
    aLookupItem.mSourceUid = aQuery.value( rec.indexOf("sourceid") ).toUInt();
    aLookupItem.mSourceType = aQuery.value( rec.indexOf("sourcetype") ).toUInt();
    aLookupItem.mDestId = aQuery.value( rec.indexOf("destid") ).toUInt();
    aLookupItem.mName = aQuery.value( rec.indexOf("name") ).toString();
    aLookupItem.mStreet = aQuery.value( rec.indexOf("street") ).toString();
    aLookupItem.mPostalCode = aQuery.value( rec.indexOf("postal") ).toString();
    aLookupItem.mCity = aQuery.value( rec.indexOf("city") ).toString();
    aLookupItem.mState = aQuery.value( rec.indexOf("state") ).toString();
    aLookupItem.mCountry = aQuery.value( rec.indexOf("country") ).toString();
    aLookupItem.mLatitude = aQuery.value( rec.indexOf("latitude") ).toDouble();
    aLookupItem.mLongitude = aQuery.value( rec.indexOf("longitude") ).toDouble();
    aLookupItem.mIconType = aQuery.value( rec.indexOf("icontype") ).toUInt();
    aLookupItem.mIconPath = aQuery.value( rec.indexOf("iconpath") ).toString();
    aLookupItem.mMapTilePath = aQuery.value( rec.indexOf("maptile") ).toString();

}

// End of file
