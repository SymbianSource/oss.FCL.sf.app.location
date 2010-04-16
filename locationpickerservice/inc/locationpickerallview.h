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
* Description: LocationPickerAllView declaration
*
*/

#ifndef LOCATIONPICKERALLVIEW_H
#define LOCATIONPICKERALLVIEW_H

#include <HbView>
#include "locationpickerproxymodel.h"

//forward declaration
class HbListView;
class HbAction;
class QStandardItemModel;
class LocationPickerAppWindow;
class LocationPickerDataManager;

/**  Class for handling a 'All' view of location picker
 *
 */
class LocationPickerAllView : public HbView
{
    Q_OBJECT
public:

    // contructor
    LocationPickerAllView( LocationPickerAppWindow *aWindow, QGraphicsItem* aParent = 0 );

    // destructor
    ~LocationPickerAllView();
    
    // locationsNotFound
    bool locationsFound();

private slots:
    // slot triggered when sort by ascending is selected in menu
    void sortAscending();
    // slot triggered when sort by descending is selected in menu
    void sortDescending();
    
    // slot to handle select event on a list item
    void handleActivated( const QModelIndex &aIndex );

private:
    // used to construct a menu for the view
    void constructMenu();

private:
    HbListView  *mListView;
    LocationPickerProxyModel *mProxyModel;
    QStandardItemModel *mModel;
    LocationPickerAppWindow *mWindow;
    LocationPickerDataManager *mDataManager;
    bool mLocationsFound;
    HbAction *mSecondaryBackAction;
};


#endif // LOCATIONPICKERALLVIEW_H
