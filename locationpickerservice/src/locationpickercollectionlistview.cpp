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
* Description: LocationPickerCollectionListView implementation
*
*/

#include <HbListView>
#include <QStandardItemModel>
#include <HbListViewItem>

#include "locationpickercollectionlistview.h"
#include "locationpickercollectioncontent.h"
#include "locationpickertoolbar.h"
#include "locationpickerappwindow.h"
#include "locationpickerdatamanager.h"
#include "locationpickertypes.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------
// LocationPickerCollectionListView::LocationPickerCollectionListView()
// ----------------------------------------------------------------------
LocationPickerCollectionListView::LocationPickerCollectionListView(
        LocationPickerAppWindow *aWindow, QGraphicsItem* aParent )
        :HbView( aParent )

{
    mWindow = aWindow;
    //Creation of List View
    mListView=new HbListView( this );
    connect(mListView, SIGNAL(activated(const QModelIndex &)), this, SLOT(handleActivated(const QModelIndex &)));

    //Set grahics size for the list items.
    HbListViewItem *hbListItem = new HbListViewItem();
    hbListItem->setGraphicsSize(HbListViewItem::MediumIcon);

    // Create a standard model for the view list
    mModel = new QStandardItemModel( this );
    mListView->setModel( mModel, hbListItem );
    
    mDataManager = new LocationPickerDataManager( *mModel, ELocationPickerCollectionListView );
    mDataManager->populateModel();

    setWidget( mListView );
    
    
    // create back action
    mSecondaryBackAction = new HbAction( Hb::BackAction, this );
    // add back key action
    setNavigationAction( mSecondaryBackAction );
    connect(mSecondaryBackAction, SIGNAL(triggered()), mWindow,
                                SLOT(backButtonTriggered()));

}

// ----------------------------------------------------------------------
// LocationPickerCollectionListView::~LocationPickerCollectionListView()
// ----------------------------------------------------------------------
LocationPickerCollectionListView::~LocationPickerCollectionListView()
{
    if( mDataManager )
        delete mDataManager;
}


// ----------------------------------------------------------------------
// LocationPickerCollectionListView::handleActivated()
// ----------------------------------------------------------------------
void LocationPickerCollectionListView::handleActivated( const QModelIndex &aIndex )
{
    // get the collection/category id
    quint32 categoryId = 0;
    mDataManager->getData( aIndex.row(), categoryId );

    // Create a content view of the selected collection
    LocationPickerToolBar *toolBar = new LocationPickerToolBar( mWindow, this );
    mCollectionContentView = new LocationPickerCollectionContent( mWindow, categoryId );
    mCollectionContentView->setToolBar(toolBar);

    // switch to the content view
    mWindow->addView( mCollectionContentView );
    mWindow->setCurrentView( mCollectionContentView );
}

// ----------------------------------------------------------------------
// LocationPickerCollectionListView::deleteCollectionContentView()
// ----------------------------------------------------------------------
void LocationPickerCollectionListView::deleteCollectionContentView()
{
    // delete the content view
    mWindow->removeView(mCollectionContentView);
    delete mCollectionContentView;
    mCollectionContentView = NULL;
}

