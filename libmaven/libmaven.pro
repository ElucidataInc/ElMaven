include(../mzroll.pri)
DESTDIR = $$OUTPUT_DIR/lib

QT+= sql network
TEMPLATE=lib

CONFIG += staticlib warn_on console silent

#Faster build + C++11 ++ OpenMP
QMAKE_CXXFLAGS += -Ofast -ffast-math -march=native -std=c++11
QMAKE_CXXFLAGS += -g -DOMP_PARALLEL -fopenmp
LIBS += -fopenmp

TARGET = maven

LIBS += -L.

INCLUDEPATH += ../pugixml/src/ ../libcdfread/ ../zlib/ ../mzroll/ ../libneural/

SOURCES = 		base64.cpp \
                mzMassCalculator.cpp \
                mzSample.cpp \
                mzUtils.cpp \
                statistics.cpp \
                mzFit.cpp \
                mzAligner.cpp \
                mzMassSlicer.cpp \
	       		PeakGroup.cpp \
	       		EIC.cpp \
	       		Scan.cpp \
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
    eiclogic.cpp

HEADERS += 		base64.h \
                mzFit.h \
                mzMassSlicer.h \
                mzSample.h \
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
    eiclogic.h

message($$LIBS)
