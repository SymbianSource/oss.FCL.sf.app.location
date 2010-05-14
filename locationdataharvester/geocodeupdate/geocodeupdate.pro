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
TARGET = geocodeupdate   

CONFIG += dll
CONFIG += Qt

DEPENDPATH += .
INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

# Input
# Input
HEADERS += ./inc/geocodeupdate.h \
           ../mylocationlogger/inc \
           ../inc   
SOURCES +=./src/geocodeupdate.cpp 

symbian: 
{ 
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = All -Tcb
    TARGET.UID3 = 0x2002C3A9
    deploy.path = $$EPOCROOT
    exportheaders.sources = $$PUBLIC_HEADERS
    exportheaders.path = epoc32/include
    DEPLOYMENT += exportheaders
    
    qtAddLibrary(QtContacts)
	LIBS += -lcntmodel \
	        -lagendainterface \
		    -leuser  \
		    -lestor \
		    -lflogger
   

}  
# End of file	--Don't remove this.