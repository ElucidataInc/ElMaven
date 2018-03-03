TEMPLATE = lib

DESTDIR=$$top_builddir/libs/
OBJECTS_DIR=$$top_buildir/tmp/error_handling
MOC_DIR=$$top_builddir/tmp/error_handling

QMAKE_CXXFLAGS += -std=c++11
CONFIG += staticlib

INCLUDEPATH += $$top_srcdir/3rdparty/libplog $$top_srcdir/3rdparty/Logger

LIBS += -lLogger

SOURCES += MavenException.cpp errorcodes.cpp
HEADERS += MavenException.h errorcodes.h
