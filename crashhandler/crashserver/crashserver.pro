TEMPLATE = app
QT -= gui

mac: DESTDIR = $$APPDIR
win32: DESTDIR = $$top_srcdir/bin/

CONFIG -= app_bundle
TARGET = crashserver

CONFIG(force_debug_info, force_debug_info) {
  warning("Removing 'force_debug_info' flag since it adds -O2 optimization flag")
  CONFIG -= force_debug_info
}

# With optimizations the program does not work as expected therefore we need to disable them
# Clang by default uses -O2
QMAKE_CXXFLAGS_DEBUG -= -O1 -O2 -O3
QMAKE_CXXFLAGS_RELEASE -= -O1 -O2 -O3

QMAKE_CFLAGS_DEBUG -= -O1 -O2 -O3
QMAKE_CFLAGS_RELEASE -= -O1 -O2 -O3


INCLUDEPATH += ../breakpad/src/src/ \

QMAKE_LFLAGS += -L$$top_builddir/libs/

mac: LIBS += -framework CoreServices 

LIBS += -lbreakpad

SOURCES += elmavcrashserver.cpp
