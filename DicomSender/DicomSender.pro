QT          += core gui widgets

CONFIG      += c++17

DEFINES     += QT_DISABLE_DEPRECATED_BEFORE=0x060000

include($$PWD/../ThirdParty/qt.pri)

SOURCES     += \
    main.cpp \
    mainwindow.cpp \
    setting.cpp

HEADERS     += \
    mainwindow.h \
    setting.h \
    version.h

FORMS       += \
    mainwindow.ui

RESOURCES   += \
    mainwindow.qrc

RC_FILE     += \
    DicomSender.rc

OTHER_FILES += \
    DicomSender.rc

INCLUDEPATH += $$PWD/../DicomBase
DEPENDPATH  += $$PWD/../DicomBase
LIBS        += -L$$DESTDIR -lDicomBase

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

