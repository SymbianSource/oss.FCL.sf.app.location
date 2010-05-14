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
* Description: LocationPickerAppWindow implementation
*
*/

#include "locationpickerappwindow.h"

#include "qlocationpickeritem.h"

#include "locationpickersearchview.h"
#include "locationpickerpotraitview.h"
#include "locationpickerservice.h"
#include "locationpickerdatamanager.h"
#include "locationpickerdocumentloader.h"
#include "locationpickerlandscapeview.h"
#include "locationpickercontent.h"

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::LocationPickerAppWindow()
// ----------------------------------------------------------------------------
LocationPickerAppWindow::LocationPickerAppWindow( QWidget *parent, Hb::WindowFlags windowFlags )
	:HbMainWindow(parent, windowFlags),
	mLocationPickerSearchView(NULL),
    mLocationPickerDocumentLoader(NULL),
	mLocationPickerPotraitView(NULL),
	mLocationPickerLandscapeView(NULL),
	mLocationPickerContent(NULL),
    mService(NULL),
	mviewType(ELocationPickerContent)
{
    // create the service object;
    mService = new LocationPickerService(this);
    mLocationPickerContent = new LocationPickerContent();
    
    //create document loader object
    mLocationPickerDocumentLoader = new LocationPickerDocumentLoader();
   
    bool ok = false;
    //load the Locationpicker portrait view
    mLocationPickerDocumentLoader->load(":/locationpickerpotrait.docml", &ok);
    Q_ASSERT_X(ok, "locationpickerservice", "invalid DocML file");
    //find graphics location picker potrait view
    QGraphicsWidget *locationPickerWidget = mLocationPickerDocumentLoader->findWidget("LocationPickerPotraitView");
    Q_ASSERT_X((locationPickerWidget != 0), "locationpickerservice", "invalid DocML file");
    mLocationPickerPotraitView = qobject_cast<LocationPickerPotraitView*>(locationPickerWidget);
    bool populated = mLocationPickerContent->populateModel(Qt::Vertical);
    //initialize potrait widgets and connect to respective signals 
    mLocationPickerPotraitView->init(populated, Qt::Vertical, mLocationPickerContent->getStandardListModel());
    connectPotraitSlots();
    if(!populated)
    {
        mLocationPickerPotraitView->disableTabs(mLocationPickerContent->getStandardListModel());
        addView( mLocationPickerPotraitView );
        setCurrentView(mLocationPickerPotraitView);
    }
    else
    {
        addView( mLocationPickerPotraitView );

        mLocationPickerDocumentLoader->reset();

        //load the Locationpicker landscape view
        mLocationPickerDocumentLoader->load(":/locationpickerlandscape.docml", &ok);
        Q_ASSERT_X(ok, "locationpickerservice", "invalid DocML file");
        //find graphics location picker landscape view
        locationPickerWidget = mLocationPickerDocumentLoader->findWidget("LocationPickerLandscapeView");
        Q_ASSERT_X((locationPickerWidget != 0), "locationpickerservice", "invalid DocML file");
        mLocationPickerLandscapeView = qobject_cast<LocationPickerLandscapeView*>(locationPickerWidget);
        mLocationPickerContent->populateModel(Qt::Horizontal);
        //initialize widgets and connect to respective signals 
        mLocationPickerLandscapeView->init(Qt::Horizontal, mLocationPickerContent->getStandardGridModel());
        connectLandscapeSlots();
        addView(mLocationPickerLandscapeView);
        //connect to orientationChanged signal
        connect(this, SIGNAL(orientationChanged(Qt::Orientation)),this, SLOT(changeOrientation(Qt::Orientation)));
        //launch the view in current orientation
        changeOrientation(this->orientation());
    }
}

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::~LocationPickerAppWindow()
// ----------------------------------------------------------------------------
LocationPickerAppWindow::~LocationPickerAppWindow()
{
    delete mService;
    delete mLocationPickerSearchView;
    delete mLocationPickerDocumentLoader;
    delete mLocationPickerPotraitView;
    delete mLocationPickerLandscapeView;
    delete mLocationPickerContent;
}

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::itemSelected()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::itemSelected( quint32 aLmid )
{
    QLocationPickerItem item;
    LocationPickerDataManager::getInstance()->getLocationItem(aLmid, item);
    // complete the request
    mService->complete(item);
}

void LocationPickerAppWindow::serviceComplete()
{
    // Other views request complete with a valid location picker item
    QLocationPickerItem item;
    item.mIsValid = false;
    mService->complete(item);
}
// ----------------------------------------------------------------------------
// LocationPickerAppWindow::activateSearchView()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::activateSearchView()
{   
    //load the LocationPickerSearchView
    bool ok = false;
    if(!mLocationPickerSearchView)
    {
        mLocationPickerDocumentLoader->load(":/locationpickersearchview.docml", &ok);
        Q_ASSERT_X(ok, "locationpickerService", "invalid DocML file");
        //find the LocationPickerSearchView
        QGraphicsWidget *locationPickerSearchWidget = mLocationPickerDocumentLoader->findWidget("LocationPickerSearchView");
        Q_ASSERT_X((locationPickerSearchWidget != 0), "locationpickerService", "invalid DocML file");
        mLocationPickerSearchView = qobject_cast<LocationPickerSearchView*>(locationPickerSearchWidget);
        Q_ASSERT_X((mLocationPickerSearchView != 0), "mLocationPickerSearchView", 
            "qobject cast failure");
        //initialize the action items and connect to slots
        mLocationPickerSearchView->init(mLocationPickerContent->getStandardListModel());
        connect(mLocationPickerSearchView,SIGNAL(switchView()),this,SLOT(activateLocationPickerView()));
        connect(mLocationPickerSearchView,SIGNAL(selectItem( quint32 )),this,SLOT(itemSelected( quint32 )));
        addView(mLocationPickerSearchView);
    }
    //set LocationPickerSearchview as current view
    setCurrentView(mLocationPickerSearchView);
    mviewType = ELocationPickerSearchView;

}

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::activateLocationPickerView()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::activateLocationPickerView()
{ 
    mviewType = ELocationPickerContent;
    //check the orientation and load view accordingly 
    if(this->orientation() == Qt::Horizontal)
    {
        mLocationPickerLandscapeView->setViewType( ELocationPickerContent );
        mLocationPickerLandscapeView->manageGridView();
        setCurrentView(mLocationPickerLandscapeView);
    }
    else
    { 
        mLocationPickerPotraitView->setViewType( ELocationPickerContent );
        mLocationPickerPotraitView->manageListView();
        setCurrentView( mLocationPickerPotraitView );
    }
}

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::changeOrientation()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::changeOrientation( Qt::Orientation )
{   
    //check the orientation and load view accordingly 
    if( orientation() == (Qt::Horizontal ))
    {
        loadLandscape();
    }
    else
    {
        loadPotrait();
    }
}

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::loadPotrait()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::loadPotrait()
{   
	//load potrait view except for search view
    if(mviewType != ELocationPickerSearchView)
	  {
        if(mLocationPickerLandscapeView->getViewType() == ELocationPickerContent)
        {
        mLocationPickerPotraitView->setViewType(ELocationPickerContent);
        }
    	mLocationPickerPotraitView->manageListView();
    	setCurrentView( mLocationPickerPotraitView );
    }
}

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::loadLandscape()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::loadLandscape()
{   
    if(mviewType != ELocationPickerSearchView)
	 {
         //load landscape view in current potrait content
         mLocationPickerLandscapeView->setViewType( mLocationPickerPotraitView->getViewType() );
         //load landscape for all content except collectionlistcontent
    	 if(mLocationPickerPotraitView->getViewType() != ELocationPickerCollectionListContent)
    	 {
             mLocationPickerLandscapeView->manageGridView();
    		 setCurrentView(mLocationPickerLandscapeView);
		 }
	 }
}

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::loadLandscape()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::connectPotraitSlots()
{
     connect(mLocationPickerPotraitView,SIGNAL(switchToSearchView()),this,SLOT(activateSearchView()));
     connect(mLocationPickerPotraitView,SIGNAL(selectItem( quint32 )),this,SLOT(itemSelected( quint32 )));
     connect(mLocationPickerPotraitView,SIGNAL(completeService()),this,SLOT(serviceComplete()));
     connect(mLocationPickerPotraitView,SIGNAL(sendCategoryID( quint32 )),this,SLOT(setCategoryId( quint32 )));
     connect(mLocationPickerPotraitView,SIGNAL(handleAllList()),this,SLOT(allListHandle()));
    connect(mLocationPickerPotraitView,SIGNAL(collectionContentExited()),this,SLOT(clearContentModels()));
}

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::loadLandscape()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::connectLandscapeSlots()
{   
    connect(mLocationPickerLandscapeView,SIGNAL(switchToSearchView()),this,SLOT(activateSearchView()));
    connect(mLocationPickerLandscapeView,SIGNAL(selectItem( quint32 )),this,SLOT(itemSelected( quint32 )));
    connect(mLocationPickerLandscapeView,SIGNAL(completeService()),this,SLOT(serviceComplete()));
    connect(mLocationPickerLandscapeView,SIGNAL(sendCategoryID( quint32 )),this,SLOT(setCategoryId( quint32 )));
    connect(mLocationPickerLandscapeView,SIGNAL(handleCollectionList()),this,SLOT(handleCollectionList()));
    connect(mLocationPickerLandscapeView,SIGNAL(collectionContentExited()),this,SLOT(clearContentModels()));
}

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::setCategoryId()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::setCategoryId( quint32 acategoryId )
{
    //set the same category id to both views
    mLocationPickerPotraitView->setCategoryID(acategoryId);
    mLocationPickerLandscapeView->setCategoryID(acategoryId);
    //Load the collectioncontent in appropriate orientation
    if(orientation() == Qt::Vertical)
    {
        mviewType = ELocationPickerCollectionContent;
        mLocationPickerPotraitView->manageListView();
        setCurrentView( mLocationPickerPotraitView );
    }
    else
    {   
        mviewType = ELocationPickerCollectionContent;
        loadLandscape();        
    }
}

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::handleCollectionList()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::handleCollectionList()
{  
    //In collection List both view should look same(list)
    mLocationPickerPotraitView->setViewType( ELocationPickerCollectionListContent );
    mviewType = ELocationPickerCollectionListContent;
    mLocationPickerPotraitView->manageListView();
    setCurrentView( mLocationPickerPotraitView );
}

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::allListHandle()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::allListHandle()
{
    //all list after collection list in horizontal orientation
    mLocationPickerLandscapeView->setViewType( ELocationPickerContent );
    mLocationPickerLandscapeView->manageGridView();
    setCurrentView(mLocationPickerLandscapeView);
}

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::clearContentModels()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::clearContentModels()
{
    mLocationPickerLandscapeView->clearContentModel();
    mLocationPickerPotraitView->clearContentModel();
}

Q_IMPLEMENT_USER_METATYPE(QLocationPickerItem)
