#ifndef TRAINDIALOG_H
#define TRAINDIALOG_H

#include <qdialog.h>
#include <qobjectdefs.h>

#include "../ui_files/ui_trainingdialog.h"

class TableDockWidget;

class TrainDialog: public QDialog, public Ui_TrainingDialog {
Q_OBJECT

public:
	TrainDialog(QWidget *parent);
};

#endif
