#include "background_peaks_update.h"
#include "Compound.h"
#include "datastructures/mzSlice.h"
#include "eiclogic.h"
#include "eicwidget.h"
#include "globals.h"
#include "isotopeDetection.h"
#include "isotopelogic.h"
#include "isotopeplot.h"
#include "isotopeswidget.h"
#include "mainwindow.h"
#include "masscutofftype.h"
#include "mavenparameters.h"
#include "mzSample.h"
#include "numeric_treewidgetitem.h"
#include "PeakDetector.h"
#include "Scan.h"

using namespace std;

IsotopeWidget::IsotopeWidget(MainWindow *mw)
{
	_mw = mw;

	isotopeParameters = new IsotopeLogic();
	isotopeParametersBarPlot = new IsotopeLogic();

	setupUi(this);
	connect(treeWidget, SIGNAL(itemSelectionChanged()), SLOT(showInfo()));
	connect(formula, SIGNAL(textEdited(QString)), this,
			SLOT(userChangedFormula(QString)));
	connect(ionization, SIGNAL(valueChanged(double)), this,
			SLOT(setCharge(double)));
	connect(sampleList, SIGNAL(currentIndexChanged(int)),
			SLOT(updateSelectedSample(int)));

	ionization->setValue(isotopeParameters->_charge);
	bookmarkflag = true;

	MavenParameters *mavenParameters = mw->mavenParameters;
	bool C13Flag = mavenParameters->C13Labeled_BPE;
	bool N15Flag = mavenParameters->N15Labeled_BPE;
	bool S34Flag = mavenParameters->S34Labeled_BPE;
	bool D2Flag = mavenParameters->D2Labeled_BPE;
	IsotopeDetection::IsotopeDetectionType isoType;
	isoType = IsotopeDetection::IsoWidget;
	isotopeDetector = new IsotopeDetection(mavenParameters, 
		isoType, 
		C13Flag, 
		N15Flag, 
		S34Flag, 
		D2Flag);

	workerThread = new BackgroundPeakUpdate(mw);
	workerThread->setRunFunction("pullIsotopes");
	workerThread->setMainWindow(mw);

	workerThread->setMavenParameters(mavenParameters);
	workerThread->setPeakDetector(new PeakDetector(mavenParameters));

	connect(workerThread, SIGNAL(finished()), this, SLOT(setClipboard()));
	connect(workerThread, SIGNAL(finished()), _mw->getEicWidget()->scene(), SLOT(update()));

	//Thread for bar plot
	workerThreadBarplot = new BackgroundPeakUpdate(mw);
	workerThreadBarplot->setRunFunction("pullIsotopesBarPlot");
	workerThreadBarplot->setMainWindow(mw);
	workerThreadBarplot->setMavenParameters(mavenParameters);
	workerThreadBarplot->setPeakDetector(new PeakDetector(mavenParameters));

	connect(workerThreadBarplot, SIGNAL(finished()), this, SLOT(updateIsotopicBarplot()));
	connect(workerThreadBarplot, SIGNAL(finished()), _mw->getEicWidget()->scene(), SLOT(update()));
}

IsotopeWidget::~IsotopeWidget()
{
	isotopeParameters->links.clear();
	isotopeParametersBarPlot->links.clear();
}

void IsotopeWidget::clearWidget()
{
	isotopeParameters->links.clear();
	showTable();
}

void IsotopeWidget::peakSelected(Peak *peak, PeakGroup *group)
{
	clearWidget();
	if (!(peak && group && group->getCompound()))
		return;

    //set selectedSample for isotope calculation
    _selectedSample = peak->getSample();
    setPeakGroupAndMore(group);

    sampleList->setCurrentText(QString::fromStdString(_selectedSample->sampleName));
}

void IsotopeWidget::setPeakGroupAndMore(PeakGroup *grp, bool bookmarkflg)
{
	clearWidget();

	if (!grp)
		return;

	//set compound, formula, window title
	setCompound(grp->getCompound());

	isotopeParameters->_group = grp;
	if (grp->isIsotope())
	{
		isotopeParameters->_group = grp->parent;
	}

	//TODO: move bookmarking functionality out of isotopeWidget
	if (bookmarkflg)
		pullIsotopes(isotopeParameters->_group);

	//select first sample if no peak or sample is selected
	if (!_selectedSample)
		updateSelectedSample(0);
	Peak *peak = isotopeParameters->_group->getPeak(_selectedSample);
	if (peak)
		isotopeParameters->_scan = peak->getScan();
	else
		return;
	
	computeIsotopes(isotopeParameters->_formula);
    updateIsotopicBarplot(grp);
}

void IsotopeWidget::updateIsotopicBarplot(PeakGroup *grp)
{
	if (!grp)
		return;
    isotopeParametersBarPlot->_group = grp;
    if (grp && grp->type() != PeakGroup::GroupType::Isotope)
		pullIsotopesForBarplot(grp);
}

void IsotopeWidget::setCompound(Compound *cpd)
{
	clearWidget();
	if (cpd == NULL)
        return;
    QString f = QString(cpd->formula().c_str());
	//isotopeParameters->_group = NULL;
	isotopeParameters->_compound = cpd;
        setWindowTitle("Isotopes:" + QString(cpd->name().c_str()));
	setFormula(f);
}

void IsotopeWidget::setIonizationMode(int mode)
{
	setCharge((float)mode);
}

void IsotopeWidget::userChangedFormula(QString f)
{
	if (f.toStdString() == isotopeParameters->_formula)
		return;

	isotopeParameters->_formula = f.toStdString();
	isotopeParameters->userChangedFormula();

	setWindowTitle("Unknown_" + f);
	computeIsotopes(isotopeParameters->_formula);
}

void IsotopeWidget::updateSampleList()
{
	sampleList->clear();
	reset();
	vector<mzSample *> samples = _mw->getVisibleSamples();
	if (samples.empty())
		return;

	sort(samples.begin(), samples.end(), mzSample::compSampleOrder);

	QString sampleName;
	int index = 0;
	vector<mzSample *>::iterator it;
	for (it = samples.begin(); it != samples.end(); it++, index++)
	{
		sampleName = QString::fromStdString((*it)->sampleName);
		sampleList->insertItem(index, sampleName, QVariant::fromValue(*it));
	}

	//set 1st sample if no sample is selected
	if (sampleList->currentIndex() < 0)
	{
		sampleList->setCurrentIndex(0);
		updateSelectedSample(0);
	}
}

void IsotopeWidget::reset()
{
	isotopeParameters->_group = NULL;
	isotopeParameters->_scan = NULL;
	isotopeParameters->_formula = "";
}

void IsotopeWidget::updateSelectedSample(int index)
{
	if (sampleList->count() == 0) 
	{
		_selectedSample = NULL;
		return;
	}
	if (index < 0) index = 0;
	_selectedSample = sampleList->itemData(index).value<mzSample *>();
	computeIsotopes(isotopeParameters->_formula);
}

void IsotopeWidget::setFormula(QString f)
{
	formula->setText(f);
	isotopeParameters->_formula = f.toStdString();
}

void IsotopeWidget::setCharge(double charge)
{
	if (charge != isotopeParameters->_charge)
	{
		ionization->setValue(charge);
		isotopeParameters->_charge = charge;
		computeIsotopes(isotopeParameters->_formula);
	}
}

void IsotopeWidget::computeIsotopes(string f)
{	
	clearWidget();
	if (f.empty())
		return;

	QSettings *settings = _mw->getSettings();
	MassCutoff *massCutoff = _mw->getUserMassCutoff();

	double maxNaturalAbundanceErr = _mw->mavenParameters->maxNaturalAbundanceErr;
	bool C13Labeled = _mw->mavenParameters->C13Labeled_BPE;
	bool N15Labeled = _mw->mavenParameters->N15Labeled_BPE;
	bool S34Labeled = _mw->mavenParameters->S34Labeled_BPE;
	bool D2Labeled = _mw->mavenParameters->D2Labeled_BPE;
	vector<Isotope> isotopes = MassCalculator::computeIsotopes(
		f,
		isotopeParameters->_charge,
		C13Labeled,
		N15Labeled,
		S34Labeled,
		D2Labeled);

	double parentMass = MassCalculator::computeMass(f, isotopeParameters->_charge);
	float mzWindow = massCutoff->massCutoffValue(parentMass);

	if (isotopeParameters->_scan == NULL)
		return;

	std::pair<float, float> parent = isotopeDetector->getIntensity(
		isotopeParameters->_scan, parentMass - mzWindow, parentMass + mzWindow);
	float parentPeakIntensity = parent.first;

	if (isotopeParameters->_group && !isotopeParameters->_group->isIsotope())
	{
		populateByParentGroup(isotopes, parentMass);
	}

	else
	{
		for (unsigned int i = 0; i < isotopes.size(); i++)
		{
			Isotope &x = isotopes[i];
			mzWindow = massCutoff->massCutoffValue(x.mass);
			std::pair<float, float> child = isotopeDetector->getIntensity(
				isotopeParameters->_scan, x.mass - mzWindow, x.mass + mzWindow);
			float isotopePeakIntensity = child.first;

			mzLink link;
			bool filterIsotope = false;

			filterIsotope = isotopeDetector->filterIsotope(x, 
				isotopePeakIntensity, 
				parentPeakIntensity, 
				_selectedSample, 
				isotopeParameters->_group);

			if (filterIsotope)
				isotopePeakIntensity = 0;

			link.mz1 = parentMass;
			link.mz2 = x.mass;
			link.note = x.name;
			link.value1 = x.abundance;
			link.value2 = isotopePeakIntensity;
			isotopeParameters->links.push_back(link);
		}
	}
	sort(isotopeParameters->links.begin(), isotopeParameters->links.end(), mzLink::compMz);

	showTable();
}

void IsotopeWidget::populateByParentGroup(vector<Isotope> masslist, double parentMass)
{
	PeakGroup *parentGroup = isotopeParameters->_group;
	if (!(parentGroup && parentGroup->isIsotope() == false))
		return;
	if (!isotopeParameters->_scan)
		return;

	map<string, PeakGroup> isotopes = isotopeDetector->getIsotopes(parentGroup, masslist);

	map<string, PeakGroup>::iterator itrIsotope;
	unsigned int index = 1;
	for (itrIsotope = isotopes.begin(); itrIsotope != isotopes.end(); ++itrIsotope, index++)
	{
		string isotopeName = (*itrIsotope).first;
		PeakGroup &child = (*itrIsotope).second;
		Peak *peak = child.getPeak(_selectedSample);

		if (!peak)
			continue;

        vector<mzSample*> samples = {_selectedSample};
        auto quantity = child.getOrderedIntensityVector(samples,
                                                        _mw->getUserQuantType()).at(0);

		mzLink link;
		link.mz1 = parentMass;
		link.mz2 = child.expectedMz;
		link.note = isotopeName;
        link.value1 = child.expectedAbundance;
        link.value2 = quantity;
		isotopeParameters->links.push_back(link);
	}
}

void IsotopeWidget::pullIsotopes(PeakGroup* group)
{
    if (!group)
        return;

    // clear clipboard
    QClipboard* clipboard = QApplication::clipboard();
    clipboard->clear(QClipboard::Clipboard);

    if (group->getCompound() == NULL) {
        _mw->setStatusText(tr("Unknown compound. Clipboard set to %1")
                               .arg(group->tagString.c_str()));
    }

    vector<mzSample*> vsamples = _mw->getVisibleSamples();
    if (workerThread->stopped()) {
        workerThread->started();

        // TODO: mavenParameters->-group is not thread-safe. Accessing it might
        // lead to crashes
        workerThread->mavenParameters->setPeakGroup(group);
        workerThread->mavenParameters->setSamples(vsamples);
        workerThread->mavenParameters->compoundMassCutoffWindow =
            _mw->getUserMassCutoff();

        workerThread->start();
        _mw->setStatusText("Isotopes pulled");
    }
}

void IsotopeWidget::pullIsotopesForBarplot(PeakGroup* group)
{
    if (!group)
        return;

    if (group->getCompound() == NULL) {
        _mw->setStatusText(tr("Unknown compound. Clipboard set to %1")
                               .arg(group->tagString.c_str()));
        return;
    }

    vector<mzSample*> vsamples = _mw->getVisibleSamples();
    if (workerThreadBarplot->stopped()) {
        workerThreadBarplot->started();

        // TODO: mavenParameters->-group is not thread-safe. Accessing it might
        // lead to crashes
        workerThreadBarplot->mavenParameters->setPeakGroup(group);
        workerThreadBarplot->mavenParameters->setSamples(vsamples);
        workerThreadBarplot->mavenParameters->compoundMassCutoffWindow =
            _mw->getUserMassCutoff();

        workerThreadBarplot->start();
        _mw->setStatusText("Isotopes pulled");
    }
}

void IsotopeWidget::setClipboard()
{

	if (isotopeParameters->_group)
	{

		//update clipboard
		setClipboard(isotopeParameters->_group);

		//update eic widget
		_mw->getEicWidget()->setSelectedGroup(isotopeParameters->_group);

		if (bookmarkflag)
		{
            auto group = _mw->bookmarkPeakGroup();
			bookmarkflag = true;
			_mw->peaksMarked++;
            _mw->autoSaveSignal({group});
		}
	}
	workerThread->stop();

	if (_mw->threadCompound != NULL)
	{
		_mw->setCompoundFocus(_mw->threadCompound);
		_mw->threadCompound = NULL;
	}
}

void IsotopeWidget::updateIsotopicBarplot()
{
    isotopeParametersBarPlot->_group = _mw->getEicWidget()->getParameters()->displayedGroup();
	if (isotopeParametersBarPlot->_group)
	{
		_mw->isotopePlot->setPeakGroup(isotopeParametersBarPlot->_group);
	}
	workerThreadBarplot->stop();

	if (_mw->threadCompound != NULL)
	{
		_mw->setCompoundFocus(_mw->threadCompound);
		_mw->threadCompound = NULL;
	}
}

void IsotopeWidget::setClipboard(QList<PeakGroup *> &groups)
{
	QString clipboardText;

	unsigned int groupCount = 0;
	bool includeSampleHeader = true;
	Q_FOREACH (PeakGroup *group, groups)
	{
		if (!group)
			continue;
		if (groupCount > 0)
			includeSampleHeader = false;
		QString infoText = groupIsotopeMatrixExport(group, includeSampleHeader);
		clipboardText += infoText;
		groupCount++;
	}
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(clipboardText);
}

void IsotopeWidget::setClipboard(PeakGroup *group)
{
	bool includeSampleHeader = true;
	QString infoText = groupIsotopeMatrixExport(group, includeSampleHeader);
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(infoText);
}

QString IsotopeWidget::groupIsotopeMatrixExport(PeakGroup *group, bool includeSampleHeader)
{
	if (group == NULL)
		return "";
	//header line
	QString tag(group->tagString.c_str());
	if (group->isIsotope())
		tag = QString(group->parent->tagString.c_str());
	if (tag.isEmpty() && group->getCompound() != NULL)
                tag = QString(group->getCompound()->name().c_str());
	if (tag.isEmpty() && group->srmId.length())
		tag = QString(group->srmId.c_str());
	if (tag.isEmpty() && group->meanMz > 0)
		tag = QString::number(group->meanMz, 'f', 6) + "@" + QString::number(group->meanRt, 'f', 2);
	if (tag.isEmpty())
		tag = QString::number(group->groupId);
	QString isotopeInfo;

	vector<mzSample *> vsamples = _mw->getVisibleSamples();
	sort(vsamples.begin(), vsamples.end(), mzSample::compSampleOrder);

	//include header
	if (includeSampleHeader)
	{
		for (int i = 0; i < vsamples.size(); i++)
		{
			isotopeInfo += "\t" + QString(vsamples[i]->sampleName.c_str());
		}
		isotopeInfo += "\n";

		bool includeSetNamesLine = true;
		if (includeSetNamesLine)
		{
			for (int i = 0; i < vsamples.size(); i++)
			{
				isotopeInfo += "\t" + QString(vsamples[i]->getSetName().c_str());
			}
			isotopeInfo += "\n";
		}
	}

	//get isotopic groups
	vector<PeakGroup *> isotopes;
	for (int i = 0; i < group->childCount(); i++)
	{
		if (group->children[i].isIsotope())
		{
			PeakGroup *isotope = &(group->children[i]);
			isotopes.push_back(isotope);
		}
	}
	//std::sort(isotopes.begin(), isotopes.end(), PeakGroup::compC13);

	if (isotopes.size() > 0)
	{
		MatrixXf MM = _mw->getIsotopicMatrixIsoWidget(group);
		for (int i = 0; i < isotopes.size(); i++)
		{
			QStringList groupInfo;
			groupInfo << tag + " | " + QString(isotopes[i]->tagString.c_str());
			for (unsigned int j = 0; j < vsamples.size(); j++)
			{
				groupInfo << QString::number(MM(j, i), 'f', 2);
			}
			isotopeInfo += groupInfo.join("\t") + "\n";
		}
		if (_mw->mavenParameters->pullIsotopesFlag && _mw->mavenParameters->isotopeC13Correction)
		{
			isotopeInfo += "Natural Abundance\n";
			for (int i = 0, k = isotopes.size(); i < isotopes.size(); i++, k++)
			{
				QStringList groupInfo;
				groupInfo << tag + " | " + QString(isotopes[i]->tagString.c_str());
				for (unsigned int j = 0; j < vsamples.size(); j++)
				{
					groupInfo << QString::number(MM(j, k), 'f', 2);
				}
				isotopeInfo += groupInfo.join("\t") + "\n";
			}
		}

		_mw->setStatusText("Clipboard set to isotope summary");
	}
	else
	{
		// non-isotopic group
		if (!group->isIsotope())
			isotopeInfo += tag + groupTextEport(group) + "\n";
		// for isotopic children
		else
			isotopeInfo += tag + " | " + groupTextEport(group) + "\n";
		_mw->setStatusText("Clipboard to group summary");
	}
	return isotopeInfo;
}

void IsotopeWidget::showTable()
{
	QTreeWidget *p = treeWidget;
	p->clear();
	p->setColumnCount(6);
	p->setHeaderLabels(QStringList() << "Isotope Name"
                                     << "m/z"
                                     << _mw->quantType->currentText()
									 << "%Labeling"
									 << "%Expected"
									 << "%Relative");
	p->setUpdatesEnabled(true);
	p->setSortingEnabled(true);

	double isotopeIntensitySum = 0;
	double maxAbundance = 0;
	//link.value1 is abundance
	//link.value2 is intensity
	//link.note is isotope label
	//link.mz2 is isotope m/z
	//link.mz1 is parent m/z
	for (unsigned int i = 0; i < isotopeParameters->links.size(); i++)
	{
		isotopeIntensitySum += isotopeParameters->links[i].value2;
		if (isotopeParameters->links[i].value1 > maxAbundance)
			maxAbundance = isotopeParameters->links[i].value1;
	}

	for (unsigned int i = 0; i < isotopeParameters->links.size(); i++)
	{
		float frac = 0;
		if (isotopeParameters->links[i].value2 == 0 && floor(isotopeParameters->links[i].value1 * 1e+6) == 0)
			continue;
		if (isotopeIntensitySum > 0)
			frac = isotopeParameters->links[i].value2 / isotopeIntensitySum * 100;

		NumericTreeWidgetItem *item = new NumericTreeWidgetItem(treeWidget,
																mzSliceType);
		QString item1 = QString(isotopeParameters->links[i].note.c_str());
		QString item2 = QString::number(isotopeParameters->links[i].mz2, 'f',
										5);
		QString item3 = QString::number(isotopeParameters->links[i].value2, 'f',
										4);
		//%labeling
		QString item4 = QString::number(frac, 'f', 4);
		//%Expected abundance
		QString item5 = QString::number(
			isotopeParameters->links[i].value1 * 100, 'f', 4);
		//%Relative abundance
		QString item6 = QString::number(isotopeParameters->links[i].value1 / maxAbundance * 100, 'f', 4);

		item->setText(0, item1);
		item->setText(1, item2);
		item->setText(2, item3);
		item->setText(3, item4);
		item->setText(4, item5);
		item->setText(5, item6);
	}
	p->resizeColumnToContents(0);
	p->setSortingEnabled(true);
	p->sortByColumn(0, Qt::AscendingOrder);
	p->setUpdatesEnabled(true);
}

void IsotopeWidget::showInfo()
{
	QList<QTreeWidgetItem *> selectedItems = treeWidget->selectedItems();
	if (selectedItems.size() < 1)
		return;
	QTreeWidgetItem *item = selectedItems[0];
	if (!item)
		return;

	QString __mz = item->text(1);
	float mz = __mz.toDouble();

	if (mz > 0)
	{
		MassCutoff *massCutoff = _mw->getUserMassCutoff();
		mzSlice slice = _mw->getEicWidget()->getParameters()->getMzSlice();
		slice.mzmin = mz - massCutoff->massCutoffValue(mz);
		slice.mzmax = mz + massCutoff->massCutoffValue(mz);
		if (isotopeParameters->_compound)
			slice.compound = isotopeParameters->_compound;
		if (!isotopeParameters->_compound)
			slice.compound = isotopeParameters->tempCompound;
		_mw->getEicWidget()->setMzSlice(slice);
	}
}

QString IsotopeWidget::groupTextEport(PeakGroup *group)
{

	if (group == NULL)
		return "";
	QStringList info;

	PeakGroup::QType qtype = _mw->getUserQuantType();
	vector<mzSample *> vsamples = _mw->getVisibleSamples();
	sort(vsamples.begin(), vsamples.end(), mzSample::compSampleOrder);
	vector<float> yvalues = group->getOrderedIntensityVector(vsamples, qtype);

	info << QString(group->tagString.c_str());
	for (unsigned int j = 0; j < vsamples.size(); j++)
	{
		mzSample *s = vsamples[j];
		info << QString::number(yvalues[j], 'f', 2);
	}
	return info.join("\t");
}

void IsotopeWidget::refreshForCurrentPeak()
{
    if (_mw->mavenParameters->_group)
        isotopeParameters->_group = _mw->mavenParameters->_group;
    computeIsotopes(isotopeParameters->_formula);
}
