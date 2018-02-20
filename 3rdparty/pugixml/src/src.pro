MOC_DIR=$$top_builddir/tmp/pugixml/
OBJECTS_DIR=$$top_builddir/tmp/pugixml/
include($$mzroll_pri)

DESTDIR=$$top_builddir/libs/

TEMPLATE = lib
CONFIG += staticlib warn_off exceptions
TARGET = pugixml


SOURCES=pugixml.cpp
HEADERS=pugixml.hpp  pugiconfig.hpp

contains(MEEGO_EDITION,harmattan) {
    target.path = /opt/src/lib
    INSTALLS += target
}
