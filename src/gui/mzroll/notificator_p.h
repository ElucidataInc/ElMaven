#ifndef NOTIFICATOR_P_H
#define NOTIFICATOR_P_H


#include <QtGui/QIcon>


class QLabel;
class QIcon;
class QProgressBar;
class QPropertyAnimation;


class NotificatorPrivate
{
public:
	NotificatorPrivate( bool autoHide = true );
	~NotificatorPrivate();

	void initialize(
			const QIcon &icon,
			const QString &title,
			const QString &message
			);

public:
	bool autoHide() const;
	QLabel * icon();
	QLabel * title();
	QLabel * message();
	QLabel * preloader();
	QProgressBar* progress();

private:
	bool m_autoHide;
	QLabel *m_icon;
	QLabel *m_title;
	QLabel *m_message;
	QLabel *m_preloader;
	QProgressBar* m_progress;
};

#endif // NOTIFICATOR_P_H
