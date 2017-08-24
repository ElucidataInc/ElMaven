include(../mzroll.pri)
DESTDIR = $$OUTPUT_DIR/lib

TEMPLATE = lib
CONFIG += staticlib warn_off console silent
TARGET = cdfread

INCLUDEPATH += ./include
LDFLAGS     +=  $$OUTPUT_DIR/lib
LDFLAGS     +=  ./lib

LIBS += -L. -L /usr/lib -L./lib -lnetcdf

macx {
	INCLUDEPATH += /usr/local/opt/netcdf/include
	QMAKE_LFLAGS += -L/usr/local/opt/netcdf/lib
	LIBS += -lnetcdf
}


SOURCES=ms10aux.c ms10enum.c ms10io.c
HEADERS=ms10.h ms10io.h

contains(MEEGO_EDITION,harmattan) {
    target.path = /opt/libcdfread/lib
    INSTALLS += target
}
