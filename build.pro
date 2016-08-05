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
        peakdetector

test {
	message(Test build)
	TEMPLATE = app
	TARGET = MavenTests
	DESTDIR = .
	QT += testlib core
	CONFIG += qtestlib
	INCLUDEPATH += MavenTests

	# Input
	HEADERS += \
        MavenTests/testEIC.h

	SOURCES += \
        MavenTests/main.cpp \
        MavenTests/testEIC.cpp

} else  {
	message(Normal build)
}