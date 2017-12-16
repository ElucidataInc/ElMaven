include($$mzroll_pri)

MOC_DIR=$$top_builddir/tmp/date/
OBJECTS_DIR=$$top_builddir/tmp/date/

DESTDIR=$$top_builddir/libs/

TEMPLATE = lib
CONFIG += staticlib warn_off console silent
TARGET = date

HEADERS = date.h chrono_io.h
