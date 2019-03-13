#include "videoplayer.h"
#include "mainwindow.h"

#include <QtWidgets>
#include <QVideoWidget>
#include <QSizePolicy>
#include <QSettings>
#include <QPushButton>


UrlLabel::UrlLabel(VideoPlayer* parent):
    QLabel(parent)
{
    setOpenExternalLinks(true);
    setText("<a href=\"https://github.com/ElucidataInc/ElMaven/issues/964\">Know more</a>");
}

void UrlLabel::mousePressEvent(QMouseEvent* event)
{
    emit clicked();
}

VideoPlayer::VideoPlayer(QSettings *settings,MainWindow* mw, QWidget* parent)
    : QWidget(parent)
    , m_settings(settings)
    , m_mainWindow(mw)
{
    QRect availableGeometry = QApplication::desktop()->availableGeometry();
    resize(availableGeometry.width()/1.7, availableGeometry.height()/1.3);
    setWindowTitle("New Machine learning Algorithm");
    QBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20,20,20,20);
    m_title = new QLabel(this);
    m_title->setText("<b>Make your analyses more insightful with ML. View your fluxomics workflow in PollyPhi</b>");
    m_title->setContentsMargins(0,0,0,10);
    m_title->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_title);

    m_message = new QLabel(this);
    m_message->setWordWrap(true);
    m_message->setAlignment(Qt::AlignCenter);
    m_message->setContentsMargins(0,0,0,20);
    m_message->setText("Classify groups as Good and Bad quickly and with accuracy with the new Machine Learning \
algorithm. Save time classifying data and spend more time analyzing it. ");
    layout->addWidget(m_message);


    m_extraText =new QLabel(this);
    m_extraText->setText("To know more view the demo below");
    m_extraText->setAlignment(Qt::AlignCenter);

    layout->addWidget(m_extraText);

    m_vidWidget = new QVideoWidget(this);

#if defined(__w64)
    m_vidWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
#elif defined(__APPLE__)
    m_vidWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
#endif

    m_vidWidget->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_vidWidget);

    m_knowMoreLabel = new UrlLabel(this);
    layout->addWidget(m_knowMoreLabel);

    layout->itemAt(0)->setAlignment(Qt::AlignCenter);
    layout->itemAt(4)->setAlignment(Qt::AlignCenter);


    m_mediaPlayer = new QMediaPlayer(this, QMediaPlayer::VideoSurface);
    m_mediaPlayer->setVideoOutput(m_vidWidget);


    QBoxLayout* controlsLayout = new QHBoxLayout(this);

    m_playButton = new QPushButton;
    m_playButton->setEnabled(false);
    m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

    controlsLayout->addWidget(m_playButton);

    m_positionSlider = new QSlider(Qt::Horizontal);
    m_positionSlider->setRange(0, 0);
    controlsLayout->addWidget(m_positionSlider);

    layout->addLayout(controlsLayout);

    QBoxLayout* footerLayout = new QHBoxLayout(this);

    m_checkBox = new QCheckBox(this);
    m_checkBox->setText("Don't show this again");
    m_checkBox->setChecked(m_settings->value("hideVideoPlayer", 0).toBool());
    if(m_checkBox->isChecked())
        m_checkBox->setEnabled(false);
    footerLayout->addWidget(m_checkBox);

    m_closeButton = new QPushButton(this);
    m_closeButton->setText("Close");



    footerLayout->addWidget(m_closeButton);
    footerLayout->itemAt(1)->setAlignment(Qt::AlignRight);

    layout->addLayout(footerLayout);


    connect(m_playButton, &QAbstractButton::clicked,
            this, &VideoPlayer::play);
    connect(m_positionSlider, &QAbstractSlider::sliderMoved,
            this, static_cast<void (VideoPlayer::*)(int)>(&VideoPlayer::setPosition));
    connect(m_positionSlider, &QAbstractSlider::sliderPressed,
            this, static_cast<void (VideoPlayer::*)()>(&VideoPlayer::setPosition));
    connect(m_mediaPlayer, &QMediaPlayer::stateChanged,
            this, &VideoPlayer::mediaStateChanged);
    connect(m_mediaPlayer, &QMediaPlayer::positionChanged, this, &VideoPlayer::positionChanged);
    connect(m_mediaPlayer, &QMediaPlayer::durationChanged, this, &VideoPlayer::durationChanged);
    connect(m_mediaPlayer,QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error),
            this, &VideoPlayer::mediaError);

    connect(m_closeButton, &QPushButton::clicked, this , &VideoPlayer::close);

    connect(m_knowMoreLabel, &UrlLabel::clicked, this, &VideoPlayer::linkClicked);

    m_mediaPlayer->setPosition(0);

#if defined(Q_OS_MACOS) || defined(Q_OS_LINUX)
    setUrl(QUrl("qrc:/videos/PollyPhi-Workflow.mp4"));
#elif defined(Q_OS_WIN32)
    setUrl(QUrl("qrc:/videos/PollyPhi-Workflow.avi"));
#endif

    playedVideoOnce = 0;
    linkClickedOnce = 0;
}

VideoPlayer::~VideoPlayer()
{
}

void VideoPlayer::showEvent(QShowEvent* event)
{
    // avoid the empty/black screen when the widget shows
    m_mediaPlayer->play();
    m_mediaPlayer->pause();
}


void VideoPlayer::closeEvent(QCloseEvent *event)
{
    m_settings->setValue("hideVideoPlayer", (int)m_checkBox->isChecked());

    if(m_checkBox->isChecked())
        m_checkBox->setEnabled(false);

    m_mediaPlayer->pause();
    m_mediaPlayer->setPosition(0);

}

void VideoPlayer::linkClicked()
{
    if(!linkClickedOnce) {
        std::cerr << "sending event for Know more prompt\n";
        m_mainWindow->getAnalytics()->hitEvent("VideoPlayer","KnowMoreMLPrompt");
        linkClickedOnce++;
    }
}

void VideoPlayer::setUrl(const QUrl &url)
{
    setWindowFilePath(url.isLocalFile() ? url.toLocalFile() : QString());
    m_mediaPlayer->setMedia(url);
    m_playButton->setEnabled(true);
}

void VideoPlayer::play()
{
    if(!playedVideoOnce) {
        // for every session we only want one event
        std::cerr << "sending event for Ml Video\n";
        m_mainWindow->getAnalytics()->hitEvent("VideoPlayer", "PlayedVideoMLPrompt");
        playedVideoOnce++;
    }

    switch (m_mediaPlayer->state()) {
    case QMediaPlayer::PlayingState:
        m_mediaPlayer->pause();
        break;
    default:
        m_mediaPlayer->play();

#if defined (__APPLE__)
        // BUG: Videos, on macOS,  dont scale according to the widget size. As a result videos get cut off from the widget
        // SOLUTION: Resize the video widget.
        // Kudos to https://stackoverflow.com/a/51736245/4069760 .
        QSize originalSize = m_vidWidget->size();
        QSize newSize = originalSize + QSize(1,1);
        m_vidWidget->resize(newSize);
        m_vidWidget->resize(originalSize);
#endif

        break;
    }
}

void VideoPlayer::mediaError(QMediaPlayer::Error err)
{
    qDebug() << "error : " << err << " " << m_mediaPlayer->errorString();
}

void VideoPlayer::mediaStateChanged(QMediaPlayer::State state)
{
    switch(state) {
    case QMediaPlayer::PlayingState:
        m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    default:
        m_playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    }
}

void VideoPlayer::positionChanged(qint64 position)
{
    m_positionSlider->setValue(position);
}

void VideoPlayer::durationChanged(qint64 duration)
{
    m_positionSlider->setRange(0, duration);
}

void VideoPlayer::setPosition()
{
    m_mediaPlayer->setPosition(m_positionSlider->value());
}
void VideoPlayer::setPosition(int position)
{
    m_mediaPlayer->setPosition(position);
}
