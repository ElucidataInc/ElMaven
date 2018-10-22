MOC_DIR=$$top_builddir/tmp/maven/
OBJECTS_DIR=$$top_builddir/tmp/maven/
include($$mzroll_pri)
DESTDIR=$$top_builddir/libs/

QT+= sql network
TEMPLATE=lib

CONFIG += staticlib warn_off console silent


QMAKE_CXXFLAGS +=  -std=c++11
QMAKE_CXXFLAGS += -DOMP_PARALLEL
linux: QMAKE_CXXFLAGS += -Ofast -ffast-math
win32: QMAKE_CXXFLAGS += -Ofast -ffast-math
!macx: QMAKE_CXXFLAGS += -fopenmp
!macx: LIBS += -fopenmp

!isEmpty(ON_TRAVIS)|!isEmpty(ON_APPVEYOR) {
    CONFIG(debug, debug|release) {
        linux|win32 {
            message("adding gcov compiler flags")
            QMAKE_CCFLAGS += -fprofile-arcs -ftest-coverage
            QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
            QMAKE_CXXFLAGS -= -Ofast -ffast-math
            QMAKE_LFLAGS += -fprofile-arcs -ftest-coverage
            QMAKE_LFLAGS += -lgcov --coverage
        }
    }
}

TARGET = maven

LIBS += -L. -lcsvparser -ldate -lErrorHandling

INCLUDEPATH +=  $$top_srcdir/3rdparty/pugixml/src/ \
                $$top_srcdir/3rdparty/libcdfread/  \
                $$top_srcdir/src/gui/mzroll/ \
                $$top_srcdir/3rdparty/libneural/ \
                $$top_srcdir/3rdparty/libcsvparser \
                $$top_srcdir/3rdparty/libdate/ \
                $$top_srcdir/3rdparty/ErrorHandling \
                $$top_srcdir/3rdparty/obiwarp

QMAKE_LFLAGS += -L$$top_builddir/libs
LIBS += -lobiwarp

macx{

    INCLUDEPATH += /usr/local/include/
    QMAKE_LFLAGS += -L/usr/local/lib/
    LIBS +=  -lboost_signals
}

message($$INCLUDEPATH)
SOURCES = 	base64.cpp \
                mzMassCalculator.cpp \
                mzPatterns.cpp \
                mzSample.cpp \
                mzUtils.cpp \
                statistics.cpp \
                elementMass.cpp \
                mzFit.cpp \
                mzAligner.cpp \
                mzMassSlicer.cpp \
	        PeakGroup.cpp \
            Fragment.cpp \
	        EIC.cpp \
	        Scan.cpp \
                SRMList.cpp \
	        Peak.cpp  \
	        Compound.cpp \
	        savgol.cpp \
       	        SavGolSmoother.cpp \
                PeakDetector.cpp \
                mavenparameters.cpp \
                classifier.cpp \
                classifierNaiveBayes.cpp \
                classifierNeuralNet.cpp \
                csvreports.cpp \
                comparesampleslogic.cpp \
                isotopelogic.cpp \
                eiclogic.cpp \
                databases.cpp \
                Peptide.cpp \
                PolyAligner.cpp \
                jsonReports.cpp \
                masscutofftype.cpp \
                peakFiltering.cpp \
                groupFiltering.cpp \
                isotopeDetection.cpp \
                datastructures/mzSlice.cpp

HEADERS += 	constants.h \
		base64.h \
                mzFit.h \
                Peak.h \
                mzAligner.h \
                mzMassSlicer.h \
	            PeakGroup.h \
                mzSample.h \
                PeptideRecord.h \
                Fragment.h \
                elementMass.h \
                mzMassCalculator.h \
                mzPatterns.h \
                mzUtils.h \
                statistics.h \
                SavGolSmoother.h \
                PeakDetector.h \
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
                databases.h \
                Peptide.hpp \
                PeptideRecord.h \
                PolyAligner.h \
                jsonReports.h \
                masscutofftype.h \
                peakFiltering.h \
                groupFiltering.h \
                isotopeDetection.h \
                datastructures/mzSlice.h \
                settings.h
