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
* Description: LocationPickerContent declaration
*
*/

#ifndef LOCATIONPICKERCONTENT_H
#define LOCATIONPICKERCONTENT_H


#include "locationpickerproxymodel.h"

//forward declaration
class QStandardItemModel;
class LocationPickerDataManager;

/**  
 *Class to create model for Location Picker
 */
class LocationPickerContent : public QObject
{
    Q_OBJECT
public:

    // contructor
    LocationPickerContent(Qt::Orientation aOrientation );

    // destructor
    ~LocationPickerContent();
    
    // locationsNotFound
    bool locationsFound();
    
    //get proxy model
    LocationPickerProxyModel* getListProxyModel();
    
    //get standard Model
    QStandardItemModel* getStandardModel();
    
    //get data manager
    LocationPickerDataManager* getDataManager();
    
    //get hbgridview proxy model
    LocationPickerProxyModel* getGridProxyModel();

private:
    Qt::Orientations mOrientation;
    LocationPickerProxyModel *mListProxyModel;
    LocationPickerProxyModel *mProxyGridModel;
    LocationPickerDataManager *mDataManager;
    QStandardItemModel *mModel;
    bool mLocationsFound;
    
};


#endif // LOCATIONPICKERCONTENT_H
