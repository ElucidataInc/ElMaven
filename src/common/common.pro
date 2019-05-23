include($$mac_compiler)
MOC_DIR=$$top_builddir/tmp/common/
OBJECTS_DIR=$$top_builddir/tmp/common/

DESTDIR=$$top_builddir/libs/

QT += core network

TEMPLATE = lib

TARGET = common

CONFIG += staticlib

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += $$top_srcdir/src

SOURCES += downloadmanager.cpp

HEADERS += downloadmanager.h
