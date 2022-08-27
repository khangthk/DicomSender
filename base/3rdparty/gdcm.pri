win32 {
    !contains(QMAKE_TARGET.arch, x86_64) {
        ARCH = x86
    } else {
        ARCH = x64
    }
}

CONFIG(debug, debug|release): {
    LIB_PATH = $$PWD/../../build/$${ARCH}/gdcm/bin/Debug
} else {
    LIB_PATH = $$PWD/../../build/$${ARCH}/gdcm/bin/Release
}

HEADER_PATH = $$PWD/../../build/$${ARCH}/gdcm/Source/Common \
              $$PWD/../../build/$${ARCH}/gdcm/Source/InformationObjectDefinition \
              $$PWD/../../base/3rdparty/gdcm/Source/Common \
              $$PWD/../../base/3rdparty/gdcm/Source/DataDictionary \
              $$PWD/../../base/3rdparty/gdcm/Source/DataStructureAndEncodingDefinition \
              $$PWD/../../base/3rdparty/gdcm/Source/InformationObjectDefinition \
              $$PWD/../../base/3rdparty/gdcm/Source/MediaStorageAndFileFormat \
              $$PWD/../../base/3rdparty/gdcm/Source/MessageExchangeDefinition

INCLUDEPATH += $${HEADER_PATH}
DEPENDPATH += $${HEADER_PATH}

win32 {LIBS += -lcrypt32 -lrpcrt4 -lws2_32}

LIBS += -L$${LIB_PATH} -lgdcmCommon -lgdcmDICT -lgdcmDSED -lgdcmIOD -lgdcmMEXD -lgdcmMSFF -lgdcmcharls -lgdcmexpat -lgdcmjpeg12 -lgdcmjpeg16 -lgdcmjpeg8 -lgdcmopenjp2 -lsocketxx

win32: LIB_EXT = lib
unix: {
    LIB_EXT = a
    LIB_PREFIX = lib
}

PRE_TARGETDEPS += $${LIB_PATH}/$${LIB_PREFIX}gdcmCommon.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}gdcmDICT.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}gdcmDSED.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}gdcmIOD.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}gdcmMEXD.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}gdcmMSFF.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}gdcmcharls.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}gdcmexpat.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}gdcmjpeg12.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}gdcmjpeg16.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}gdcmjpeg8.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}gdcmopenjp2.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}socketxx.$${LIB_EXT}
