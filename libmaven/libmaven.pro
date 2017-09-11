include(../mzroll.pri)
DESTDIR = $$OUTPUT_DIR/lib

QT+= sql network
TEMPLATE=lib

CONFIG += staticlib warn_off console silent

#Faster build + C++11 ++ OpenMP

#QMAKE_CFLAGS_RELEASE += -fopenmp
#QMAKE_CFLAGS_DEBUG += -fopenmp

QMAKE_CXXFLAGS += -Ofast -ffast-math -march=native -std=c++11
QMAKE_CXXFLAGS += -DOMP_PARALLEL

#QMAKE_CXXFLAGS += -Ofast -ffast-math -march=native -std=c++11
QMAKE_CXXFLAGS += -fopenmp
LIBS += -fopenmp

TARGET = maven

LIBS += -L. -lcsvparser

INCLUDEPATH += ../pugixml/src/ ../libcdfread/ ../zlib/ ../mzroll/ ../libneural/ ../libcsvparser

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
                jsonReports.cpp

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
                jsonReports.h

message($$LIBS)
