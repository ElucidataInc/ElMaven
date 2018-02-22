TEMPLATE = lib

DESTDIR=$$top_builddir/libs/
OBJECTS_DIR=$$top_buildir/tmp/error_handling
MOC_DIR=$$top_builddir/tmp/error_handling

CONFIG += staticlib

INCLUDEPATH += $$top_srcdir/3rdparty/libplog

SOURCES += MavenException.cpp
HEADERS += MavenException.h
