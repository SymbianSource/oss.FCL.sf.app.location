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
* Description: LocationPickerAllView implementation
*
*/

#include <HbMenu>
#include <HbListViewItem>
#include <HbAction>
#include <HbListView>
#include <QStandardItemModel>

#include "locationpickerallview.h"
#include "locationpickerproxymodel.h"
#include "locationpickertypes.h"
#include "locationpickerappwindow.h"
#include "locationpickerdatamanager.h"

// ======== MEMBER FUNCTIONS ========

// -----------------------------------------------------------------------------
// LocationPickerAllView::LocationPickerAllView()
// -----------------------------------------------------------------------------
LocationPickerAllView::LocationPickerAllView( LocationPickerAppWindow *aWindow, QGraphicsItem* aParent )
        : HbView( aParent )
{
    mWindow = aWindow;
    
    // Create a standard model for the view list
    mModel = new QStandardItemModel( this );

    // create the list view and connect the signal
    mListView=new HbListView( this );

    // create data manager to manage data in the model
    mDataManager = new LocationPickerDataManager( *mModel, ELocationPickerAllView );
    if( mDataManager->populateModel() )
    {
        // connect the signal of the list activated to a slot.
        connect(mListView, SIGNAL(activated(const QModelIndex &)), this, SLOT(handleActivated(const QModelIndex &)));

        //Creation of List View
        //Set grahics size for the list items.
        HbListViewItem *hbListItem = new HbListViewItem();
        hbListItem->setGraphicsSize(HbListViewItem::Thumbnail);

        // Create the proxy model and set source model
        mProxyModel = new LocationPickerProxyModel( this );
        mProxyModel->setSourceModel(mModel);

        // set proxymodel with list view
        mListView->setModel(mProxyModel, hbListItem);

        // set sort properties
        mProxyModel->setDynamicSortFilter(TRUE);
        mProxyModel->setSortRole(Qt::DisplayRole);
        mProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);

        // sort in ascending order
        mProxyModel->sort(0, Qt::AscendingOrder);

        // construct menu for the view
        constructMenu();
        
        mLocationsFound = true;
    }

    else
    {
        // no locations to display.
        QStandardItem *modelItem = new QStandardItem();
        modelItem->setData(QVariant(KNoLocations), Qt::DisplayRole);
        mModel->appendRow( modelItem );
        mListView->setModel(mModel);
        mLocationsFound = false;
    }

    // set the listview as the view's widget
    setWidget(mListView);
    
    // create back action
    mSecondaryBackAction = new HbAction( Hb::BackAction, this );
    // add back key action
    setNavigationAction( mSecondaryBackAction );
    connect(mSecondaryBackAction, SIGNAL(triggered()), mWindow,
                                SLOT(backButtonTriggered()));

}

// -----------------------------------------------------------------------------
// LocationPickerAllView::LocationPickerAllView()
// -----------------------------------------------------------------------------
LocationPickerAllView::~LocationPickerAllView()
{
    // delete data manager
    if( mDataManager )
        delete mDataManager;
}

// -----------------------------------------------------------------------------
// LocationPickerAllView::locationsFound()
// -----------------------------------------------------------------------------
bool LocationPickerAllView::locationsFound()
{
    return mLocationsFound;
}


// -----------------------------------------------------------------------------
// LocationPickerAllView::constructMenu()
// -----------------------------------------------------------------------------
void LocationPickerAllView::constructMenu()
{
    HbMenu *menu = this->menu();
    HbMenu *subMenu = menu->addMenu(KSortBy);
    HbAction *act;

    act = new HbAction(KAscend, this );
    subMenu->addAction(act);
    connect(act,SIGNAL(triggered()),this,SLOT(sortAscending()));

    act = subMenu->addAction( KDescend );
    connect(act, SIGNAL(triggered()), SLOT(sortDescending()));
}

// -----------------------------------------------------------------------------
// LocationPickerAllView::sortAscending()
// -----------------------------------------------------------------------------
void LocationPickerAllView::sortAscending()
{
    mProxyModel->sort(0, Qt::AscendingOrder);
}

// -----------------------------------------------------------------------------
// LocationPickerAllView::sortDescending()
// -----------------------------------------------------------------------------
void LocationPickerAllView::sortDescending()
{
    mProxyModel->sort(0, Qt::DescendingOrder);
}

// -----------------------------------------------------------------------------
// LocationPickerAllView::handleActivated()
// -----------------------------------------------------------------------------
void LocationPickerAllView::handleActivated( const QModelIndex &aIndex)
{
    QModelIndex index = mProxyModel->mapToSource(aIndex);
    quint32 lm = 0;
    mDataManager->getData( index.row(), lm );
    mWindow->itemSelected( lm );
}
