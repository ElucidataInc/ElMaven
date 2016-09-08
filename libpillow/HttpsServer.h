#ifndef PILLOW_HTTPSSERVER_H
#define PILLOW_HTTPSSERVER_H

#ifndef PILLOW_NO_SSL

#ifndef PILLOW_PILLOWCORE_H
#include "PillowCore.h"
#endif // PILLOW_PILLOWCORE_H
#ifndef PILLOW_HTTPSERVER_H
#include "HttpServer.h"
#endif // PILLOW_HTTPSERVER_H
#ifndef QSSLCERTIFICATE_H
#include <QtNetwork/QSslCertificate>
#endif // QSSLCERTIFICATE_H
#ifndef QSSLKEY_H
#include <QtNetwork/QSslKey>
#endif // QSSLKEY_H
#ifndef QSSLERROR_H
#include <QtNetwork/QSslError>
#endif // QSSLERROR_H

namespace Pillow
{
	//
	// HttpsServer
	//

	class PILLOWCORE_EXPORT HttpsServer : public Pillow::HttpServer
	{
		Q_OBJECT
		QSslCertificate _certificate;
		QSslKey _privateKey;

	public slots:
		void sslSocket_encrypted();
		void sslSocket_sslErrors(const QList<QSslError>& sslErrors);

	protected:
		virtual void incomingConnection(int socketDescriptor);

	public:
		HttpsServer(QObject* parent = 0);
		HttpsServer(const QSslCertificate& certificate, const QSslKey& privateKey, const QHostAddress& serverAddress, quint16 serverPort, QObject *parent = 0);

		const QSslCertificate& certificate() const { return _certificate; }
		const QSslKey& privateKey() const { return _privateKey; }

	public slots:
		void setCertificate(const QSslCertificate& certificate);
		void setPrivateKey(const QSslKey& privateKey);
	};
}

#endif // !PILLOW_NO_SSL

#endif // PILLOW_HTTPSSERVER_H
