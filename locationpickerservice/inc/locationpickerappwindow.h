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
* Description: LocationPickerAppWindow declaration
*
*/

#ifndef LOCATIONPICKERAPPWINDOW_H
#define LOCATIONPICKERAPPWINDOW_H
#include <HbMainWindow>
#include <HbAction>

#include "qlocationpickeritem.h"

// Forward declarations
class LocationPickerAllView;
class LocationPickerSearchView;
class LocationPickerCollectionListView;
class LocationPickerService;

class LocationPickerAppWindow: public HbMainWindow
{
    Q_OBJECT
public:

    // constructor
    LocationPickerAppWindow( QWidget* aParent = 0 );
    
    // destructor
    ~LocationPickerAppWindow();
	
	// deletes the collection content view
    void deleteCollectionContentView();

public slots:
    // a list item is selected
    void itemSelected( quint32 aLm);

public slots:
	// slot used for back action on the top right corner
    void backButtonTriggered();

private:
    // all view
    LocationPickerAllView* mLocationPickerAllView;
    // search view
    LocationPickerSearchView* mLocationPickerSearchView;
    // collection list view
    LocationPickerCollectionListView* mLocationPickerCollectionListView;

    // location picker service;
    LocationPickerService *mService;

};
#endif // LOCATIONPICKERAPPWINDOW_H
