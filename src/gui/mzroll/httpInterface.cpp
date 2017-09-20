#include "HttpServer.h"
#include "HttpHandler.h"
#include "HttpConnection.h"

static const QByteArray someHeaderToken("Some-Header");
static const QString someParam("Some-Param");
static const QByteArray helloWorldToken("Hello World!");

class HttpInterface : public Pillow::HttpHandler
{
	qint64 n;
public:
	HttpInterface(QObject* parent = 0) : Pillow::HttpHandler(parent), n(0)
	{}

	bool handleRequest(Pillow::HttpConnection *connection)
	{
		n += connection->requestContent().size();
		n += connection->requestFragment().size();
		n += connection->requestHeaderValue(someHeaderToken).size();
		n += connection->requestMethod().size();
		n += connection->requestPath().size();
		n += connection->requestQueryString().size();
		n += connection->requestUri().size();
		n += connection->requestUriDecoded().size();
		n += connection->requestFragmentDecoded().size();
		n += connection->requestPathDecoded().size();
		n += connection->requestQueryStringDecoded().size();
		n += connection->requestParamValue(someParam).size();
		connection->writeResponse(200, Pillow::HttpHeaderCollection(), helloWorldToken);
		return true;
	}
};

int startHTTPServer(int argc, char *argv[])
{
	Pillow::HttpServer server(QHostAddress(QHostAddress::Any), 4567);
	if (!server.isListening()) {
		qDebug() << "Server failed to start";
	}

	Pillow::HttpHandler* handler = new HttpInterface(&server);

	QObject::connect(&server, SIGNAL(requestReady(Pillow::HttpConnection*)), handler, SLOT(handleRequest(Pillow::HttpConnection*)));
}
