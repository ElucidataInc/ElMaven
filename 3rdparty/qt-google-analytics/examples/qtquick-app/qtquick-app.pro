TEMPLATE = app
QT += quick qml widgets network

SOURCES += main.cpp

include(../../qt-google-analytics.pri)

OTHER_FILES += \
    MainWindow.qml

RESOURCES += \
    resource.qrc
