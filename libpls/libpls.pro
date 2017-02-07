include(../mzroll.pri)
DESTDIR = $$OUTPUT_DIR/lib

TEMPLATE=lib
CONFIG += staticlib warn_off console silent
TARGET = pls 
QMAKE_CXXFLAGS += --std=c++11
#QMAKE_CXXFLAGS_RELEASE += --std=c++0x
#QMAKE_CXXFLAGS_DEBUG   += --std=c++0x


LIBS += -L.

INCLUDEPATH += ../Eigen

SOURCES += pls.cpp utility.cpp
HEADERS += pls.h utility.h
message($$LIBS)
