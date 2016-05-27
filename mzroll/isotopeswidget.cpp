#include "isotopeswidget.h"
using namespace std;

IsotopeWidget::IsotopeWidget(MainWindow* mw) {
	_mw = mw;
	_scan = NULL;
	_group = NULL;
	_compound = NULL;
	_charge = -1;

	tempCompound = new Compound("Unknown", "Unknown", string(), 0);	//temp compound

	setupUi(this);
	connect(treeWidget, SIGNAL(itemSelectionChanged()), SLOT(showInfo()));
	connect(formula, SIGNAL(textEdited(QString)), this,
			SLOT(userChangedFormula(QString)));
	connect(ionization, SIGNAL(valueChanged(double)), this,
			SLOT(setCharge(double)));

	ionization->setValue(_charge);

	workerThread = new BackgroundPeakUpdate(mw);
	workerThread->setRunFunction("pullIsotopes");
	workerThread->setMainWindow(mw);

	PeakDetector pd = workerThread->getPeakDetector();

	pd.minGoodPeakCount = 1;
	pd.minSignalBlankRatio = 2;
	pd.minSignalBaseLineRatio = 2;
	pd.minNoNoiseObs = 2;
	pd.minGroupIntensity = 0;
	pd.writePdfReportFlag = false;
	pd.writeCSVFlag = false;
	pd.matchRtFlag = false;
	pd.showProgressFlag = true;
	pd.pullIsotopesFlag = true;
	pd.keepFoundGroups = true;

	connect(workerThread, SIGNAL(finished()), this, SLOT(setClipboard()));
	connect(workerThread, SIGNAL(finished()), mw->getEicWidget()->scene(),
			SLOT(update()));

}

IsotopeWidget::~IsotopeWidget() {
	links.clear();
}

void IsotopeWidget::setPeakGroup(PeakGroup* grp) {
	if (!grp)
		return;

	_group = grp;
	if (grp && grp->type() != PeakGroup::Isotope)
		pullIsotopes(grp);
}

void IsotopeWidget::setPeak(Peak* peak) {
	if (peak == NULL)
		return;

	mzSample* sample = peak->getSample();
	if (sample == NULL)
		return;

	Scan* scan = sample->getScan(peak->scan);
	if (scan == NULL)
		return;
	_scan = scan;

	if (!_formula.empty())
		computeIsotopes(_formula);
}

void IsotopeWidget::setCompound(Compound* cpd) {
	if (cpd == NULL)
		return;
	QString f = QString(cpd->formula.c_str());
	_group = NULL;
	_compound = cpd;
	setWindowTitle("Isotopes:" + QString(cpd->name.c_str()));
	setFormula(f);
}

void IsotopeWidget::setIonizationMode(int mode) {
	setCharge((float) mode);
}

void IsotopeWidget::userChangedFormula(QString f) {
	if (f.toStdString() == _formula)
		return;

	_formula = f.toStdString();
	_group = NULL;

	tempCompound->formula = _formula;
	tempCompound->name = "Unknown_" + _formula;
	tempCompound->id = "unknown";
	_compound = tempCompound;

	setWindowTitle("Unknown_" + f);
	computeIsotopes(_formula);

}

void IsotopeWidget::setFormula(QString f) {
	formula->setText(f);
	userChangedFormula(f);
}

void IsotopeWidget::setCharge(double charge) {
	if (charge != _charge) {
		ionization->setValue(charge);
		_charge = charge;
		computeIsotopes(_formula);
	}
}

void IsotopeWidget::computeIsotopes(string f) {
	if (f.empty())
		return;
	if (links.size() > 0)
		links.clear();
	double parentMass = mcalc.computeMass(f, _charge);
	float parentPeakIntensity = getIsotopeIntensity(parentMass);
	QSettings* settings = _mw->getSettings();

	double maxIsotopeScanDiff =
			settings->value("maxIsotopeScanDiff").toDouble();
	double minIsotopicCorrelation =
			settings->value("minIsotopicCorrelation").toDouble();
	double maxNaturalAbundanceErr =
			settings->value("maxNaturalAbundanceErr").toDouble();
	bool C13Labeled = settings->value("C13Labeled").toBool();
	bool N15Labeled = settings->value("N15Labeled").toBool();
	bool S34Labeled = settings->value("S34Labeled").toBool();
	bool D2Labeled = settings->value("D2Labeled").toBool();

	vector<Isotope> isotopes = mcalc.computeIsotopes(f, _charge);
	for (int i = 0; i < isotopes.size(); i++) {
		Isotope& x = isotopes[i];

		float expectedAbundance = x.abundance;

		mzLink link;
		if ((x.C13 > 0 && C13Labeled == false)
				|| (x.N15 > 0 && N15Labeled == false)
				|| (x.S34 > 0 && S34Labeled == false)
				|| (x.H2 > 0 && D2Labeled == false)) {

			if (expectedAbundance < 1e-8)
				continue;
			// if (expectedAbundance * parentPeakIntensity < 500) continue;
			float isotopePeakIntensity = getIsotopeIntensity(x.mass);
			float observedAbundance = isotopePeakIntensity
					/ (parentPeakIntensity + isotopePeakIntensity);
			float naturalAbundanceError = abs(
					observedAbundance - expectedAbundance) / expectedAbundance
					* 100;

			if (naturalAbundanceError > maxNaturalAbundanceErr)
				continue;
		}
		link.mz1 = parentMass;
		link.mz2 = x.mass;
		link.note = x.name;
		link.value1 = x.abundance;
		link.value2 = getIsotopeIntensity(x.mass);
		links.push_back(link);
	}

	sort(links.begin(), links.end(), mzLink::compMz);
	showTable();
}

float IsotopeWidget::getIsotopeIntensity(float mz) {
	float highestIntensity = 0;
	double ppm = _mw->getUserPPM();

	if (_scan == NULL)
		return 0;
	mzSample* sample = _scan->getSample();
	if (sample == NULL)
		return 0;

	for (int i = _scan->scannum - 2; i < _scan->scannum + 2; i++) {
		Scan* s = sample->getScan(i);
		vector<int> matches = s->findMatchingMzs(mz - mz / 1e6 * ppm,
				mz + mz / 1e6 * ppm);
		for (int i = 0; i < matches.size(); i++) {
			int pos = matches[i];
			if (s->intensity[pos] > highestIntensity)
				highestIntensity = s->intensity[pos];
		}
	}
	return highestIntensity;
}

Peak* IsotopeWidget::getSamplePeak(PeakGroup* group, mzSample* sample) {
	for (int i = 0; i < group->peaks.size(); i++) {
		if (group->peaks[i].getSample() == sample) {
			return &(group->peaks[i]);
		}
	}
	return NULL;
}
void IsotopeWidget::pullIsotopes(PeakGroup* group) {
	if (!group)
		return;

	//clear clipboard
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->clear(QClipboard::Clipboard);

	setClipboard(group);

	if (group->compound == NULL) {
		_mw->setStatusText(
				tr("Unknown compound. Clipboard set to %1").arg(
						group->tagString.c_str()));
		return;
	}

	int isotopeCount = 0;
	for (int i = 0; i < group->children.size(); i++) {
		if (group->children[i].isIsotope())
			isotopeCount++;
	}

	vector<mzSample*> vsamples = _mw->getVisibleSamples();
	PeakDetector pd = workerThread->getPeakDetector();

	workerThread->stop();
	pd.setPeakGroup(group);
	pd.setSamples(vsamples);
	pd.compoundPPMWindow = _mw->getUserPPM();
	workerThread->start();
	_mw->setStatusText("IsotopeWidget:: pullIsotopes(() started");
}

void IsotopeWidget::setClipboard() {
	if (_group) {

		//update clipboard
		setClipboard(_group);

		//update eic widget
		_mw->getEicWidget()->setSelectedGroup(_group);

		//update gallery widget
		if (_mw->galleryDockWidget->isVisible()) {
			vector<PeakGroup*> isotopes;
			for (int i = 0; i < _group->children.size(); i++) {
				if (_group->children[i].isIsotope())
					isotopes.push_back(&_group->children[i]);
			}
			_mw->galleryWidget->clear();
			_mw->galleryWidget->addEicPlots(isotopes);
		}
	}
}

void IsotopeWidget::setClipboard(PeakGroup* group) {
	if (group == NULL)
		return;

	//header line
	QString tag(group->tagString.c_str());
	if (tag.isEmpty() && group->compound != NULL)
		tag = QString(group->compound->name.c_str());
	if (tag.isEmpty() && group->srmId.length())
		tag = QString(group->srmId.c_str());
	if (tag.isEmpty())
		tag = QString::number(group->groupId);
	QString isotopeInfo = tag;

	vector<mzSample*> vsamples = _mw->getVisibleSamples();
	sort(vsamples.begin(), vsamples.end(), mzSample::compSampleOrder);
	for (int i = 0; i < vsamples.size(); i++) {
		isotopeInfo += "\t" + QString(vsamples[i]->sampleName.c_str());
	}
	isotopeInfo += "\n";

	//get isotopic groups
	vector<PeakGroup*> isotopes;
	for (int i = 0; i < group->childCount(); i++) {
		if (group->children[i].isIsotope()) {
			PeakGroup* isotope = &(group->children[i]);
			isotopes.push_back(isotope);
		}
	}
	std::sort(isotopes.begin(), isotopes.end(), PeakGroup::compC13);

	if (isotopes.size() > 0) {
		MatrixXf MM = _mw->getIsotopicMatrix(group);
		///qDebug() << "MM row=" << MM.rows() << " " << MM.cols() << " " << isotopes.size() <<  " " << vsamples.size() << endl;
		for (int i = 0; i < isotopes.size(); i++) {
			QStringList groupInfo;
			groupInfo << QString(isotopes[i]->tagString.c_str());
			for (unsigned int j = 0; j < vsamples.size(); j++) {
				//qDebug() << i << " " << j << " " << MM(j,i);
				groupInfo << QString::number(MM(j, i), 'f', 2);
			}
			isotopeInfo += groupInfo.join("\t") + "\n";
		}
		_mw->setStatusText("Clipboard set to isotope summary");
	} else {
		isotopeInfo += groupTextEport(group) + "\n";
		_mw->setStatusText("Clipboard to group summary");
	}

	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(isotopeInfo);
}

void IsotopeWidget::showTable() {
	QTreeWidget *p = treeWidget;
	p->clear();
	p->setColumnCount(5);
	p->setHeaderLabels(
			QStringList() << "Isotope Name" << "m/z" << "Intensity"
					<< "%Labeling" << "%Expected");
	p->setUpdatesEnabled(true);
	p->setSortingEnabled(true);

	float isotopeIntensitySum = 0;
	for (unsigned int i = 0; i < links.size(); i++)
		isotopeIntensitySum += links[i].value2;

	for (unsigned int i = 0; i < links.size(); i++) {
		float frac = 0;
		if (isotopeIntensitySum > 0)
			frac = links[i].value2 / isotopeIntensitySum * 100;

		NumericTreeWidgetItem *item = new NumericTreeWidgetItem(treeWidget,
				mzSliceType);
		QString item1 = QString(links[i].note.c_str());
		QString item2 = QString::number(links[i].mz2, 'f', 5);
		QString item3 = QString::number(links[i].value2, 'f', 2);
		QString item4 = QString::number(frac, 'f', 1);
		QString item5 = QString::number(links[i].value1 * 100, 'f', 2);

		item->setText(0, item1);
		item->setText(1, item2);
		item->setText(2, item3);
		item->setText(3, item4);
		item->setText(4, item5);
	}
	p->resizeColumnToContents(0);
	p->setSortingEnabled(true);
	p->sortByColumn(1, Qt::AscendingOrder);
	p->setUpdatesEnabled(true);
}

void IsotopeWidget::showInfo() {
	QList<QTreeWidgetItem *> selectedItems = treeWidget->selectedItems();
	if (selectedItems.size() < 1)
		return;
	QTreeWidgetItem* item = selectedItems[0];
	if (!item)
		return;

	QString __mz = item->text(1);
	float mz = __mz.toDouble();

	if (mz > 0) {
		double ppm = _mw->getUserPPM();
		mzSlice slice = _mw->getEicWidget()->getMzSlice();
		slice.mzmin = mz - mz / 1e6 * ppm;
		slice.mzmax = mz + mz / 1e6 * ppm;
		if (_compound)
			slice.compound = _compound;
		if (!_compound)
			slice.compound = tempCompound;
		_mw->getEicWidget()->setMzSlice(slice);
	}
}

QString IsotopeWidget::groupTextEport(PeakGroup* group) {

	if (group == NULL)
		return "";
	QStringList info;

	PeakGroup::QType qtype = _mw->getUserQuantType();
	vector<mzSample*> vsamples = _mw->getVisibleSamples();
	sort(vsamples.begin(), vsamples.end(), mzSample::compSampleOrder);
	vector<float> yvalues = group->getOrderedIntensityVector(vsamples, qtype);

	info << QString(group->tagString.c_str());
	for (unsigned int j = 0; j < vsamples.size(); j++) {
		mzSample* s = vsamples[j];
		info << QString::number(yvalues[j], 'f', 2);
	}
	return info.join("\t");
}
