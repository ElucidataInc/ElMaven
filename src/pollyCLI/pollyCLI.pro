include($$mac_compiler)
MOC_DIR=$$top_builddir/tmp/pollyCLI/
OBJECTS_DIR=$$top_builddir/tmp/pollyCLI/

DESTDIR=$$top_builddir/libs/

QT += core network

TEMPLATE = lib
TARGET = pollyCLI

CONFIG += warn_off xml console staticlib

QMAKE_CXXFLAGS += -std=c++11

!macx: LIBS += -fopenmp

INCLUDEPATH += $$top_srcdir/src/

SOURCES	=   pollyintegration.cpp \
            $$top_srcdir/src/gui/mzroll/downloadmanager.cpp
			

HEADERS += 	pollyintegration.h \
            $$top_srcdir/src/gui/mzroll/downloadmanager.h
