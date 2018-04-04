
MOC_DIR=$$top_builddir/tmp/pollyCLI/
OBJECTS_DIR=$$top_builddir/tmp/pollyCLI/

DESTDIR=$$top_builddir/libs/

TEMPLATE = lib
TARGET = pollyCLI

CONFIG += warn_off xml console staticlib

QMAKE_CXXFLAGS += -std=c++11

!macx: LIBS += -fopenmp

SOURCES	= 	pollyintegration.cpp
			

HEADERS += 	pollyintegration.h