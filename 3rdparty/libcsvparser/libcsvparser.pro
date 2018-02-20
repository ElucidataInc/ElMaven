


MOC_DIR=$$top_builddir/tmp/csv_parser/
OBJECTS_DIR=$$top_builddir/tmp/csv_parser/
include($$mzroll_pri)
DESTDIR=$$top_builddir/libs/

TEMPLATE = lib
CONFIG += staticlib warn_off console silent
TARGET = csvparser

SOURCES = csvparser.c
HEADERS = csvparser.h
