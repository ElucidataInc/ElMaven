#include "isotopeswidget.h"
using namespace std;

IsotopeWidget::IsotopeWidget(MainWindow* mw) {
	_mw = mw;

	isotopeParameters = new IsotopeLogic();

	setupUi(this);
	connect(treeWidget, SIGNAL(itemSelectionChanged()), SLOT(showInfo()));
	connect(formula, SIGNAL(textEdited(QString)), this,
			SLOT(userChangedFormula(QString)));
	connect(ionization, SIGNAL(valueChanged(double)), this,
			SLOT(setCharge(double)));

	ionization->setValue(isotopeParameters->_charge);

	workerThread = new BackgroundPeakUpdate(mw);
	workerThread->setRunFunction("pullIsotopes");
	workerThread->setMainWindow(mw);

	MavenParameters* mavenParameters = mw->mavenParameters;
	mavenParameters->minGoodGroupCount = 1;
	mavenParameters->minSignalBlankRatio = 2;
	mavenParameters->minSignalBaseLineRatio = 2;
	mavenParameters->minNoNoiseObs = 2;
	mavenParameters->minGroupIntensity = 0;
	mavenParameters->writeCSVFlag = false;
	mavenParameters->matchRtFlag = false;
	mavenParameters->showProgressFlag = true;
	mavenParameters->pullIsotopesFlag = true;
	mavenParameters->keepFoundGroups = true;

	workerThread->setMavenParameters(mavenParameters);
	workerThread->setPeakDetector(new PeakDetector(mavenParameters));

	connect(workerThread, SIGNAL(finished()), this, SLOT(setClipboard()));
	connect(workerThread, SIGNAL(finished()), mw->getEicWidget()->scene(),
			SLOT(update()));

}

IsotopeWidget::~IsotopeWidget() {
	isotopeParameters->links.clear();
}

void IsotopeWidget::setPeakGroup(PeakGroup* grp) {
	if (!grp)
		return;

	isotopeParameters->_group = grp;
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
	isotopeParameters->_scan = scan;

	if (!isotopeParameters->_formula.empty())
		computeIsotopes(isotopeParameters->_formula);
}

void IsotopeWidget::setCompound(Compound* cpd) {
	if (cpd == NULL)
		return;
	QString f = QString(cpd->formula.c_str());
	isotopeParameters->_group = NULL;
	isotopeParameters->_compound = cpd;
	setWindowTitle("Isotopes:" + QString(cpd->name.c_str()));
	setFormula(f);
}

void IsotopeWidget::setIonizationMode(int mode) {
	setCharge((float) mode);
}

void IsotopeWidget::userChangedFormula(QString f) {
	if (f.toStdString() == isotopeParameters->_formula)
		return;

	isotopeParameters->_formula = f.toStdString();
	isotopeParameters->userChangedFormula();

	setWindowTitle("Unknown_" + f);
	computeIsotopes(isotopeParameters->_formula);

}

void IsotopeWidget::setFormula(QString f) {
	formula->setText(f);
	userChangedFormula(f);
}

void IsotopeWidget::setCharge(double charge) {
	if (charge != isotopeParameters->_charge) {
		ionization->setValue(charge);
		isotopeParameters->_charge = charge;
		computeIsotopes(isotopeParameters->_formula);
	}
}

void IsotopeWidget::computeIsotopes(string f) {
	if (f.empty())
		return;
	if (isotopeParameters->links.size() > 0)
		isotopeParameters->links.clear();

	QSettings* settings = _mw->getSettings();
	double ppm = _mw->getUserPPM();

	//N TODO:remove unneeded settings
	double maxIsotopeScanDiff =
			settings->value("maxIsotopeScanDiff").toDouble();
	double minIsotopicCorrelation =
			settings->value("minIsotopicCorrelation").toDouble();
	double maxNaturalAbundanceErr =
			settings->value("maxNaturalAbundanceErr").toDouble();
	bool C13Labeled = settings->value("C13Labeled_BPE").toBool();
	bool N15Labeled = settings->value("N15Labeled_BPE").toBool();
	bool S34Labeled = settings->value("S34Labeled_BPE").toBool();
	bool D2Labeled = settings->value("D2Labeled_BPE").toBool();

	isotopeParameters->computeIsotopes(f, ppm, maxNaturalAbundanceErr,
			C13Labeled, N15Labeled, S34Labeled, D2Labeled);
	showTable();
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
	workerThread->stop();

	MavenParameters* mavenParameters =
			workerThread->peakDetector.getMavenParameters();
	mavenParameters->setPeakGroup(group);
	mavenParameters->setSamples(vsamples);
	mavenParameters->compoundPPMWindow = _mw->getUserPPM();
	if (_mw->getIonizationMode()) {
    	mavenParameters->ionizationMode = _mw->getIonizationMode();
    } else {
    	mavenParameters->setIonizationMode();
    }
	workerThread->start();
	_mw->setStatusText("IsotopeWidget:: pullIsotopes(() started");
}

void IsotopeWidget::setClipboard() {
	if (isotopeParameters->_group) {
		
		//update clipboard
		setClipboard(isotopeParameters->_group);

		//update eic widget
		_mw->getEicWidget()->setSelectedGroup(isotopeParameters->_group);

		/*
		 //update gallery widget
		 if (_mw->galleryDockWidget->isVisible() ) {
		 vector<PeakGroup*>isotopes;
		 for (int i=0; i < _group->children.size(); i++ ) {
		 if ( _group->children[i].isIsotope() ) isotopes.push_back(&_group->children[i]);
		 }
		 _mw->galleryWidget->clear();
		 _mw->galleryWidget->addEicPlots(isotopes);
		 }
		 */
	}
}

void IsotopeWidget::setClipboard(QList<PeakGroup*>& groups) {
	QString clipboardText;

    unsigned int groupCount=0;
    bool includeSampleHeader=true;
    Q_FOREACH(PeakGroup* group, groups) {
        if(!group) continue;
        if (groupCount>0) includeSampleHeader=false;
         QString infoText = groupIsotopeMatrixExport(group,includeSampleHeader);
         clipboardText += infoText;
         groupCount++;
    }
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText( clipboardText );
}

void IsotopeWidget::setClipboard(PeakGroup* group) {
    bool includeSampleHeader=true;
    QString infoText = groupIsotopeMatrixExport(group,includeSampleHeader);
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText( infoText );
}

QString IsotopeWidget::groupIsotopeMatrixExport(PeakGroup* group, bool includeSampleHeader) {
    if (group == NULL ) return "";
		//header line
		QString tag(group->tagString.c_str());
        if ( tag.isEmpty() && group->compound != NULL ) tag = QString(group->compound->name.c_str());
        if ( tag.isEmpty() && group->srmId.length()   ) tag = QString(group->srmId.c_str());
        if ( tag.isEmpty() && group->meanMz > 0 )       tag = QString::number(group->meanMz,'f',6) + "@" +  QString::number(group->meanRt,'f',2);
        if ( tag.isEmpty() )  tag = QString::number(group->groupId);
        QString isotopeInfo;

       vector <mzSample*> vsamples = _mw->getVisibleSamples();
       sort(vsamples.begin(), vsamples.end(), mzSample::compSampleOrder);

        //include header
       if (includeSampleHeader) {
           for(int i=0; i < vsamples.size(); i++ ) {
               isotopeInfo += "\t" + QString(vsamples[i]->sampleName.c_str());
           }
           isotopeInfo += "\n";


           bool includeSetNamesLine=true;
           if (includeSetNamesLine){
               for(int i=0; i < vsamples.size(); i++ ) {
                   isotopeInfo += "\t" + QString(vsamples[i]->getSetName().c_str());
               }
               isotopeInfo += "\n";
           }
       }

       //get isotopic groups
		vector<PeakGroup*>isotopes;
		for(int i=0; i < group->childCount(); i++ ) {
			if (group->children[i].isIsotope() ) {
                PeakGroup* isotope = &(group->children[i]);
                isotopes.push_back(isotope);
			}
		}
		std::sort(isotopes.begin(), isotopes.end(), PeakGroup::compC13);

		if (isotopes.size() > 0 ) {
				MatrixXf MM = _mw->getIsotopicMatrix(group);
				///qDebug() << "MM row=" << MM.rows() << " " << MM.cols() << " " << isotopes.size() <<  " " << vsamples.size() << endl;
				for (int i=0; i < isotopes.size(); i++ ) {
						QStringList groupInfo;
                        groupInfo << tag + " " + QString(isotopes[i]->tagString.c_str());
						for( unsigned int j=0; j < vsamples.size(); j++) {
								//qDebug() << i << " " << j << " " << MM(j,i);
								groupInfo << QString::number(MM(j,i), 'f', 2 );
						}
						isotopeInfo += groupInfo.join("\t") + "\n";
				}
				_mw->setStatusText("Clipboard set to isotope summary");
		} else {
                isotopeInfo += tag + groupTextEport(group) + "\n";
				_mw->setStatusText("Clipboard to group summary");
		}
        return isotopeInfo;

}

void IsotopeWidget::showTable() {
	QTreeWidget *p = treeWidget;
	p->clear();
    p->setColumnCount( 6);
    p->setHeaderLabels(  QStringList() << "Isotope Name" << "m/z" << "Intensity" << "%Labeling" << "%Expected" << "%Relative");
	p->setUpdatesEnabled(true);
	p->setSortingEnabled(true);

    double isotopeIntensitySum=0;
    double maxIntensity=0;
	for (unsigned int i = 0; i < isotopeParameters->links.size(); i++) {
		isotopeIntensitySum += isotopeParameters->links[i].value2;
        if( isotopeParameters->links[i].value1 > maxIntensity) maxIntensity= isotopeParameters->links[i].value1;
    }

	for (unsigned int i = 0; i < isotopeParameters->links.size(); i++) {
		float frac = 0;
		if (isotopeIntensitySum > 0)
			frac = isotopeParameters->links[i].value2 / isotopeIntensitySum
					* 100;

		NumericTreeWidgetItem *item = new NumericTreeWidgetItem(treeWidget,
				mzSliceType);
		QString item1 = QString(isotopeParameters->links[i].note.c_str());
		QString item2 = QString::number(isotopeParameters->links[i].mz2, 'f',
				5);
		QString item3 = QString::number(isotopeParameters->links[i].value2, 'f',
				4);
		QString item4 = QString::number(frac, 'f', 4);
		QString item5 = QString::number(
				isotopeParameters->links[i].value1 * 100, 'f', 4);
        QString item6 = QString::number(isotopeParameters->links[i].value1/maxIntensity*100, 'f', 4 );

		item->setText(0, item1);
		item->setText(1, item2);
		item->setText(2, item3);
		item->setText(3, item4);
		item->setText(4, item5);
		item->setText(5,item6);
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
		mzSlice slice = _mw->getEicWidget()->getParameters()->getMzSlice();
		slice.mzmin = mz - mz / 1e6 * ppm;
		slice.mzmax = mz + mz / 1e6 * ppm;
		if (isotopeParameters->_compound)
			slice.compound = isotopeParameters->_compound;
		if (!isotopeParameters->_compound)
			slice.compound = isotopeParameters->tempCompound;
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
