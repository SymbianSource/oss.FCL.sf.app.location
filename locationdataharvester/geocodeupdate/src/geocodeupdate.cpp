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
#include <agendautil.h>
#include <agendaentry.h>
#include <QString>
#include <locationservicedefines.h>

#include "geocodeupdate.h"
#include "mylocationsdefines.h"
#include "mylocationlogger.h"

using namespace QTM_NAMESPACE;
// ----------------------------------------------------------------------------
// GeocodeUpdate::createContactdb()
// ----------------------------------------------------------------------------
EXPORT_C void GeocodeUpdate::createContactdb()
{
    QContactManager* contactManger = NULL;
    MYLOCLOGSTRING("call to create contactManger object and contactdb as well.");
    contactManger = new QContactManager("symbian");
    delete contactManger;
}

// ----------------------------------------------------------------------------
// CGeocodeUpdate::updateGeocodeToContactDB()
// Geo-cordinate updation to contact db  
// ----------------------------------------------------------------------------
EXPORT_C void GeocodeUpdate::updateGeocodeToContactDB(const quint32 contactId,
        const int addressType, const double latitude,
        const double longitude)

{
    __TRACE_CALLSTACK;
    QContactManager* contactManger = NULL;
    MYLOCLOGSTRING("call to create contactManger object .");
    contactManger = new QContactManager("symbian");
    MYLOCLOGSTRING("contactManger object created .");

    MYLOCLOGSTRING("contactManger object is not null .");
    QStringList definitionRestrictions;
    QContact contact = contactManger->contact(contactId ,definitionRestrictions);
    QContactGeoLocation location;

    switch (addressType)
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
    location.setLongitude(longitude);
    location.setLatitude(latitude);
    contact.saveDetail(&location);
    contactManger->saveContact(&contact);
    delete contactManger;

}

// ----------------------------------------------------------------------------
// CGeocodeUpdate::updateGeocodeToCalenderDB()
// Geo-cordinate updation to contact db  
// ----------------------------------------------------------------------------
EXPORT_C void GeocodeUpdate::updateGeocodeToCalenderDB(const ulong calEntryId,
        const double latitude, const double longitude)

{
    __TRACE_CALLSTACK;
    AgendaUtil agendaUtil;
    AgendaEntry agendaEntry (agendaUtil.fetchById(calEntryId));
    MYLOCLOGSTRING("agenda entry created from calender id .");
    AgendaGeoValue geoValue;
    geoValue.setLatLong(latitude,longitude);
    MYLOCLOGSTRING("latitude and longitude set to  AgendaGeoValue object.");
    agendaEntry.setGeoValue(geoValue);
    agendaUtil.updateEntry(agendaEntry);
}
//end of line
