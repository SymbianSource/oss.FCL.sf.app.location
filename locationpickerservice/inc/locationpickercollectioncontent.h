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
* Description: LocationPickerCollectionContent declaration
*
*/

#ifndef LOCATIONPICKERCOLLECTIONCONTENT_H
#define LOCATIONPICKERCOLLECTIONCONTENT_H

class QStandardItemModel;
class LocationPickerProxyModel;
class LocationPickerDataManager;

/**  
 * Class for handling collection content
 */
class LocationPickerCollectionContent : public QObject
{
public:
    // constructor
    LocationPickerCollectionContent( Qt::Orientations aOrientation , quint32 aCollectionid );
    //Destructor
    ~LocationPickerCollectionContent();
    //get proxy model
    LocationPickerProxyModel* getProxyModel();
    //get LocationPickerDataManager
    LocationPickerDataManager* getDataManager();

private:
    Qt::Orientations mOrientation;
    LocationPickerProxyModel *mProxyModel;
    QStandardItemModel *mModel;
    LocationPickerDataManager *mDataManager;
};

#endif // LOCATIONPICKERCOLLECTIONCONTENT_H
