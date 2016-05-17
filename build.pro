TEMPLATE = subdirs
CONFIG += ordered qt thread
SUBDIRS += \
        libneural\
        libcdfread\
        pugixml/src\
        libmaven\
        mzroll

#!win32:SUBDIRS += tests
