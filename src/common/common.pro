include($$mac_compiler)
MOC_DIR=$$top_builddir/tmp/common/
OBJECTS_DIR=$$top_builddir/tmp/common/

DESTDIR=$$top_builddir/libs/

QT += core network

TEMPLATE = lib

TARGET = common

CONFIG += staticlib

INCLUDEPATH += $$top_srcdir/src

SOURCES += downloadmanager.cpp

HEADERS += downloadmanager.h
