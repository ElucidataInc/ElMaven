TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = breakpad
macx|win32 {
    SUBDIRS += crashserver
}

SUBDIRS += crashreporter
