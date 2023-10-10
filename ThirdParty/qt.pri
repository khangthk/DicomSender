CONFIG  += no_batch

win32 {
    !contains(QMAKE_TARGET.arch, x86_64) {
        ARCH = x86
    } else {
        ARCH = x64
    }
}

CONFIG(debug, debug|release) {
    DESTDIR    = $$PWD/../bin/$${ARCH}/Debug
    BUILD_MODE = Debug
} else {
    DESTDIR    = $$PWD/../bin/$${ARCH}/Release
    BUILD_MODE = Release
    DEFINES   += QT_NO_DEBUG_OUTPUT
}

win32 {
    mkdestdir.commands = $(CHK_DIR_EXISTS) $$shell_path($$DESTDIR) $(MKDIR) $$shell_path($$DESTDIR)
}
else {
    mkdestdir.commands = $(CHK_DIR_EXISTS) $$shell_path($$DESTDIR) & $(MKDIR) $$shell_path($$DESTDIR)
}

QMAKE_EXTRA_TARGETS   += mkdestdir
PRE_TARGETDEPS        += mkdestdir
