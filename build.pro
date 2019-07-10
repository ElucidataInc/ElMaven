macx {
    compiler_version = $$system(clang -v 2>&1 | head -n1 | grep -o  "[0-9\.]" | head -n1)
    message("compiler major version $$compiler_version")
    if(greaterThan(compiler_version, 5):lessThan(compiler_version, 7)) {
        message("Clang Version : $$system( clang -v 2>&1 | head -n1 )")
    }
    else {
        warning("Make sure you have installed clang using brew and are not using clang that comes along with XCode.")
        warning("If you have installed clang using brew, please make sure it's added correctly in the PATH variable")
        message("exiting now")
        error("Compiler not found")
    }
}

win32 {
    #package 'mingw-w64-x86_64-curl' is required by libnetcdf.
    CONFIG += link_pkgconfig
    PKGCONFIG = libcurl
}

include($$mac_compiler)

TEMPLATE = subdirs
CONFIG += ordered qt thread
SUBDIRS += 3rdparty crashhandler src

!equals(NOTESTS, "yes"): SUBDIRS+=tests/MavenTests
