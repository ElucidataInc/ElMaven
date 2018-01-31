INCLUDEPATH += $$PWD/include
QMAKE_CXXFLAGS += -Ofast -ffast-math  -std=c++11
QT += network

HEADERS += \
    $$PWD/include/qs3/qs3.h \
    $$PWD/include/qs3/qbucket.h

SOURCES += \
    $$PWD/src/qs3/qs3.cpp \
    $$PWD/src/qs3/qbucket.cpp
