TEMPLATE = subdirs
CONFIG += ordered qt thread

SUBDIRS += 3rdparty/ src/core/libmaven/ src/gui/mzroll/ src/cli/peakdetector tests/MavenTests CrashReporter

# src/gui/mzroll/ src/cli/peakdetector tests/MavenTests CrashReporter

#SUBDIRS +=	\
#		libneural \
#        libcdfread \
#        libplog \
#		libcsvparser\
#		pugixml/src \
#		libmaven \
#        libpillow\
#        libpls \
#    	mzroll \
#        peakdetector \
#        MavenTests \
#       CrashReporter
