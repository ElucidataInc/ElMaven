#ifndef ANIMATIONCONTROL_H
#define ANIMATIONCONTROL_H
#include "ui_animationcontrol.h"


/**
 * \class AnimationControl
 *
 * \ingroup mzroll
 *
 * \brief Class for Animation Control.
 *
 * This class is used for Animation Control.
 *
 * \author Euigen
 * \author(documentation prepared by naman)
 */

class AnimationControl: public QWidget, public Ui_AnimationControl
{
	Q_OBJECT

public:
	AnimationControl(QWidget *parent);
};

#endif
