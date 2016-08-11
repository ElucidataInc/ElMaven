include(../mzroll.pri)

TEMPLATE = app
TARGET = MavenTests

DESTDIR = ../bin

QT += testlib core
CONFIG += qtestlib

INCLUDEPATH += ../pugixml/src/ ../sqlite ../libmaven ../libneural ../zlib/ ../libcsvparser

LIBS += -L.  -lmaven -lpugixml -lneural -lcsvparser

QMAKE_CXXFLAGS_RELEASE -= -O3 -Wall -Wno-sign-compare
QMAKE_CXXFLAGS_DEBUG   -= -O2 -g -Wall -Wno-sign-compare

QMAKE_CXXFLAGS_DEBUG -= -pg
QMAKE_LFLAGS_DEBUG -= -pg


# Input
HEADERS += \
    common.h \
    testEIC.h \
    testMassCalculator.h \
    testCSVReports.h \
    testMzSlice.h \
    testPeakDetection.h


SOURCES += \
    common.cpp \
    testEIC.cpp \
    testMassCalculator.cpp \
    testCSVReports.cpp \
    testPeakDetection.cpp \
    testMzSlice.cpp \
    main.cpp

    
