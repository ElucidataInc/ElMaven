#include "comparesamplesdialog.h"

CompareSamplesDialog::CompareSamplesDialog(QWidget *parent) :
		QDialog(parent) {
	setupUi(this);
	table = NULL;
	setModal(false);
	_qtype = PeakGroup::AreaTop;

	CompareSamplesLogic* c = new CompareSamplesLogic();
	compareLogic = *c;

	connect(compareButton, SIGNAL(clicked(bool)), SLOT(compareSamples()));
	connect(resetButton, SIGNAL(clicked(bool)), SLOT(resetSamples()));
	connect(cancelButton, SIGNAL(clicked(bool)), SLOT(cancel()));
	connect(filelist1, SIGNAL(itemSelectionChanged()),
			SLOT(updateSampleList()));
	connect(filelist2, SIGNAL(itemSelectionChanged()),
			SLOT(updateSampleList()));
	/*

	 float A[] = { 368, 390, 379, 260, 404, 318, 352, 359, 216, 222, 283, 332 };
	 float B[] = { 423, 340, 392, 339, 341, 226, 320, 295, 334, 322, 297, 318 };
	 int n1=12; int n2=12;
	 StatisticsVector<float> setA(n1);
	 StatisticsVector<float> setB(n2);
	 for(int i=0; i < n1;  i++ ) setA[i]=A[i];
	 for(int i=0; i < n2;  i++ ) setB[i]=B[i];
	 shuffle(setA,setB);
	 */
}

CompareSamplesDialog::~CompareSamplesDialog() {
}

void CompareSamplesDialog::cancel() {
	QDialog::close();
}

void CompareSamplesDialog::setTableWidget(TableDockWidget* w) {
	if (!w)
		return;
	table = w;
	samples.clear();
	QList<PeakGroup*> allgroups = table->getGroups();
	Q_FOREACH (PeakGroup* group, allgroups){
	for(int i=0; i < group->peakCount(); i++ ) {
		mzSample* sample = group->peaks[i].getSample();
		if (sample) samples.insert(sample);
	}
}
	qDebug() << " Load Samples: " << samples.size() << endl;
}

void CompareSamplesDialog::resetSamples() {
	filelist1->clear();
	filelist2->clear();
}

void CompareSamplesDialog::updateSampleList() {

	vector<mzSample*> sset1 = getSampleSet(filelist1);
	vector<mzSample*> sset2 = getSampleSet(filelist2);
	fileCount1->setText(tr("%1 samples selected").arg(sset1.size()));
	fileCount2->setText(tr("%1 samples selected").arg(sset2.size()));

	/*
	 QString setName1 = set1->currentText();
	 QString setName2 = set2->currentText();
	 filelist1->clear();
	 filelist2->clear();

	 Q_FOREACH(mzSample* sample, samples ) {
	 QString setname(sample->getSetName().c_str());
	 QString sampleName(sample->sampleName.c_str());
	 //if( setname.contains(setName1) ) filelist1->addItem(sampleName);
	 //if( setname.contains(setName2) ) filelist2->addItem(sampleName);
	 }
	 */
}

void CompareSamplesDialog::showEvent(QShowEvent *) {
	if (!table || table->groupCount() == 0)
		return;

	filelist1->clear();
	filelist2->clear();

	QSet<QString> setnames;
    //TODO: Expression updated to ";", don't know why - Kiran
	QRegExp splitStr(";");

	Q_FOREACH(mzSample* sample, samples ){
		QString qname( sample->getSetName().c_str());
		qname=qname.simplified();
		QList<QString> names = qname.split(splitStr);
		if(!qname.isEmpty() && qname != "") setnames.insert(qname);
		Q_FOREACH(QString name, names) {name=name.simplified(); if(!name.isEmpty() && name != "") setnames.insert(name);}
	}

	Q_FOREACH(QString name, setnames ){
	filelist1->addItem(name);
	filelist2->addItem(name);
}

	if (setnames.size() >= 2) {
		//filelist1->setCurrentIndex(0);
		//filelist2->setCurrentIndex(1);
	} else {
		QMessageBox msgBox;
		msgBox.setText(
				"Sets were not defined. Please map samples to sets in  the Samples Widget");
		msgBox.exec();
		return;
	}
}

vector<mzSample*> CompareSamplesDialog::getSampleSet(QListWidget *set) {
	vector<mzSample*> subset;

	Q_FOREACH(mzSample* sample, samples ){
	QString sampleName( sample->getSetName().c_str());

	for(int j=0; j < set->count(); j++ ) {
		if (set->item(j)->isSelected() == false) continue;

		QString setName=set->item(j)->text();
		if ( sampleName.contains(setName) ) {
			qDebug() << setName << " sample:" << sampleName;
			subset.push_back(sample);
			break;
		}
	}
}
	return subset;
}

void CompareSamplesDialog::compareSamples() {
	if (!table)
		return;

	vector<mzSample*> sset1 = getSampleSet(filelist1);
	vector<mzSample*> sset2 = getSampleSet(filelist2);
	if (sset1.size() == 0 || sset2.size() == 0)
		return;
	compareSets(sset1, sset2);
}

void CompareSamplesDialog::compareSets(vector<mzSample*> sset1,
		vector<mzSample*> sset2) {
	if (!table)
		return;

	vector<mzSample*> sampleSet;
	for (int i = 0; i < sset1.size(); i++)
		sampleSet.push_back(sset1[i]);
	for (int i = 0; i < sset2.size(); i++)
		sampleSet.push_back(sset2[i]);

	QList<PeakGroup*> allgroups = table->getGroups();
	compareLogic.rand_scores.clear();

	for (int i = 0; i < allgroups.size(); i++) {

		//replace missing values
		float _missingValue = missingValue->value();

		PeakGroup* group = allgroups[i];
		compareLogic.computeStats(group, sampleSet, sset1, sset2,
				_missingValue);

		//qDebug() << "CompareSamplesDialog: " << i << " " << meanA << " " << meanB;
		Q_EMIT(setProgressBar("CompareSamples", i + 1, allgroups.size()));
	}

	float alpha = minPValue->value(); //alpha value //TODO: Alpha value is not being used

	//calculate Pvalues
	compareLogic.computeMinPValue(allgroups);

	//correct P-values (FDR)
	int correction = correctionBox->currentIndex();
	compareLogic.FDRCorrection(allgroups, correction);

	//if (table) { table->updateTable();}
	if (parentWidget())
		((ScatterPlot*) parentWidget())->replot();

	//show results
	/*
	 TableDockWidget* peaksTable = mainwindow->addPeaksTable("Contrasts");
	 peaksTable->setWindowTitle("Contrasts: Peaks");
	 peaksTable->treeWidget->setSortingEnabled(true);
	 for(int i=0; i < goodgroups.size(); i++) {
	 if (goodgroups[i]->changeFoldRatio > _minFoldDiff && goodgroups[i]->changePValue < alpha) { peaksTable->addPeakGroup(goodgroups[i]); }
	 }
	 peaksTable->showAllGroups();
	 */

	//cleanup
	compareLogic.rand_scores.clear();
	allgroups.clear();
}
