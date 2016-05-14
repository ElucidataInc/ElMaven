#ifndef ALIGNDIALOG_H
#define ALIGNDIALOG_H

#include "stable.h"
#include "ui_alignmentdialog.h"


/**
 * \class AlignmentDialog
 *
 * \ingroup mzroll
 *
 * \brief Class for Alignment Dialog.
 *
 * This class is used for Alignment Dialog.
 *
 * \author Euigen
 * \author(documentation prepared by naman)
 */

class AlignmentDialog : public QDialog, public Ui_AlignmentDialog {
	Q_OBJECT

public:
	AlignmentDialog(QWidget *parent);
};

#endif
