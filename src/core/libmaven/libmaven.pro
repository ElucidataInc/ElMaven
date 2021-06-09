include($$mac_compiler)
MOC_DIR=$$top_builddir/tmp/maven/
OBJECTS_DIR=$$top_builddir/tmp/maven/
include($$mzroll_pri)
DESTDIR=$$top_builddir/libs/

QT+= core
TEMPLATE=lib

CONFIG += staticlib warn_off console silent

DEFINES += DOCTEST_CONFIG_DISABLE
equals(ENABLE_DOCTEST, "yes") {
    message("Doctest enabled.")
    DEFINES -= DOCTEST_CONFIG_DISABLE
}

QMAKE_CXXFLAGS +=  -std=c++11
QMAKE_CXXFLAGS += -DOMP_PARALLEL
linux: QMAKE_CXXFLAGS += -Ofast -ffast-math
win32: QMAKE_CXXFLAGS += -Ofast -ffast-math
macx: QMAKE_CXXFLAGS += -O3
QMAKE_CXXFLAGS += -fopenmp

CONFIG(debug, debug|release) {
    linux|win32 {
        QMAKE_CXXFLAGS -= -Ofast -ffast-math
    }
    macx: QMAKE_CXXFLAGS -= -O3
}

!isEmpty(ON_TRAVIS)|!isEmpty(ON_APPVEYOR) {
    !isEmpty(IS_TRAVIS_PR)|!isEmpty(IS_APPVEYOR_PR) {
        CONFIG(debug, debug|release) {
            linux|win32 {
                message("adding gcov compiler flags")
                QMAKE_CCFLAGS += -fprofile-arcs -ftest-coverage
                QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
                QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
                QMAKE_LFLAGS += -lgcov --coverage
            }
        }
    }
}

TARGET = maven

LIBS += -L. -lcsvparser -ldate -lErrorHandling -lmgf

INCLUDEPATH +=  $$top_srcdir/3rdparty/pugixml/src/ \
                $$top_srcdir/3rdparty/libcdfread/  \
                $$top_srcdir/src/gui/mzroll/ \
                $$top_srcdir/3rdparty/libneural/ \
                $$top_srcdir/3rdparty/libcsvparser \
                $$top_srcdir/3rdparty/libdate/ \
                $$top_srcdir/3rdparty/ErrorHandling \
                $$top_srcdir/3rdparty/obiwarp/ \
                $$top_srcdir/3rdparty/Eigen/ \
                $$top_srcdir/3rdparty/NimbleDSP/src \
                $$top_srcdir/3rdparty/doctest       \
                $$top_srcdir/3rdparty/json      \
                $$top_srcdir/tests/test-libmaven    \
                $$top_srcdir/3rdparty/libmgf


QMAKE_LFLAGS += -L$$top_builddir/libs

LIBS += -lobiwarp

macx{

    INCLUDEPATH += /usr/local/include/
    QMAKE_LFLAGS += -L/usr/local/lib/
    LIBS +=  -lboost_signals
}

message($$INCLUDEPATH)
SOURCES = base64.cpp \
          massslicer.cpp \
          mzMassCalculator.cpp \
          mzPatterns.cpp \
          mzSample.cpp \
          mzUtils.cpp \
          peakdetector.cpp \
          statistics.cpp \
          elementMass.cpp \
          mzFit.cpp \
          mzAligner.cpp \
	      PeakGroup.cpp \
          Fragment.cpp \
	      EIC.cpp \
	      Scan.cpp \
          SRMList.cpp \
	      Peak.cpp  \
	      Compound.cpp \
	      savgol.cpp \
       	  SavGolSmoother.cpp \
          mavenparameters.cpp \
          classifier.cpp \
          classifierNaiveBayes.cpp \
          classifierNeuralNet.cpp \
          csvreports.cpp \
          comparesampleslogic.cpp \
          isotopelogic.cpp \
          eiclogic.cpp \
          database.cpp \
          PolyAligner.cpp \
          jsonReports.cpp \
          masscutofftype.cpp \
          peakFiltering.cpp \
          groupFiltering.cpp \
          datastructures/adduct.cpp \
          datastructures/mzSlice.cpp \
          zlib.cpp \
          spectrallibexport.cpp \
          datastructures/isotope.cpp

HEADERS += constants.h \
           base64.h \
           massslicer.h \
           mzFit.h \
           Peak.h \
           mzAligner.h \
	       PeakGroup.h \
           mzSample.h \
           Fragment.h \
           elementMass.h \
           mzMassCalculator.h \
           mzPatterns.h \
           mzUtils.h \
           peakdetector.h \
           standardincludes.h \
           statistics.h \
           SavGolSmoother.h \
           mavenparameters.h\
           classifier.h \
           classifierNaiveBayes.h \
           classifierNeuralNet.h \
           csvreports.h \
           comparesampleslogic.h \
           isotopelogic.h \
           eiclogic.h \
           EIC.h \
	       Scan.h \
           SRMList.h \
           database.h \
           PolyAligner.h \
           jsonReports.h \
           masscutofftype.h \
           peakFiltering.h \
           groupFiltering.h \
           datastructures/adduct.h \
           datastructures/mzSlice.h \
           settings.h \
           groupClassifier.h \
           groupFeatures.h \
           svmPredictor.h \
           spectrallibexport.h \
           datastructures/isotope.h
