#include "alignmentdialog.h"

AlignmentDialog::AlignmentDialog(QWidget *parent) : QDialog(parent) { 
		setupUi(this); 
		setModal(false);
		
		if (peakDetectionAlgo->currentIndex() == 0) {
			selectDatabase->setVisible(true);
		}
		connect(peakDetectionAlgo, SIGNAL(currentIndexChanged(int)), this, SLOT(algoChanged()));
}

void AlignmentDialog::setMainWindow(MainWindow* mw) {
    _mw=mw;

}

void AlignmentDialog::show() {

    inputInitialValuesAlignmentDialog();
	QDialog::show();

}

void AlignmentDialog::inputInitialValuesAlignmentDialog() {

	minGoodPeakCount->setValue(_mw->mavenParameters->minGoodGroupCount);
	groupingWindow->setValue(_mw->mavenParameters->rtStepSize);

	minGroupIntensity->setValue(_mw->mavenParameters->minGroupIntensity);
	minSN->setValue(_mw->mavenParameters->minSignalBaseLineRatio);
	minPeakWidth->setValue(_mw->mavenParameters->minNoNoiseObs);
	minIntensity->setValue(_mw->mavenParameters->minIntensity);
	maxIntensity->setValue(_mw->mavenParameters->maxIntensity);

}

void AlignmentDialog::setProgressBar(QString text, int progress,
                                         int totalSteps) {
        showInfo(text);
        progressBar->setRange(0, totalSteps);
        progressBar->setValue(progress);
}

void AlignmentDialog::showInfo(QString text) {
        statusText->setText(text);
}

void AlignmentDialog::intialSetup() {

	setProgressBar("Status", 0, 1);
	setDatabase();
	setDatabase(_mw->ligandWidget->getDatabaseName());
	algoChanged();
	minIntensity->setValue(_mw->mavenParameters->minIntensity);
	maxIntensity->setValue(_mw->mavenParameters->maxIntensity);
}

void AlignmentDialog::algoChanged() {


	if (peakDetectionAlgo->currentIndex() == 0) {
		selectDatabase->setVisible(true);
		selectDatabaseComboBox->setVisible(true);
		label_10->setVisible(false);
		label_11->setVisible(false);
		minIntensity->setVisible(false);
		maxIntensity->setVisible(false);

	} else {
		selectDatabase->setVisible(false);
		selectDatabaseComboBox->setVisible(false);
		label_10->setVisible(true);
		label_11->setVisible(true);
		minIntensity->setVisible(true);
		maxIntensity->setVisible(true);
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
