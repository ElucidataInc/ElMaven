
MOC_DIR=$$top_builddir/tmp/plog/
OBJECTS_DIR=$$top_builddir/tmp/plog/
include($$mzroll_pri)
DESTDIR=$$top_builddir/libs/

TEMPLATE = lib
CONFIG += staticlib warn_off console silent
TARGET = plog


HEADERS += \
    init.h \
    Log.h \
    Logger.h \
    Record.h \
    Severity.h \
    Util.h \
    Formatters/CsvFormatter.h \
    Formatters/FuncMessageFormatter.h \
    Formatters/TxtFormatter.h \
    Converters/UTF8Converter.h \
    Appenders/AndroidAppender.h \
    Appenders/ColorConsoleAppender.h \
    Appenders/ConsoleAppender.h \
    Appenders/CustomAppender.h \
    Appenders/DebugOutputAppender.h \
    Appenders/EventLogAppender.h \
    Appenders/IAppender.h \
    Appenders/RollingFileAppender.h
