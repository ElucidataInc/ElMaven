#ifndef PILLOW_NO_SSL

#include "HttpsServer.h"
#include "HttpConnection.h"
#include <QtNetwork/QSslSocket>
using namespace Pillow;

//
// HttpsServer
//

HttpsServer::HttpsServer(QObject *parent)
	: HttpServer(parent)
{
}

HttpsServer::HttpsServer(const QSslCertificate& certificate, const QSslKey& privateKey, const QHostAddress &serverAddress, quint16 serverPort, QObject *parent)
	: HttpServer(serverAddress, serverPort, parent), _certificate(certificate), _privateKey(privateKey)
{
}

void HttpsServer::setCertificate(const QSslCertificate &certificate)
{
	_certificate = certificate;
}

void HttpsServer::setPrivateKey(const QSslKey &privateKey)
{
	_privateKey = privateKey;
}

void HttpsServer::incomingConnection(int socketDescriptor)
{
	QSslSocket* sslSocket = new QSslSocket(this);
	if (sslSocket->setSocketDescriptor(socketDescriptor))
	{
		sslSocket->setPrivateKey(privateKey());
		sslSocket->setLocalCertificate(certificate());
		sslSocket->startServerEncryption();
		connect(sslSocket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(sslSocket_sslErrors(QList<QSslError>)));
		connect(sslSocket, SIGNAL(encrypted()), this, SLOT(sslSocket_encrypted()));
		addPendingConnection(sslSocket);
		nextPendingConnection();
		createHttpConnection()->initialize(sslSocket, sslSocket);
	}
	else
	{
		qWarning() << "HttpsServer::incomingConnection: failed to set socket descriptor '" << socketDescriptor << "' on ssl socket.";
		delete sslSocket;
	}
}

void HttpsServer::sslSocket_sslErrors(const QList<QSslError>&)
{
}

void HttpsServer::sslSocket_encrypted()
{
}

#endif // !PILLOW_NO_SSL
