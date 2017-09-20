include($$mzroll_pri)
DESTDIR = $$top_builddir/bin/

MOC_DIR=$$top_builddir/tmp/peakdetector/
OBJECTS_DIR=$$top_builddir/tmp/peakdetector/

TEMPLATE = app
TARGET = peakdetector

CONFIG += warn_off xml -std=c++14

LIBS += -fopenmp

INCLUDEPATH +=  $$top_srcdir/src/core/libmaven  $$top_srcdir/3rdparty/pugixml/src $$top_srcdir/3rdparty/libneural $$top_srcdir/3rdparty/libpls \
				$$top_srcdir/3rdparty/libcsvparser

QMAKE_LFLAGS  +=  -L$$top_builddir/libs/

LIBS +=  -lmaven -lpugixml -lneural -lcsvparser -lpls

SOURCES	= 	PeakDetectorCLI.cpp  \
		 	options.cpp \
			$$top_srcdir/src/core/libmaven/classifier.cpp \
			$$top_srcdir/src/core/libmaven/classifierNeuralNet.cpp \
			parseOptions.cpp \
			main.cpp

HEADERS += 	PeakDetectorCLI.h \
			$$top_srcdir/src/core/libmaven/classifier.h \
			$$top_srcdir/src/core/libmaven/classifierNeuralNet.h \
			parseOptions.h \
			options.h
