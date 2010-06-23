
TEMPLATE=app
TARGET=mylocationsengine
CONFIG += Qt mobility
MOBILITY = publishsubscribe
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += ../inc
INCLUDEPATH += ../../inc
INCLUDEPATH += ../geocodeupdate/inc
INCLUDEPATH += ../mylocationlogger/inc
INCLUDEPATH += ../mylocationsdatabasemanager/inc
INCLUDEPATH += /epoc32/include/app


symbian: { 

    TARGET.UID3 = 0x2002680A 
    isEmpty(TARGET.EPOCSTACKSIZE):TARGET.EPOCSTACKSIZE = 0x8000
    isEmpty(TARGET.EPOCHEAPSIZE):TARGET.EPOCHEAPSIZE = 20480 \
        16943040
    TARGET.CAPABILITY = ALL -TCB
    INCLUDEPATH += $$APP_LAYER_SYSTEMINCLUDE

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
	-llocationdatalookupdb \
	-lmylocationsdatabasemanager
	
	
	myCrml.sources = ./conf/maptilestatuspublisher.qcrml
        myCrml.path = c:/resource/qt/crml
        DEPLOYMENT += myCrml

}

SOURCES += src/appmain.cpp \
    src/mylocationsengine.cpp \
    src/maptileinterface.cpp \
    src/lookupmaptiledb.cpp    \
    src/calendernotification.cpp \
    src/mylocationgeotagtimerao.cpp \
    src/contactsubscriber.cpp \
    src/calendarsubscriber.cpp

HEADERS += inc/appmain.h \
    inc/mylocationsengine.h \
    inc/maptileinterface.h \
    inc/lookupmaptiledb.h \
    inc/calendernotification.h \
    inc/mylocationgeotagtimerao.h \
    inc/contactsubscriber.h \
    inc/calendarsubscriber.h




