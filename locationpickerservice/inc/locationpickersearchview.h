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

class HbListView;
class QStandardItemModel;
class HbSearchPanel;
class LocationPickerDataManager;
class LocationPickerProxyModel;
class HbDocumentLoader;
class HbTextItem;
class QGraphicsLinearLayout;

/**  
 * Class defines the search view of location picker
 */
class LocationPickerSearchView : public HbView
{
    Q_OBJECT
public:
    // constructor
    LocationPickerSearchView(HbDocumentLoader &aLoader);
    // destructor
    ~LocationPickerSearchView();
    //initialize the action items and connect to slots
    void init();
private slots:
    // slot to perform search
    void doSearch(QString aCriteria);
    // slot to handle select event on a list item
    void handleActivated(const QModelIndex &aIndex);
    // slot to handle backbutton on search panel
    void handleExit();
signals:
    //signals to switch current view
    void switchView();
    //signals when any item is selected
    void selectItem( quint32 aLm );
private:
    //proxymodel used for sort and filter
    LocationPickerProxyModel *mProxyModel;
    // model for the view
    QStandardItemModel *mModel;
    // list view
    HbListView  *mListView;
    // search panel
    HbSearchPanel *mSearchPanel;
    // handle to data manager to populate model
    LocationPickerDataManager *mDataManager;
    //TextItem
    HbTextItem* mEmptyLabel;
    //Graphicslayout
    QGraphicsLinearLayout* mVerticalLayout;
    //Documentloader
    HbDocumentLoader &mDocumentLoader;
};


#endif // LOCATIONPICKERSEARCHVIEW_H
