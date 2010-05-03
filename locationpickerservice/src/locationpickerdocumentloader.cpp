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
* Description: locationpickerdocumentloader implementation
*
*/

#include "locationpickerdocumentloader.h"
#include "locationpickerpotraitview.h"
#include "locationpickersearchview.h"
#include "locationpickerlandscapeview.h"

// ---------------------------------------------------------------------------
// LocationPickerDocumentLoader::LocationPickerDocumentLoader()
// ---------------------------------------------------------------------------
//
LocationPickerDocumentLoader::LocationPickerDocumentLoader()
{
}

// ---------------------------------------------------------------------------
// LocationPickerDocumentLoader::~LocationPickerDocumentLoader()
// ---------------------------------------------------------------------------
//
LocationPickerDocumentLoader::~LocationPickerDocumentLoader()
{
}

// ---------------------------------------------------------------------------
// LocationPickerDocumentLoader::createObject
// ---------------------------------------------------------------------------
//
QObject *LocationPickerDocumentLoader::createObject(const QString& type, const QString &name)
{   
    //for locationpickerpotraitview
    if (name == LocationPickerPotraitView::staticMetaObject.className() )
    {
        QObject *object = new LocationPickerPotraitView(this);
        object->setObjectName(name);
        return object;
    }
    //for locationpickerlandscapeview
    if (name == LocationPickerLandscapeView::staticMetaObject.className())
    {
        QObject *object = new LocationPickerLandscapeView(this);
        object->setObjectName(name);
        return object;
    }
    //for locationpickersearchview
    if (name == LocationPickerSearchView::staticMetaObject.className())
    {
        QObject *object = new LocationPickerSearchView(*this);
        object->setObjectName(name);
        return object;
    }
    //default case
    return HbDocumentLoader::createObject(type, name);
}
