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
* Description: LocationPickerSearchView declaration
*
*/

#ifndef LOCATIONPICKERSEARCHVIEW_H
#define LOCATIONPICKERSEARCHVIEW_H


#include <HbView>

//forward declarations
class HbListView;
class HbAction;
class QGraphicsLinearLayout;
class QStandardItemModel;
class HbSearchPanel;
class LocationPickerAppWindow;
class LocationPickerDataManager;
class LocationPickerProxyModel;

/**  Class defines the search view of location picker
 *
 */
class LocationPickerSearchView : public HbView
{
    Q_OBJECT
public:
    // constructor
    LocationPickerSearchView( LocationPickerAppWindow *aWindow, QGraphicsItem* aParent = 0 );
    // destructor
    ~LocationPickerSearchView();

private slots:
    // slot to perform search
    void doSearch(QString aCriteria);
    // slot to handle select event on a list item
    void handleActivated(const QModelIndex &aIndex);

private:

    //proxymodel used for sort and filter
    LocationPickerProxyModel *mProxyModel;
    // model for the view
    QStandardItemModel *mModel;
    // graphics linear layout for list items and search panel
    QGraphicsLinearLayout* mLayout;
    // list view
    HbListView  *mListView;
    // search panel
    HbSearchPanel *mSearchPanel;
    // handle to main window
    LocationPickerAppWindow *mWindow;
    // handle to data manager to populate model
    LocationPickerDataManager *mDataManager;
    HbAction *mSecondaryBackAction;
};


#endif // LOCATIONPICKERSEARCHVIEW_H
