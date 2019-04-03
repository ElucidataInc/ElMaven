include($$mac_compiler)
include($$mzroll_pri)

DESTDIR = $$top_srcdir/bin
MOC_DIR = $$top_builddir/tmp/peakdetector/
OBJECTS_DIR = $$top_builddir/tmp/peakdetector/

TEMPLATE = app
TARGET = peakdetector

CONFIG += warn_off xml console

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH +=  $$top_srcdir/src/core/libmaven     \
                $$top_srcdir/3rdparty/pugixml/src  \
                $$top_srcdir/3rdparty/libneural    \
                $$top_srcdir/3rdparty/libpls       \
                $$top_srcdir/3rdparty/libcsvparser \
                $$top_srcdir/3rdparty/libdate      \
                $$top_srcdir/3rdparty/libcdfread   \
                $$top_srcdir/src/pollyCLI          \
                $$top_srcdir/3rdparty/obiwarp      \
                $$top_srcdir/3rdparty/Eigen

QMAKE_LFLAGS  +=  -L$$top_builddir/libs/

LIBS +=  -lmaven         \
         -lpugixml       \
         -lneural        \
         -lcsvparser     \
         -lpls           \
         -lErrorHandling \
         -lLogger        \
         -lcdfread       \
         -lnetcdf        \
         -lz             \
         -lobiwarp       \
         -lpollyCLI

LIBS += -fopenmp

macx {
    DYLIBPATH = $$(LDFLAGS)
    isEmpty(DYLIBPATH) {
        warning("LDFLAGS variable is not set. Linking operation might complain about missing OMP library")
        warning("Please follow the README to make sure you have correctly set the LDFLAGS variable")
    }
    QMAKE_LFLAGS += $$(LDFLAGS)
    QMAKE_CXXFLAGS += -fopenmp
    LIBS += -lomp
    LIBS -= -lnetcdf -lcdfread
}

SOURCES	= options.cpp                                            \
          $$top_srcdir/src/core/libmaven/classifier.cpp          \
          $$top_srcdir/src/core/libmaven/classifierNeuralNet.cpp \
          main.cpp                                               \
          parseoptions.cpp                                       \
          peakdetectorcli.cpp

HEADERS += $$top_srcdir/src/core/libmaven/classifier.h          \
           $$top_srcdir/src/core/libmaven/classifierNeuralNet.h \
           options.h                                            \
           parseoptions.h                                       \
           peakdetectorcli.h
