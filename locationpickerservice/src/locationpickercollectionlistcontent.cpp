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
* Description: LocationPickerCollectionListContent implementation
*
*/

#include <QStandardItemModel>

#include "locationpickercollectionlistcontent.h"
#include "locationpickerdatamanager.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------
// LocationPickerCollectionListContent::LocationPickerCollectionListContent()
// ----------------------------------------------------------------------
LocationPickerCollectionListContent::LocationPickerCollectionListContent(Qt::Orientation aOrientation )
    :mOrientation(aOrientation),
    mModel(NULL),
    mDataManager(NULL)
{
    // Create a standard model for the view list
    mModel = new QStandardItemModel( this );
    
    mDataManager = new LocationPickerDataManager( *mModel, ELocationPickerCollectionListContent );
    mDataManager->populateModel( mOrientation );
}

// ----------------------------------------------------------------------
// LocationPickerCollectionListContent::~LocationPickerCollectionListContent()
// ----------------------------------------------------------------------
LocationPickerCollectionListContent::~LocationPickerCollectionListContent()
{
    if( mDataManager )
        delete mDataManager;
    delete mModel;
}

// ----------------------------------------------------------------
// LocationPickerCollectionListContent::getStandardModel
// -----------------------------------------------------------------
QStandardItemModel* LocationPickerCollectionListContent::getStandardModel()
    {
    return mModel;
    }

// ----------------------------------------------------------------
// LocationPickerCollectionListContent::getDataManager
// -----------------------------------------------------------------
LocationPickerDataManager* LocationPickerCollectionListContent::getDataManager()
    {
    return mDataManager;
    }

