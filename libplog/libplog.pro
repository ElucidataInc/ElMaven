include(../mzroll.pri)
DESTDIR = $$OUTPUT_DIR/lib

TEMPLATE = lib
CONFIG += staticlib warn_off console silent
TARGET = plog

INCLUDEPATH += ./include
LDFLAGS     +=  $$OUTPUT_DIR/lib
LDFLAGS     +=  ./lib

LIBS += -L.

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
    