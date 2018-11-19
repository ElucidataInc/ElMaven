TEMPLATE = lib

OBJECTS_DIR = $$top_builddir/tmp/projectDB/
DESTDIR = $$top_builddir/libs/
TARGET = projectDB

CONFIG += xml console staticlib warn_off

QMAKE_CXXFLAGS += -std=c++11

INCLUDEPATH += $$top_srcdir/src/core/libmaven \
               $$top_srcdir/3rdparty/obiwarp   \
               $$top_srcdir/3rdparty/pugixml/src \
               $$top_srcdir/3rdparty/libneural \
               $$top_srcdir/3rdparty/Eigen/ \
               $$top_srcdir/3rdparty/libpls \
               $$top_srcdir/3rdparty/libcsvparser \
               $$top_srcdir/3rdparty/libcdfread \
               $$top_srcdir/3rdparty/ \
               $$top_srcdir/3rdparty/libpillow \
               $$top_srcdir/3rdparty/libdate/ \
               $$top_srcdir/3rdparty/ErrorHandling \
               $$top_srcdir/3rdparty/Logger \
               $$top_srcdir/src/pollyCLI \
               /usr/local/include/

QMAKE_LFLAGS += -L$$top_builddir/libs/

LIBS += -lmaven \
        -fopenmp \
        -lboost_filesystem \
        -lsqlite3

macx {
    LIBS -= -fopenmp
}

SOURCES	= connection.cpp \
          cursor.cpp \
          projectdatabase.cpp

HEADERS += 	schema.h \
            connection.h \
            cursor.h \
            projectdatabase.h
