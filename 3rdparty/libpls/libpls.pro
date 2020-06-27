include($$mac_compiler)
DESTDIR=$$top_builddir/libs/

MOC_DIR=$$top_builddir/tmp/pls/
OBJECTS_DIR=$$top_builddir/tmp/pls/
include($$mzroll_pri)
TEMPLATE=lib
CONFIG += staticlib warn_off silent
TARGET = pls
QMAKE_CXXFLAGS += --std=c++11

LIBS += -L.

INCLUDEPATH += $$top_srcdir/3rdparty/Eigen

SOURCES += utility.cpp
HEADERS += pls.h utility.h
