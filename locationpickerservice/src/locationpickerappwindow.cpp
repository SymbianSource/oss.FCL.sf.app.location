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
#include "locationpickertoolbar.h"
#include "locationpickerallview.h"
#include "locationpickersearchview.h"
#include "locationpickercollectionlistview.h"
#include "locationpickerservice.h"
#include "locationpickertypes.h"
#include "locationpickertypes.h"
#include "locationpickerdatamanager.h"
 
// ----------------------------------------------------------------------------
// LocationPickerAppWindow::LocationPickerAppWindow()
// ----------------------------------------------------------------------------
LocationPickerAppWindow::LocationPickerAppWindow(
        QWidget* aParent ):
        HbMainWindow( aParent )
{
    // create the service object;
    mService = new LocationPickerService( this );
  
    LocationPickerToolBar *toolBar = new LocationPickerToolBar( this );

    // Create 'all' view, set the tool bar and add the view to main window
    mLocationPickerAllView = new LocationPickerAllView( this ); 
    if( !mLocationPickerAllView->locationsFound() )
    {
        toolBar->disableTabs();
    }
    mLocationPickerAllView->setToolBar(toolBar);
    addView(mLocationPickerAllView);
    setCurrentView(mLocationPickerAllView);

    // Create 'search' view, set the tool bar and add the view to main window
    mLocationPickerSearchView = new LocationPickerSearchView( this );
    mLocationPickerSearchView->setToolBar(toolBar);
    addView(mLocationPickerSearchView);

    // Create 'collection' view, set the tool bar and add the view to main window
    mLocationPickerCollectionListView = new LocationPickerCollectionListView( this );
    mLocationPickerCollectionListView->setToolBar(toolBar);
    addView(mLocationPickerCollectionListView);
    
}


// ----------------------------------------------------------------------------
// LocationPickerAppWindow::~LocationPickerAppWindow()
// ----------------------------------------------------------------------------
LocationPickerAppWindow::~LocationPickerAppWindow()
{
    delete mService;
}


// ----------------------------------------------------------------------------
// LocationPickerAppWindow::backButtonTriggered()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::backButtonTriggered()
{
    if( currentViewIndex() > KNumberOfMainViews-1 )
    {
        //the current view is collection content view.
        //so come back to collection list view
        setCurrentView(mLocationPickerCollectionListView);
        deleteCollectionContentView();
    }
    else
    {
		// Other views request complete with a invalid location picker item
		QLocationPickerItem item;
		item.mIsValid = false;
        mService->complete(item);
    }
}

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::deleteCollectionContentView()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::deleteCollectionContentView()
{
	// call collection view's deleteCollectionContentView()
    mLocationPickerCollectionListView->deleteCollectionContentView();
}

// ----------------------------------------------------------------------------
// LocationPickerAppWindow::itemSelected()
// ----------------------------------------------------------------------------
void LocationPickerAppWindow::itemSelected( quint32 aLmid )
{
    QLocationPickerItem item;
    
    LocationPickerDataManager dataManager;
    dataManager.getLocationItem( aLmid, item );

    // complete the request
    mService->complete(item);
   
}


Q_IMPLEMENT_USER_METATYPE(QLocationPickerItem)
