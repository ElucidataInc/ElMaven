#include "peptidefragmentation.h"
using namespace std;


PeptideFragmentationWidget::PeptideFragmentationWidget(MainWindow* mw) { 
  setupUi(this);
  setCharge(+1);
  setResolution(mw->getUserMassCutoff());
  _scan = new Scan(NULL,0,0,0,0,0);
  _mw = mw;

  connect(computeButton, SIGNAL(clicked(bool)), SLOT(compute()));
  connect(peptideSequence,SIGNAL(returnPressed()),SLOT(compute()));
  connect(charge,SIGNAL(valueChanged(double)),SLOT(compute()));
  connect(resolution,SIGNAL(valueChanged(double)),SLOT(compute()));
  connect(fragmenationType,SIGNAL(currentIndexChanged(int)),SLOT(compute()));

  connect(precursorMZ,SIGNAL(returnPressed()),SLOT(focusPrecursorMz()));
  connect(charge,SIGNAL(valueChanged(double)),SLOT(focusPrecursorMz()));

  resolution->setSingleStep(0.05);
  resolution->setMaximum(1.5);
}

void PeptideFragmentationWidget::setPeptideSequence(QString sequence) {
	if ( sequence == _sequence ) return;
    _sequence = sequence;
    peptideSequence->setText(sequence);
    showTable();
}

void PeptideFragmentationWidget::setCharge(float c) {
    if (c != 0 ) {
        charge->setValue(c);
        _charge=c;
    }
}

void PeptideFragmentationWidget::setResolution(MassCutoff *massCutoff) {
    if (massCutoff->getMassCutoff() > 0 ) {
     _massCutoff=massCutoff;
    }
}

void PeptideFragmentationWidget::setScan(Scan* s) {
    if (s and s->mslevel > 1) {
        _scan->deepcopy(s);
      //  _scan->quantileFilter(80);
        compute();
    }
}


void PeptideFragmentationWidget::compute() {
    
     _sequence = 	peptideSequence->text();
  	 _charge =  	charge->value();
       _massCutoff->setMassCutoff(resolution->value());
     Peptide pept(_sequence.toStdString(),_charge,"");
     if(pept.isGood()) {
        showTable();
        if (_scan) {
            _mw->spectraWidget->overlayPeptideFragmentation(_sequence,_massCutoff);
        }

     } else {
        mTable->clear();
     }

}

void PeptideFragmentationWidget::focusPrecursorMz() {
    float precMz = precursorMZ->text().toDouble();
    _mw->setMzValue(precMz);
}


void PeptideFragmentationWidget::showTable() {
    qDebug() << "PeptideFragmentationWidget::showTable: " << _sequence << " " << _massCutoff->getMassCutoff();

    Peptide pept(_sequence.toStdString(),_charge,"");
    QString precMz = QString::number(pept.monoisotopicMZ(),'f',5);
    precursorMZ->setText(precMz);


    vector<FragmentIon*> ions;
    string fragType = fragmenationType->currentText().toStdString();
    pept.generateFragmentIons(ions,fragType);

    QString strippedSequence(pept.stripped.c_str());
    unsigned int aalen=strippedSequence.length();


    QMap<QString, QMap<int,FragmentIon*> > ionMap;
    for(unsigned int i=0; i < ions.size();i++ ) {
        FragmentIon *ion = ions[i];
        QString mzStr = QString::number(ion->m_mz,'f',3);
        QString ionType(ion->m_ionType.c_str());
        if (ion->m_charge>1) for(int j=0;j<ion->m_charge;j++) ionType += "+";
        if (ion->m_loss>0)  { ionType += "-";  ionType += QString::number(ion->m_loss); }

        if (_scan) {
            int matchedPos = _scan->findClosestHighestIntensityPos(ion->m_mz,_massCutoff);
            if (matchedPos>0) {
                ion->m_mzDiff = abs(_scan->mz[matchedPos]-ion->m_mz);
                qDebug() << "showTable IONS: " << ion->m_ion.c_str() << " ->" << ionType << " " << ion->m_mz << " mzdiff=" << ion->m_mzDiff;
            } else {
                ion->m_mzDiff =-1;
            }
        }
        ionMap[ionType][ion->m_pos]=ion;
    }

    QStringList selectedIons;
    selectedIons << "p" << "y"    << "b"    << "y-17"
                    <<  "y-18" << "b-17" << "y++"
                    << "b++" << "a" << "z"  << "c" << "x";


    mTable->clear();
    QTableWidget *p = mTable; 
    p->setUpdatesEnabled(false); 
    p->clear(); 
    p->setColumnCount( selectedIons.length()+2 );
    p->setColumnWidth(0,  30);
    p->setColumnWidth(1,  30);
    p->setRowCount(  strippedSequence.length());
    p->setHorizontalHeaderLabels(  QStringList() << "#" << "aa" << selectedIons );
    p->setSortingEnabled(false);
    p->setUpdatesEnabled(false);

    for(unsigned int i=0;  i < aalen; i++ ) {
        QString item1 = QString::number(i);
        QString item2 = QString(strippedSequence[i]);
        p->setItem(i,0, new QTableWidgetItem(item1));
        p->setItem(i,1, new QTableWidgetItem(item2));
    }



    float maxIntensity=0;

    if (_scan) {
        maxIntensity = _scan->maxIntensity();
        _scan->summary();
    }

    QFont fnt= QApplication::font();  fnt.setPointSize(8);

    /* SpectralHit hit;
    hit.score = 0;
    hit.matchCount=0;
    hit.sampleName="";
    hit.productPPM=1;
    hit.scan = _scan;
*/
    int colm=2;
    Q_FOREACH (QString ionType, selectedIons ) {
        if (ionMap.count(ionType) == 0 ) p->hideColumn(colm);
        else { p->showColumn(colm); p->setColumnWidth(colm,50); }

        for( int pos=0; pos< aalen; pos++ ) {
            if (ionMap.count(ionType) and ionMap[ionType].count(pos)) {
                int row=pos-1;
                if (ionType.at(0) == 'y' or ionType.at(0) == 'x' or ionType.at(0) == 'z') row = aalen-pos;
                FragmentIon* ion = ionMap[ionType][pos];
                double mz = ion->m_mz;
                QString mzStr = QString::number(mz,'f',3);
                QTableWidgetItem*item = new QTableWidgetItem(mzStr);
                item->setFont(fnt);

                if (ion->m_mzDiff != -1) {
                    float alpha = (_massCutoff->getMassCutoff()-ion->m_mzDiff)/_massCutoff->getMassCutoff();
                    if (alpha > 0) {
                         item->setBackground(QBrush(QColor::fromHsvF(0.3,alpha,0.8,1)));
                    }
                    // hit.mzList << _scan->mz[matchedPos];
                    // hit.intensityList << _scan->intensity[matchedPos];;
                    // hit.annotations << ionType;

                }
                p->setItem(row,colm,item);
            }
        }
        colm++;
    }

    delete_all(ions);

    /*
     if (_scan) { _mw->spectraWidget->overlaySpectralHit(hit);}
    */

    p->setUpdatesEnabled(true);
    p->update();
    delete_all(ions);
}
