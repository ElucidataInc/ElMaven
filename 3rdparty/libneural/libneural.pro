

MOC_DIR=$$top_builddir/tmp/neural/
OBJECTS_DIR=$$top_builddir/tmp/neural/
include($$mzroll_pri)
DESTDIR=$$top_builddir/libs/

TEMPLATE = lib
CONFIG += staticlib warn_off
TARGET = neural

SOURCES=neuron.cc nnwork.cc
HEADERS=neuron.h  nnwork.h

INCLUDEPATH += ./


contains(MEEGO_EDITION,harmattan) {
    target.path = /opt/libneural/lib
    INSTALLS += target
}
