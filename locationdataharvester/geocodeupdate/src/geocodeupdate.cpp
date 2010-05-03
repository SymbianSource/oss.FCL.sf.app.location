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
* Description: 
*
*/

#include <qcontactgeolocation.h>
#include <qcontactmanager.h>
#include <qtcontacts.h>
#include <cntdef.h>
#include <qstring>

#include "geocodeupdate.h"
#include "mylocationsdefines.h"
#include "mylocationlogger.h"

using namespace QTM_NAMESPACE;
// ----------------------------------------------------------------------------
// CGeocodeUpdate::CreateContactdb()
// ----------------------------------------------------------------------------
EXPORT_C void GeocodeUpdate::CreateContactdb()
{
    QContactManager* contactManger = NULL;
    MYLOCLOGSTRING("call to create contactManger object and contactdb as well.");
    contactManger = new QContactManager("symbian");
    delete contactManger;
}

// ----------------------------------------------------------------------------
// CGeocodeUpdate::UpDateL()
// To update contact db with this latitude and longitude value 
// ----------------------------------------------------------------------------
EXPORT_C void GeocodeUpdate::UpDate(const TInt32 aCntId,
        const TInt32 aCntAddressType, const TReal aLatitude,
        const TReal aLongitude)

{
    __TRACE_CALLSTACK;
    QContactManager* contactManger = NULL;
    MYLOCLOGSTRING("call to create contactManger object .");
    contactManger = new QContactManager("symbian");
    MYLOCLOGSTRING("contactManger object created .");

    MYLOCLOGSTRING("contactManger object is not null .");
    QStringList definitionRestrictions;
    QContact contact = contactManger->contact(aCntId ,definitionRestrictions);
    QContactGeoLocation location;

    switch (aCntAddressType)
    {
    case ESourceContactsPref:
    {
        break;
    }
    case ESourceContactsWork:
    {
        location.setContexts(QContactDetail::ContextWork);
        break;
    }
    case ESourceContactsHome:
    {
        location.setContexts(QContactDetail::ContextHome);
        break;
    }
    default:
    {
        break;
    }
    }
    location.setLongitude(aLongitude);
    location.setLatitude(aLatitude);
    contact.saveDetail(&location);
    contactManger->saveContact(&contact);
    delete contactManger;

}

//end of line
