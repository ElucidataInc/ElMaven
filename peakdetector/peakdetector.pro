include(../mzroll.pri)

TEMPLATE = app
TARGET = peakdetector
DESTDIR = ../bin
CONFIG += qt thread warn_off sql svg console precompile_header
QT += network gui opengl

DEFINES += QT_CORE_LIB QT_DLL QT_NETWORK_LIB QT_SQL_LIB QT_NO_DEBUG QT_THREAD_LIB
INCLUDEPATH += ../pugixml/src/ ../sqlite ../libmaven ../pugixml/src ../libneural ../mzroll/tmp ../mzroll
INCLUDEPATH += /usr/include/qt4/QtSql /usr/include/qt4/QtNetwork /usr/include/qt/QtGui

LDFLAGS     +=  $$OUTPUT_DIR/lib

LIBS += -L. \
 -lmaven \
 -lpugixml \
 -lneural \
 -lcdfread \
 -lnetcdf

SOURCES= peakdetector.cpp  options.cpp
HEADERS += options.h
