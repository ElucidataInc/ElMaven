TEMPLATE = subdirs
CONFIG += ordered qt thread

#Faster build + C++11 ++ OpenMP
#QMAKE_CXXFLAGS += -Ofast

SUBDIRS +=	\
		libneural \
        libcdfread \
        libplog \
		libcsvparser\
		pugixml/src \
		libmaven \
        libpillow\
        libpls \
    	mzroll \
        peakdetector \
        MavenTests \
        CrashReporter
