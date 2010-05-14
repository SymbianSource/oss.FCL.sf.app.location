#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

TEMPLATE=app
TARGET=mylocationsengine
CONFIG += Qt
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../inc
INCLUDEPATH += ../../inc
INCLUDEPATH += ../geocodeupdate/inc
INCLUDEPATH += ../mylocationlogger/inc

symbian: { 

    TARGET.UID3 = 0x2002680A 
    isEmpty(TARGET.EPOCSTACKSIZE):TARGET.EPOCSTACKSIZE = 0x6000
    isEmpty(TARGET.EPOCHEAPSIZE):TARGET.EPOCHEAPSIZE = 20480 \
        16943040
    TARGET.CAPABILITY = ALL -TCB
    
    LIBS += -llbs \
        -leposlandmarks \
        -leposlmsearchlib \
        -leuser \
        -leposlmdbmanlib \
        -lcntmodel \
        -lefsrv \
        -ledbms \
        -lbafl \
        -lflogger \
        -lQtContacts \
        -lgeocodeupdate \
        -lecom \
	-lcalinterimapi \
	-llocationdatalookupdb

}

SOURCES += src/appmain.cpp \
    src/mylocationsengine.cpp \
    src/mylocationsdatabasemanager.cpp \
    src/mylocationslookupdb.cpp \
    src/maptileinterface.cpp \
    src/addresscomparision.cpp \   
    src/lookupmaptiledb.cpp    \
    src/calendernotification.cpp 

HEADERS += inc/appmain.h \
    inc/mylocationsengine.h \
    inc/mylocationslookupdb.h \
    inc/mylocationsdatabasemanager.h \
    inc/maptileinterface.h \
    inc/addresscomparision.h \
    inc/lookupmaptiledb.h \
    inc/calendernotification.h




