QT      -= gui

TEMPLATE = lib

DEFINES += DICOMBASE_LIBRARY

CONFIG  += c++17

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

include($$PWD/../ThirdParty/dcmtk.pri)
include($$PWD/../ThirdParty/gdcm.pri)
include($$PWD/../ThirdParty/zlib.pri)
include($$PWD/../ThirdParty/qt.pri)

SOURCES += \
    dcmtkhelper.cpp \
    echodcmtk.cpp \
    echogdcm.cpp \
    echothread.cpp \
    gdcmhelper.cpp \
    scandcmtk.cpp \
    scangdcm.cpp \
    scanthread.cpp \
    scuinfo.cpp \
    storedcmtk.cpp \
    storegdcm.cpp \
    storethread.cpp

HEADERS += \
    dcmtkhelper.h \
    dicombase.h \
    echobase.h \
    echodcmtk.h \
    echogdcm.h \
    echothread.h \
    gdcmhelper.h \
    global.h \
    scanbase.h \
    scandcmtk.h \
    scangdcm.h \
    scanthread.h \
    scuinfo.h \
    storebase.h \
    storedcmtk.h \
    storegdcm.h \
    storethread.h \
    version.h

RC_FILE = DicomBase.rc

OTHER_FILES += DicomBase.rc

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
