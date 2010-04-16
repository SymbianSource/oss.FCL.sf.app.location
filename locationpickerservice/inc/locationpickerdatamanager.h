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
* Description: LocationPickerDataManager declaration
*
*/

#ifndef LOCATIONPICKERDATAMANAGER_H
#define LOCATIONPICKERDATAMANAGER_H

#include <QStandardItemModel>
#include "locationpickertypes.h"
#include "qlocationpickeritem.h"

class LocationPickerDataManagerPrivate;

/**  Class used for managing the data of model
 *
 */
class LocationPickerDataManager
{
public:
    // constructors
    LocationPickerDataManager();
    LocationPickerDataManager( QStandardItemModel &aModel, TViewType aViewType );

    // destructor
    ~LocationPickerDataManager();

    // populates the model with data
    bool populateModel( quint32 aCollectionId = 0 );

    // gets the data pointed to by index and copies to the aValue
    void getData(int index, quint32& aValue );

    // gets the location item
    void getLocationItem( quint32 aLmId, QLocationPickerItem &aItem );

private:

    LocationPickerDataManagerPrivate* const d_ptr;
    Q_DECLARE_PRIVATE_D(d_ptr, LocationPickerDataManager)
};

#endif // LOCATIONPICKERDATAMANAGER_H
