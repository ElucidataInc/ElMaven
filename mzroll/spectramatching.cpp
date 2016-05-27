#include "spectramatching.h"

SpectraMatching::SpectraMatching(MainWindow *w) :
		QDialog(w) {
	setupUi(this);
	mainwindow = w;
	connect(resultTable, SIGNAL(itemSelectionChanged()), SLOT(showScan()));
	connect(findButton, SIGNAL(clicked(bool)), SLOT(findMatches()));
}

void SpectraMatching::findMatches() {
	getFormValues();
	doSearch();
	/*
	 //update isotope plot in EICview
	 if (mainwindow->getEicWidget()->isVisible()) {
	 PeakGroup* group =mainwindow->getEicWidget()->getSelectedGroup();
	 cerr << "recomputeIsotopes() " << group << endl;
	 mainwindow->isotopeWidget->setPeakGroup(group);
	 }

	 //update isotopes in pathwayview
	 if (mainwindow->pathwayWidget ) {
	 if ( mainwindow->pathwayWidget->isVisible()) {
	 mainwindow->pathwayWidget->recalculateConcentrations();
	 }
	 }
	 */
}

void SpectraMatching::getFormValues() {
	qDebug() << "SpectraMatching::getFormValues() ";

	//get precursor mass
	_precursorMz = this->precursorMz->text().toDouble();

	//get scan type
	_msScanType = 0;
	QString scanType = this->scanTypeComboBox->currentText();
	if (scanType != "any")
		_msScanType = scanType.mid(2, 1).toInt();

	//parse fragmentation mz. intensity pairs
	QString mzpairs = this->fragmentsText->toPlainText();
	_mzsList.clear();
	_intensityList.clear();

	QRegExp SPACE("\\s+");
	QRegExp COMMA("[;|,|\n|\r]");
	if (mzpairs.contains(COMMA)) {
		QStringList mz_ints_pairs = mzpairs.split(COMMA);
		foreach (QString mzint, mz_ints_pairs){
		mzint = mzint.simplified();
		qDebug() << "Pair:" << mzint;

		QStringList _mz_int = mzint.split(SPACE);
		if (_mz_int.size() == 2) {
			float mz = _mz_int[0].simplified().toDouble();
			float ints = _mz_int[1].simplified().toDouble();
			if (mz > 0 && ints > 0 ) {
				_mzsList << mz;
				_intensityList << ints;
			}
		} else if (_mz_int.size() == 1) {
			float mz = _mz_int[0].simplified().toDouble();
			if (mz > 0) {
				_mzsList << mz;
				_intensityList << 0.0;
			}
		}
	}
} else {
	QStringList mzs = mzpairs.split(SPACE);
	foreach (QString mzstr, mzs) {
		float mz = mzstr.simplified().toDouble();
		if (mz > 0) _mzsList << mz;
	}
}

	qDebug() << _msScanType;
	qDebug() << _precursorMz;
	qDebug() << _mzsList;
	qDebug() << _intensityList;

}

void SpectraMatching::showScan() {
	qDebug() << "showScan()";
	foreach(QTreeWidgetItem* item, resultTable->selectedItems() ){
	QVariant v = item->data(0,Qt::UserRole);
	Scan* scan = v.value<Scan*>();
	if (scan) {
		mainwindow->getSpectraWidget()->setScan(scan);
		mainwindow->getSpectraWidget()->overlaySpectra(_mzsList,_intensityList);
	}
	break;
}
}

void SpectraMatching::doSearch() {
	resultTable->clear();
	vector<mzSample*> samples = mainwindow->getVisibleSamples();
	for (int i = 0; i < samples.size(); i++) {
		int nscans = samples[i]->scanCount();
		for (int j = 0; j < nscans; j++) {
			Scan* scan = samples[i]->scans[j];
			double score = scoreScan(scan);
			if (score) {
				NumericTreeWidgetItem *item = new NumericTreeWidgetItem(
						resultTable, 0);
				item->setText(0, QString::number(scan->scannum));
				item->setText(1, QString::number(score, 'f', 2));
				item->setData(0, Qt::UserRole, QVariant::fromValue(scan));
			}
		}
	}
	resultTable->sortItems(1, Qt::DescendingOrder);
}

double SpectraMatching::scoreScan(Scan* scan) {

	if (_msScanType > 0 && scan->mslevel != _msScanType)
		return 0;

	if (_precursorMz > 0
			&& mzUtils::ppmDist(_precursorMz, (double) scan->precursorMz) > 100)
		return 0;

	double score = 0;
	int N = _mzsList.size();
	int Nc = _intensityList.size();

	for (int i = 0; i < N; i++) {
		if (scan->hasMz(_mzsList[i], 100)) {
			score++;
		}
	}
	return score;
}
