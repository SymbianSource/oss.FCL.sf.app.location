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
* Description: LocationPickerLandscapeView declaration
*
*/

#ifndef LOCATIONPICKERLANDSCAPEVIEW_H_
#define LOCATIONPICKERLANDSCAPEVIEW_H_



#include <hbview>
#include <hbdocumentloader.h>
#include "locationpickertypes.h"

//forward declarations
class HbListView;
class QStandardItemModel;
class LocationPickerProxyModel;
class LocationPickerContent;
class LocationPickerCollectionListContent;
class LocationPickerCollectionContent;
class HbGridViewItem;
class HbGridView;
class HbAction;

/**  
 * Class defines the location picker view
 */
class LocationPickerLandscapeView : public HbView
{

    Q_OBJECT
public:
    // constructor
    LocationPickerLandscapeView(HbDocumentLoader* aLoader);
    // destructor
    ~LocationPickerLandscapeView();
private:
    //disable the tabs
    void disableTabs();
public:
    //get the items from docml and connect to respective slots
    void init(Qt::Orientation aOrientation );
    //Set the appropriate model on grid view
    void manageGridView();
    //Create collection list and sets to list view
    void setCollectionData( quint32 aCategoryId );
    //Set Ctegory ID
    void setCategoryID( quint32 aCategoryId  );
    //Get the view type
    TViewType getViewType();
    //set the view type
    void setViewType(TViewType aViewType);
private slots:
    //slot to handle list item actions     
    void handleActivated(const QModelIndex &aIndex);
    //slots to handle menu action items     
    void sortDescending();
    void sortAscending();
    void backButtonTriggered();
    //slot to handle search tab
    void searchTabTriggered();
public slots:
    //slot to handle all tab
    void allTabTriggered();
    //slot to handle collection tab
    void colectionTabTriggered();
signals:
    void switchToSearchView();
    void handleCollectionList();
    void selectItem( quint32 aLm );
    void completeService();
    void sendCategoryID(  quint32 acategoryId  );
private:
    //document loader
    HbDocumentLoader* mDocumentLoader;
    // all view
    LocationPickerContent* mLocationPickerContent;
    // collection list content
    LocationPickerCollectionListContent* mLocationPickerCollectionListContent;
    //actions
    HbAction *mAllAction;
    HbAction *mCollectionAction;
    HbAction *mSearchAction;
    HbAction *mAscendingAction;
    HbAction *mDescendingAction;
    //secondary back action
    HbAction *mLandscapeBackAction;
    HbGridView *mGridView;
    //collection content 
    LocationPickerCollectionContent *mCollectionContent;
    HbGridViewItem *mGridViewItem;
    //view type
    TViewType mViewType ;
    //get the collection/category id
    quint32 mCategoryId;

};

#endif /* LOCATIONPICKERLANDSCAPEVIEW_H_ */
