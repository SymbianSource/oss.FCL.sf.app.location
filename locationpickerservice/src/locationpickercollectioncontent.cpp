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
* Description: LocationPickerCollectionContent implementation
*
*/

#include <QStandardItemModel>

#include "locationpickerproxymodel.h"
#include "locationpickercollectioncontent.h"
#include "locationpickerdatamanager.h"
#include "locationpickertypes.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------
// LocationPickerCollectionContent::LocationPickerCollectionContent()
// -----------------------------------------------------------------
LocationPickerCollectionContent::LocationPickerCollectionContent( Qt::Orientations aOrientation , quint32 aCollectionId )
    :mOrientation(aOrientation),
	 mProxyModel(NULL),
    mModel(NULL),
    mDataManager(NULL)
{
    // Create a standard model for the view list
    mModel = new QStandardItemModel( this );
    mDataManager = new LocationPickerDataManager( *mModel, ELocationPickerCollectionContent );
    if( mDataManager->populateModel( mOrientation , aCollectionId ) )
    {
        // Create the proxy model.
        mProxyModel = new LocationPickerProxyModel(mOrientation);
        mProxyModel->setSourceModel(mModel);
        mProxyModel->setDynamicSortFilter(TRUE);
        mProxyModel->setSortRole(Qt::DisplayRole);
        mProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
        // sort
        mProxyModel->sort(0, Qt::AscendingOrder);
    }
    else
    {
        // no locations to display.
        QStandardItem *modelItem = new QStandardItem();
        modelItem->setData(QVariant(hbTrId("txt_lint_list_no_location_entries_present")), Qt::DisplayRole);
        mModel->appendRow( modelItem );
     }
}

// ----------------------------------------------------------------
// LocationPickerCollectionContent::~LocationPickerCollectionContent
// -----------------------------------------------------------------
LocationPickerCollectionContent::~LocationPickerCollectionContent()
{
    delete mProxyModel;
    delete mModel;
    delete mDataManager;
}

// ----------------------------------------------------------------
// LocationPickerCollectionContent::getProxyModel
// -----------------------------------------------------------------
LocationPickerProxyModel* LocationPickerCollectionContent::getProxyModel()
    {
    return mProxyModel;
    }


// ----------------------------------------------------------------
// LocationPickerCollectionContent::getDataManager
// -----------------------------------------------------------------
LocationPickerDataManager* LocationPickerCollectionContent::getDataManager()
    {
    return mDataManager;
    }
