#ifndef TRAINDIALOG_H
#define TRAINDIALOG_H

#include "stable.h"
#include "ui_trainingdialog.h"
#include "tabledockwidget.h"

class TableDockWidget;

class TrainDialog : public QDialog, public Ui_TrainingDialog
{
		Q_OBJECT

		public:
			 TrainDialog(QWidget *parent);
};

#endif
