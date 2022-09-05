QT      += core gui svg widgets

CONFIG  += c++17

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

include($$PWD/../../base/base.pri)
include($$PWD/../../base/3rdparty/dcmtk.pri)
include($$PWD/../../base/3rdparty/gdcm.pri)
include($$PWD/../../base/3rdparty/zlib.pri)

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    setting.cpp

HEADERS += \
    mainwindow.h \
    setting.h \
    version.h

FORMS   += \
    mainwindow.ui

RESOURCES += \
    mainwindow.qrc

RC_FILE = DicomSender.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
