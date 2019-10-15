include($$mac_compiler)

TEMPLATE = lib
CONFIG += c++11 console staticlib warn_off
CONFIG -= app_bundle
CONFIG -= qt

OBJECTS_DIR = $$top_builddir/tmp/libmgf
DESTDIR = $$top_builddir/libs/
TARGET = mgf

QMAKE_CXXFLAGS += -std=c++11

parser.target = Parser.h
parser.depends = FORCE
parser.commands = bash parsergen.sh
PRE_TARGETDEPS += Parser.h
QMAKE_EXTRA_TARGETS += parser

INCLUDEPATH += $$top_srcdir/3rdparty/libmgf/src \
               $$top_srcdir/3rdparty/libmgf/mgf \
               /usr/local/include/

SOURCES	+= Driver.cpp      \
           MgfFile.cpp     \
           MgfHeader.cpp   \
           MgfSpectrum.cpp \
           Parser.cpp      \
           Scanner.cpp

HEADERS += Collection.h        \
           config.h           \
           Context.h           \
           Driver.h            \
           FlexLexer.h         \
           location.h          \
           MassAbundancePair.h \
           mgf.h               \
           MgfFile.h           \
           MgfHeader.h         \
           MgfSpectrum.h       \
           Nodes.h             \
           Parser.h            \
           position.h          \
           Scanner.h           \
           stack.h             \
           utils.h

DISTFILES += \
           parsergen.sh
