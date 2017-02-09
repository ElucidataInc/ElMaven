include(../mzroll.pri)

TEMPLATE = app
TARGET = MavenTests

DESTDIR = ../bin

QT += testlib core
QT -= gui
CONFIG += qtestlib warn_off

INCLUDEPATH += ../pugixml/src/ ../sqlite ../libmaven ../libneural ../zlib/ ../libcsvparser ../libpls
QMAKE_CXXFLAGS += -fopenmp
LIBS += -L.  -lmaven -lpugixml -lneural -lcsvparser -lpls -fopenmp

QMAKE_CXXFLAGS_RELEASE -= -O3 -Wall -Wno-sign-compare -std=c++11



# Input
HEADERS += \
    common.h \
    testLoadSamples.h \
    testMassCalculator.h \
    testCSVReports.h \
    testMzSlice.h \
    testLoadDB.h \
    testPeakDetection.h \
    testScan.h \
    testEIC.h \
    testPeakDetection.h \
    testbase64.h \
    testMzFit.h \
    testMzAligner.h


SOURCES += \
    common.cpp \
    testLoadSamples.cpp \
    testMassCalculator.cpp \
    testCSVReports.cpp \
    testPeakDetection.cpp \
    testMzSlice.cpp \
    testLoadDB.cpp \
    testScan.cpp \
    testEIC.cpp \
    testbase64.cpp \
    testMzFit.cpp \
    testMzAligner.cpp \
    main.cpp


    
