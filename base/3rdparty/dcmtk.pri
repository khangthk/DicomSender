win32 {
    !contains(QMAKE_TARGET.arch, x86_64) {
        ARCH = x86
    } else {
        ARCH = x64
    }
}

CONFIG(debug, debug|release): {
    LIB_PATH = $$PWD/../../build/$${ARCH}/dcmtk/lib/Debug
} else {
    LIB_PATH = $$PWD/../../build/$${ARCH}/dcmtk/lib/Release
}

HEADER_PATH = $$PWD/../../build/$${ARCH}/dcmtk/config/include \
              $$PWD/../../base/3rdparty/dcmtk/ofstd/include \
              $$PWD/../../base/3rdparty/dcmtk/oflog/include \
              $$PWD/../../base/3rdparty/dcmtk/oficonv/include \
              $$PWD/../../base/3rdparty/dcmtk/dcmwlm/include \
              $$PWD/../../base/3rdparty/dcmtk/dcmtract/include \
              $$PWD/../../base/3rdparty/dcmtk/dcmtls/include \
              $$PWD/../../base/3rdparty/dcmtk/dcmsr/include \
              $$PWD/../../base/3rdparty/dcmtk/dcmsign/include \
              $$PWD/../../base/3rdparty/dcmtk/dcmseg/include \
              $$PWD/../../base/3rdparty/dcmtk/dcmrt/include \
              $$PWD/../../base/3rdparty/dcmtk/dcmqrdb/include \
              $$PWD/../../base/3rdparty/dcmtk/dcmpstat/include \
              $$PWD/../../base/3rdparty/dcmtk/dcmpmap/include \
              $$PWD/../../base/3rdparty/dcmtk/dcmnet/include \
              $$PWD/../../base/3rdparty/dcmtk/dcmjpls/include \
              $$PWD/../../base/3rdparty/dcmtk/dcmjpeg/include \
              $$PWD/../../base/3rdparty/dcmtk/dcmiod/include \
              $$PWD/../../base/3rdparty/dcmtk/dcmimgle/include \
              $$PWD/../../base/3rdparty/dcmtk/dcmimage/include \
              $$PWD/../../base/3rdparty/dcmtk/dcmfg/include \
              $$PWD/../../base/3rdparty/dcmtk/dcmect/include \
              $$PWD/../../base/3rdparty/dcmtk/dcmdata/include \
              $$PWD/../../base/3rdparty/dcmtk/config/include

INCLUDEPATH += $${HEADER_PATH}
DEPENDPATH += $${HEADER_PATH}

win32 {LIBS += -ladvapi32 -lcomdlg32 -lgdi32 -liphlpapi -lkernel32 -lnetapi32 -lole32 -loleaut32 -lshell32 -luser32 -luuid -lwinspool -lws2_32 -lwsock32}

LIBS += -L$${LIB_PATH} -lcmr -ldcmdata -ldcmdsig -ldcmect -ldcmfg -ldcmimage -ldcmimgle -ldcmiod -ldcmjpeg -ldcmjpls -ldcmnet -ldcmpmap -ldcmpstat -ldcmqrdb -ldcmrt -ldcmseg -ldcmsr -ldcmtkcharls -ldcmtls -ldcmtract -ldcmwlm -ldcmxml -li2d -lijg12 -lijg16 -lijg8 -loficonv -loflog -lofstd

win32: LIB_EXT = lib
unix: {
    LIB_EXT = a
    LIB_PREFIX = lib
}

PRE_TARGETDEPS += $${LIB_PATH}/$${LIB_PREFIX}cmr.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmdata.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmdsig.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmect.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmfg.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmimage.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmimgle.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmiod.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmjpeg.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmjpls.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmnet.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmpmap.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmpstat.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmqrdb.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmrt.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmseg.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmsr.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmtkcharls.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmtls.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmtract.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmwlm.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}dcmxml.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}i2d.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}ijg12.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}ijg16.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}ijg8.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}oficonv.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}oflog.$${LIB_EXT} \
                  $${LIB_PATH}/$${LIB_PREFIX}ofstd.$${LIB_EXT}
