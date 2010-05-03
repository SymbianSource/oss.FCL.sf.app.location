#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).


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


TEMPLATE = lib
TARGET = maptileservice   

CONFIG += dll
CONFIG += hb
    

DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../inc


MOC_DIR = moc

DEFINES += MAPTILESERVICEDLL

INTERNAL_PUBLIC_HEADERS +=  inc/maptileservice.h        

# Input
HEADERS +=  $$INTERNAL_PUBLIC_HEADERS \
           ./inc/maptiledblookuptable.h  
           
               
    
SOURCES += ./src/maptileservice.cpp ./src/maptiledblookuptable.cpp

symbian: 
{

    :BLD_INF_RULES.prj_exports += "conf/maptileservice.confml APP_LAYER_CONFML(maptileservice.confml)"
    :BLD_INF_RULES.prj_exports += "conf/maptileservice_2002E6E8.crml APP_LAYER_CRML(maptileservice_2002E6E8.crml)"
    :BLD_INF_RULES.prj_exports += "conf/2002E6E8.txt /epoc32/winscw/c/private/10202be9/2002E6E8.txt"
    
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = All -Tcb
    TARGET.UID3 = 0x2002E6E8

    
   LIBS += -ledbms  \
           -lbafl \
           -lcentralrepository \
           -leuser
   

}  

deploy.path = /
headers.sources = $$INTERNAL_PUBLIC_HEADERS
headers.path = epoc32/include/app
DEPLOYMENT += exportheaders

# This is for new exporting system coming in garden
for(header, headers.sources):BLD_INF_RULES.prj_exports += "$$header $$deploy.path$$headers.path/$$basename(header)"