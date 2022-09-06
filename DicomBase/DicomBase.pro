QT      -= gui

TEMPLATE = lib

DEFINES += DICOMBASE_LIBRARY

CONFIG  += c++17

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

include($$PWD/../ThirdParty/dcmtk.pri)
include($$PWD/../ThirdParty/gdcm.pri)
include($$PWD/../ThirdParty/zlib.pri)

SOURCES += \
    dicombase.cpp

HEADERS += \
    dicombase.h \
    global.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
