include(../mzroll.pri)

TEMPLATE = app
TARGET = peakdetector
DESTDIR = ../bin
CONFIG += warn_off xml 

QT -= network gui opengl 
CONFIG -= network gui opengl 

INCLUDEPATH += ../pugixml/src/ ../sqlite ../libmaven ../pugixml/src ../libneural ../zlib/

LDFLAGS     +=  $$OUTPUT_DIR/lib

LIBS += -L.  -lmaven -lpugixml -lneural

SOURCES	= 	PeakDetectorCLI.cpp  \
		 	options.cpp \
			../libmaven/classifier.cpp \  
			../libmaven/classifierNeuralNet.cpp\

HEADERS += 	../libmaven/classifier.h \
			../libmaven/classifierNeuralNet.h \
			options.h


