#ifndef RCONSOLEDIALOG_H
#define RCONSOLEDIALOG_H

#include "stable.h"

#include "ui_rconsoledialog.h"

class Rconnection;


class RConsoleDialog : public QDialog, public Ui_RConsoleDialog {
		Q_OBJECT

		public:
			 RConsoleDialog(QWidget *parent);
			 int connectToRserve();

		public slots:
			 void runScript();
			 void clearScript();

		private:
			  Rconnection *rc ;
};

#endif
