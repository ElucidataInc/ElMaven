macx {
    compiler_version = $$system(clang -v 2>&1 | head -n1 | grep -o  "[0-9\.]" | head -n1)
    message("compiler major version $$compiler_version")
}

win32 {
    #package 'mingw-w64-x86_64-curl' is required by libnetcdf.
    CONFIG += link_pkgconfig
    PKGCONFIG = libcurl
}

include($$mac_compiler)

TEMPLATE = subdirs
CONFIG += ordered qt thread
SUBDIRS += 3rdparty
win32 {
    CONFIG(release, debug|release) {
        SUBDIRS += crashhandler
    }
}

SUBDIRS += src

!equals(NOTESTS, "yes") {
    ENABLE_DOCTEST = "yes"
    SUBDIRS += src/core/libmaven
    SUBDIRS += tests/MavenTests
    SUBDIRS += tests/test-libmaven
}
