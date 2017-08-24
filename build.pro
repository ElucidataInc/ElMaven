TEMPLATE = subdirs
CONFIG += ordered qt thread


macx{
    QMAKE_CXX = /usr/local/opt/llvm@3.7/lib/llvm-3.7/bin/clang++
}


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
macx{
    SUBDIRS -= libplog
}
