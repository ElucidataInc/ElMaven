#include "masscalcgui.h"
using namespace std;


MassCalcWidget::MassCalcWidget(MainWindow* mw) {
  setupUi(this);
  _mw = mw;
  _mz = 0;
  setCharge(-1);
  setMassCutoff(mw->getUserMassCutoff());

  connect(computeButton, SIGNAL(clicked(bool)), SLOT(compute()));
  connect(lineEdit,SIGNAL(returnPressed()),SLOT(compute()));
  connect(ionization,SIGNAL(valueChanged(double)),SLOT(compute()));
  connect(maxppmdiff,SIGNAL(valueChanged(double)),SLOT(compute()));
  connect(mTable, SIGNAL(currentCellChanged(int,int,int,int)), SLOT(showCellInfo(int,int,int,int)));
}

void MassCalcWidget::setMass(float mz) {
	if ( mz == _mz ) return;

    lineEdit->setText(QString::number(mz,'f',5));
    _mz = mz;
    delete_all(matches);
    getMatches();
    showTable();
}

void MassCalcWidget::setCharge(float charge) {

                ionization->setValue(charge);
                _charge=charge;
}
void MassCalcWidget::setMassCutoff(MassCutoff *massCutoff) { maxppmdiff->setValue(massCutoff->getMassCutoff()); _massCutoff=massCutoff;
    maxppmdiff->setValue(massCutoff->getMassCutoff());
    string massCutoffType=massCutoff->getMassCutoffType();

    label_3->setText(QApplication::translate("MassCalcWidget", &massCutoffType[0], 0));
    showTable();
 }

void MassCalcWidget::compute() {
	 bool isDouble =false;
	 _mz = 		lineEdit->text().toDouble(&isDouble);
  	 _charge =  ionization->value();
       _massCutoff->setMassCutoff(maxppmdiff->value());
	 if (!isDouble) return;
	 cerr << "massCalcGui:: compute() " << _charge << " " << _mz << endl;

     delete_all(matches);

	_mw->setStatusText("Searching for formulas..");
     mcalc.enumerateMasses(_mz,_charge,_massCutoff, matches);
	 getMatches();
	_mw->setStatusText(tr("Found %1 formulas").arg(matches.size()));

     showTable();
}

void MassCalcWidget::showTable() {

    QTableWidget *p = mTable;
    p->setUpdatesEnabled(false);
    p->clear();
    p->setColumnCount( 5 );
    p->setRowCount(  matches.size() ) ;
    QString massCutoffDiff=label_3->text()+"Diff";
    p->setHorizontalHeaderLabels(  QStringList() << "Formula" << "Compound" << "Mass" << massCutoffDiff << "DB");
    p->setSortingEnabled(false);
    p->setUpdatesEnabled(false);

    for(unsigned int i=0;  i < matches.size(); i++ ) {
        //no duplicates in the list
        Compound* c = matches[i]->compoundLink;
        QString compoundName="";
        if (c != NULL) compoundName = QString(c->name.c_str());
        QString databaseName="";
        if(c != NULL ) databaseName = QString(c->db.c_str());
        QString item1 = QString(matches[i]->name.c_str() );
        QString item2 = QString(compoundName);
        QString item3 = QString::number( matches[i]->mass , 'f', 4);
        QString item4 = QString::number( matches[i]->diff , 'f', 4);
        QString item5 = QString(databaseName);

		QTableWidgetItem* item = new QTableWidgetItem(item1,0);

        p->setItem(i,0, item );
        p->setItem(i,1, new QTableWidgetItem(item2,0));
        p->setItem(i,2, new QTableWidgetItem(item3,0));
        p->setItem(i,3, new QTableWidgetItem(item4,0));
        p->setItem(i,4, new QTableWidgetItem(item5,0));
		if (c != NULL) item->setData(Qt::UserRole,QVariant::fromValue(c));
    }

    p->setSortingEnabled(true);
    p->setUpdatesEnabled(true);
    p->update();

}

void MassCalcWidget::setupSortedCompoundsDB() {
    sortedcompounds.clear();
    sortedcompounds.resize(DB.compoundsDB.size());
    copy(DB.compoundsDB.begin(), DB.compoundsDB.end(),   sortedcompounds.begin());
    sort(sortedcompounds.begin(), sortedcompounds.end(), Compound::compMass);
}

QSet<Compound*> MassCalcWidget::findMathchingCompounds(float mz, MassCutoff *massCutoff, float charge) {
	if (sortedcompounds.size() != DB.compoundsDB.size() ) { setupSortedCompoundsDB(); }

	QSet<Compound*>uniqset;
    MassCalculator mcalc;
    Compound x("find", "", "",0);
    x.mass = mz-2;
    vector<Compound*>::iterator itr = lower_bound(sortedcompounds.begin(), sortedcompounds.end(), &x, Compound::compMass);

	//cerr << "findMathchingCompounds() mz=" << mz << " ppm=" << ppm << " charge=" <<  charge;
    for(;itr != sortedcompounds.end(); itr++ ) {
        Compound* c = *itr; if (!c) continue;
        double cmass = MassCalculator::computeMass(c->formula, charge);
        if ( mzUtils::massCutoffDist((double) cmass, (double) mz,massCutoff) < massCutoff->getMassCutoff() && !uniqset.contains(c) ) uniqset << c;
        if (cmass > mz+2) break;
	}
	return uniqset;
}

void MassCalcWidget::getMatches() {
    int charge = _mw->mavenParameters->getCharge();
	QSet<Compound*> compounds = findMathchingCompounds(_mz,_massCutoff,charge);
	Q_FOREACH(Compound* c, compounds) {
          MassCalculator::Match* m = new MassCalculator::Match();
          m->name = c->formula;
          m->mass = MassCalculator::computeMass(c->formula,_mw->mavenParameters->getCharge(c));
          m->diff = mzUtils::massCutoffDist((double) m->mass,(double) _mz,_massCutoff);
          m->compoundLink = c;
          matches.push_back(m);
    }
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

void MassCalcWidget::showCellInfo(int row, int col, int lrow, int lcol) {

	lrow=lcol=0;
    if ( row < 0 || col < 0 )  return;
    if ( row < mTable->rowCount()  ) {

		QVariant v = mTable->item(row,0)->data(Qt::UserRole);
    	Compound*  c =  v.value<Compound*>();

		if ( c) {
			_mw->setCompoundFocus(c);
			return;
		}

        QString formula = mTable->item(row,0)->text();
		if (!formula.isEmpty()) {
			_mw->setFormulaFocus(formula);
			return;
		}

    }
}

MassCalcWidget::~MassCalcWidget() {
    delete_all(matches);
}
