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
* Description: LocationPickerDataManager private declaration
*
*/

#ifndef LOCATIONPICKERDATAMANAGER_P_H
#define LOCATIONPICKERDATAMANAGER_P_H

#include <QStandardItemModel>
#include "locationpickertypes.h"
#include "qlocationpickeritem.h"

class CPosLmItemIterator;
class CPosLandmarkDatabase;
class CPosLmCategoryManager;
class CPosLandmarkSearch;

/**  Class used for managing the data of model
 *
 */
class LocationPickerDataManagerPrivate
{
public:
    // constructors
    LocationPickerDataManagerPrivate();
    LocationPickerDataManagerPrivate( QStandardItemModel &aModel, TViewType aViewType );
    
    // destructor
    ~LocationPickerDataManagerPrivate();
    
    // populates the model with data
    bool populateModel( quint32 aCollectionId = 0 );
    
    // gets the data pointed to by index and copies to the aValue
    void getData(int index, quint32& aValue );
    
    // gets the location item
    void getLocationItem( quint32 aLmId, QLocationPickerItem &aItem );

private:
    // populates model
    bool populateModelL( quint32 aCollectionId );
    // populates landmarks
    void populateLandmarksL();
    // populates collections
    void populateCollectionsL();
    // gets the location item
    void getLocationItemL( quint32 aLmId, QLocationPickerItem &aItem );

private:
	
    QStandardItemModel *mModel;
    TViewType mViewType;
    CPosLmItemIterator* mIterator;
    CPosLandmarkDatabase* mLandmarkDb;
    CPosLmCategoryManager *mLmCategoryManager;
    CPosLandmarkSearch *mLandmarkSearch;
    quint32 mCategoryId;
};

#endif // LOCATIONPICKERDATAMANAGER_P_H
