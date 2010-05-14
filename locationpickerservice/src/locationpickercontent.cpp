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
LocationPickerContent::LocationPickerContent()
	:mDataManager(NULL),
	mListModel(NULL),
	mGridModel(NULL)
{
    // create data manager to manage data in the model
    mDataManager = LocationPickerDataManager::getInstance();

}

// -----------------------------------------------------------------------------
// LocationPickerContent::populateModel()
// -----------------------------------------------------------------------------
bool LocationPickerContent::populateModel( Qt::Orientation aOrientation )
{
    bool locationsFound;
    if(aOrientation == Qt::Vertical)
    {
        // Create a standard model for the list view
        mListModel = new QStandardItemModel( this );
        if( mDataManager->populateModel( *mListModel, ELocationPickerContent, aOrientation) )
        {
            locationsFound = true;
        }
        else
        {
            createNoEntryDisplay(mListModel);
            locationsFound = false;
        }
    }
    //for landscape view
    else
    {
        // Create a standard model for the grid view
        mGridModel = new QStandardItemModel( this );
        if( mDataManager->populateModel(*mGridModel, ELocationPickerContent, aOrientation) )
        {
            locationsFound = true;
        }
        else
        {
            createNoEntryDisplay(mGridModel);
            locationsFound = false;
        }
    }
    return locationsFound;
}

// -----------------------------------------------------------------------------
// LocationPickerContent::LocationPickerContent()
// -----------------------------------------------------------------------------
LocationPickerContent::~LocationPickerContent()
{
    delete mListModel;
    delete mGridModel;
}

// -----------------------------------------------------------------------------
// LocationPickerContent::getStandardListModel()
// -----------------------------------------------------------------------------
QStandardItemModel* LocationPickerContent::getStandardListModel()
{
    return mListModel;
}

// -----------------------------------------------------------------------------
// LocationPickerContent::getStandardGridModel()
// -----------------------------------------------------------------------------
QStandardItemModel* LocationPickerContent::getStandardGridModel()
{
    return mGridModel;
}

// ----------------------------------------------------------------------------
// LocationPickerContent::createNoEntryDisplay()
// ----------------------------------------------------------------------------

void LocationPickerContent::createNoEntryDisplay( QStandardItemModel *aModel )
{
    // no locations to display.
    QStandardItem *modelItem = new QStandardItem();
    modelItem->setData(QVariant(hbTrId("txt_lint_list_no_location_entries_present")), Qt::DisplayRole);
    aModel->appendRow( modelItem );
}
