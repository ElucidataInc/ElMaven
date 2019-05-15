include($$mac_compiler)
TEMPLATE = subdirs
CONFIG += ordered qt thread

SUBDIRS += core/libmaven \
           common \
           pollyCLI \
           projectDB \
           gui/mzroll \
           cli/peakdetector
