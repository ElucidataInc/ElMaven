#include "Compound.h"
#include "eicwidget.h"
#include "eiclogic.h"
#include "globals.h"
#include "mainwindow.h"
#include "masscalcgui.h"
#include "masscutofftype.h"
#include "mavenparameters.h"
#include "Scan.h"
#include "spectrawidget.h"
#include "mzSample.h"
#include "mzUtils.h"

using namespace std;

MassCalcWidget::MassCalcWidget(MainWindow* mw) {
  setupUi(this);
  _mw = mw;
  _mz = 0;
  _currentGroup = nullptr;
  _currentScan = nullptr;
  setMassCutoff(mw->getUserMassCutoff());

  database->addItem("All");

  connect(computeButton, SIGNAL(clicked(bool)), SLOT(compute()));
  connect(database, SIGNAL(currentIndexChanged(int)), SLOT(showTable()));
  connect(precursorMz,SIGNAL(returnPressed()),SLOT(compute()));
  connect(precursorPpm,SIGNAL(valueChanged(double)),SLOT(compute()));
  connect(mTable, SIGNAL(itemSelectionChanged()), SLOT(_showInfo()));
  connect(fragPpm, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [&] {
              if (_currentGroup) {
                  auto newGroup = new PeakGroup(*_currentGroup);
                  setPeakGroup(newGroup);
              } else if (_currentScan) {
                  auto newScan = new Scan(nullptr, -1, 1, 0.0f, 0.0f, -1);
                  newScan->deepcopy(_currentScan);
                  setFragmentationScan(newScan);
              }
          });
}

void MassCalcWidget::setMass(float mz) {
	if ( mz == _mz ) return;

    precursorMz->setText(QString::number(mz,'f',5));
    _mz = mz;
    getMatches();
    showTable();
}

void MassCalcWidget::setMassCutoff(MassCutoff *massCutoff) { precursorPpm->setValue(massCutoff->getMassCutoff()); _massCutoff=massCutoff;
    precursorPpm->setValue(massCutoff->getMassCutoff());
    string massCutoffType=massCutoff->getMassCutoffType();

    label_3->setText(QApplication::translate("MassCalcWidget", &massCutoffType[0], 0));
    showTable();
 }

void MassCalcWidget::compute() {
	 bool isDouble =false;
         _mz = 		precursorMz->text().toDouble(&isDouble);
       _massCutoff->setMassCutoff(precursorPpm->value());
	 if (!isDouble) return;

	_mw->setStatusText("Searching for formulas..");
	 getMatches();
	_mw->setStatusText(tr("Found %1 formulas").arg(matches.size()));

     showTable();
}

void MassCalcWidget::showTable()
{
    QTreeWidget *p = mTable;
    p->setUpdatesEnabled(false);
    p->clear();
    p->setColumnCount(6);
    QString massCutoffDiff = "Δm/z (" + label_3->text() + ")";
    p->setHeaderLabels(QStringList() << "Formula"
                                     << "Compound"
                                     << "Δrt"
                                     << massCutoffDiff
                                     << "MS2 Score"
                                     << "DB");
    p->setSortingEnabled(false);
    p->setUpdatesEnabled(false);

    for(unsigned int i=0;  i < matches.size(); i++ ) {
        //no duplicates in the list
        auto match = matches[i];
        Compound* c = match->compoundLink;

        QString compoundName="";
        if (c != nullptr)
            compoundName = QString(c->name().c_str());

        QString databaseName="";
        if(c != nullptr)
            databaseName = QString(c->db().c_str());

        if (database->currentIndex() != 0
            && database->currentText() != databaseName)
            continue;

        QString item1 = QString(match->name.c_str());
        QString item2 = compoundName;
        QString item3 = QString::number(match->rtDiff, 'f', 2);
        QString item4 = QString::number(match->diff, 'f', 2);
        QString item5 = QString::number(match->fragScore.hypergeomScore, 'f', 3);
        QString item6 = databaseName;
        QStringList rowItems = QStringList() << item1
                                             << item2
                                             << item3
                                             << item4
                                             << item5
                                             << item6;
        QTreeWidgetItem *item = new QTreeWidgetItem(rowItems);
        item->setData(0, Qt::UserRole, QVariant(i));
        p->addTopLevelItem(item);
    }

    p->sortByColumn(3,Qt::DescendingOrder);
    p->header()->setStretchLastSection(true);
    p->setSortingEnabled(true);
    p->setUpdatesEnabled(true);
    p->update();
}

void MassCalcWidget::setupSortedCompoundsDB() {
    sortedcompounds.clear();
    copy(DB.compoundsDB.begin(),
         DB.compoundsDB.end(),
         back_inserter(sortedcompounds));
    sort(sortedcompounds.begin(), sortedcompounds.end(), Compound::compMass);
}

QSet<Compound*> MassCalcWidget::findMathchingCompounds(float mz, MassCutoff *massCutoff, float charge) {
    setupSortedCompoundsDB();
    MassCalculator mcalc;
    QSet<Compound*>uniqset;
    Compound x("", "", "", 0);
    x.setMz( mz - 2);
    vector<Compound*>::iterator itr = lower_bound(sortedcompounds.begin(),
                                                  sortedcompounds.end(),
                                                  &x,
                                                  Compound::compMass);
    for (; itr != sortedcompounds.end(); itr++) {
        Compound* c = *itr;
        if (!c)
            continue;
        double cmass = MassCalculator::computeMass(c->formula(), charge);
        if (mzUtils::massCutoffDist((double) cmass,
                                    (double) mz,
                                    massCutoff) < massCutoff->getMassCutoff()
            && !uniqset.contains(c))
            uniqset << c;
        if (cmass > mz + 2)
            break;
    }
    return uniqset;
}

void MassCalcWidget::getMatches() {
    int charge = _mw->mavenParameters->getCharge();
    QSet<Compound*> compounds = findMathchingCompounds(_mz,
                                                       _massCutoff,
                                                       charge);
    delete_all(matches);
    Q_FOREACH(Compound* c, compounds) {
        MassCalculator::Match* m = new MassCalculator::Match();
        m->name = c->formula();
        m->mass = MassCalculator::computeMass(c->formula(),
                                              _mw->mavenParameters->getCharge(c));
        m->diff = mzUtils::massCutoffDist((double)m->mass,
                                          (double)_mz,
                                          _massCutoff);
        m->compoundLink = c;
        matches.push_back(m);
    }
}

void MassCalcWidget::setPeakGroup(PeakGroup* grp) {
    if(!grp)
        return;

    if (_currentGroup)
        delete _currentGroup;
    _currentGroup = new PeakGroup(*grp);

    _mz = grp->meanMz;
    precursorMz->setText(QString(to_string(_mz).c_str()));
    getMatches();

    if(grp->ms2EventCount == 0)
        grp->computeFragPattern(fragPpm->value());

    for(auto& m : matches) {
        Compound* cpd = m->compoundLink;

        if(grp->fragmentationPattern.nobs() != 0) {
            m->fragScore = cpd->scoreCompoundHit(&(grp->fragmentationPattern),
                                                 fragPpm->value());
        }

        if (cpd->expectedRt() > 0)
            m->rtDiff = grp->meanRt - cpd->expectedRt();
    }
    showTable();
}

void MassCalcWidget::setFragmentationScan(Scan* scan) {
    if(!scan)
        return;

    if (_currentScan)
        delete _currentScan;
    _currentScan = new Scan(nullptr, -1, 1, 0.0f, 0.0f, -1);
    _currentScan->deepcopy(scan);

    Fragment f(scan, 0, 0, 1024);
    _mz = scan->precursorMz;
    precursorMz->setText(QString(to_string(_mz).c_str()));
    getMatches();

    for(auto& m : matches ) {
        Compound* cpd = m->compoundLink;
        m->fragScore = cpd->scoreCompoundHit(&f, fragPpm->value(), false);
        m->fragScore.mergedScore = m->fragScore.hypergeomScore;
    }
    showTable();
}

void MassCalcWidget::pubChemLink(QString formula){
	_mw->setStatusText("Searhing pubchem");
	QString requestStr(
		tr("http://pubchem.ncbi.nlm.nih.gov/search/search.cgi?cmd=search&q_type=mf&q_data=%1&simp_schtp=mf")
    .arg(formula));

    _mw->setUrl(requestStr);
}

void MassCalcWidget::keggLink(QString formula){
	_mw->setStatusText("Searhing Kegg");
	QString requestStr(
		tr("http://www.genome.jp/ligand-bin/txtsearch?column=formula&query=%1&DATABASE=compound").arg(formula));
    _mw->setUrl(requestStr);
}

void MassCalcWidget::_showInfo()
{
    if (matches.size() == 0)
        return;

    auto items = mTable->selectedItems();
    if (items.isEmpty())
        return;

    QTreeWidgetItem* item = items.last();
    if(!item)
        return;

    QVariant v = item->data(0, Qt::UserRole);
    int matchNum = v.toInt();

    MassCalculator::Match* match = matches[matchNum];
    if (match->compoundLink) {
        auto selectedCompound = match->compoundLink;
        MassCutoff* massCutoff = _mw->getUserMassCutoff();
        auto eicSlice = _mw->getEicWidget()->getParameters()->getMzSlice();

        // we create a slice with match's m/z but with the rt bounds of the EIC
        // widget's current slice, which should hopefully be that of the last
        // query group itself.
        float minmz = match->mass - massCutoff->massCutoffValue(match->mass);
        float maxmz = match->mass + massCutoff->massCutoffValue(match->mass);
        float rtmin = eicSlice.rtmin;
        float rtmax = eicSlice.rtmax;
        mzSlice slice(minmz, maxmz, rtmin, rtmax);
        slice.compound = selectedCompound;
        if (!selectedCompound->srmId().empty())
            slice.srmId = selectedCompound->srmId();

        _mw->searchText->setText(QString::number(slice.mz, 'f', 8));
        _mw->getEicWidget()->setMzSlice(slice);
        _mw->fragSpectraWidget->overlayCompoundFragmentation(selectedCompound);
    }
}

MassCalcWidget::~MassCalcWidget() {
    mzUtils::delete_all(matches);
}
