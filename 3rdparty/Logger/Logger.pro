include($$mac_compiler)
TEMPLATE = lib

CONFIG += staticlib


MOC_DIR=$$top_builddir/tmp/logger/
OBJECTS_DIR=$$top_builddir/tmp/logger/

DESTDIR=$$top_builddir/libs/

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += $$top_srcdir/3rdparty/Logger/spdlog/


SOURCES += elmavenlogger.cpp

HEADERS += elmavenlogger.h
