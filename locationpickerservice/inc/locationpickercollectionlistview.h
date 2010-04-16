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

#ifndef LOCATIONPICKERCOLLECTIONLISTVIEW_H
#define LOCATIONPICKERCOLLECTIONLISTVIEW_H

#include <HbView>

class HbListView;
class HbAction;
class QStandardItemModel;
class LocationPickerCollectionContent;
class LocationPickerAppWindow;
class LocationPickerDataManager;

/**  Class for handling Collections view of location picker
 *
 */
class LocationPickerCollectionListView : public HbView
{
    Q_OBJECT
public:

    // constructor
    LocationPickerCollectionListView( LocationPickerAppWindow *aWindow, QGraphicsItem* aParent = 0);

    // destructor
    ~LocationPickerCollectionListView();
	
	// slot used to delete the child view, ie collection content view.
    void deleteCollectionContentView();


private slots:

    // slot to handle event when a collection is selected
    void handleActivated( const QModelIndex &aIndex );
    	
private:
    HbListView  *mListView;
    QStandardItemModel *mModel;
    LocationPickerCollectionContent *mCollectionContentView;

    LocationPickerAppWindow *mWindow;
    LocationPickerDataManager *mDataManager;
    HbAction *mSecondaryBackAction;
};

#endif // LOCATIONPICKERCOLLECTIONLISTVIEW_H
