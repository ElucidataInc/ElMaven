include($$mac_compiler)
DESTDIR=$$top_builddir/libs/

MOC_DIR=$$top_builddir/tmp/customPlot/
OBJECTS_DIR=$$top_builddir/tmp/customPlot/
include($$mzroll_pri)
TEMPLATE=lib
CONFIG += staticlib warn_off silent
TARGET = customPlot
QMAKE_CXXFLAGS += --std=c++11

LIBS += -L.


SOURCES += qCustomplot.cpp
HEADERS += qCustomplot.h
