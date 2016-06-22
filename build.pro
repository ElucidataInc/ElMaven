TEMPLATE = subdirs
CONFIG += ordered qt thread
CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11
SUBDIRS +=	libneural \
        	libcdfread \
			pugixml/src \
			libmaven \
                mzroll
