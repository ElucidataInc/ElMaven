include(../mzroll.pri)
DESTDIR = $$OUTPUT_DIR/lib

TEMPLATE = lib
CONFIG += staticlib warn_off console silent
TARGET = csvparser

INCLUDEPATH += ./include
LDFLAGS     +=  $$OUTPUT_DIR/lib
LDFLAGS     +=  ./lib

LIBS += -L.

SOURCES = csvparser.c
HEADERS = csvparser.h
