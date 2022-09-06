win32 {
    !contains(QMAKE_TARGET.arch, x86_64) {
        ARCH = x86
    } else {
        ARCH = x64
    }
}

LIB_PATH = $$PWD/../build/$${ARCH}/zlib/bin/lib
HEADER_PATH = $$PWD/../build/$${ARCH}/zlib/bin/include

INCLUDEPATH += $${HEADER_PATH}
DEPENDPATH += $${HEADER_PATH}

win32 {LIBS += -ladvapi32 -lcomdlg32 -lgdi32 -lkernel32 -lole32 -loleaut32 -lshell32 -luser32 -luuid -lwinspool}

win32: LIB_EXT = lib
unix: {
    LIB_EXT = a
    LIB_PREFIX = lib
}

CONFIG(debug, debug|release): {
    win32: {
        LIBS += -L$${LIB_PATH} -lzlib_d
        PRE_TARGETDEPS += $${LIB_PATH}/zlib_d.$${LIB_EXT}
    }
    unix: {
        LIBS += -L$${LIB_PATH} -lz_d
        PRE_TARGETDEPS += $${LIB_PATH}/$${LIB_PREFIX}z_d.$${LIB_EXT}
    }
} else {
    win32: {
        LIBS += -L$${LIB_PATH} -lzlib_o
        PRE_TARGETDEPS += $${LIB_PATH}/zlib_o.$${LIB_EXT}
    }
    unix: {
        LIBS += -L$${LIB_PATH} -lz_o
        PRE_TARGETDEPS += $${LIB_PATH}/$${LIB_PREFIX}z_o.$${LIB_EXT}
    }
}
