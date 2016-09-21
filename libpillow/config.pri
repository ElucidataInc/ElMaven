#
# Configurable Pillow Features
#

# Uncomment the following to build pillow as a static lib instead of a shared libary
unix {
	CONFIG += pillow_static
}

mac {
	CONFIG += pillow_static
	CONFIG -= debug
	CONFIG += release
}


message($$CONFIG)

# Uncomment the following to disable SSL support in Pillow.
#CONFIG += pillow_no_ssl

#
# Project Setup (not configurable)
#

pillow_no_ssl: DEFINES += PILLOW_NO_SSL

PILLOWCORE_LIB_NAME = pillowcore


contains(QMAKE_CC, cl) {
	PILLOWCORE_LIB_FILE = $${PILLOWCORE_LIB_NAME}.lib
} else {
	pillow_static: PILLOWCORE_LIB_FILE = lib$${PILLOWCORE_LIB_NAME}.a
	!pillow_static: PILLOWCORE_LIB_FILE = lib$${PILLOWCORE_LIB_NAME}.so
        #QMAKE_CXXFLAGS += --std=c++0x
}

