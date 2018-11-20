TEMPLATE = subdirs
CONFIG += ordered qt thread

SUBDIRS += core/libmaven \
           pollyCLI \
           projectDB \
           gui/mzroll \
           cli/peakdetector
