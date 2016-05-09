include(../mzroll.pri)
DESTDIR = $$OUTPUT_DIR/lib

TEMPLATE = lib
CONFIG += staticlib warn_off
TARGET = neural
SOURCES=neuron.cc nnwork.cc
HEADERS=neuron.h  nnwork.h
INCLUDEPATH += ./

