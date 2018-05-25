#-------------------------------------------------
#
# Project created by QtCreator 2017-02-04T15:26:12
#
#-------------------------------------------------

TEMPLATE = app
TARGET = CrashReporter


QT       += widgets core gui network
CONFIG += console

MOC_DIR=$$top_builddir/tmp/crash_reporter/
OBJECTS_DIR=$$top_builddir/tmp/crash_reporter/
DESTDIR = $$top_srcdir/bin/

QMAKE_CXXFLAGS += -std=c++11

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

win32 {
    INCLUDEPATH  += $$top_srcdir/3rdparty/google-breakpad/src/
    QMAKE_LFLAGS += -L$$top_builddir/libs/
    LIBS += -lgoogle-breakpad -pthread -lwininet
}



SOURCES += main.cpp\
        mainwindow.cpp




HEADERS  += mainwindow.h \

win32 | macx {

    SOURCES += file_uploader.cpp
    HEADERS += file_uploader.h
}

FORMS    += mainwindow.ui
