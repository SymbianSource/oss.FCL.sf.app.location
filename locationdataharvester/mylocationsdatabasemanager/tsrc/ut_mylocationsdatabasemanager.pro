#
# Copyright (c) 2010  Nokia Corporation and/or its subsidiary(-ies).


#
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
# 
# Description:
#
#

TEMPLATE = app
TARGET = ut_mylocationsdatabasemanager   

CONFIG += hb
QT += testlib

symbian:
{
   SYSTEMINCLUDEPATH += \epoc32\include\stdapis
}

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../../inc
INCLUDEPATH += ../../mylocationlogger/inc

defines += EPOCALLOWDLLDATA
DEFINES += LOCATION_DATA_HARVESTER_UNIT_TEST

symbian: { 

    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = All -Tcb
    
    LIBS += -llbs \
        -leposlandmarks \
        -leposlmsearchlib \
        -leuser \
        -leposlmdbmanlib \
        -lcntmodel \
        -lefsrv \
        -lflogger \
        -ledbms \
        -lbafl \
        -lcalinterimapi \
        -llocationdatalookupdb

}

SOURCES += ../src/mylocationsdatabasemanager.cpp \
ut_mylocationsdatabasemanager.cpp

HEADERS += ../inc/mylocationsdatabasemanager.h 




