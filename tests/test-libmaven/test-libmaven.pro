include($$mac_compiler)
DESTDIR = $$top_srcdir/bin/


OBJECTS_DIR=$$top_builddir/tmp/doctest/
include($$mzroll_pri)
TEMPLATE = app
TARGET = test-libmaven

CONFIG+=console
CONFIG-=app_bundle

QT -= gui core

QMAKE_CXXFLAGS +=  -std=c++11
QMAKE_CXXFLAGS += -DOMP_PARALLEL
QMAKE_CXXFLAGS += -fopenmp

INCLUDEPATH +=  $$top_srcdir/src/core/libmaven  $$top_srcdir/3rdparty/pugixml/src $$top_srcdir/3rdparty/libneural $$top_srcdir/3rdparty/libpls \
				$$top_srcdir/3rdparty/libcsvparser $$top_srcdir/src/cli/peakdetector $$top_srcdir/3rdparty/libdate $$top_srcdir/3rdparty/libcdfread \
                $$top_srcdir/3rdparty/obiwarp $$top_srcdir/src/pollyCLI \
                $$top_srcdir/3rdparty/Eigen $$top_srcdir/src/      \
                $$top_srcdir/3rdparty/doctest       \
                $$top_srcdir/3rdparty/json
macx {

    DYLIBPATH = $$system(source ~/.bash_profile ; echo $LDFLAGS)
    isEmpty(DYLIBPATH) {
        warning("LDFLAGS variable is not set. Linking operation might complain about missing OMP library")
        warning("Please follow the README to make sure you have correctly set the LDFLAGS variable")
    }
    QMAKE_LFLAGS += $$DYLIBPATH
}
QMAKE_LFLAGS += -L$$top_builddir/libs/

LIBS += -lmaven -lpugixml -lneural -lcsvparser -lpls -lErrorHandling -lLogger -lcdfread -lz -lnetcdf -lobiwarp -lpollyCLI -lcommon
unix: LIBS += -lboost_system -lboost_filesystem
win32: LIBS += -lboost_system-mt -lboost_filesystem-mt
!macx: LIBS += -fopenmp

macx {
    LIBS += -lomp
    LIBS -= -lnetcdf -lcdfread
}


# Input
HEADERS += \
    $$top_srcdir/src/core/libmaven/jsonReports.h        \
    $$top_srcdir/src/core/libmaven/csvreports.h         \
    $$top_srcdir/src/core/libmaven/Compound.h
 
SOURCES += \
    main.cpp \
    $$top_srcdir/src/core/libmaven/jsonReports.cpp      \
    $$top_srcdir/src/core/libmaven/csvreports.cpp       \
    $$top_srcdir/src/core/libmaven/Compound.cpp
