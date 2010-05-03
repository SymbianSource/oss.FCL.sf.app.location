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
* Description: LocationPickerDataManager implementation
*
*/

#include "locationpickerdatamanager.h"
#include "locationpickerdatamanager_p.h"


// ----------------------------------------------------------------------------
// LocationPickerDataManager::LocationPickerDataManager()
// ----------------------------------------------------------------------------

LocationPickerDataManager::LocationPickerDataManager() :
    d_ptr( new LocationPickerDataManagerPrivate() )
{
}

// ----------------------------------------------------------------------------
// LocationPickerDataManager::LocationPickerDataManager()
// ----------------------------------------------------------------------------

LocationPickerDataManager::LocationPickerDataManager(
        QStandardItemModel &aModel, TViewType aViewType ) :
        d_ptr( new LocationPickerDataManagerPrivate( aModel, aViewType) )
{
}


// ----------------------------------------------------------------------------
// LocationPickerDataManager::~LocationPickerDataManager()
// ----------------------------------------------------------------------------
LocationPickerDataManager::~LocationPickerDataManager()
{
    delete d_ptr;
}

// ----------------------------------------------------------------------------
// LocationPickerDataManager::populateModel()
// ----------------------------------------------------------------------------
bool LocationPickerDataManager::populateModel(Qt::Orientations aOrientation, quint32 aCollectionId)
{   
    Q_D( LocationPickerDataManager);
    return( d->populateModel(aOrientation, aCollectionId) );
}


// ----------------------------------------------------------------------------
// LocationPickerDataManager::getData()
// ----------------------------------------------------------------------------

void LocationPickerDataManager::getData( int aIndex, quint32& aValue )
{
    Q_D( LocationPickerDataManager);
    return( d->getData( aIndex, aValue ) );
}

// ----------------------------------------------------------------------------
// LocationPickerDataManager::getLocationItem()
// ----------------------------------------------------------------------------

void LocationPickerDataManager::getLocationItem( quint32 aLmId, QLocationPickerItem& aItem )
{
    Q_D( LocationPickerDataManager);
    return( d->getLocationItem( aLmId, aItem ) );
}
