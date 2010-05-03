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
* Description: LocationPickerContent implementation
*
*/

#include <QStandardItemModel>

#include "locationpickercontent.h"
#include "locationpickerproxymodel.h"
#include "locationpickertypes.h"
#include "locationpickerdatamanager.h"

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// LocationPickerContent::LocationPickerContent()
// -----------------------------------------------------------------------------
LocationPickerContent::LocationPickerContent( Qt::Orientation aOrientation )
	:mOrientation(aOrientation),
	mListProxyModel(NULL),
	mProxyGridModel(NULL),
    mDataManager(NULL),
	mModel(NULL),
	mLocationsFound(true)
{
    // Create a standard model for the list view
    mModel = new QStandardItemModel( this );
    // create data manager to manage data in the model
    mDataManager = new LocationPickerDataManager( *mModel, ELocationPickerContent );
    if(mOrientation == Qt::Vertical)
    {
        if( mDataManager->populateModel(mOrientation) )
        {
            // Create the proxy model and set source model
            mListProxyModel = new LocationPickerProxyModel( mOrientation, this );
            mListProxyModel->setSourceModel(mModel);
            // set sort properties
            mListProxyModel->setDynamicSortFilter(TRUE);
            mListProxyModel->setSortRole(Qt::DisplayRole);
            mListProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);

            // sort in ascending order
            mListProxyModel->sort(0, Qt::AscendingOrder);    
            mLocationsFound = true;
        }
        else
        {
            // no locations to display.
            QStandardItem *modelItem = new QStandardItem();
            modelItem->setData(QVariant(hbTrId("txt_lint_list_no_location_entries_present")), Qt::DisplayRole);
            mModel->appendRow( modelItem );
            mLocationsFound = false;
        }
    }
    //for landscape view
    else
    {
        if( mDataManager->populateModel(mOrientation) )
        {
            // Create the proxy model and set source model
            mProxyGridModel = new LocationPickerProxyModel( mOrientation , this );
            mProxyGridModel->setSourceModel(mModel);
            // set sort properties
            mProxyGridModel->setDynamicSortFilter(TRUE);
            mProxyGridModel->setSortRole(Qt::DisplayRole);
            mProxyGridModel->setSortCaseSensitivity(Qt::CaseInsensitive);

            // sort in ascending order
            mProxyGridModel->sort(0, Qt::AscendingOrder);    
            mLocationsFound = true;
        }
        else
        {
             // no locations to display.
             QStandardItem *modelItem = new QStandardItem();
             modelItem->setData(QVariant(hbTrId("txt_lint_list_no_location_entries_present")), Qt::DisplayRole);
             mModel->appendRow( modelItem );
             mLocationsFound = false;
        }
    }
}

// -----------------------------------------------------------------------------
// LocationPickerContent::LocationPickerContent()
// -----------------------------------------------------------------------------
LocationPickerContent::~LocationPickerContent()
{
    // delete data manager
    if( mDataManager )
        delete mDataManager;
    delete mProxyGridModel;
    delete mListProxyModel;
    delete mModel;
}

// -----------------------------------------------------------------------------
// LocationPickerContent::locationsFound()
// -----------------------------------------------------------------------------
bool LocationPickerContent::locationsFound()
{
    return mLocationsFound;
}

// -----------------------------------------------------------------------------
// LocationPickerContent::getListProxyModel()
// -----------------------------------------------------------------------------
LocationPickerProxyModel* LocationPickerContent::getListProxyModel()
{   
    return mListProxyModel;
}

// -----------------------------------------------------------------------------
// LocationPickerContent::getStandardModel()
// -----------------------------------------------------------------------------
QStandardItemModel* LocationPickerContent::getStandardModel()
{
    return mModel;
}

// -----------------------------------------------------------------------------
// LocationPickerContent::getDataManager()
// -----------------------------------------------------------------------------
LocationPickerDataManager* LocationPickerContent::getDataManager()
{
    return mDataManager;
}

// -----------------------------------------------------------------------------
// LocationPickerContent::getGridProxyModel()
// -----------------------------------------------------------------------------
LocationPickerProxyModel* LocationPickerContent::getGridProxyModel()
{
    return mProxyGridModel;
}
