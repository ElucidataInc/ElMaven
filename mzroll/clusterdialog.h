#ifndef CLUSTERDIALOG_H
#define CLUSTERDIALOG_H

#include "stable.h"
#include "ui_clusterdialog.h"
#include "tabledockwidget.h"

class TableDockWidget;

class ClusterDialog : public QDialog, public Ui_ClusterDialog
{
		Q_OBJECT

		public:
			 ClusterDialog(QWidget *parent);
};

#endif
