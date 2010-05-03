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

#include "locationpickerproxymodel.h"
#include "locationpickerdatamanager.h"
#include "locationpickercontent.h" 
#include "locationpickercollectionlistcontent.h"
#include "locationpickercollectioncontent.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------
// LocationPickerPotraitView::LocationPickerView()
// ----------------------------------------------------
LocationPickerPotraitView::LocationPickerPotraitView(HbDocumentLoader* aLoader)
    :mDocumentLoader(aLoader),
    mLocationPickerContent(NULL),
    mLocationPickerCollectionListContent(NULL),
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
    mPotraitBackAction = new HbAction(Hb::BackAction);
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
    delete mLocationPickerContent;
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
        delete mCollectionContent;
        mCollectionContent=NULL; 
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
void LocationPickerPotraitView::init(Qt::Orientation aOrientation )
{   
    // Create Collection List Content
    mLocationPickerCollectionListContent = new LocationPickerCollectionListContent(aOrientation);
    mLocationPickerContent = new LocationPickerContent(aOrientation);

    //Get HbAction items
    mListView = qobject_cast<HbListView*> (mDocumentLoader->findObject(QString(
            "ListView")));

    //get the action items from docml
    mAllAction = qobject_cast<HbAction*> (mDocumentLoader->findObject(QString(
            "allAction")));
    mCollectionAction = qobject_cast<HbAction*> (mDocumentLoader->findObject(
            QString("collectionAction")));
    mSearchAction = qobject_cast<HbAction*> (mDocumentLoader->findObject(QString(
            "searchAction")));
    mAscendingAction = qobject_cast<HbAction*> (mDocumentLoader->findObject(
            QString("ascendingAction")));
    mDescendingAction = qobject_cast<HbAction*> (mDocumentLoader->findObject(
            QString("descendingAction")));
      if( !mAllAction || !mCollectionAction || !mSearchAction || !mListView || !mAscendingAction || !mDescendingAction)
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
    if (!mLocationPickerContent->locationsFound())
    {
        //if no location entries present
        mListView->setModel(mLocationPickerContent->getStandardModel(),mListItem);
        disableTabs();
    }
    else
    {   
        //set the appropriate model
        switch(mViewType)
        {
            case ELocationPickerContent:
            {   
                mListItem->setGraphicsSize(HbListViewItem::Thumbnail);
                mListView->setModel(mLocationPickerContent->getListProxyModel(),mListItem);
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
}


void LocationPickerPotraitView::disableTabs()
{
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
    //handle the activated signal according to model set
    switch(mViewType)
    {
        case ELocationPickerContent:
        {
            QModelIndex   index = mLocationPickerContent->getListProxyModel()->mapToSource(
                    aIndex);
            quint32 lm = 0;
            mLocationPickerContent->getDataManager()->getData(index.row(), lm);
            //item selected, complete request
            emit selectItem( lm );
        }
            break;
        case ELocationPickerCollectionListContent:
        {
            mLocationPickerCollectionListContent->getDataManager()->getData(
                    aIndex.row(), mCategoryId);
            mViewType = ELocationPickerCollectionContent;
            //send categoryID to set the collection content
            emit sendCategoryID(mCategoryId);
        }
            break;
        case ELocationPickerCollectionContent:
        {
            QModelIndex  index = mCollectionContent->getProxyModel()->mapToSource(
                        aIndex);
            quint32 lm = 0;
            mCollectionContent->getDataManager()->getData(index.row(), lm);
            //item selected, complete request
            emit selectItem(lm);
        }
            break;
        default:
            break;
    }
}

// -----------------------------------------------------------------------------
// LocationPickerView::sortAscending()
// -----------------------------------------------------------------------------
void LocationPickerPotraitView::sortAscending()
{   
    //check the model set and do sorting accordingly
    if (mViewType == ELocationPickerContent)
    {
        mLocationPickerContent->getListProxyModel()->sort(0, Qt::AscendingOrder);
    }
    else
    {
        mCollectionContent->getProxyModel()->sort(0, Qt::AscendingOrder);
    }
}

// -----------------------------------------------------------------------------
// LocationPickerView::sortDescending()
// -----------------------------------------------------------------------------
void LocationPickerPotraitView::sortDescending()
{
    //check the model set and do sorting accordingly
    if (mViewType == ELocationPickerContent)
    {
        mLocationPickerContent->getListProxyModel()->sort(0, Qt::DescendingOrder);
    }
    else
    {
        mCollectionContent->getProxyModel()->sort(0, Qt::DescendingOrder);
    }
}

// -----------------------------------------------------------------------------
// LocationPickerView::handleAllTab()
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
            mListView->setModel(mLocationPickerContent->getListProxyModel(),mListItem);
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
// LocationPickerView::handleCollectionTab()
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
// LocationPickerView::searchTabTriggered()
// -----------------------------------------------------------------------------
void LocationPickerPotraitView::searchTabTriggered()
{
    emit switchToSearchView();
}

// -----------------------------------------------------------------------------
// LocationPickerView::setCollectionData()
// -----------------------------------------------------------------------------
void LocationPickerPotraitView::setCollectionData( quint32 acategoryId )
{
    mCollectionContent
    = new LocationPickerCollectionContent(this->mainWindow()->orientation() , acategoryId);
    mListItem->setGraphicsSize(HbListViewItem::Thumbnail);
    mListView->setModel(mCollectionContent->getProxyModel(),mListItem);
    mViewType = ELocationPickerCollectionContent;
    //Enable the options
    mAscendingAction->setEnabled(true);
    mDescendingAction->setEnabled(true);
    mCollectionAction->setChecked(true);
}

// -----------------------------------------------------------------------------
// LocationPickerView::setCategoryID()
// -----------------------------------------------------------------------------
void LocationPickerPotraitView::setCategoryID( quint32 aCategoryId  )
{
    mCategoryId = aCategoryId;
}

// -----------------------------------------------------------------------------
// LocationPickerView::getViewType()
// -----------------------------------------------------------------------------
TViewType LocationPickerPotraitView::getViewType()
{
    return mViewType;
}

// -----------------------------------------------------------------------------
// LocationPickerView::setViewType()
// -----------------------------------------------------------------------------
void LocationPickerPotraitView::setViewType(TViewType aViewType)
{
    mViewType = aViewType;
}
