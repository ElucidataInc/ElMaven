OUTPUT_DIR = $$(OUTPUT_DIR)
isEmpty(OUTPUT_DIR):OUTPUT_DIR=$$PWD/build


INSTALL_LIBDIR = $$(INSTALL_LIBDIR)
unix {
  !mac {
    isEmpty(INSTALL_LIBDIR):INSTALL_LIBDIR=lib
} }

INSTALL_PREFIX=$$(DESTDIR)$$INSTALL_PREFIX
DEFINES += INSTALL_LIBDIR=\\\"$$INSTALL_LIBDIR\\\"

QMAKE_CXXFLAGS_RELEASE += -O3 -Wall -Wno-sign-compare

CONFIG += no_keywords
unix: INCLUDEPATH += /usr/lib/x86_64-linux-gnu/
unix:!macx {
LIBS += -lboost_signals
}
win32: LIBS += -lboost_signals-mt

QT += core
CONFIG += silent exceptions
OBJECTS_DIR = tmp
MOC_DIR = tmp
UI_DIR   =  tmp
QMAKE_CC = gcc
QMAKE_CXX = g++


win32-g++:contains(QMAKE_HOST.arch, x86_64):{
    DEFINES -= CDFPARSER
    DEFINES -= ZLIB
    LIBS -= -lz -lcdfread -lnetcdf
}

win32 {
    message("using win32 config")
    DEFINES += MINGW
    DEFINES += WIN32
}

mac {
    message("using mac config")
    DEFINES += CDFPARSER
    DEFINES += ZLIB
    DEFINES += unix
    LIBS += -lz -lcdfread
    DEFINES += MAC
    INCLUDEPATH += /usr/local/include
    QMAKE_LFLAGS += -L/usr/local/lib/
    QMAKE_LFLAGS += -L/usr/local/opt/netcdf/lib
    LIBS += -lnetcdf
    LIBS +=  -lboost_signals
}


unix {
    QMAKE_CCFLAGS+= -fprofile-arcs -ftest-coverage
    QMAKE_CXXFLAGS+= -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS+= -fprofile-arcs -ftest-coverage
   message("using unix config")
   DEFINES -= LITTLE_ENDIAN
   DEFINES += UNIX
 #  DEFINES += CDFPARSER
 #  LIBS += -lcdfread -lnetcdf
 #  DEFINES += ZLIB
 #  LIBS += -lz -lcdfread -lnetcdf 
}


#TOPLEVELDIR = $$PWD
#INCLUDEPATH += $$TOPLEVELDIR

INCLUDEPATH += $$PWD
win32:INCLUDEPATH += "C:/msys64/mingw64/lib"

LIBS += -L$$OUTPUT_DIR/lib
