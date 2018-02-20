TEMPLATE = lib

DESTDIR=$$top_builddir/libs/
OBJECTS_DIR=$$top_buildir/tmp/error_handling

CONFIG += staticlib

SOURCES += MavenException.cpp
HEADERS += MavenException.h
