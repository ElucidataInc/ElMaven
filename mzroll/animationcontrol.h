#ifndef ANIMATIONCONTROL_H
#define ANIMATIONCONTROL_H
#include "ui_animationcontrol.h"

class AnimationControl: public QWidget, public Ui_AnimationControl
{
		Q_OBJECT

		public:
			 AnimationControl(QWidget *parent);
};

#endif
