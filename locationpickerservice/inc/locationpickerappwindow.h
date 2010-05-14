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

#include "locationpickertypes.h"

// Forward declarations
class LocationPickerSearchView;
class LocationPickerDocumentLoader;
class LocationPickerPotraitView;
class LocationPickerService;
class LocationPickerProxyModel;
class LocationPickerLandscapeView;
class LocationPickerContent;

class LocationPickerAppWindow: public HbMainWindow
{
    Q_OBJECT
public:
    // constructor
    LocationPickerAppWindow(QWidget *parent=0, Hb::WindowFlags 
            windowFlags=Hb::WindowFlagNone);
    
    // destructor
    ~LocationPickerAppWindow();

private:
    //Loads the Potrait View
    void loadPotrait();
    //Loads landscape View
    void loadLandscape();
    //Connect the slots for Potrait view
    void connectPotraitSlots();
    //Connect the slots for Landscape view
    void connectLandscapeSlots();
public slots:
    // a list item is selected
    void itemSelected( quint32 aLm );
private slots:
    //activate search view
    void activateSearchView();
    //activate locationpicker view
    void activateLocationPickerView();
    //changes the orientation
    void changeOrientation( Qt::Orientation );
    //complete the service
    void serviceComplete();
    //sets the category ID during orientation change in collection content
    void setCategoryId( quint32 acategoryId );
    //handles orientation change in collection list
    void handleCollectionList();
    //handle all List
    void allListHandle();
    //clear content models
    void clearContentModels();
private:
    // search view
    LocationPickerSearchView* mLocationPickerSearchView;
    //document loader
    LocationPickerDocumentLoader* mLocationPickerDocumentLoader;
    //location picker potrait view
    LocationPickerPotraitView* mLocationPickerPotraitView;
    //location picker landscape view
    LocationPickerLandscapeView* mLocationPickerLandscapeView;
	//locationpicker content
    LocationPickerContent* mLocationPickerContent;
    //location picker service;
    LocationPickerService *mService;
    //View Type
    TViewType mviewType;
};
#endif // LOCATIONPICKERAPPWINDOW_H
