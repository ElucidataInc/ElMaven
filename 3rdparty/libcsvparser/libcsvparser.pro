include($$mzroll_pri)


MOC_DIR=$$top_builddir/tmp/csv_parser/
OBJECTS_DIR=$$top_builddir/tmp/csv_parser/

DESTDIR=$$top_builddir/libs/

TEMPLATE = lib
CONFIG += staticlib warn_off console silent
TARGET = csvparser

SOURCES = csvparser.c
HEADERS = csvparser.h
