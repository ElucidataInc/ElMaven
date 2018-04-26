include($$mzroll_pri)

MOC_DIR=$$top_builddir/tmp/qt-google-analytics/
OBJECTS_DIR=$$top_builddir/tmp/qt-google-analytics/

DESTDIR=$$top_builddir/libs/

TARGET = qt-google-analytics

TEMPLATE = lib
CONFIG += staticlib

QT += network

include(qt-google-analytics.pri)

OTHER_FILES += qt-google-analytics.pri
