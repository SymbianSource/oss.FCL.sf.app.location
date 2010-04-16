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
* Description: LocationPickerCollectionListView declaration
*
*/

#ifndef LOCATIONPICKERCOLLECTIONCONTENT_H
#define LOCATIONPICKERCOLLECTIONCONTENT_H

#include <HbView>

class HbListView;
class HbAction;
class QStandardItemModel;
class LocationPickerProxyModel;
class LocationPickerDataManager;
class LocationPickerAppWindow;

/**  Class for handling a view collection content
 *
 */
class LocationPickerCollectionContent : public HbView
{
    Q_OBJECT
public:
    // constructor
    LocationPickerCollectionContent( LocationPickerAppWindow *aWindow,
                                     quint32 aCollectionid, QGraphicsItem* aParent = 0);
    ~LocationPickerCollectionContent();
private slots:
    // slot triggered when sort ascending is selected in menu
    void sortAscending();
    // slot triggered when sort descending is selected in menu
    void sortDescending();

    // slot used to handle when a location is selected in list view
    void handleActivated(const QModelIndex &aIndex);

private:
    // constructs a menu for the view
    void constructMenu();

private:
    HbListView  *mListView;
    LocationPickerProxyModel *mProxyModel;
    QStandardItemModel *mModel;
    LocationPickerDataManager *mDataManager;
    LocationPickerAppWindow* mWindow;
    HbAction *mSecondaryBackAction;
};

#endif // LOCATIONPICKERCOLLECTIONCONTENT_H
