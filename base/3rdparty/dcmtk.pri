win32:CONFIG(debug, debug|release): {
    LIB_PATH = $$PWD/../../build/dcmtk/lib/Debug
} else {
    LIB_PATH = $$PWD/../../build/dcmtk/lib/Release
}

HEADER_PATH = $$PWD/../../build/dcmtk/config/include \
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

PRE_TARGETDEPS += $${LIB_PATH}/cmr.lib \
                  $${LIB_PATH}/dcmdata.lib \
                  $${LIB_PATH}/dcmdsig.lib \
                  $${LIB_PATH}/dcmect.lib \
                  $${LIB_PATH}/dcmfg.lib \
                  $${LIB_PATH}/dcmimage.lib \
                  $${LIB_PATH}/dcmimgle.lib \
                  $${LIB_PATH}/dcmiod.lib \
                  $${LIB_PATH}/dcmjpeg.lib \
                  $${LIB_PATH}/dcmjpls.lib \
                  $${LIB_PATH}/dcmnet.lib \
                  $${LIB_PATH}/dcmpmap.lib \
                  $${LIB_PATH}/dcmpstat.lib \
                  $${LIB_PATH}/dcmqrdb.lib \
                  $${LIB_PATH}/dcmrt.lib \
                  $${LIB_PATH}/dcmseg.lib \
                  $${LIB_PATH}/dcmsr.lib \
                  $${LIB_PATH}/dcmtkcharls.lib \
                  $${LIB_PATH}/dcmtls.lib \
                  $${LIB_PATH}/dcmtract.lib \
                  $${LIB_PATH}/dcmwlm.lib \
                  $${LIB_PATH}/dcmxml.lib \
                  $${LIB_PATH}/i2d.lib \
                  $${LIB_PATH}/ijg12.lib \
                  $${LIB_PATH}/ijg16.lib \
                  $${LIB_PATH}/ijg8.lib \
                  $${LIB_PATH}/oficonv.lib \
                  $${LIB_PATH}/oflog.lib \
                  $${LIB_PATH}/ofstd.lib
