include(../mzroll.pri)

TEMPLATE = app
TARGET = MavenTests

DESTDIR = ../bin

QT += testlib core
QT -= gui
CONFIG += qtestlib warn_off

QMAKE_CXXFLAGS += -Ofast -ffast-math -march=native -std=c++11
QMAKE_CXXFLAGS += -DOMP_PARALLEL
QMAKE_CXXFLAGS += -fopenmp

INCLUDEPATH += ../pugixml/src/ ../sqlite ../libmaven ../libneural ../zlib/ ../libcsvparser ../libpls ../peakdetector
LIBS += -L.  -lmaven -lpugixml -lneural -lcsvparser -lpls -fopenmp



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
    testMzAligner.h \
    testCLI.h \
    ../peakdetector/PeakDetectorCLI.h \
    ../libmaven/classifier.h \
    ../libmaven/classifierNeuralNet.h \
    ../peakdetector/parseOptions.h \
    ../peakdetector/options.h

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
    testCLI.cpp \
    main.cpp \
    ../peakdetector/PeakDetectorCLI.cpp  \
    ../peakdetector/options.cpp \
    ../libmaven/classifier.cpp \  
    ../libmaven/classifierNeuralNet.cpp \
    ../peakdetector/parseOptions.cpp     


    
