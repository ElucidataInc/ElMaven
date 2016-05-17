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
#QMAKE_CXXFLAGS = -Wno-sign-compare -Wunused-parameter -Wmissing-declarations  -Wno-conversion -Wcast-align -Wparentheses -Wsequence-point -Wundef -Wpointer-arith -Wredundant-decls -Wdisabled-optimization -Wunused-value -pedantic -Wfatal-errors

INCLUDEPATH += ../pugixml/src/ ../libcdfread/

SOURCES=base64.cpp mzMassCalculator.cpp mzSample.cpp  mzUtils.cpp statistics.cpp mzFit.cpp mzAligner.cpp mzMassSlicer.cpp\
       PeakGroup.cpp EIC.cpp Scan.cpp Peak.cpp  \
       Compound.cpp \
       savgol.cpp \
       SavGolSmoother.cpp

HEADERS += base64.h mzFit.h mzMassSlicer.h mzSample.h  mzMassCalculator.h mzPatterns.h mzUtils.h  statistics.h SavGolSmoother.h
