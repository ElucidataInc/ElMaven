TEMPLATE = subdirs
CONFIG += ordered qt thread

#Faster build + C++11 ++ OpenMP
QMAKE_CXXFLAGS += -Ofast

SUBDIRS +=	\
		libneural \
        libcdfread \
		libcsvparser\
		pugixml/src \
		libmaven \
    	mzroll \
        peakdetector \
        MavenTests