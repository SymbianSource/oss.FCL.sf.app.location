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
* Description: LocationPickerPotraitView declaration
*
*/

#ifndef LOCATIONPICKERPOTRAITVIEW_H
#define LOCATIONPICKERPOTRAITVIEW_H


#include <HbView>
#include <hbdocumentloader.h>
#include "locationpickertypes.h"
#include <QGraphicsLinearLayout>

#include <HbLabel>

//forward declarations
class HbListView;
class QStandardItemModel;
class LocationPickerProxyModel;
class LocationPickerCollectionListContent;
class LocationPickerCollectionContent;
class HbListViewItem;
class HbAction;
class HbAbstractViewItem;
class HbMenu;
class QPoint;
/**  
 * Class defines the location picker view
 */
class LocationPickerPotraitView : public HbView
{
    Q_OBJECT
public:
    // constructor
    LocationPickerPotraitView( HbDocumentLoader* aLoader );
    // destructor
    ~LocationPickerPotraitView();
public:
    //disable the tabs
    void disableTabs( QStandardItemModel *aModel );
    //get the items from docml and connect to respective slots
    void init( bool aPopulated, Qt::Orientation aOrientation, QStandardItemModel *aModel );
    //Set the appropriate model on list view
    void manageListView();
    //Create collection list and sets to list view
    void setCollectionData( quint32 acategoryId );
    //Set Ctegory ID
    void setCategoryID( quint32 acategoryId  );
    //Get the view type
    TViewType getViewType();
    //set the view type
    void setViewType( TViewType aViewType );
    //clear collection Model
    void clearContentModel();
private slots:
    //slot to handle list item actions     
    void handleActivated( const QModelIndex &aIndex );
    //slots to handle menu action items     
    void sortDescending();
    void sortAscending();
    void backTriggered();
    //slot to handle search tab
    void searchTabTriggered();
	//launch context menu
    void launchPopUpMenu(HbAbstractViewItem *aItem, const QPointF &aPoint);
	//handle long press
    void handleLongPress();
public slots:
    //slot to handle all tab
    void allTabTriggered();
    //slot to handle collection tab
    void colectionTabTriggered();
signals:
    void switchToSearchView();
    void selectItem( quint32 aLm );
    void completeService();
    void sendCategoryID( quint32 aCategoryId );
    void handleAllList();
    void collectionContentExited();
private:
    //document loader
    HbDocumentLoader* mDocumentLoader;
    // collection list content
    LocationPickerCollectionListContent* mLocationPickerCollectionListContent;
    //locationPickerProxyModel
    LocationPickerProxyModel *mProxyModel;
	//standard model
    QStandardItemModel *mModel;
    //actions
    HbAction *mAllAction;
    HbAction *mCollectionAction;
    HbAction *mSearchAction;
    HbAction *mAscendingAction;
    HbAction *mDescendingAction;
    //secondary back action
    HbAction *mPotraitBackAction;
    // list view
    HbListView  *mListView;
    //list item
    HbListViewItem *mListItem;
    //collection content 
    LocationPickerCollectionContent *mCollectionContent;
    //ViewType
    TViewType mViewType ;
    // get the collection/category id
    quint32 mCategoryId;
    //label to show current view
    QModelIndex mIndex;
    //linear Layout
    QGraphicsLinearLayout *mLinerLayout;
	//label to show detail of view
    HbLabel *mColllabel;
    //context menu
    HbMenu* mLongPressMenu;
};


#endif // LOCATIONPICKERPOTRAITVIEW_H
