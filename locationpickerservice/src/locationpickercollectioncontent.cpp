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

#include <HbListView>
#include <QStandardItemModel>
#include <HbMenu>
#include <HbListViewItem>
#include <HbAction>

#include "locationpickerproxymodel.h"
#include "locationpickercollectioncontent.h"
#include "locationpickerdatamanager.h"
#include "locationpickertypes.h"
#include "locationpickerappwindow.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------
// LocationPickerCollectionContent::LocationPickerCollectionContent()
// -----------------------------------------------------------------
LocationPickerCollectionContent::LocationPickerCollectionContent(
        LocationPickerAppWindow *aWindow, quint32 aCollectionId, QGraphicsItem* aParent):
        HbView( aParent )

{
    // inditialize window
    mWindow = aWindow;
    // Create a standard model for the view list
    mModel = new QStandardItemModel( this );
    mDataManager = new LocationPickerDataManager( *mModel, ELocationPickerCollectionContentView );
    mListView = new HbListView( this );


    if( mDataManager->populateModel( aCollectionId ) )
    {
        // connect the activated signal of list view item
        connect(mListView, SIGNAL(activated(const QModelIndex &)), this, SLOT(handleActivated(const QModelIndex &)));
        //Creation of List View
        //Set graphics size for the list items.
        HbListViewItem *hbListItem = new HbListViewItem();
        hbListItem->setGraphicsSize(HbListViewItem::Thumbnail);

        // Create the proxy model.
        mProxyModel = new LocationPickerProxyModel();
        mProxyModel->setSourceModel(mModel);

        // set the model
        mListView->setModel( mProxyModel, hbListItem );
        mProxyModel->setDynamicSortFilter(TRUE);
        mProxyModel->setSortRole(Qt::DisplayRole);
        mProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);

        // sort
        mProxyModel->sort(0, Qt::AscendingOrder);

        // construct menu for the view
        constructMenu();
    }

    else
    {
        // no locations to display.
        QStandardItem *modelItem = new QStandardItem();
        modelItem->setData(QVariant(KNoLocations), Qt::DisplayRole);
        mModel->appendRow( modelItem );
        mListView->setModel(mModel);
    }

    setWidget(mListView);

    
    // create back action
    mSecondaryBackAction = new HbAction( Hb::BackAction, this );
    // add back key action
    setNavigationAction( mSecondaryBackAction );
    connect(mSecondaryBackAction, SIGNAL(triggered()), mWindow,
                                SLOT(backButtonTriggered()));

}

// ----------------------------------------------------------------
// LocationPickerCollectionContent::~LocationPickerCollectionContent
// -----------------------------------------------------------------
LocationPickerCollectionContent::~LocationPickerCollectionContent()
{
    if( mDataManager )
        delete mDataManager;
}

// ----------------------------------------------------------------
// LocationPickerCollectionContent::constructMenu()
// -----------------------------------------------------------------
void LocationPickerCollectionContent::constructMenu()
{
    HbMenu *menu = this->menu();
    HbMenu *subMenu = menu->addMenu(KSortBy);
    HbAction *act;

    act = new HbAction(QString(KAscend), this);
    subMenu->addAction(act);
    connect(act,SIGNAL(triggered()),this,SLOT(sortAscending()));

    act = subMenu->addAction( KDescend );
    connect(act, SIGNAL(triggered()), SLOT(sortDescending()));
}


// ----------------------------------------------------------------
// LocationPickerCollectionContent::sortAscending()
// -----------------------------------------------------------------
void LocationPickerCollectionContent::sortAscending()
{
    mProxyModel->sort(0, Qt::AscendingOrder);
}
// ----------------------------------------------------------------
// LocationPickerCollectionContent::sortDescending()
// -----------------------------------------------------------------
void LocationPickerCollectionContent::sortDescending()
{
    mProxyModel->sort(0, Qt::DescendingOrder);
}

// ----------------------------------------------------------------
// LocationPickerCollectionContent::handleActivated()
// -----------------------------------------------------------------
void LocationPickerCollectionContent::handleActivated(const QModelIndex &aIndex)
{
    QModelIndex index = mProxyModel->mapToSource(aIndex);
    quint32 lm = 0;
    mDataManager->getData( index.row(), lm );
    mWindow->itemSelected( lm );
}
