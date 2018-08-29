//notificator class and related headers have been borrowed from dimkanovikov
//link: https://gist.github.com/dimkanovikov/7cf62ff0fdf833512d33b1134d434de

#ifndef NOTIFICATOR_H
#define NOTIFICATOR_H

#include <QFrame>
#include "notificator_p.h"

#include <QLabel>;
#include <QIcon>;
#include <QProgressBar>;
#include <QPropertyAnimation>;

class Notificator : public QFrame
{
	Q_OBJECT

public:
	static void critical(
			const QString &title,
			const QString &message
			);
	static void warning(
			const QString &title,
			const QString &message
			);
	static void information(
			const QString &title,
			const QString &message
			);
	static void question(
			const QString &title,
			const QString &message
			);
	static void showMessage(
			const QIcon &icon,
			const QString &title,
			const QString &message
			);
	static Notificator* showMessage(
			const QIcon &icon,
			const QString &title,
			const QString &message,
			const QObject *sender,
			const char *finishedSignal
			);

public slots:
	void setMessage(const QString& _message);
	void setProgressValue(int _value);

protected:
	bool event( QEvent* );

private:
	Notificator(
			bool autohide = true
			);
	~Notificator();

	void notify(
			const QIcon &icon,
			const QString &title,
			const QString &message
			);

private:
	void initializeLayout();
	void initializeUI();
	void correctPosition();

private:
	NotificatorPrivate *d;

	static void configureInstance( Notificator *notificator );
	static QList<Notificator*> instances;
};

#endif //NOTIFICATOR_H
