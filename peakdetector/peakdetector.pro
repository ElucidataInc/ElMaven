include(../mzroll.pri)

TEMPLATE = app
TARGET = peakdetector
DESTDIR = ../bin
CONFIG += warn_off xml -std=c++14

QT -= network gui opengl 
CONFIG -= network gui opengl 
QMAKE_CXXFLAGS += -fopenmp
LIBS += -fopenmp
INCLUDEPATH += ../pugixml/src/ ../sqlite ../libmaven ../pugixml/src ../libneural ../zlib/ ../libcsvparser  ../libpls

LDFLAGS     +=  $$OUTPUT_DIR/lib

LIBS += -L.  -lmaven -lpugixml -lneural -lcsvparser -lpls

SOURCES	= 	PeakDetectorCLI.cpp  \
		 	options.cpp \
			../libmaven/classifier.cpp \  
			../libmaven/classifierNeuralNet.cpp \
			parseOptions.cpp \
			main.cpp 

HEADERS += 	PeakDetectorCLI.h \
			../libmaven/classifier.h \
			../libmaven/classifierNeuralNet.h \
			parseOptions.h \
			options.h


