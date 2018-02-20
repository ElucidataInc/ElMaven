#-------------------------------------------------
#
# Project created by QtCreator 2017-02-04T15:26:12
#
#-------------------------------------------------
include(./s3-qt.pri)

QT       += core gui network

MOC_DIR=$$top_builddir/tmp/crash_reporter/
OBJECTS_DIR=$$top_builddir/tmp/crash_reporter/

CONFIG(debug, debug|release){
    message("running in debug mode  ")
    unix:!macx {
        QMAKE_CCFLAGS+= -fprofile-arcs -ftest-coverage
        QMAKE_CXXFLAGS+= -fprofile-arcs -ftest-coverage
        QMAKE_LFLAGS+= -fprofile-arcs -ftest-coverage
        MOC_DIR=tmp/
        OBJECTS_DIR=tmp/
    }
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CrashReporter
TEMPLATE = app

DESTDIR = $$top_srcdir/bin/

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
