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
* Description: LocationPickerView implementation
*
*/

#include "locationpickerpotraitview.h"

#include <HbMainWindow>
#include <qstandarditemmodel.h>
#include <HbListView>
#include <HbListViewItem>
#include <HbAction>
#include <QTimer>
#include <HbToolBar>

#include "locationpickerproxymodel.h"
#include "locationpickerdatamanager.h"
#include "locationpickercontent.h" 
#include "locationpickercollectionlistcontent.h"
#include "locationpickercollectioncontent.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------
// LocationPickerPotraitView::LocationPickerView()
// ----------------------------------------------------
LocationPickerPotraitView::LocationPickerPotraitView( HbDocumentLoader* aLoader )
    :mDocumentLoader(aLoader),
    mLocationPickerCollectionListContent(NULL),
    mProxyModel(NULL),
    mAllAction(NULL),
    mCollectionAction(NULL),
    mSearchAction(NULL),
    mAscendingAction(NULL),
    mDescendingAction(NULL),
    mListView(NULL),
    mCollectionContent(NULL),
    mViewType(ELocationPickerContent)
{   
    // create back action
    mPotraitBackAction = new HbAction(Hb::BackNaviAction);
    // add back key action
    setNavigationAction(mPotraitBackAction);
    //connect to slots
     connect(mPotraitBackAction, SIGNAL(triggered()), this,
            SLOT(backTriggered()));     
    //create list item  
    mListItem = new HbListViewItem();
    //set the graphics size
    mListItem->setGraphicsSize(HbListViewItem::Thumbnail);
}
// ----------------------------------------------------
// LocationPickerPotraitView::~LocationPickerView()
// ----------------------------------------------------
LocationPickerPotraitView::~LocationPickerPotraitView()
{
    delete mCollectionContent;
    delete mLocationPickerCollectionListContent;
    delete mAllAction;
    delete mCollectionAction;
    delete mAscendingAction;
    delete mDescendingAction;
    delete mListView;
}

// ----------------------------------------------------------------------------
// LocationPickerPotraitView::backButtonTriggered()
// ----------------------------------------------------------------------------
void LocationPickerPotraitView::backTriggered()
{
    //if current model is collection content, go back to collectionlist content  
    if(mViewType == ELocationPickerCollectionContent)
    {
        colectionTabTriggered();
        emit collectionContentExited(); 
    }
    else
    {
        //complete the service
        emit completeService();
    } 
}


// ----------------------------------------------------
// LocationPickerPotraitView::init()
// ----------------------------------------------------
void LocationPickerPotraitView::init( bool aPopulated, Qt::Orientation aOrientation, QStandardItemModel *aModel )
{   
    mModel = aModel;
    if(aPopulated)
    {
    // Create Collection List Content
    mLocationPickerCollectionListContent = new LocationPickerCollectionListContent(aOrientation);
    //create proxy model
    mProxyModel = new LocationPickerProxyModel( aOrientation , this  );
    mProxyModel->setSourceModel(aModel);
    // set sort properties
    mProxyModel->setDynamicSortFilter(TRUE);
    mProxyModel->setSortRole(Qt::DisplayRole);
    mProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    // sort in ascending order
    mProxyModel->sort(0, Qt::AscendingOrder); 
    }
    //Get HbAction items
    mListView = qobject_cast<HbListView*> (mDocumentLoader->findObject(QString(
                   "ListView")));
    HbToolBar* toolbar = new HbToolBar();
    //Create Action Items
    mAllAction = new HbAction();
    mAllAction->setIcon(QString("qtg_mono_location"));
    mAllAction->setCheckable(true);
    mCollectionAction =new HbAction();
    mCollectionAction->setIcon(QString("qtg_mono_location_collection"));
    mCollectionAction->setCheckable(true);
    mSearchAction = new HbAction();
    mSearchAction->setIcon(QString("qtg_mono_search"));
    
    toolbar->addAction(mAllAction);
    toolbar->addAction(mCollectionAction);
    toolbar->addAction(mSearchAction);
    this->setToolBar(toolbar);
    mAscendingAction = qobject_cast<HbAction*> (mDocumentLoader->findObject(
            QString("ascendingAction")));
    mDescendingAction = qobject_cast<HbAction*> (mDocumentLoader->findObject(
            QString("descendingAction")));
    if( !mAllAction || !mCollectionAction || !mSearchAction || !mAscendingAction || !mDescendingAction || !mListView)
    {
        qFatal("Error Reading Docml");
    }
      
    //connect to slots
    connect(mAscendingAction, SIGNAL(triggered()), this, SLOT(sortAscending()));
    connect(mDescendingAction, SIGNAL(triggered()), this,
            SLOT(sortDescending()));
    //connect all action Items to respective slots
    connect(mAllAction, SIGNAL(triggered()), this, SLOT(allTabTriggered()));
    connect(mCollectionAction, SIGNAL(triggered()), this,SLOT(colectionTabTriggered()));
    connect(mSearchAction, SIGNAL(triggered()), this,
            SLOT(searchTabTriggered()));
    // connect the signal of the list activated to a slot.
    connect(mListView, SIGNAL(activated(const QModelIndex &)), this, SLOT(handleActivated(const QModelIndex &)));
}

void LocationPickerPotraitView::manageListView()
{   
    //set the appropriate model
    switch(mViewType)
    {
        case ELocationPickerContent:
        {   
            mListItem->setGraphicsSize(HbListViewItem::Thumbnail);
            mListView->setModel(mProxyModel,mListItem);
            mAllAction->setChecked(true);
            mCollectionAction->setChecked(false);
            mViewType = ELocationPickerContent;
        }
        break;
        case ELocationPickerCollectionListContent:
        {
            mListItem->setGraphicsSize(HbListViewItem::MediumIcon);
            mListView->setModel(mLocationPickerCollectionListContent->getStandardModel(),mListItem);
            mCollectionAction->setChecked(true);
            mAllAction->setChecked(false);
            mViewType = ELocationPickerCollectionListContent;
        }
        break;
        case ELocationPickerCollectionContent:
        {
            setCollectionData(mCategoryId);
            mCollectionAction->setChecked(true);
            mAllAction->setChecked(false);
            mViewType = ELocationPickerCollectionContent;
        }
        break;
        default:
            break;
    }
}


void LocationPickerPotraitView::disableTabs( QStandardItemModel *aModel )
{
    //if no location entries present
    mListView->setModel(aModel,mListItem);
    mAllAction->setDisabled(true);
    mCollectionAction->setDisabled(true);
    mSearchAction->setDisabled(true);
    mAscendingAction->setDisabled(true);
    mDescendingAction->setDisabled(true);
}

// -----------------------------------------------------------------------------
// LocationPickerView::handleActivated()
// -----------------------------------------------------------------------------
void LocationPickerPotraitView::handleActivated(const QModelIndex &aIndex)
{
    mIndex = aIndex;
    QTimer::singleShot(0, this, SLOT(changeModel()));
}

// -----------------------------------------------------------------------------
// LocationPickerPotraitView::changeModel()
// -----------------------------------------------------------------------------
void LocationPickerPotraitView::changeModel()
{   
    //handle the activated signal according to model set
    switch(mViewType)
    {
        case ELocationPickerContent:
        {   
            if(!mProxyModel)
            {
            break;
            }
            QModelIndex   index = mProxyModel->mapToSource(
                    mIndex);
            quint32 lm = 0;
            QStandardItem* item = mModel->item( index.row(), index.column() );
            QVariant var = item->data( Qt::UserRole );
            lm = var.toUInt();
            //item selected, complete request
            emit selectItem( lm );
        }
            break;
        case ELocationPickerCollectionListContent:
        {
            mLocationPickerCollectionListContent->getData(
                    mIndex, mCategoryId );
            mViewType = ELocationPickerCollectionContent;
            //send categoryID to set the collection content
            emit sendCategoryID(mCategoryId);
        }
            break;
        case ELocationPickerCollectionContent:
        {
            if(!mCollectionContent->getProxyModel())
            {
                break;
            }
            QModelIndex  index = mCollectionContent->getProxyModel()->mapToSource(
                    mIndex);
            quint32 lm = 0;
            mCollectionContent->getData(index, lm);
            //item selected, complete request
            emit selectItem(lm);
        }
            break;
        default:
            break;
    }
}

// -----------------------------------------------------------------------------
// LocationPickerPotraitView::sortAscending()
// -----------------------------------------------------------------------------
void LocationPickerPotraitView::sortAscending()
{   
    //check the model set and do sorting accordingly
    if (mViewType == ELocationPickerContent)
    {
        mProxyModel->sort(0, Qt::AscendingOrder);
    }
    else
    {
        mCollectionContent->getProxyModel()->sort(0, Qt::AscendingOrder);
    }
}

// -----------------------------------------------------------------------------
// LocationPickerPotraitView::sortDescending()
// -----------------------------------------------------------------------------
void LocationPickerPotraitView::sortDescending()
{
    //check the model set and do sorting accordingly
    if (mViewType == ELocationPickerContent)
    {
        mProxyModel->sort(0, Qt::DescendingOrder);
    }
    else
    {
        mCollectionContent->getProxyModel()->sort(0, Qt::DescendingOrder);
    }
}

// -----------------------------------------------------------------------------
// LocationPickerPotraitView::handleAllTab()
// -----------------------------------------------------------------------------
void LocationPickerPotraitView::allTabTriggered()
{
    //execute only if tab is not pressed
    if (mAllAction->isChecked())
    {    
        mViewType = ELocationPickerContent;
        if(this->mainWindow()->orientation() == Qt::Horizontal)
        {
            //if in landscape send signal to launch grid view
            emit handleAllList();
        }
        else
        {
            mListItem->setGraphicsSize(HbListViewItem::Thumbnail);
            mListView->setModel(mProxyModel,mListItem);
            mCollectionAction->setChecked(false);
        }
        mAscendingAction->setEnabled(true);
        mDescendingAction->setEnabled(true);
        //delete mCollectionContent if coming back from collectioncontent
        if (mCollectionContent)
        {
            delete mCollectionContent;
            mCollectionContent = NULL;
        }
    }
    else
    {
        //Keep the tab pressed
        mAllAction->setChecked(true);
    }
    
}

// -----------------------------------------------------------------------------
// LocationPickerPotraitView::handleCollectionTab()
// -----------------------------------------------------------------------------
void LocationPickerPotraitView::colectionTabTriggered()
{
    //execute only if tab is not pressed
    if (mCollectionAction->isChecked())
    {   
        mListItem->setGraphicsSize(HbListViewItem::MediumIcon);
        mListView->setModel(mLocationPickerCollectionListContent->getStandardModel(),mListItem);
        mAscendingAction->setDisabled(true);
        mDescendingAction->setDisabled(true);
        mAllAction->setChecked(false);
        mViewType = ELocationPickerCollectionListContent;
    }
    else
    {
        //Keep the tab pressed
        mCollectionAction->setChecked(true);
    }
}

// -----------------------------------------------------------------------------
// LocationPickerPotraitView::searchTabTriggered()
// -----------------------------------------------------------------------------
void LocationPickerPotraitView::searchTabTriggered()
{
    emit switchToSearchView();
}

// -----------------------------------------------------------------------------
// LocationPickerPotraitView::setCollectionData()
// -----------------------------------------------------------------------------
void LocationPickerPotraitView::setCollectionData( quint32 acategoryId )
{
    
    if(!mCollectionContent)
    {
        mCollectionContent
        = new LocationPickerCollectionContent(Qt::Vertical , acategoryId);
    }
    mListItem->setGraphicsSize(HbListViewItem::Thumbnail);
    if(mCollectionContent->locationFound())
    {
        mListView->setModel(mCollectionContent->getProxyModel(),mListItem);
        //Enable the options
        mAscendingAction->setEnabled(true);
        mDescendingAction->setEnabled(true);
    }
    else
    {
        mListView->setModel(mCollectionContent->getStandardModel(),mListItem);
    }
    mViewType = ELocationPickerCollectionContent;
    
    mCollectionAction->setChecked(true);
}

// -----------------------------------------------------------------------------
// LocationPickerPotraitView::setCategoryID()
// -----------------------------------------------------------------------------
void LocationPickerPotraitView::setCategoryID( quint32 aCategoryId  )
{
    mCategoryId = aCategoryId;
}

// -----------------------------------------------------------------------------
// LocationPickerPotraitView::getViewType()
// -----------------------------------------------------------------------------
TViewType LocationPickerPotraitView::getViewType()
{
    return mViewType;
}

// -----------------------------------------------------------------------------
// LocationPickerPotraitView::setViewType()
// -----------------------------------------------------------------------------
void LocationPickerPotraitView::setViewType(TViewType aViewType)
{
    mViewType = aViewType;
}


// -----------------------------------------------------------------------------
// LocationPickerPotraitView::clearContentModel()
// -----------------------------------------------------------------------------
void LocationPickerPotraitView::clearContentModel()
{
    if(mCollectionContent)
    {
    delete mCollectionContent;
    mCollectionContent = NULL;
    }
}

