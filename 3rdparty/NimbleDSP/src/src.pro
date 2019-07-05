include($$mac_compiler)

TEMPLATE = lib
CONFIG += c++11 console staticlib warn_off
CONFIG -= app_bundle
CONFIG -= qt

OBJECTS_DIR = $$top_builddir/tmp/nimble
DESTDIR = $$top_builddir/libs/
TARGET = NimbleDSP

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += $$top_srcdir/src/   \
               /usr/local/include/

SOURCES	+= kiss_fft.c     \
           kiss_fftr.c

HEADERS += _kiss_fft_guts.h    \
           ComplexVector.h     \
           kiss_fft.h          \
           kiss_fftr.h         \
           kissfft.hh          \
           NimbleDspCommon.h   \
           ParksMcClellan.h    \
           RealFirFilter.h     \
           RealVector.h        \
           Vector.h
