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
* Description: LocationPickerToolBar implementation
*
*/

#include "locationpickertoolbar.h"
#include "locationpickertypes.h"
#include "locationpickerappwindow.h"


// --------------------------------------------------
// LocationPickerToolBar::LocationPickerToolBar()
// --------------------------------------------------
LocationPickerToolBar::LocationPickerToolBar( LocationPickerAppWindow* aWindow, QGraphicsItem* aParent ):
        HbToolBar( aParent )
{
    mWindow = aWindow;

    // Add all tab and connect to the slot
    mAllAction = addAction(HbIcon(KAllTab),QString(""));
    connect(mAllAction, SIGNAL(triggered()), this, SLOT(AllTabTriggered()));

    // Add collection tab and connect to the slot
    mCollectionAction = addAction(HbIcon(KCollectionTab),QString(""));
    connect(mCollectionAction, SIGNAL(triggered()), this, SLOT(CollectionTabTriggered()));

     // Add search tab and connect to the slot
    mSearchAction = addAction(HbIcon(KSearchTab),QString(""));
    connect(mSearchAction, SIGNAL(triggered()), this, SLOT(SearchTabTriggered()));
}

// --------------------------------------------------
//  LocationPickerToolBar::disableTabs()
// --------------------------------------------------
void LocationPickerToolBar::disableTabs()
{
    mAllAction->setDisabled( true );
    mCollectionAction->setDisabled( true );
    mSearchAction->setDisabled( true );
}
// --------------------------------------------------
//  LocationPickerToolBar::AllTabTriggered()
// --------------------------------------------------
void LocationPickerToolBar::AllTabTriggered()
{
    int viewIndex = mWindow->currentViewIndex();
    mWindow->setCurrentViewIndex(0);

    if( viewIndex > KNumberOfMainViews-1 )
    {
        // the current view is collection view, so delete before switching to another view
        mWindow->deleteCollectionContentView();
    }
}

// --------------------------------------------------
// LocationPickerToolBar::CollectionTabTriggered()
// --------------------------------------------------
void LocationPickerToolBar::CollectionTabTriggered()
{
    int viewIndex = mWindow->currentViewIndex();
    mWindow->setCurrentViewIndex(2);
   
    if( viewIndex > KNumberOfMainViews-1 )
    {
        // the current view is collection view, so delete before switching to another view
        mWindow->deleteCollectionContentView();
    }
}

// --------------------------------------------------
// LocationPickerToolBar::SearchTabTriggered()
// --------------------------------------------------
void LocationPickerToolBar::SearchTabTriggered()
{
    int viewIndex = mWindow->currentViewIndex();
    mWindow->setCurrentViewIndex(1);
    
    if( viewIndex > KNumberOfMainViews-1 )
    {
        // the current view is collection view, so delete before switching to another view
        mWindow->deleteCollectionContentView();
    }
}
