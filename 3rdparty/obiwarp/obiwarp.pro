include($$mac_compiler)
include($$mzroll_pri)

MOC_DIR=$$top_builddir/tmp/obiwarp/
OBJECTS_DIR=$$top_builddir/tmp/obiwarp/

DESTDIR=$$top_builddir/libs/
TEMPLATE=lib

CONFIG += staticlib warn_off console silent


QMAKE_CXXFLAGS +=   -std=c++11
QMAKE_CXXFLAGS += -DOMP_PARALLEL

TARGET = obiwarp

linux: QMAKE_CXXFLAGS += -Ofast -ffast-math
win32: QMAKE_CXXFLAGS += -Ofast -ffast-math
macx: QMAKE_CXXFLAGS += -O3

macx{
    INCLUDEPATH += /usr/local/include/
    QMAKE_LFLAGS += -L/usr/local/lib/
}

message($$INCLUDEPATH)

SOURCES =   obiwarp.cpp \
            dynprog.cpp \
            mat.cpp \
            vec.cpp \
            
            
            

HEADERS += 	obiwarp.h \
            dynprog.h \
            mat.h \
            vec.h \
            
            
