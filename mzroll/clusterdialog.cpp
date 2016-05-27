#include "clusterdialog.h"

ClusterDialog::ClusterDialog(QWidget *parent) : 
	QDialog(parent) { 
		setupUi(this); 
		setModal(false);
}
