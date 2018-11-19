TEMPLATE = lib

OBJECTS_DIR = $$top_builddir/tmp/projectDB/
DESTDIR = $$top_builddir/libs/
TARGET = projectDB

CONFIG += xml console staticlib warn_off

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += $$top_srcdir/src/core/libmaven \
               /usr/local/include/

QMAKE_LFLAGS += -L$$top_builddir/libs/

LIBS += -lmaven \
        -fopenmp \
        -lboost_filesystem \
        -lsqlite3

macx {
    LIBS -= -fopenmp
}

HEADERS += 	schema.h \
