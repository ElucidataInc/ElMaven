include(../mzroll.pri)
DESTDIR = $$OUTPUT_DIR/lib

TEMPLATE=lib
CONFIG += staticlib warn_on console silent
TARGET = pls 
#QMAKE_CXXFLAGS_RELEASE += --std=c++0x
#QMAKE_CXXFLAGS_DEBUG   += --std=c++0x


LIBS += -L.

INCLUDEPATH += ../Eigen

SOURCES=plsutility.cpp 
HEADERS += plsutility.h pls.h
message($$LIBS)
