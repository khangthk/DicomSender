QT      += core gui widgets

CONFIG  += c++17

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

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

INCLUDEPATH += $$PWD/../DicomBase
DEPENDPATH  += $$PWD/../DicomBase

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../DicomBase/release/ -lDicomBase
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../DicomBase/debug/ -lDicomBase
else:unix: LIBS += -L$$OUT_PWD/../DicomBase/ -lDicomBase

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

