#include "backgroundopsthread.h"
#include "Compound.h"
#include "datastructures/isotope.h"
#include "datastructures/mzSlice.h"
#include "eiclogic.h"
#include "eicwidget.h"
#include "globals.h"
#include "isotopelogic.h"
#include "isotopeplot.h"
#include "isotopeswidget.h"
#include "mainwindow.h"
#include "masscutofftype.h"
#include "mavenparameters.h"
#include "mzSample.h"
#include "numeric_treewidgetitem.h"
#include "peakdetector.h"
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

	workerThread = new BackgroundOpsThread(mw);
	workerThread->setMainWindow(mw);

    MavenParameters *mavenParameters = mw->mavenParameters;

	//Thread for bar plot
	workerThreadBarplot = new BackgroundOpsThread(mw);
    workerThreadBarplot->setRunFunction("pullIsotopesForBarPlot");
	workerThreadBarplot->setMainWindow(mw);
	workerThreadBarplot->setMavenParameters(mavenParameters);
	workerThreadBarplot->setPeakDetector(new PeakDetector(mavenParameters));
	connect(workerThreadBarplot, SIGNAL(finished()), this, SLOT(updateIsotopicBarplot()));
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

void IsotopeWidget::peakSelected(Peak *peak, shared_ptr<PeakGroup> group)
{
	clearWidget();
    if (peak == nullptr || group == nullptr || !group->hasCompoundLink())
		return;

    //set selectedSample for isotope calculation
    _selectedSample = peak->getSample();
    setPeakGroupAndMore(group);

    sampleList->setCurrentText(
        QString::fromStdString(_selectedSample->sampleName));
}

void IsotopeWidget::setPeakGroupAndMore(shared_ptr<PeakGroup> group,
                                        bool bookmark)
{
	clearWidget();

    if (group == nullptr)
		return;

    // set compound, formula, window title
    isotopeParameters->_group = make_shared<PeakGroup>(*group);
    if (group->isIsotope() && group->parent != nullptr)
        isotopeParameters->_group = make_shared<PeakGroup>(*(group->parent));

    setCompound(isotopeParameters->_group->getCompound());

    // TODO: move bookmarking functionality out of isotopeWidget
    if (bookmark)
        pullIsotopes(isotopeParameters->_group.get());

    // select first sample if no peak or sample is selected
    if (_selectedSample == nullptr) {
        updateSelectedSample(0);
    } else {
        Peak *peak = isotopeParameters->_group->getPeak(_selectedSample);
        if (peak != nullptr) {
            isotopeParameters->_scan = peak->getScan();
        } else {
            isotopeParameters->_scan = nullptr;
        }
        computeIsotopes(isotopeParameters->_formula);
    }
}

void IsotopeWidget::updateIsotopicBarplot(shared_ptr<PeakGroup> grp)
{
    if (grp == nullptr)
		return;

    isotopeParametersBarPlot->_group = grp;
    if (grp && grp->type() != PeakGroup::GroupType::Isotope)
        pullIsotopesForBarplot(isotopeParametersBarPlot->_group.get());
}

void IsotopeWidget::setCompound(Compound *compound)
{
	clearWidget();

    if (compound == NULL)
        return;

    QString formula = QString(compound->formula().c_str());
    isotopeParameters->_compound = compound;
    setWindowTitle("Isotopes: " + QString(compound->name().c_str()));
    setFormula(formula);
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
    if (isotopeParameters->_group != nullptr) {
        Peak *peak = isotopeParameters->_group->getPeak(_selectedSample);
        if (peak != nullptr) {
            isotopeParameters->_scan = peak->getScan();
        } else {
            isotopeParameters->_scan = nullptr;
        }
    }
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

void IsotopeWidget::_insertLinkForPeakGroup(PeakGroup* group)
{
    auto linkForSamplePeak = [this, group]() {
        mzLink link;

        mzSample* sample = _selectedSample;
        if (_selectedSample == nullptr)
            sample = isotopeParameters->_scan->sample;

        Peak* peak = group->getPeak(sample);
        if (peak == nullptr)
            return link;

        vector<mzSample*> samples = {sample};
        auto quantity = group->getOrderedIntensityVector(
                                 samples,
                                 _mw->getUserQuantType()).at(0);

        link.mz1 = peak->baseMz;
        link.mz2 = group->isotope().mass;
        link.note = group->isotope().name;
        link.value1 = group->isotope().abundance;
        link.value2 = quantity;
        return link;
    };

    if (group->isGhost())
        return;

    mzLink link = linkForSamplePeak();
    if (link.mz1 > 0.0f && link.mz2 > 0.0f)
        isotopeParameters->links.push_back(link);
}

void IsotopeWidget::_pullIsotopesForFormula(string formula)
{
    while(workerThread->isRunning())
        QCoreApplication::processEvents();

    disconnect(workerThread, &BackgroundOpsThread::finished, nullptr, nullptr);

    if (workerThread->mavenParameters != nullptr)
        delete workerThread->mavenParameters;

    int charge = isotopeParameters->_charge;
    float expectedRt = isotopeParameters->_scan->rt;
    auto mp = new MavenParameters(*_mw->mavenParameters);
    workerThread->setMavenParameters(mp);
    workerThread->peakDetector->setMavenParameters(mp);
    workerThread->setPullIsotopesForFormulaArgs(formula, charge, expectedRt);
    workerThread->setRunFunction("pullIsotopesForFormula");

    auto callback = [this, expectedRt]() {
        float leastRtDiff = numeric_limits<float>::max();
        PeakGroup* closestParent = nullptr;
        if (workerThread->mavenParameters->allgroups.size() == 1) {
            closestParent = &workerThread->mavenParameters->allgroups[0];
        } else {
            for (auto& group : workerThread->mavenParameters->allgroups) {
                float rtDiff = abs(group.meanRt - expectedRt);
                if (rtDiff < leastRtDiff) {
                    closestParent = &group;
                    leastRtDiff = rtDiff;
                }
            }
        }
        if (closestParent == nullptr)
            return; // TODO: clear the table as well?

        _insertLinkForPeakGroup(closestParent);
        for (auto& child : closestParent->childIsotopes())
            _insertLinkForPeakGroup(child.get());

        sort(begin(isotopeParameters->links),
             end(isotopeParameters->links),
             mzLink::compMz);
        showTable();
    };

    connect(workerThread, &BackgroundOpsThread::finished, this, callback);
    workerThread->start();
}

void IsotopeWidget::_pullIsotopesForGroup(PeakGroup* group)
{
    while(workerThread->isRunning())
        QCoreApplication::processEvents();
    if (workerThread->stopped())
        workerThread->setStopped(false);

    disconnect(workerThread, &BackgroundOpsThread::finished, nullptr, nullptr);

    if (workerThread->mavenParameters != nullptr)
        delete workerThread->mavenParameters;

    auto mp = new MavenParameters(*_mw->mavenParameters);
    workerThread->setMavenParameters(mp);
    workerThread->peakDetector->setMavenParameters(mp);

    // TODO: mavenParameters->group is not thread-safe. Accessing it might
    // lead to crashes
    workerThread->mavenParameters->setPeakGroup(group);

    vector<mzSample*> vsamples = _mw->getVisibleSamples();
    workerThread->mavenParameters->setSamples(vsamples);
    workerThread->mavenParameters->compoundMassCutoffWindow =
        _mw->getUserMassCutoff();
    workerThread->setRunFunction("pullIsotopesForGroup");

    connect(workerThread,
            &BackgroundOpsThread::finished,
            this,
            QOverload<>::of(&IsotopeWidget::setClipboard));
    workerThread->start();
}

void IsotopeWidget::computeIsotopes(string formula)
{
    clearWidget();
    if (formula.empty())
        return;

    if (isotopeParameters->_scan == nullptr)
        return;

    if (isotopeParameters->_group) {
        PeakGroup *parentGroup = isotopeParameters->_group.get();
        if (parentGroup->parent != nullptr)
            parentGroup = parentGroup->parent;
        if (parentGroup == nullptr || parentGroup->isIsotope())
            return;

        if (!parentGroup->childIsotopes().empty()) {
            _insertLinkForPeakGroup(parentGroup);
            for (auto& child : parentGroup->childIsotopes())
                _insertLinkForPeakGroup(child.get());

            sort(begin(isotopeParameters->links),
                 end(isotopeParameters->links),
                 mzLink::compMz);
            showTable();
        } else {
            _pullIsotopesForFormula(formula);
        }
    } else {
        _pullIsotopesForFormula(formula);
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

    _pullIsotopesForGroup(group);
    _mw->setStatusText("Pulling isotopes…");
}

void IsotopeWidget::pullIsotopesForBarplot(PeakGroup* group)
{
    if (!group)
        return;

    if (group->getCompound() == nullptr) {
        _mw->setStatusText(tr("Unknown compound. Clipboard set to %1")
                               .arg(group->tagString.c_str()));
        return;
    }

    while(workerThreadBarplot->isRunning())
        QCoreApplication::processEvents();
    if (workerThreadBarplot->stopped())
        workerThreadBarplot->setStopped(false);

    // TODO: mavenParameters->group is not thread-safe. Accessing it might
    // lead to crashes
    workerThreadBarplot->mavenParameters->setPeakGroup(group);

    vector<mzSample*> vsamples = _mw->getVisibleSamples();
    workerThreadBarplot->mavenParameters->setSamples(vsamples);
    workerThreadBarplot->mavenParameters->compoundMassCutoffWindow =
        _mw->getUserMassCutoff();

    workerThreadBarplot->start();
    _mw->setStatusText("Pulling isotopes…");
}

void IsotopeWidget::updateIsotopicBarplot()
{
    isotopeParametersBarPlot->_group = _mw->getEicWidget()->getParameters()->displayedGroup();
	if (isotopeParametersBarPlot->_group)
    {
        _mw->isotopePlot->setPeakGroup(isotopeParametersBarPlot->_group.get());
	}
    workerThreadBarplot->setStopped(true);
}

void IsotopeWidget::setClipboard()
{
    if (isotopeParameters->_group) {
        // update clipboard
        setClipboard(isotopeParameters->_group.get());

        // update eic widget
        _mw->getEicWidget()->setSelectedGroup(isotopeParameters->_group);

        if (bookmarkflag) {
            auto group = _mw->bookmarkPeakGroup();
            bookmarkflag = true;
            _mw->autoSaveSignal({group});
        }
    }
    _mw->setStatusText("Isotopes pulled");
    workerThread->setStopped(true);
}

void IsotopeWidget::setClipboard(QList<shared_ptr<PeakGroup>>& groups)
{
	QString clipboardText;

	unsigned int groupCount = 0;
	bool includeSampleHeader = true;
    Q_FOREACH (shared_ptr<PeakGroup> group, groups)
	{
        if (group == nullptr)
			continue;
		if (groupCount > 0)
            includeSampleHeader = false;
        QString infoText = groupIsotopeMatrixExport(group.get(),
                                                    includeSampleHeader);
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
        tag = QString::number(group->groupId());
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
    for (int i = 0; i < group->childIsotopeCount(); i++)
    {
        if (group->childIsotopes()[i]->isIsotope())
        {
            PeakGroup *isotope = group->childIsotopes()[i].get();
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
    if (_mw->mavenParameters->_group != nullptr) {
        isotopeParameters->_group =
            make_shared<PeakGroup>(*_mw->mavenParameters->_group);
    }
    computeIsotopes(isotopeParameters->_formula);
}
