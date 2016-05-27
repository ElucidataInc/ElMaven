include(../mzroll.pri)
DESTDIR = $$OUTPUT_DIR/lib

TEMPLATE=lib
CONFIG += staticlib warn_on console silent
TARGET = mzorbi

LIBS += -L.


INCLUDEPATH += ../pugixml/src/ ../libcdfread/ ../zlib/

SOURCES=base64.cpp mzMassCalculator.cpp mzSample.cpp  mzUtils.cpp statistics.cpp mzFit.cpp mzAligner.cpp mzMassSlicer.cpp\
       PeakGroup.cpp EIC.cpp Scan.cpp Peak.cpp  \
       Compound.cpp \
       savgol.cpp \
       SavGolSmoother.cpp

HEADERS += base64.h mzFit.h mzMassSlicer.h mzSample.h  mzMassCalculator.h mzPatterns.h mzUtils.h  statistics.h SavGolSmoother.h
message($$LIBS)
