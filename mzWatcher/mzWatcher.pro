

TEMPLATE = app
TARGET = mzWatcher
DESTDIR = ../bin
include(../mzroll.pri)
CONFIG += warn_off gui network
#CONFIG += console
CONFIG -= opengl


FORMS=mzWatcherGui.ui
QT -= opengl
QT += sql network

DEFINES += QT_CORE_LIB QT_DLL QT_NETWORK_LIB QT_SQL_LIB QT_NO_DEBUG QT_THREAD_LIB
SOURCES=mzWatcher.cpp mainWindow.cpp 
HEADERS=mainWindow.h

RC_FILE = mzWatcher.rc

contains(MEEGO_EDITION,harmattan) {
    target.path = /opt/mzWatcher/bin
    INSTALLS += target
}
