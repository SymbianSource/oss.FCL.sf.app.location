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
* Description: LocationPickerDataManager private implementation
*
*/

#include <HbIcon>
#include <QPixmap>  
#include <QPainter>
#include <QIcon>
#include <locationdatalookupdb.h>
#include <QFile>
#include "locationpickerdatamanager_p.h"

//constant value used
const int ASPECTRATIOHEIGHT(3);
const int ASPECTRATIOWIDTH (4);


// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::LocationPickerDataManagerPrivate()
// ----------------------------------------------------------------------------

LocationPickerDataManagerPrivate::LocationPickerDataManagerPrivate() :
        mModel( NULL ),
        mViewType( ELocationPickerContent ),
        mDb( NULL )
{
    mDb = new LocationDataLookupDb();
    mDb->open();
}


// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::~LocationPickerDataManagerPrivate()
// ----------------------------------------------------------------------------
LocationPickerDataManagerPrivate::~LocationPickerDataManagerPrivate()
{
    // delete the member variables;
    if( mDb )
    {
        mDb->close();
        delete mDb;
        mDb = NULL;
    }
}

// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::populateModel()
// ----------------------------------------------------------------------------
bool LocationPickerDataManagerPrivate::populateModel(  QStandardItemModel &aModel, 
        TViewType aViewType, const Qt::Orientations aOrientation, quint32 aCollectionId )
{
    mModel = &aModel;
    mViewType = aViewType;
    mOrientation = aOrientation;

    if( !mDb )
    {
        // no items in the landmark database, so return false.
        return false;
    }

    switch( mViewType )
    {
        case ELocationPickerContent:
        case ELocationPickerSearchView:
             {
                 QList<QLookupItem> itemArray;
                 mDb->getEntries( itemArray );
                 return populateLandmarks( itemArray );
             }
             
        case ELocationPickerCollectionListContent:
             {
                 populateCollections();
             }
             break;

        case ELocationPickerCollectionContent:
             {
                 QList<QLookupItem> itemArray;
                 mDb->getEntries( itemArray, aCollectionId );             
                 if( itemArray.count() == 0 )
                     return false;
                 
                 return populateLandmarks( itemArray );
             }
    }
    return true;
}

// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::populateLandmarks()
// ----------------------------------------------------------------------------

bool LocationPickerDataManagerPrivate::populateLandmarks( QList<QLookupItem> &aItemArray )
{    
    mModel->clear();

    if( !aItemArray.count() )
    {
        return false;
    }
    QString lmAddressLine1;
    QString lmAddressLine2;
    
    for( int i = 0; i < aItemArray.count(); i++ )
    {
    
        if( !aItemArray[i].mIsDuplicate )
        {
            lmAddressLine1 = aItemArray[i].mName;
            if( lmAddressLine1.isEmpty() )
                lmAddressLine1 = KSpace;
            
            bool addressEmtpy = true; // used to check if address line 2 is empty
            if( !aItemArray[i].mStreet.isEmpty() )
            {
                lmAddressLine2 = aItemArray[i].mStreet;
                addressEmtpy = EFalse;
            }
            if( !aItemArray[i].mCity.isEmpty() )
            {
                if( !addressEmtpy )
                {
                    lmAddressLine2 = lmAddressLine2 + KSeparator;
                    lmAddressLine2 = lmAddressLine2 + KSpace;
                    lmAddressLine2 = lmAddressLine2 + aItemArray[i].mCity;
                }
                else
                {
                    lmAddressLine2 = aItemArray[i].mCity;
                    addressEmtpy = EFalse;
                }
            }
            if( !aItemArray[i].mState.isEmpty() )
            {
                if( !addressEmtpy )
                {
                    lmAddressLine2 = lmAddressLine2 + KSeparator;
                    lmAddressLine2 = lmAddressLine2 + KSpace;
                    lmAddressLine2 = lmAddressLine2 + aItemArray[i].mState;
                }
                else
                {
                    lmAddressLine2 = aItemArray[i].mState;
                    addressEmtpy = EFalse;
                }
            }
            if( !aItemArray[i].mCountry.isEmpty() )
            {
                if( !addressEmtpy )
                {
                    lmAddressLine2 = lmAddressLine2 + KSeparator;
                    lmAddressLine2 = lmAddressLine2 + KSpace;
                    lmAddressLine2 = lmAddressLine2 + aItemArray[i].mCountry;
                }
                else
                {
                    lmAddressLine2 = aItemArray[i].mCountry;
                    addressEmtpy = EFalse;
                }
            }
            // set icons based on contact address type
            QVariantList icons;
            
            HbIcon adressTypeIcon;
        bool adressIconPresent = false;
        if( aItemArray[i].mSourceType == ESourceContactsHome )
        {
            adressTypeIcon = HbIcon(KContactHomeIcon);
            adressIconPresent = true;
        }
        else if( aItemArray[i].mSourceType == ESourceContactsWork )
        {
            adressTypeIcon =HbIcon(KContactWorkIcon);
            adressIconPresent = true;
        }
        else if( aItemArray[i].mSourceType == ESourceContactsPref )
        {
            adressTypeIcon =HbIcon(KContactPrefIcon);
            adressIconPresent = true;
        }
        
        // create a list item and set to model
        QStringList addressData;
        //create model for grid view in landscape mode
        if( mOrientation == Qt::Horizontal && ( mViewType == ELocationPickerCollectionContent || 
                mViewType == ELocationPickerContent ) )
        {   
            addressData.clear();
            
            HbIcon landscapeIcon;
            
            if( QFile::exists( aItemArray[i].mMapTilePath ) )
            {
                //draw maptile Icon
                QPainter painter;
                QPixmap sourcePixmap;
                sourcePixmap = QPixmap( QString(aItemArray[i].mMapTilePath) );
                int mapHeight = (sourcePixmap.height()/ASPECTRATIOHEIGHT)*ASPECTRATIOHEIGHT;
                int mapWidth = mapHeight*ASPECTRATIOWIDTH/ASPECTRATIOHEIGHT;
                QPixmap mapPixmap(mapWidth, mapHeight);
                painter.begin( &mapPixmap );
                painter.drawPixmap( 0,0,sourcePixmap,(((sourcePixmap.width()-mapWidth))/2),
                        ((sourcePixmap.height()-mapHeight)/2),mapWidth,mapHeight );
                painter.end();
                if(adressIconPresent)
                {
                //draw the adressType Icon over mapTile Icon
                QPixmap adressTypePixmap = adressTypeIcon.pixmap();
                painter.begin( &mapPixmap );
                painter.drawPixmap( (mapPixmap.width()-adressTypePixmap.width()),0,adressTypePixmap );
                painter.end();
                }
                QIcon landscape( mapPixmap );
                landscapeIcon = HbIcon( landscape );
            }
            else
            {
                //draw dummy icon
                landscapeIcon = HbIcon( KDummyImage );
            }

                icons<<landscapeIcon;
                QStandardItem *modelItem = new QStandardItem();
                addressData << lmAddressLine1;
                modelItem->setData(QVariant(addressData), Qt::DisplayRole);
                modelItem->setData( icons[0], Qt::DecorationRole );
                modelItem->setData( aItemArray[i].mId, Qt::UserRole );
                mModel->appendRow( modelItem );
            }
            else
            {   
                //create model for list view in potrait mode
                addressData.clear();
                HbIcon potraitIcon( KDummyImage );
            icons<<potraitIcon;
            if(adressIconPresent)
            {
                icons<<adressTypeIcon;
            }
                QStandardItem *modelItem = new QStandardItem();
                addressData << lmAddressLine1 << lmAddressLine2;
                modelItem->setData(QVariant(addressData), Qt::DisplayRole);
                modelItem->setData( icons, Qt::DecorationRole );
                modelItem->setData( aItemArray[i].mId, Qt::UserRole );
                mModel->appendRow( modelItem );
            }
        }
    }
    
    return true;
}

// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::populateCollections()
// ----------------------------------------------------------------------------

void LocationPickerDataManagerPrivate::populateCollections()
{
    // add contact collection
    QStandardItem *modelItemContact = new QStandardItem();
    QString contactCollectionName( hbTrId("txt_lint_list_contact_addresses") );
    modelItemContact->setData( QVariant( contactCollectionName ), Qt::DisplayRole );
    modelItemContact->setData( HbIcon ( KCollectionsContacts ), Qt::DecorationRole );
    modelItemContact->setData( ESourceLandmarksContactsCat, Qt::UserRole );
    mModel->appendRow( modelItemContact );

    QStandardItem *modelItemCalendar = new QStandardItem();
    QString calendarCollectionName( hbTrId("txt_lint_list_calendar_event_locations") );
    modelItemCalendar->setData( QVariant( calendarCollectionName ), Qt::DisplayRole );
    modelItemCalendar->setData( HbIcon ( KCollectionsCalendar ), Qt::DecorationRole );
    modelItemCalendar->setData( ESourceLandmarksCalendarCat, Qt::UserRole );
    mModel->appendRow( modelItemCalendar );
    
    QStandardItem *modelItemPlaces = new QStandardItem();
    QString placesCollectionName( hbTrId("txt_lint_list_places") );
    modelItemPlaces->setData( QVariant( placesCollectionName ), Qt::DisplayRole );
    modelItemPlaces->setData( HbIcon (KCollectionsPlaces), Qt::DecorationRole );
    modelItemPlaces->setData( ESourceLandmarks, Qt::UserRole );
    mModel->appendRow( modelItemPlaces );
}


// ----------------------------------------------------------------------------
// LocationPickerDataManagerPrivate::getLocationItem()
// ----------------------------------------------------------------------------

void LocationPickerDataManagerPrivate::getLocationItem( quint32 aId, QLocationPickerItem& aItem )
{
    QLookupItem item;
    item.mId = aId;
    if( mDb->findEntryById( item ) )
    {
        aItem.mName = item.mName;
        aItem.mStreet = item.mStreet;
        aItem.mPostalCode = item.mPostalCode;
        aItem.mCity = item.mCity;
        aItem.mState = item.mState;
        aItem.mCountry = item.mCountry;
        aItem.mLatitude = item.mLatitude;
        aItem.mLongitude = item.mLongitude;
        aItem.mIsValid = true;
    }
    else
    {
        aItem.mIsValid = false;
    }
    
}

