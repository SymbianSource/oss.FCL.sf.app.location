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
* Description: LocationPickerSearchView implementation
*
*/

#include <HbListViewItem>
#include <QGraphicsLinearLayout>
#include <QStandardItemModel>
#include <HbSearchPanel>
#include <HbListView>

#include "locationpickerproxymodel.h"
#include "locationpickersearchview.h"
#include "locationpickerdatamanager.h"
#include "locationpickerappwindow.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------
// LocationPickerSearchView::LocationPickerSearchView()
// ----------------------------------------------------
LocationPickerSearchView::LocationPickerSearchView( LocationPickerAppWindow *aWindow, QGraphicsItem* aParent ):
        HbView( aParent ),
        mSearchPanel(new HbSearchPanel(this))

{
    
    mWindow = aWindow;
    //Create a linear layout
    mLayout = new QGraphicsLinearLayout(Qt::Vertical);

    // create the list view
    mListView=new HbListView();
    connect(mListView, SIGNAL(activated(const QModelIndex &)), this, SLOT(handleActivated(const QModelIndex &)));

    //Set graphics size for the list items.
    HbListViewItem *hbListItem = new HbListViewItem();
    hbListItem->setGraphicsSize(HbListViewItem::Thumbnail);
    mListView->setItemPrototype( hbListItem );

    // Create a standard model for the view list
    mModel = new QStandardItemModel();
    // create a data manager to populate the model
    mDataManager = new LocationPickerDataManager( *mModel, ELocationPickerSearchView );

    // Create the proxy model.
    mProxyModel = new LocationPickerProxyModel();
    mProxyModel->setSourceModel(mModel);
    mListView->setModel(mProxyModel);

    // set filter properties
    mProxyModel->setDynamicSortFilter(TRUE);
    mProxyModel->setSortRole(Qt::DisplayRole);
    mProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);

    // populate data
    mDataManager->populateModel();

    // sort
    mProxyModel->sort(0, Qt::AscendingOrder);

    // add the list view to layout
    mLayout->addItem(mListView);

    // create search panel
    mSearchPanel->setProgressive(true);
    mSearchPanel->setSearchOptionsEnabled(false);
    connect(mSearchPanel,SIGNAL(criteriaChanged(QString)),this,SLOT(doSearch(QString)));

    // add search panel to the layout
    mLayout->addItem(mSearchPanel);

    // setlayout for the view
    setLayout(mLayout);
    
    
    // create back action
    mSecondaryBackAction = new HbAction( Hb::BackAction, this );
    // add back key action
    setNavigationAction( mSecondaryBackAction );
    connect(mSecondaryBackAction, SIGNAL(triggered()), mWindow,
                                SLOT(backButtonTriggered()));


}
// ----------------------------------------------------
// LocationPickerSearchView::~LocationPickerSearchView()
// ----------------------------------------------------
LocationPickerSearchView::~LocationPickerSearchView()
{
    // delete mDataManager
    if( mDataManager )
        delete mDataManager;
}

// ----------------------------------------------------
// LocationPickerSearchView::doSearch()
// ----------------------------------------------------
void LocationPickerSearchView::doSearch(QString aCriteria)
{
    // use the string to search
    mProxyModel->filterParameterChanged(aCriteria);
    mProxyModel->setFilterFixedString(aCriteria);
}

// ----------------------------------------------------
// LocationPickerSearchView::handleActivated()
// ----------------------------------------------------
void LocationPickerSearchView::handleActivated(const QModelIndex &aIndex)
{
    QModelIndex index = mProxyModel->mapToSource(aIndex);
    quint32 lm = 0;
    mDataManager->getData( index.row(), lm );
    mWindow->itemSelected( lm );
}

