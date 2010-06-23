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
#include <HbStyleLoader>
#include <HbAction>
#include <HbMenu>
#include <HbToolBar>
#include <QGraphicsLinearLayout>
#include <HbLabel>
#include <HbTextItem>
#include <hgwidgets/hgmediawall.h>

#include "hgwidgetdatamodel.h"
#include "locationpickerproxymodel.h"
#include "locationpickerdatamanager.h"
#include "locationpickercollectionlistcontent.h"
#include "locationpickercollectioncontent.h"



// ----------------------------------------------------
// LocationPickerLandscapeView::LocationPickerLandscapeView()
// ----------------------------------------------------
LocationPickerLandscapeView::LocationPickerLandscapeView(HbDocumentLoader* aLoader)
    :mDocumentLoader(aLoader),
    mAllAction(NULL),
    mCollectionAction(NULL),
    mSearchAction(NULL),
    mAscendingAction(NULL),
    mDescendingAction(NULL),
    mCollectionContent(NULL),
    mViewType(ELocationPickerContent),
    mModel(NULL),
    mWidget(NULL),
    mLayout(NULL),
    mEmptyLabel(NULL),
	mLongPressMenu(NULL)
{       
    // create back action
    mLandscapeBackAction = new HbAction(Hb::BackNaviAction);
    setNavigationAction(mLandscapeBackAction);
    connect(mLandscapeBackAction, SIGNAL(triggered()), this,
            SLOT(backButtonTriggered()));
    

}
// ----------------------------------------------------
// LocationPickerLandscapeView::~LocationPickerLandscapeView()
// ----------------------------------------------------
LocationPickerLandscapeView::~LocationPickerLandscapeView()
{
    delete mCollectionContent;
    delete mAllAction;
    delete mCollectionAction;
    delete mAscendingAction;
    delete mDescendingAction;
    delete mWidget;
    delete mModel;
    delete mLongPressMenu;
}

// ----------------------------------------------------------------------------
// LocationPickerLandscapeView::backButtonTriggered()
// ----------------------------------------------------------------------------
void LocationPickerLandscapeView::backButtonTriggered()
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
// LocationPickerLandscapeView::init()
// ----------------------------------------------------
void LocationPickerLandscapeView::init(Qt::Orientation aOrientation, QStandardItemModel *aModel )
{   
      mStandardModel = aModel;      
      //create proxy model
      mProxyModel = new LocationPickerProxyModel( aOrientation , this  );
      mProxyModel->setSourceModel(aModel);
      // set sort properties
      mProxyModel->setDynamicSortFilter(TRUE);
      mProxyModel->setSortRole(Qt::DisplayRole);
      mProxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
      // sort in ascending order
      mProxyModel->sort(0, Qt::AscendingOrder);    
      
     //Get HbAction items
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
      if( !mAllAction || !mCollectionAction || !mSearchAction || !mAscendingAction || !mDescendingAction)
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
      
      //hurrriganes widget
      QGraphicsWidget *widget = NULL;
      widget = mDocumentLoader->findWidget(QString("vertical"));
      mLayout =  static_cast<QGraphicsLinearLayout*>(widget->layout());
      
      mModel = new HgWidgetDataModel( mProxyModel, this );
      mModel->setImageDataType(HgWidgetDataModel::ETypeQImage);
      //create MediaWall Object
      mWidget = new HgMediawall();
      HbIcon defaultIcon(KDummyImage);
      QImage defaultImage = defaultIcon.pixmap().toImage().convertToFormat(QImage::Format_ARGB32_Premultiplied);
      mWidget->setDefaultImage(defaultImage);
      mWidget->setObjectName("location");
      HbStyleLoader::registerFilePath(":/location.hgmediawall.widgetml");
      HbStyleLoader::registerFilePath(":/location.hgmediawall.css");
      mWidget->enableReflections(false);
      mWidget->setFontSpec(HbFontSpec(HbFontSpec::Primary));
      mWidget->setItemSize(QSize(4.6,2));
      connect(this->mainWindow(), SIGNAL(orientationChanged(Qt::Orientation)),mWidget,
	   SLOT(orientationChanged(Qt::Orientation)));
      
      mWidget->setAcceptTouchEvents(true);
      connect(mWidget, SIGNAL(activated(const QModelIndex &)),this, SLOT(handleActivated(const QModelIndex &)));
      connect(mWidget, SIGNAL(longPressed(const QModelIndex &, const QPointF &)),this, 
              SLOT(launchPopUpMenu(const QModelIndex &, const QPointF &)));
      mLayout->addItem(mWidget);
      mWidget->setModel(mModel);
      mWidget->setLongPressEnabled(true);
      mWidget->scrollTo(mWidget->currentIndex());
}

// -----------------------------------------------------------------------------
// LocationPickerLandscapeView::manageHgWidget()
// -----------------------------------------------------------------------------
void LocationPickerLandscapeView::manageHgWidget()
{
    //set the appropriate model
    switch(mViewType)
    {
        case ELocationPickerContent:
        {  
            mModel->resetModel(mProxyModel);
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

// -----------------------------------------------------------------------------
// LocationPickerLandscapeView::handleActivated()
// -----------------------------------------------------------------------------
void LocationPickerLandscapeView::handleActivated(const QModelIndex &aIndex)
{   
    //handle the activated signal according to model set
    int row= aIndex.row();
    int col = aIndex.column();
    switch(mViewType)
    { 
        case ELocationPickerContent:
        {
            QModelIndex proxyModelIndex = mProxyModel->index(row,col);
            QModelIndex  index = mProxyModel->mapToSource(
                    proxyModelIndex);
            quint32 lm = 0;
            QStandardItem* item = mStandardModel->item( index.row(), index.column() );
            QVariant var = item->data( Qt::UserRole );
            lm = var.toUInt();
            //item selected, complete request
            emit selectItem( lm );
        }
        break;
        case ELocationPickerCollectionContent:
        {
            if(!mCollectionContent->getProxyModel())
            {
                break;
            }
            QModelIndex proxyModelIndex = mCollectionContent->getProxyModel()->index(row,col);
            QModelIndex   index = mCollectionContent->getProxyModel()->mapToSource(
                    proxyModelIndex);
            quint32 lm = 0;
            mCollectionContent->getData(index, lm);
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
        mProxyModel->sort(0,Qt::AscendingOrder);
        mModel->resetModel(mProxyModel);
    }
    else
    {
        mCollectionContent->getProxyModel()->sort(0,Qt::AscendingOrder);
        mModel->resetModel(mCollectionContent->getProxyModel());
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
        mProxyModel->sort(0,Qt::DescendingOrder);
        mModel->resetModel(mProxyModel);
    }
    else
    {
        mCollectionContent->getProxyModel()->sort(0,Qt::DescendingOrder);
        mModel->resetModel(mCollectionContent->getProxyModel());
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
        if(mEmptyLabel)
        {
            mLayout->removeItem(mEmptyLabel);
            mEmptyLabel->hide();
            mLayout->insertItem(0,mWidget);
            mWidget->setVisible(true);
            delete mEmptyLabel;
            mEmptyLabel = NULL;
        }
        mModel->resetModel(mProxyModel);
    }
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
        mAscendingAction->setDisabled(true);
        mDescendingAction->setDisabled(true);
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
    if(mEmptyLabel)
    {   
        mLayout->removeItem(mEmptyLabel);
        mEmptyLabel->hide();
        mLayout->insertItem(0,mWidget);
        mWidget->show();
        delete mEmptyLabel;
        mEmptyLabel = NULL;
    }
    if(!mCollectionContent)
    {
        mCollectionContent = new LocationPickerCollectionContent(Qt::Horizontal , aCategoryId);
    }
    if(mCollectionContent->locationFound())
    {
        mModel->resetModel(mCollectionContent->getProxyModel());
        //Enable the options
        mAscendingAction->setEnabled(true);
        mDescendingAction->setEnabled(true);
    }
    else
    {
    if(!mEmptyLabel)
    {
        mEmptyLabel =  new HbTextItem(hbTrId("txt_lint_list_no_location_entries_present"));
    }
    mEmptyLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    mEmptyLabel->setFontSpec(HbFontSpec(HbFontSpec::Primary));
    mLayout->removeItem(mWidget);
    mWidget->setVisible(false);
    mLayout->insertItem(0, mEmptyLabel);
    }

    mViewType = ELocationPickerCollectionContent;
    
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

// -----------------------------------------------------------------------------
// LocationPickerLandscapeView::clearContentModel()
// -----------------------------------------------------------------------------
void LocationPickerLandscapeView::clearContentModel()
{
   if(mCollectionContent)
   {
   delete mCollectionContent;
   mCollectionContent = NULL;
   }
}

// -----------------------------------------------------------------------------
// LocationPickerLandscapeView::launchPopUpMenu()
// -----------------------------------------------------------------------------
void LocationPickerLandscapeView::launchPopUpMenu(const QModelIndex &aIndex, const QPointF &aPoint)
{
    mLongPressMenu = new HbMenu();
    mLongPressMenu->setTimeout(HbMenu::NoTimeout);
    HbAction* selectAction  = mLongPressMenu->addAction(hbTrId("Select"));
    mIndex = aIndex;
    connect(selectAction, SIGNAL(triggered()), this, SLOT(handleLongPress()));
    mLongPressMenu->setPreferredPos(aPoint);
    mLongPressMenu->open();
}

// -----------------------------------------------------------------------------
// LocationPickerLandscapeView::handleLongPress()
// -----------------------------------------------------------------------------
void LocationPickerLandscapeView::handleLongPress()
{
    handleActivated(mIndex);
}
