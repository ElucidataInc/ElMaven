#ifndef ALIGNDIALOG_H
#define ALIGNDIALOG_H

#include "stable.h"
#include "globals.h"
#include "ui_alignmentdialog.h"
#include "mainwindow.h"

class Database;
class MainWindow;

extern Database DB; 

class AlignmentDialog : public QDialog, public Ui_AlignmentDialog {
		Q_OBJECT

		public:
			 AlignmentDialog(QWidget *parent);
			 void setMainWindow(MainWindow* mw);
			 MainWindow* _mw;

		public Q_SLOTS:
			void intialSetup();
			void algoChanged();
			void setDatabase();
			void setDatabase(QString db);
};

#endif
