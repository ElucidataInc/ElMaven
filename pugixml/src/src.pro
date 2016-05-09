include(../../mzroll.pri)
DESTDIR = $$OUTPUT_DIR/lib

TEMPLATE = lib
CONFIG += staticlib warn_off
TARGET = pugixml

SOURCES=pugixml.cpp  pugixpath.cpp
HEADERS=pugixml.hpp  pugiconfig.hpp

