include($$mzroll_pri)
DESTDIR = $$top_srcdir/bin/

MOC_DIR=$$top_builddir/tmp/maven_tests/
OBJECTS_DIR=$$top_builddir/tmp/maven_tests/

TEMPLATE = app
TARGET = MavenTests



QT += testlib
QT -= gui

CONFIG += qtestlib warn_off

QMAKE_CXXFLAGS += -Ofast -ffast-math -march=native -std=c++11
QMAKE_CXXFLAGS += -DOMP_PARALLEL
QMAKE_CXXFLAGS += -fopenmp

INCLUDEPATH +=  $$top_srcdir/src/core/libmaven  $$top_srcdir/3rdparty/pugixml/src $$top_srcdir/3rdparty/libneural $$top_srcdir/3rdparty/libpls \
				$$top_srcdir/3rdparty/libcsvparser $$top_srcdir/src/cli/peakdetector


QMAKE_LFLAGS += -L$$top_builddir/libs/

LIBS += -lmaven -lpugixml -lneural -lcsvparser -lpls -fopenmp


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
    testbase64.h \
    testMzFit.h \
    testMzAligner.h \
    testCLI.h \
    testCharge.h \
    $$top_srcdir/src/cli/peakdetector/PeakDetectorCLI.h \
    $$top_srcdir/src/core/libmaven/classifier.h \
    $$top_srcdir/src/core/libmaven/classifierNeuralNet.h \
    $$top_srcdir/src/cli/peakdetector/parseOptions.h \
    $$top_srcdir/src/cli/peakdetector/options.h

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
    testCharge.cpp \
    main.cpp \
    $$top_srcdir/src/cli/peakdetector/PeakDetectorCLI.cpp  \
    $$top_srcdir/src/cli/peakdetector/options.cpp \
    $$top_srcdir/src/core/libmaven/classifier.cpp \
    $$top_srcdir/src/core/libmaven/classifierNeuralNet.cpp \
    $$top_srcdir/src/cli/peakdetector/parseOptions.cpp
