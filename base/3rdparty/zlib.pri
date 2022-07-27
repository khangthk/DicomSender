LIB_PATH = $$PWD/../../build/zlib/bin/lib
HEADER_PATH = $$PWD/../../build/zlib/bin/include

INCLUDEPATH += $${HEADER_PATH}
DEPENDPATH += $${HEADER_PATH}

win32 {LIBS += -ladvapi32 -lcomdlg32 -lgdi32 -lkernel32 -lole32 -loleaut32 -lshell32 -luser32 -luuid -lwinspool}

win32:CONFIG(debug, debug|release): {
    LIBS += -L$${LIB_PATH} -lzlib_d
    PRE_TARGETDEPS += $${LIB_PATH}/zlib_d.lib
} else {
    LIBS += -L$${LIB_PATH} -lzlib_o
    PRE_TARGETDEPS += $${LIB_PATH}/zlib_o.lib
}
