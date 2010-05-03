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
* Description: LocationPickerLandscapeView implementation
*
*/

#include "locationpickerlandscapeview.h"

#include <qstandarditemmodel.h>
#include <HbMainWindow>
#include <HbGridViewItem>
#include <HbGridView>
#include <HbStyleLoader>
#include <HbAction>

#include "locationpickerproxymodel.h"
#include "locationpickerdatamanager.h"
#include "locationpickercontent.h" 
#include "locationpickercollectionlistcontent.h"
#include "locationpickercollectioncontent.h"


// ----------------------------------------------------
// LocationPickerLandscapeView::LocationPickerLandscapeView()
// ----------------------------------------------------
LocationPickerLandscapeView::LocationPickerLandscapeView(HbDocumentLoader* aLoader)
    :mDocumentLoader(aLoader),
    mLocationPickerContent(NULL),
    mLocationPickerCollectionListContent(NULL),
    mAllAction(NULL),
    mCollectionAction(NULL),
    mSearchAction(NULL),
    mAscendingAction(NULL),
    mDescendingAction(NULL),
    mGridView(NULL),
    mCollectionContent(NULL),
    mGridViewItem(NULL),
    mViewType(ELocationPickerContent)
{   
    HbStyleLoader::registerFilePath(":/locationgrid.css"); 
    HbStyleLoader::registerFilePath(":/locationgrid.hbgridviewitem.widgetml");
    //create grid view item
    mGridViewItem = new HbGridViewItem();
    mGridViewItem->setObjectName("locationgrid");
    
    // create back action
    mLandscapeBackAction = new HbAction(Hb::BackAction);
    setNavigationAction(mLandscapeBackAction);
    connect(mLandscapeBackAction, SIGNAL(triggered()), this,
            SLOT(backButtonTriggered()));
}
// ----------------------------------------------------
// LocationPickerPotraitView::~LocationPickerLandscapeView()
// ----------------------------------------------------
LocationPickerLandscapeView::~LocationPickerLandscapeView()
{
    delete mCollectionContent;
    delete mLocationPickerContent;
    delete mLocationPickerCollectionListContent;
    delete mAllAction;
    delete mCollectionAction;
    delete mAscendingAction;
    delete mDescendingAction;
}

// ----------------------------------------------------------------------------
// LocationPickerPotraitView::backButtonTriggered()
// ----------------------------------------------------------------------------
void LocationPickerLandscapeView::backButtonTriggered()
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
// LocationPickerPotraitView::~init()
// ----------------------------------------------------
void LocationPickerLandscapeView::init(Qt::Orientation aOrientation )
{   
      // Create Collection List Content
      mLocationPickerCollectionListContent = new LocationPickerCollectionListContent(aOrientation);
      
      mLocationPickerContent = new LocationPickerContent(aOrientation);
      
     //Get HbAction items
	 mGridView = qobject_cast<HbGridView*> (mDocumentLoader->findObject(QString(
                  "gridView")));
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
      if( !mAllAction || !mCollectionAction || !mSearchAction || !mGridView || !mAscendingAction || !mDescendingAction)
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
      
      connect(mGridView, SIGNAL(activated(const QModelIndex &)), this, SLOT(handleActivated(const QModelIndex &)));
}


void LocationPickerLandscapeView::manageGridView()
{   
    // Create Locationpicker Content
    if (!mLocationPickerContent->locationsFound())
    {
        //if no location entries present
        mGridView->setModel(mLocationPickerContent->getStandardModel(),mGridViewItem); 
        disableTabs();
        mViewType = ELocationPickerContent;
    }
    else
    {
        //set the appropriate model
        switch(mViewType)
            {
                case ELocationPickerContent:
                {
                    mGridView->setModel(mLocationPickerContent->getGridProxyModel(),mGridViewItem);
                    mAllAction->setChecked(true);
                    mCollectionAction->setChecked(false);
                }
                break;
                case ELocationPickerCollectionContent:
                {
                    setCollectionData(mCategoryId);
                    mCollectionAction->setChecked(true);
                    mAllAction->setChecked(false);
                }
                break;
            }
    }
}

void LocationPickerLandscapeView::disableTabs()
{
    mAllAction->setDisabled(true);
    mCollectionAction->setDisabled(true);
    mSearchAction->setDisabled(true);
    mAscendingAction->setDisabled(true);
    mDescendingAction->setDisabled(true);
}


// -----------------------------------------------------------------------------
// LocationPickerLandscapeView::handleActivated()
// -----------------------------------------------------------------------------
void LocationPickerLandscapeView::handleActivated(const QModelIndex &aIndex)
{   
    //handle the activated signal according to model set
    
       switch(mViewType)
           {
           case ELocationPickerContent:
               {
                   QModelIndex  index = mLocationPickerContent->getGridProxyModel()->mapToSource(
                                  aIndex);
                quint32 lm = 0;
                mLocationPickerContent->getDataManager()->getData(index.row(), lm);
                emit selectItem( lm );
               }
           break;
           case ELocationPickerCollectionListContent:
               {
               mLocationPickerCollectionListContent->getDataManager()->getData(
                       aIndex.row(), mCategoryId);
               setCollectionData(mCategoryId);
               emit sendCategoryID(mCategoryId);
               }
               break;
              //default
           case ELocationPickerCollectionContent:
               {
               QModelIndex   index = mCollectionContent->getProxyModel()->mapToSource(
                                      aIndex);
               quint32 lm = 0;
               mCollectionContent->getDataManager()->getData(index.row(), lm);
               emit selectItem(lm);
               }
               break;
           default:
               break;
     }
}



// -----------------------------------------------------------------------------
// LocationPickerLandscapeView::sortAscending()
// -----------------------------------------------------------------------------
void LocationPickerLandscapeView::sortAscending()
{   
    //check the model set and do sorting accordingly
        if (mViewType == ELocationPickerContent)
        {
            mLocationPickerContent->getGridProxyModel()->sort(0, Qt::AscendingOrder);
        }
        else
        {
            mCollectionContent->getProxyModel()->sort(0, Qt::AscendingOrder);
        }
}

// -----------------------------------------------------------------------------
// LocationPickerLandscapeView::sortDescending()
// -----------------------------------------------------------------------------
void LocationPickerLandscapeView::sortDescending()
{
    //check the model set and do sorting accordingly
        if (mViewType == ELocationPickerContent)
        {
            mLocationPickerContent->getGridProxyModel()->sort(0, Qt::DescendingOrder);
        }
        else
        {
            mCollectionContent->getProxyModel()->sort(0, Qt::DescendingOrder);
        }   
}

// -----------------------------------------------------------------------------
// LocationPickerLandscapeView::handleAllTab()
// -----------------------------------------------------------------------------
void LocationPickerLandscapeView::allTabTriggered()
{
    //execute only if tab is not pressed
       if (mAllAction->isChecked())
       {    
           mGridView->setModel(mLocationPickerContent->getGridProxyModel(),mGridViewItem);
           mAscendingAction->setEnabled(true);
           mDescendingAction->setEnabled(true);
           mCollectionAction->setChecked(false);
           mViewType = ELocationPickerContent;
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
// LocationPickerLandscapeView::handleCollectionTab()
// -----------------------------------------------------------------------------
void LocationPickerLandscapeView::colectionTabTriggered()
{
    //execute only if tab is not pressed
    if (mCollectionAction->isChecked())
    {   
        mViewType = ELocationPickerCollectionListContent;
    	emit handleCollectionList();
    }
    else
    {
	    //Keep the tab pressed
        mCollectionAction->setChecked(true);
    }
}

// -----------------------------------------------------------------------------
// LocationPickerLandscapeView::searchTabTriggered()
// -----------------------------------------------------------------------------
void LocationPickerLandscapeView::searchTabTriggered()
{
    emit switchToSearchView();
}

void LocationPickerLandscapeView::setCollectionData( quint32 aCategoryId )
{   
    mCollectionContent
    = new LocationPickerCollectionContent(this->mainWindow()->orientation() , aCategoryId);
    mGridView->setModel(mCollectionContent->getProxyModel(),mGridViewItem);
    mViewType = ELocationPickerCollectionContent;
    //Enable the options
    mAscendingAction->setEnabled(true);
    mDescendingAction->setEnabled(true);
    mCollectionAction->setChecked(true);
}

// -----------------------------------------------------------------------------
// LocationPickerLandscapeView::setCategoryID()
// -----------------------------------------------------------------------------
void LocationPickerLandscapeView::setCategoryID( quint32 aCategoryId  )
{
    mCategoryId = aCategoryId;
}

// -----------------------------------------------------------------------------
// LocationPickerLandscapeView::getViewType()
// -----------------------------------------------------------------------------
TViewType LocationPickerLandscapeView::getViewType()
{
    return mViewType;
}

// -----------------------------------------------------------------------------
// LocationPickerLandscapeView::setViewType()
// -----------------------------------------------------------------------------
void LocationPickerLandscapeView::setViewType(TViewType aViewType)
{
    mViewType = aViewType;
}
