#include "alignmentdialog.h"

AlignmentDialog::AlignmentDialog(QWidget *parent) : QDialog(parent) { 
		setupUi(this); 
		setModal(false);
		
		if (peakDetectionAlgo->currentIndex() == 0) {
			selectDatabase->setVisible(true);
		}
		connect(peakDetectionAlgo, SIGNAL(currentIndexChanged(int)), this, SLOT(algoChanged()));
		setDatabase();
}

void AlignmentDialog::setMainWindow(MainWindow* mw) {
    _mw=mw;
	setDatabase();
}

void AlignmentDialog::algoChanged() {


	if (peakDetectionAlgo->currentIndex() == 0) {
		selectDatabase->setVisible(true);
		selectDatabaseComboBox->setVisible(true);
		setDatabase();

	} else {
		selectDatabase->setVisible(false);
		selectDatabaseComboBox->setVisible(false);
	}
}

void AlignmentDialog::setDatabase() {

	selectDatabaseComboBox->disconnect(SIGNAL(currentIndexChanged(QString)));
	selectDatabaseComboBox->clear();
	QSet<QString>set;
	for(int i=0; i< DB.compoundsDB.size(); i++) {
		if (! set.contains( DB.compoundsDB[i]->db.c_str() ) )
			set.insert( DB.compoundsDB[i]->db.c_str() );
	}

	QIcon icon(rsrcPath + "/dbsearch.png");
	QSetIterator<QString> i(set);
	int pos=0;
	while (i.hasNext()) { 
		selectDatabaseComboBox->addItem(icon,i.next());
	}
}


void AlignmentDialog::setDatabase(QString db) {

	selectDatabaseComboBox->setCurrentIndex(selectDatabaseComboBox->findText(db));

}
