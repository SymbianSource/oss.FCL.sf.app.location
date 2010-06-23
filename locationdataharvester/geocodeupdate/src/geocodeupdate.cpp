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
#include <QEventLoop>
#include <locationservicedefines.h>
#include "geocodeupdate.h"
#include "mylocationsdefines.h"
#include "mylocationlogger.h"

using namespace QTM_NAMESPACE;


// ----------------------------------------------------------------------------
// GeocodeUpdate::GeocodeUpdate()
// ----------------------------------------------------------------------------
GeocodeUpdate::GeocodeUpdate()
{
    
}

// ----------------------------------------------------------------------------
// GeocodeUpdate::~GeocodeUpdate()
// ----------------------------------------------------------------------------
GeocodeUpdate::~GeocodeUpdate()
{
    __TRACE_CALLSTACK;
    
}

// ----------------------------------------------------------------------------
// GeocodeUpdate::createContactdb()
// ----------------------------------------------------------------------------
void GeocodeUpdate::createContactdb()
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
bool GeocodeUpdate::updateGeocodeToContactDB(const quint32 contactId,
        const int addressType, const double latitude,
        const double longitude)

{
    __TRACE_CALLSTACK;
    QContactManager* contactManger = NULL;
    MYLOCLOGSTRING("call to create contactManger object .");
    contactManger = new QContactManager("symbian");
    MYLOCLOGSTRING("contactManger object created .");

    MYLOCLOGSTRING("contactManger object is not null .");
    QContact contact = contactManger->contact( contactId );
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
            return false;       
        }
    }
    location.setLongitude(longitude);
    location.setLatitude(latitude);
    contact.saveDetail(&location);
    bool ret=false;
    ret=contactManger->saveContact(&contact);
    delete contactManger;
    return ret;
}

// ----------------------------------------------------------------------------
// CGeocodeUpdate::updateGeocodeToCalenderDB()
// Geo-cordinate updation to contact db  
// ----------------------------------------------------------------------------
bool GeocodeUpdate::updateGeocodeToCalenderDB(const ulong calEntryId,
        const double latitude, const double longitude)

{
    __TRACE_CALLSTACK;
    AgendaUtil agendaUtil ;    
    connect(&agendaUtil , SIGNAL(instanceViewCreationCompleted(int)) ,this ,SLOT(agendautilInstanceCreated(int)));
    QEventLoop loop;
    connect(this,SIGNAL(eventCompleted()),&loop,SLOT(quit()));
    loop.exec();
    AgendaEntry agendaEntry (agendaUtil.fetchById(calEntryId));
    MYLOCLOGSTRING("agenda entry created from calender id .");
    AgendaGeoValue geoValue;
    geoValue.setLatLong(latitude,longitude);
    MYLOCLOGSTRING("latitude and longitude set to  AgendaGeoValue object.");
    agendaEntry.setGeoValue(geoValue);
    bool ret=false;
    ret=agendaUtil.updateEntry(agendaEntry) ;
    return ret;
}

void GeocodeUpdate::agendautilInstanceCreated(int status)
{
    if (AgendaUtil::NoError == status){
        emit eventCompleted();
    }
}
//end of line
