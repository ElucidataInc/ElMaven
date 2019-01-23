TEMPLATE = app
QT -= gui

mac: DESTDIR = $$top_srcdir/binaries/mac
win32: DESTDIR = $$top_srcdir/bin/


TARGET = crashserver

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
