CONFIG(debug, debug|release){
    message("running in debug mode  ")
    unix:!macx {
        QMAKE_CCFLAGS+= -fprofile-arcs -ftest-coverage
        QMAKE_CXXFLAGS+= -fprofile-arcs -ftest-coverage
        QMAKE_LFLAGS+= -fprofile-arcs -ftest-coverage
    }
}

QT += sql core  gui opengl webkitwidgets

CONFIG += silent exceptions

# this is important. Used in mzUtils to make use of correct mkdir function
win32 {
    message("using win32 config")
    DEFINES += MINGW
    DEFINES += WIN32
}

unix: {
    INCLUDEPATH += /usr/local/include/
    QMAKE_LFLAGS += -L/usr/local/lib/
    LIBS +=  -lboost_signals
}

#INSTALL_LIBDIR = $$(INSTALL_LIBDIR)
#unix {
#  !mac {
#    isEmpty(INSTALL_LIBDIR):INSTALL_LIBDIR=lib
#} }


#INSTALL_PREFIX=$$(DESTDIR)$$INSTALL_PREFIX
#DEFINES += INSTALL_LIBDIR=\\\"$$INSTALL_LIBDIR\\\"



#QMAKE_CXXFLAGS_RELEASE += -O3 -Wall -Wno-sign-compare
#CONFIG += no_keywords
#unix: INCLUDEPATH += /usr/lib/x86_64-linux-gnu/
#unix: LIBS += -lboost_signals
#win32: LIBS += -lboost_signals-mt
#
#QT += core

#
#QMAKE_CC = gcc
#QMAKE_CXX = g++
#
#win32-g++:contains(QMAKE_HOST.arch, x86_64):{
#    DEFINES -= CDFPARSER
#    DEFINES -= ZLIB
#    LIBS -= -lz -lcdfread -lnetcdf
#}
#

#mac {
#    message("using mac config")
#    DEFINES += CDFPARSER
#    DEFINES += ZLIB
#    DEFINES += unix
#    LIBS += -lz -lcdfread -lnetcdf
#    DEFINES += MAC
#}
#
#unix {
#   message("using unix config")
#   DEFINES -= LITTLE_ENDIAN
#   DEFINES += UNIX
# #  DEFINES += CDFPARSER
# #  LIBS += -lcdfread -lnetcdf
# #  DEFINES += ZLIB
# #  LIBS += -lz -lcdfread -lnetcdf
#}
#
#
##TOPLEVELDIR = $$PWD
##INCLUDEPATH += $$TOPLEVELDIR
#
#INCLUDEPATH += $$PWD
#win32:INCLUDEPATH += "C:/msys64/mingw64/lib"
#
#LIBS += -L$$OUTPUT_DIR/lib -L$$OUTPUT_DIR/plugin
