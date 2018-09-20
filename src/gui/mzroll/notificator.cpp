#include "notificator.h"
#include "notificator_p.h"
#include "tabledockwidget.h"

#include <QtWidgets>


namespace
{
const int DEFAULT_MESSAGE_SHOW_TIME = 10000;
const float WINDOW_TRANSPARENT_OPACITY = 0.7;
const float WINDOW_NONTRANSPARENT_OPACITY = 1.0;

const int NOTIFICATION_MARGIN = 20;
const int ICON_SPACING = 16;
const int TEXT_SPACING = 8;
}



void Notificator::critical(
		const QString &title,
		const QString &message
		)
{
	QIcon icon = qApp->style()->standardIcon( QStyle::SP_MessageBoxCritical );
	showMessage( icon, title, message );
}

void Notificator::warning(
		const QString &title,
		const QString &message
		)
{
	QIcon icon = qApp->style()->standardIcon( QStyle::SP_MessageBoxWarning );
	showMessage( icon, title, message );
}

void Notificator::information(
		const QString &title,
		const QString &message
		)
{
	QIcon icon = qApp->style()->standardIcon( QStyle::SP_MessageBoxInformation );
	showMessage( icon, title, message );
}

void Notificator::question(
		const QString &title,
		const QString &message
		)
{
	QIcon icon = qApp->style()->standardIcon( QStyle::SP_MessageBoxQuestion );
	showMessage( icon, title, message );
}


Notificator* Notificator::showMessage(
		const QIcon& icon,
		const QString& title,
		const QString& message,
		TableDockWidget* table
		)
{
	Notificator* instance = new Notificator;
	configureInstance(instance);
	instance->_tableDockWidget = table;

	// Запуск уведомления
	instance->notify( icon, title, message );
	QTimer::singleShot( DEFAULT_MESSAGE_SHOW_TIME, instance, SLOT(hide()) );
	return instance;
}

Notificator* Notificator::showMessage(
		const QIcon &icon,
		const QString &title,
		const QString &message,
		const QObject *sender,
		const char *finishedSignal
		)
{
	Notificator* instance = 0;
	if ( sender != 0) {
		instance = new Notificator(false);
		configureInstance(instance);

		// Запуск уведомления
		instance->notify( icon, title, message );
		connect( sender, finishedSignal, instance, SLOT(hide()) );
	}
	return instance;
}


// ******** Реализация скрытых методов ********


Notificator::Notificator(
		bool autohide
		) :
	QFrame( 0 ),
	d( new NotificatorPrivate( autohide ) )
{
	hide();
	initializeLayout();
	initializeUI();
}

Notificator::~Notificator()
{
	delete d;
}

void Notificator::setMessage(const QString &_message)
{
	d->message()->setText(_message);
}

void Notificator::setProgressValue(int _value)
{
	d->progress()->show();
	d->progress()->setValue(_value);
}

bool Notificator::event( QEvent *event )
{
	if ( event->type() == QEvent::HoverEnter ) {
		setWindowOpacity( WINDOW_NONTRANSPARENT_OPACITY );
	} else if ( event->type() == QEvent::HoverLeave ) {
		setWindowOpacity( WINDOW_TRANSPARENT_OPACITY );
	} else if ( event->type() == QEvent::MouseButtonPress
				&& d->autoHide() ) {
		// Если сообщение не отображает информации о выполняющемся действии можно его закрыть.
		// Т.к. кликом может быть активирована ссылка, то необходимо выждать 100 мсек
		// перед скрытием уведомления, для открытия её в браузере
		QTimer::singleShot( 200, this, SLOT(hide()) );
		emit promptClicked(_tableDockWidget);
	}

	return QFrame::event(event);
}

void Notificator::notify(
		const QIcon& icon,
		const QString& title,
		const QString& message
		)
{
	hide();
	d->initialize( icon, title, message );
	correctPosition();
	show();
}

void Notificator::initializeLayout()
{
	QGridLayout *layout = new QGridLayout( this );
	layout->setHorizontalSpacing( 12 );
	layout->addWidget( d->icon(), 0, 0, 2, 1, Qt::AlignTop );
	layout->addWidget( d->title(), 0, 1 );
	layout->addWidget( d->preloader(), 0, 2, 1, 1, Qt::AlignRight );
	layout->addWidget( d->message(), 1, 1, 1, 2 );
	layout->addWidget( d->progress(), 2, 1, 1, 2);
}

void Notificator::initializeUI()
{
	QPalette palette = this->palette();
	palette.setColor(QPalette::Base, Qt::red);
	palette.setColor(QPalette::AlternateBase, Qt::green);
	setPalette(palette);

	setWindowFlags( Qt::ToolTip | Qt::FramelessWindowHint );
	setAttribute( Qt::WA_Hover, true );
	setStyleSheet(
				"Notificator { background-color: orange; border: none; }"
				"QLabel { color: black; }"
				"QLabel#title { font-weight: bold; }"
				"QProgressBar { border: 1px solid black; text-align: top; background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #fff, stop: 0.4999 #eee, stop: 0.5 #ddd, stop: 1 #eee ); }"
				"QProgressBar::chunk { background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #b5e2f9, stop: 0.4999 #68c2f3, stop: 0.5 #67bff0, stop: 1 #1e9bda );  }"
//				"QProgressBar::chunk { background: QLinearGradient( x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #9bd66d, stop: 0.4999 #81d142, stop: 0.5 #81d143, stop: 1 #58bf08 );  }"
	);
	setAutoFillBackground( true );
	setWindowOpacity( WINDOW_TRANSPARENT_OPACITY );
}

void Notificator::correctPosition()
{
	// Вычисляем позицию для отображения уведомления
	// ... сперва сформируем позицию для самой верхней точки
	QRect notificationGeometry = QApplication::desktop()->availableGeometry();
	QSize notificationSize = sizeHint();
	notificationGeometry.setTop( notificationGeometry.top() + NOTIFICATION_MARGIN );
	notificationGeometry.setLeft( notificationGeometry.right() - notificationSize.width() - NOTIFICATION_MARGIN );
	// ... определяем доступную верхнюю координату
	foreach (Notificator* instance, instances) {
		if (instance != this) {
			if (instance->geometry().bottom() > notificationGeometry.top()) {
				notificationGeometry.setTop(instance->geometry().bottom() + NOTIFICATION_MARGIN);
			}
		}
	}
	// Устанавливаем размер
	notificationGeometry.setSize( notificationSize );
	// Отображаем
	setGeometry( notificationGeometry );
}

QList<Notificator*> Notificator::instances;
void Notificator::configureInstance( Notificator *notificator )
{
	// Удаляем уже отработавшие уведомления
	QMutableListIterator<Notificator*> iter(instances);
	while (iter.hasNext()) {
		Notificator* instance = iter.next();
		if (instance->isHidden()) {
			iter.remove();
			instance = 0;
		}
	}

	// Добавляем новое уведомление к списку всех уведомлений
	if ( notificator != 0 ) {
		instances.append(notificator);
	}
}


// ******** private d-pointer implementation ********

namespace {
const int ICON_SIZE = 32;
const int PRELOADER_SIZE = 20;
}

NotificatorPrivate::NotificatorPrivate(
		bool autohide
		) :
	m_autoHide( autohide ),
	m_icon( 0 ),
	m_title( 0 ),
	m_message( 0 ),
	m_preloader( 0 ),
	m_progress(0)
{

}

NotificatorPrivate::~NotificatorPrivate()
{
	if ( icon()->parent() == 0 ) {
		delete icon();
	}
	if ( title()->parent() == 0 ) {
		delete title();
	}
	if ( message()->parent() == 0 ) {
		delete message();
	}
	if ( preloader()->parent() == 0 ) {
		delete preloader();
	}
}

void NotificatorPrivate::initialize(
		const QIcon &icon,
		const QString &title,
		const QString &message
		)
{
	this->icon()->setPixmap( icon.pixmap( ICON_SIZE ) );
	this->title()->setVisible( !title.isEmpty() );
	this->title()->setText( title );
	this->message()->setText( QString( message ).replace( "\n", "<br/>" ) );
	// Если сообщение не настроено на отображение какого-либо процесса,
	// то и ни к чему отображать прелоадер
	this->preloader()->setVisible( !autoHide() );
	this->progress()->hide();
}

bool NotificatorPrivate::autoHide() const
{
	return m_autoHide;
}

QLabel *NotificatorPrivate::icon()
{
	if ( m_icon == 0 ) {
		m_icon = new QLabel;
		m_icon->setObjectName( "icon" );
		m_icon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	}
	return m_icon;
}

QLabel *NotificatorPrivate::title()
{
	if ( m_title == 0 ) {
		m_title = new QLabel;
		m_title->setObjectName( "title" );
		m_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	}
	return m_title;
}

QLabel *NotificatorPrivate::message()
{
	if ( m_message == 0 ) {
		m_message = new QLabel;
		m_message->setObjectName( "message" );
		m_message->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		m_message->setTextFormat( Qt::RichText );
		m_message->setOpenExternalLinks( true );
		m_message->setTextInteractionFlags( Qt::LinksAccessibleByMouse );
	}
	return m_message;
}

QLabel *NotificatorPrivate::preloader()
{
	if ( m_preloader == 0 ) {
		m_preloader = new QLabel;
		m_preloader->setObjectName( "preloader" );
		m_preloader->setMinimumSize( PRELOADER_SIZE, PRELOADER_SIZE );
		m_preloader->setMaximumSize( PRELOADER_SIZE, PRELOADER_SIZE );
		m_preloader->setScaledContents( true );
		// Настройка анимации прелоадера
		QMovie *preloaderAnimation = new QMovie( ":/images/Icons/preloader.gif" );
		preloaderAnimation->setParent( m_preloader );
		preloaderAnimation->setScaledSize( preloader()->size() );
		m_preloader->setMovie( preloaderAnimation );
		preloaderAnimation->start();
	}
	return m_preloader;
}

QProgressBar* NotificatorPrivate::progress()
{
	if (m_progress == 0) {
		m_progress = new QProgressBar;
		m_progress->setObjectName( "progress" );
#ifdef Q_OS_WIN
		m_progress->setMaximumHeight(20);
#else
		m_progress->setMaximumHeight(26);
#endif
		m_progress->setTextVisible(false);
	}
	return m_progress;
}
