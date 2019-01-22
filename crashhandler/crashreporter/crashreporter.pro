#-------------------------------------------------
#
# Project created by QtCreator 2017-02-04T15:26:12
#
#-------------------------------------------------

include($$info_pri)
TEMPLATE = app
TARGET = crashreporter


QT       += widgets core gui network
CONFIG += console

MOC_DIR=$$top_builddir/tmp/crashreporter/
OBJECTS_DIR=$$top_builddir/tmp/crashreporter/

win32: DESTDIR = $$top_srcdir/bin/


QMAKE_CXXFLAGS += -std=c++11


INCLUDEPATH += $$top_srcdir/crashhandler/breakpad/src/src

QMAKE_LFLAGS += -L$$top_builddir/libs/

LIBS += -lbreakpad -pthread



win32: LIBS += -lwininet

linux: LIBS += -ldl

exists($$top_srcdir/keys) {
    RESOURCES = $$top_srcdir/endpoint.qrc
}

SOURCES += main.cpp\
        mainwindow.cpp




HEADERS  += mainwindow.h \


SOURCES += file_uploader.cpp
HEADERS += file_uploader.h


FORMS    += mainwindow.ui
