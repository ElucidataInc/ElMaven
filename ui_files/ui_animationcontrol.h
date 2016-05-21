/********************************************************************************
** Form generated from reading UI file 'animationcontrol.ui'
**
** Created by: Qt User Interface Compiler version 4.8.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ANIMATIONCONTROL_H
#define UI_ANIMATIONCONTROL_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSlider>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AnimationControl
{
public:
    QHBoxLayout *horizontalLayout;
    QSlider *slider;
    QLabel *titleLabel;

    void setupUi(QWidget *AnimationControl)
    {
        if (AnimationControl->objectName().isEmpty())
            AnimationControl->setObjectName(QString::fromUtf8("AnimationControl"));
        AnimationControl->resize(368, 38);
        horizontalLayout = new QHBoxLayout(AnimationControl);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        slider = new QSlider(AnimationControl);
        slider->setObjectName(QString::fromUtf8("slider"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(slider->sizePolicy().hasHeightForWidth());
        slider->setSizePolicy(sizePolicy);
        slider->setMaximumSize(QSize(250, 16777215));
        slider->setOrientation(Qt::Horizontal);
        slider->setInvertedAppearance(false);
        slider->setInvertedControls(false);
        slider->setTickPosition(QSlider::NoTicks);
        slider->setTickInterval(1);

        horizontalLayout->addWidget(slider);

        titleLabel = new QLabel(AnimationControl);
        titleLabel->setObjectName(QString::fromUtf8("titleLabel"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(titleLabel->sizePolicy().hasHeightForWidth());
        titleLabel->setSizePolicy(sizePolicy1);
        titleLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        titleLabel->setOpenExternalLinks(false);

        horizontalLayout->addWidget(titleLabel);


        retranslateUi(AnimationControl);

        QMetaObject::connectSlotsByName(AnimationControl);
    } // setupUi

    void retranslateUi(QWidget *AnimationControl)
    {
        AnimationControl->setWindowTitle(QApplication::translate("AnimationControl", "AnimationControl", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        slider->setToolTip(QApplication::translate("AnimationControl", "Animation Control", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        titleLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class AnimationControl: public Ui_AnimationControl {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ANIMATIONCONTROL_H
