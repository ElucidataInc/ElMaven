include(../mzroll.pri)
DESTDIR = $$OUTPUT_DIR/lib

TEMPLATE = lib
CONFIG += staticlib warn_off console silent
TARGET = maven

win32 {
    DEFINES += MINGW
    DEFINES += CDFPARSER
}

unix {
	DEFINE -= LITTLE_ENDIAN
}

QMAKE_CXXFLAGS_RELEASE += -O3 -Wno-sign-compare

INCLUDEPATH += ../pugixml/src/ ../libcdfread/ ../libneural

HEADERS +=  base64.h \
			mzFit.h \
			mzMassSlicer.h \
			mzSample.h  \
			mzMassCalculator.h \
			mzPatterns.h \
			mzUtils.h  \
			statistics.h \
			SavGolSmoother.h \
			PeakDetector.h \
			database.h \
    		csvreports.h \
    		classifier.h

SOURCES +=  base64.cpp \
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
    		database.cpp \
    		csvreports.cpp \
    		classifier.cpp
