

MOC_DIR=$$top_builddir/tmp/date/
OBJECTS_DIR=$$top_builddir/tmp/date/
include($$mzroll_pri)
DESTDIR=$$top_builddir/libs/

TEMPLATE = lib
CONFIG += staticlib warn_off console silent
TARGET = date

HEADERS = date.h chrono_io.h
