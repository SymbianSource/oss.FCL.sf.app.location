#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: Location picker service project file

SERVICEAPP = app
TARGET = locationpickerservice
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../inc
MOC_DIR = moc
CONFIG += hb


symbian: { 
    # Build.inf rules
    BLD_INF_RULES.prj_exports += \
         "$${LITERAL_HASH}include <platform_paths.hrh>" \
         "rom/locationpickerservice.iby  CORE_APP_LAYER_IBY_EXPORT_PATH(locationpickerservice.iby)"

    TARGET.UID3 = 0x2002C3AA 
    isEmpty(TARGET.EPOCSTACKSIZE):TARGET.EPOCSTACKSIZE = 0x14000
    isEmpty(TARGET.EPOCHEAPSIZE):TARGET.EPOCHEAPSIZE = 20480 \
        16943040
    TARGET.CAPABILITY = ALL \
        -TCB
    
    LIBS += -llbs \
        -leposlandmarks \
        -leposlmsearchlib
}

# ##### qthighway
CONFIG += service
LIBS += -lxqservice \
    -lxqserviceutil \
    -lflogger
SERVICE.FILE = service_conf.xml
SERVICE.OPTIONS = embeddable
SERVICE.OPTIONS += hidden

# #### /qthighway

RESOURCES += .\resources\locationpicker.qrc 


SOURCES += src/main.cpp \
    src/locationpickerallview.cpp \
    src/locationpickerappwindow.cpp \
    src/locationpickertoolbar.cpp \
    src/locationpickersearchview.cpp \
    src/locationpickerproxymodel.cpp \
    src/locationpickercollectionlistview.cpp \
    src/locationpickercollectioncontent.cpp \
    src/locationpickerservice.cpp \
    src/locationpickerdatamanager.cpp \
    src/locationpickerdatamanager_p.cpp 
    
    

HEADERS += inc/locationpickerallview.h \
    inc/locationpickerappwindow.h \
    inc/locationpickertoolbar.h \
    inc/locationpickersearchview.h \
    inc/locationpickerproxymodel.h \
    inc/locationpickercollectionlistview.h \
    inc/locationpickercollectioncontent.h \
    inc/locationpickertypes.h \
    inc/locationpickerservice.h \
    inc/locationpickerdatamanager.h \
    inc/locationpickerdatamanager_p.h

