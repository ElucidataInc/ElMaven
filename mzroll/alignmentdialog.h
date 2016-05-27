#ifndef ALIGNDIALOG_H
#define ALIGNDIALOG_H

#include "stable.h"
#include "ui_alignmentdialog.h"

class AlignmentDialog : public QDialog, public Ui_AlignmentDialog {
		Q_OBJECT

		public:
			 AlignmentDialog(QWidget *parent);
};

#endif
