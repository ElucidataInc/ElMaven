DESTDIR = $$top_srcdir/bin/

MOC_DIR=$$top_builddir/tmp/maven_tests/
OBJECTS_DIR=$$top_builddir/tmp/maven_tests/
include($$mzroll_pri)
TEMPLATE = app
TARGET = MavenTests



QT += testlib
QT -= gui

CONFIG += qtestlib warn_off

QMAKE_CXXFLAGS +=  -std=c++11
QMAKE_CXXFLAGS += -DOMP_PARALLEL
!macx: QMAKE_CXXFLAGS += -fopenmp

INCLUDEPATH +=  $$top_srcdir/src/core/libmaven  $$top_srcdir/3rdparty/pugixml/src $$top_srcdir/3rdparty/libneural $$top_srcdir/3rdparty/libpls \
				$$top_srcdir/3rdparty/libcsvparser $$top_srcdir/src/cli/peakdetector $$top_srcdir/3rdparty/libdate $$top_srcdir/3rdparty/libcdfread \
                $$top_srcdir/3rdparty/obiwarp $$top_srcdir/src/pollyCLI $$top_srcdir/src/core/libmaven/alignment


QMAKE_LFLAGS += -L$$top_builddir/libs/

LIBS += -lmaven -lpugixml -lneural -lcsvparser -lpls -lErrorHandling -lLogger -lcdfread -lz -lnetcdf -lobiwarp -lpollyCLI
!macx: LIBS += -fopenmp

macx {
LIBS -= -lnetcdf -lcdfread
}


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
    testSRMList.h \
    testGroupFiltering.h \
    testIsotopeLogic.h \
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
    testSRMList.cpp \
    testGroupFiltering.cpp \
    testIsotopeLogic.cpp \
    main.cpp \
    $$top_srcdir/src/cli/peakdetector/PeakDetectorCLI.cpp  \
    $$top_srcdir/src/cli/peakdetector/options.cpp \
    $$top_srcdir/src/core/libmaven/classifier.cpp \
    $$top_srcdir/src/core/libmaven/classifierNeuralNet.cpp \
    $$top_srcdir/src/cli/peakdetector/parseOptions.cpp
