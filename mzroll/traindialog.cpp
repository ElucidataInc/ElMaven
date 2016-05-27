#include "traindialog.h"

TrainDialog::TrainDialog(QWidget *parent) : 
	QDialog(parent) { 
		setupUi(this); 
		setModal(false);
}
