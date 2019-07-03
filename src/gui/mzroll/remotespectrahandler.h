//Merged with Maven776
#ifndef REMOTESPECTRAHANDLER
#define REMOTESPECTRAHANDLER
#include "stable.h"

#include "HttpServer.h"
#include "HttpHandler.h"
#include "HttpConnection.h"

class MainWindow;
class mzSample;

class RemoteSpectraHandler : public Pillow::HttpHandler
{
	qint64 n;
public:
	RemoteSpectraHandler(QObject* parent = 0) : Pillow::HttpHandler(parent), n(0) {}
	bool handleRequest(Pillow::HttpConnection *connection);
    int doCommand(QString path, QString queryString, QString content, QByteArray& response);

    void setMainWindow(MainWindow* mw) { _mw = mw; }

   private:
    MainWindow* _mw;
};

#endif


