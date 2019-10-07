include($$mac_compiler)
DESTDIR = $$top_srcdir/bin/

MOC_DIR=$$top_builddir/tmp/maven_tests/
OBJECTS_DIR=$$top_builddir/tmp/maven_tests/
include($$mzroll_pri)
TEMPLATE = app
TARGET = MavenTests



QT += testlib network
QT -= gui

CONFIG += qtestlib warn_off

QMAKE_CXXFLAGS +=  -std=c++11
QMAKE_CXXFLAGS += -DOMP_PARALLEL
QMAKE_CXXFLAGS += -fopenmp

INCLUDEPATH +=  $$top_srcdir/src/core/libmaven  $$top_srcdir/3rdparty/pugixml/src $$top_srcdir/3rdparty/libneural $$top_srcdir/3rdparty/libpls \
				$$top_srcdir/3rdparty/libcsvparser $$top_srcdir/src/cli/peakdetector $$top_srcdir/3rdparty/libdate $$top_srcdir/3rdparty/libcdfread \
                $$top_srcdir/3rdparty/obiwarp $$top_srcdir/src/pollyCLI \
                $$top_srcdir/3rdparty/Eigen $$top_srcdir/src/
macx {

    DYLIBPATH = $$system(source ~/.bash_profile ; echo $LDFLAGS)
    isEmpty(DYLIBPATH) {
        warning("LDFLAGS variable is not set. Linking operation might complain about missing OMP library")
        warning("Please follow the README to make sure you have correctly set the LDFLAGS variable")
    }
    QMAKE_LFLAGS += $$DYLIBPATH
}
QMAKE_LFLAGS += -L$$top_builddir/libs/

LIBS += -lmaven -lpugixml -lneural -lcsvparser -lpls -lErrorHandling -lLogger -lcdfread -lz -lnetcdf -lobiwarp -lpollyCLI -lcommon
unix: LIBS += -lboost_system -lboost_filesystem
win32: LIBS += -lboost_system-mt -lboost_filesystem-mt
!macx: LIBS += -fopenmp

macx {
    LIBS += -lomp
    LIBS -= -lnetcdf -lcdfread
}


# Input
HEADERS += \
    testLoadSamples.h \
    testMassCalculator.h \
    testCSVReports.h \
    testMzSlice.h \
    testLoadDB.h \
    testPeakDetection.h \
    testIsotopeDetection.h \
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
    $$top_srcdir/src/cli/peakdetector/peakdetectorcli.h \
    $$top_srcdir/src/core/libmaven/classifier.h \
    $$top_srcdir/src/core/libmaven/classifierNeuralNet.h \
    $$top_srcdir/src/cli/peakdetector/parseoptions.h \
    $$top_srcdir/src/cli/peakdetector/options.h \
    utilities.h

SOURCES += \
    testLoadSamples.cpp \
    testMassCalculator.cpp \
    testCSVReports.cpp \
    testPeakDetection.cpp \
    testIsotopeDetection.cpp \
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
    $$top_srcdir/src/cli/peakdetector/peakdetectorcli.cpp  \
    $$top_srcdir/src/cli/peakdetector/options.cpp \
    $$top_srcdir/src/core/libmaven/classifier.cpp \
    $$top_srcdir/src/core/libmaven/classifierNeuralNet.cpp \
    $$top_srcdir/src/cli/peakdetector/parseoptions.cpp \
    utilities.cpp
