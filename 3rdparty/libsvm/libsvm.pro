TEMPLATE = lib

DESTDIR=$$top_builddir/libs/
OBJECTS_DIR=$$top_builddir/tmp/svm
MOC_DIR=$$top_builddir/tmp/svm

CONFIG += staticlib
TARGET = svm

INCLUDEPATH += $$top_srcdir/3rdparty/libsvm

SOURCES += svm.cpp
HEADERS += svm.h