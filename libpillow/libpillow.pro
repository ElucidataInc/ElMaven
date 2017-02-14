include(../mzroll.pri)
include(config.pri)

TARGET = $$PILLOWCORE_LIB_NAME
TEMPLATE = lib
DESTDIR = $$OUTPUT_DIR/lib

QT       += core network script
QT       -= gui

CONFIG   += precompile_header staticlib

PRECOMPILED_HEADER = pch.h

DEPENDPATH = .
INCLUDEPATH = .

DEFINES += PILLOWCORE_BUILD

pillow_static {
	CONFIG += static
	DEFINES += PILLOWCORE_BUILD_STATIC
}

pillow_zlib {
	LIBS += $$PILLOW_ZLIB_LIBS
}

SOURCES += \
	parser/parser.c \
	parser/http_parser.c \
	HttpServer.cpp \
	HttpHandler.cpp \
	HttpHandlerQtScript.cpp \
	HttpHelpers.cpp \
	HttpsServer.cpp \
	HttpHandlerSimpleRouter.cpp \
	HttpConnection.cpp \
	HttpHandlerProxy.cpp \
	HttpClient.cpp \
	HttpHeader.cpp

HEADERS += \
	parser/parser.h \
	parser/http_parser.h \
	HttpServer.h \
	HttpHandler.h \
	HttpHandlerQtScript.h \
	HttpHelpers.h \
	HttpsServer.h \
	HttpHandlerSimpleRouter.h \
	HttpConnection.h \
	HttpHandlerProxy.h \
	ByteArrayHelpers.h \
	private/ByteArray.h \
	HttpClient.h \
	pch.h \
	HttpHeader.h \
	PillowCore.h

OTHER_FILES += \
    pillowcore.qbs
