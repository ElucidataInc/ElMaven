#include "spectramatching.h"

SpectraMatching::SpectraMatching(MainWindow *w): QDialog(w) { 
    setupUi(this);
    mainwindow = w;
    connect(resultTable,SIGNAL(itemSelectionChanged()), SLOT(showScan()));
    connect(findButton, SIGNAL(clicked(bool)), SLOT(findMatches()));
    connect(exportButton, SIGNAL(clicked(bool)), SLOT(exportMatches()));
    resultTable->setSortingEnabled(true);
    bound_checking_pattern=false;
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

    //get tollerance
    _precursorMassCutoff = this->_precursorMassCutoff;
    _productMassCutoff   = this->_productMassCutoff;

    //get scan type
   _msScanType=0;
   QString scanType=this->scanTypeComboBox->currentText();
   if (scanType != "any")
       _msScanType = scanType.mid(2,1).toInt();


   //parse fragmentation mz. intensity pairs
   QString mzpairs = this->fragmentsText->toPlainText();
   _mzsList.clear();
   _intensityList.clear();
   _intensityMaxErr.clear();
   _intensityMinErr.clear();
   bound_checking_pattern=false;

   QRegExp SPACE("\\s+");
   QRegExp COMMA("[;|,|\n|\r]");
   if ( mzpairs.contains(COMMA) ) {
       QStringList mz_ints_pairs = mzpairs.split(COMMA);
       Q_FOREACH (QString mzint, mz_ints_pairs) {
           mzint = mzint.simplified();
           qDebug() << "Pair:" << mzint;

           QStringList _mz_int = mzint.split(SPACE);
           if (_mz_int.size() == 4) {
               double mz =   _mz_int[0].simplified().toDouble();
               double ints = _mz_int[1].simplified().toDouble();
               double minerr = _mz_int[2].simplified().toDouble();
               double maxerr = _mz_int[3].simplified().toDouble();
               if (mz > 0 && ints >= 0) {
                _mzsList << mz;
                _intensityList << ints;
                _intensityMaxErr << maxerr;
                _intensityMinErr << minerr;
                 bound_checking_pattern=true;
               }
          } else if (_mz_int.size() == 3) {
               double mz =   _mz_int[0].simplified().toDouble();
               double ints = _mz_int[1].simplified().toDouble();
               double maxerr = _mz_int[2].simplified().toDouble();
               if (mz > 0 && ints >= 0 && maxerr >= 0) {
                _mzsList << mz;
                _intensityList << ints;
                _intensityMaxErr << maxerr;
               }
           } else if (_mz_int.size() == 2) {
               double mz =   _mz_int[0].simplified().toDouble();
               double ints = _mz_int[1].simplified().toDouble();
               if (mz > 0 && ints >= 0 ) {
                _mzsList << mz;
                _intensityList << ints;
               }
           }  else if (_mz_int.size() == 1) {
               double mz =   _mz_int[0].simplified().toDouble();
               if (mz > 0) {
                _mzsList << mz;
                _intensityList << 0.0;
               }
           }
       }
   } else {
       QStringList mzs = mzpairs.split(SPACE);
       Q_FOREACH (QString mzstr, mzs) {
           float mz = mzstr.simplified().toDouble();
           if (mz > 0) _mzsList << mz;
       }
   }

   qDebug() << _msScanType;
   qDebug() << _precursorMz;
   qDebug() << _mzsList;
   qDebug() << _intensityList;
   qDebug() << _intensityMinErr;
   qDebug() << _intensityMaxErr;

}

void SpectraMatching::showScan() {
    qDebug() << "showScan()";
    Q_FOREACH(QTreeWidgetItem* item, resultTable->selectedItems() ) {

        QVariant v =   item->data(0,Qt::UserRole);
        unsigned int hitId=  v.value<int>();

        if (hitId >= 0 && hitId < matches.size()) {
            mainwindow->getSpectraWidget()->overlaySpectralHit(matches[hitId]);
        }
        break;
    }
}

void SpectraMatching::doSearch() {
    resultTable->clear();
    matches.clear();

    vector<mzSample*>samples = mainwindow->getVisibleSamples();
    resultTable->setEnabled(false);
    findButton->setEnabled(false);


    QString _algorithm = this->algorithm->currentText();

    for(int i=0; i < samples.size(); i++) {
        int nscans = samples[i]->scanCount();
        for(int j=0; j< nscans; j++) {
            Scan* scan = samples[i]->scans[j];
	    //if (scan->scannum != 7925) continue;
            double score=0;
            if(_algorithm == "Isotopic Pattern Search") {
               score = matchPattern(scan);
            } else if ( _algorithm == "Fragment Search") {
               score = scoreScan(scan);
            }

            //update progress
            if(j % 10 || j+1 == nscans) progressBar->setValue((j+1)/nscans*100);
        }

    }

    for(int i=0; i <matches.size(); i++ ) {
        SpectralHit& hit = matches[i];
        NumericTreeWidgetItem *item = new NumericTreeWidgetItem(resultTable,0);

        item->setData(0,Qt::UserRole,QVariant::fromValue(i));
        item->setText(0,QString::number(hit.score,'f',2));
        item->setText(1,QString::number(hit.scan->scannum));
        item->setText(2,QString::number(hit.precursorMz,'f',4));
        item->setText(3,QString::number(hit.matchCount));

        QString mzString;
        for(int i=0; i < hit.mzList.size(); i++ ) {
                mzString += tr("%1 [%2], ").arg(mzUtils::ppmround(hit.mzList[i],100000)).arg(round(hit.intensityList[i]));
        }
        item->setText(4,mzString);
    }

    if(matches.size() > 0 ) {
	    exportButton->setEnabled(true);
    	    resultTable->setEnabled(true);
    } else { 
	    exportButton->setEnabled(false);
    	   resultTable->setEnabled(false);
    }

    int Nbins=100;
    vector<unsigned int> bin(Nbins,0);
    float minscore=allscores.minimum();
    float maxscore=allscores.maximum();
    float binsize = (maxscore-minscore)/Nbins;
    allscores.histogram(bin,Nbins);

    qDebug() << "Histogram";
    for(int i=0; i <100; i++ ) {
	    qDebug() << i << " " << minscore+(i*binsize) << "\t" <<bin[i];
    }

    findButton->setEnabled(true);
    resultTable->sortItems(0,Qt::DescendingOrder);
    qDebug() << "search Done";
}

void SpectraMatching::addHit(double score, float precursormz, QString samplename, int matchCount, Scan* scan,QVector<double>&mzs,QVector<double>&ints) {
       SpectralHit hit;
       hit.score = score;
       hit.precursorMz=precursormz;
       hit.sampleName=samplename;
       hit.matchCount=matchCount;
       hit.scan = scan;
       hit.mzList = mzs;
       hit.intensityList=ints;
       hit.productMassCutoff=_productMassCutoff;
       matches.push_back(hit);
}

double SpectraMatching::scoreScan(Scan* scan) {

    if (_msScanType  > 0 && scan->mslevel != _msScanType) return 0;
    if (_precursorMz > 0 && mzUtils::massCutoffDist(_precursorMz,(double)scan->precursorMz,_precursorMassCutoff) > _precursorMassCutoff->getMassCutoff()) return 0;

   float score=0;
   int matchCount=0;
   int N = _mzsList.size();
   int Nc = _intensityList.size();
   int minMatches = minPeakMatches->value();

   float totalIntensity = scan->totalIntensity();


   QVector<float> x;
   QVector<float> y;
   for(int i=0; i < N; i++ ) {
       int pos = scan->findHighestIntensityPos(_mzsList[i],_productMassCutoff);
       if (pos >= 0) {
           matchCount++;
            if(Nc==0) { score += log(scan->intensity[pos]); }
            else { 
                x.push_back(_intensityList[i]); 
                y.push_back(scan->intensity[pos]/totalIntensity); 
            }
       } else { 
            if(Nc==0) { score--; } 
            else { 
                x.push_back(_intensityList[i]); 
                y.push_back(-_intensityList[i]);
            }
      }
   }

   if(Nc) score=mzUtils::correlation(x.toStdVector(),y.toStdVector());

   if (score > 0 and matchCount > minMatches ) {
       QString sampleName(scan->sample->sampleName.c_str());
       float precursorMz = _mzsList[0];
       addHit(score,precursorMz,sampleName,matchCount,scan,_mzsList,_intensityList);
   }
   return score;
}

double SpectraMatching::matchPattern(Scan* scan) {

   if (_msScanType  > 0 && scan->mslevel != _msScanType) return 0;
   int minMatches = minPeakMatches->value();

   //convert mzs to deltaMasses
   unsigned int N = _mzsList.size();

   QVector<double> patternMzsObserved(N,0);
   QVector<double> patternItensityObserved(N,0);
   QVector<double> patternIntensityGiven(N,0);
   QVector<double> deltaListGiven(N-1,0);
 
   //delta mass list
   for(int i=1; i<N; i++) {
	deltaListGiven[i-1]=_mzsList[0]-_mzsList[i]; 
   }

   //find largest intensity in a pattern
   double maxGivenIntensity=0;
   for(int i=0; i<_intensityList.size(); i++) { 
	   if(_intensityList[i] > maxGivenIntensity) maxGivenIntensity=_intensityList[i]; 
   }
  
   //normalize patern intensities by the biggest value
   for(int i=0; i<_intensityList.size(); i++) { 
	   patternIntensityGiven[i] = _intensityList[i]/maxGivenIntensity*100;
   }

   //compute max allowed difference between pattern and match SUM( patternIntensity**2 )
   double maxDiff=0;
   for(int i=0; i<patternIntensityGiven.size(); i++) { 
        //maxDiff += pow(patternIntensityGiven[i]/2,2);
	maxDiff += log(100);
   }

   //sliding window pattern search.. compare pattern to every peak in a scan
   int hitCount=0;
   for(int i=0; i<scan->nobs(); i++) {

       //first value to match
       double startMz = patternMzsObserved[0]=scan->mz[i];
       double maxObservedIntensity =patternItensityObserved[0]=scan->intensity[i];

       int matchCount=0;
       for(int j=0; j < deltaListGiven.size(); j++ ) {
           double expectedMz = startMz - deltaListGiven[j];
           int   pos = scan->findHighestIntensityPos(expectedMz,_productMassCutoff);

           if(pos >= 0 ) {
               matchCount++;
               if (scan->intensity[pos] > maxObservedIntensity) maxObservedIntensity = scan->intensity[pos];
               patternMzsObserved[j+1]=scan->mz[pos];
               patternItensityObserved[j+1]=scan->intensity[pos];
           } else {
               patternMzsObserved[j+1]=expectedMz;
               patternItensityObserved[j+1]=0;
           }
       }

       //score = 1-SUM((observedIntensity-givenIntsity)**2) / maxDifference
       double score=0;
       for(int k=0; k < N; k++ ) {
           patternItensityObserved[k] = patternItensityObserved[k]/maxObservedIntensity*100;
           double deltaIntensity =  abs(patternIntensityGiven[k]-patternItensityObserved[k]);
           double ratio =  abs((patternIntensityGiven[k]+1)/(patternItensityObserved[k]+1));

           //bound checking if specified
           if ( bound_checking_pattern) {
           	score+= abs(log(ratio));
               if(patternItensityObserved[k] < _intensityMinErr[k] || patternItensityObserved[k] > _intensityMaxErr[k]  ) {
                    score = maxDiff;
                    break;
                }
           } else if (k < _intensityMaxErr.size()) {
		float weight = _intensityMaxErr[k];
		score += weight*abs(log(ratio));
		// if (deltaIntensity/patternIntensityGiven[k] * 100 > _intensityMaxErr[k] ) {
		//    score = maxDiff;
		//   break;
		//}
           } else {
           	score+= abs(log(ratio));
	   }
       }
       //qDebug() << "b: " << patternItensityObserved;

       double scoreN = 1.0-(score/maxDiff);

       if( scoreN > -1 ) {
	       allscores.push_back(scoreN);
       }

       if(matchCount >= minMatches and scoreN > 0 ) {
           hitCount++;
           //    cerr << "i=" << i << "startMz=" << startMz << " maxObservedIntensity=" << maxObservedIntensity << endl;
           QString sampleName(scan->sample->sampleName.c_str());
           float precursorMz = patternMzsObserved[0];
           addHit(scoreN,precursorMz,sampleName,matchCount,scan,patternMzsObserved,patternItensityObserved);
           /* for(int k=0; k < N; k++ ) {
            fprintf(stderr, "%3.5f (%3.2f) (%3.2f)\n",
            patternMzsObserved[k],
            patternItensityObserved[k],
            patternIntensityGiven[k]);
            }
             cerr << "scoreN=" << scoreN << " score=" << score << " maxDiff=" << maxDiff << endl;
             */
       }
       //if(hitCount > 2 ) return scoreN;

   }

    return 0;
}

void SpectraMatching::exportMatches() { 
    
    if (matches.size() == 0 ) { return; }
    QString dir = ".";
    QSettings* settings = mainwindow->getSettings();

    QString groupsTAB = "Tab Delimited File (*.tab)";

    QString sFilterSel;
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Hits"), dir,  groupsTAB , &sFilterSel);

    if(fileName.isEmpty()) return;
    QFile myfile(fileName);
    if(!myfile.open(QIODevice::WriteOnly | QIODevice::Text)) return;

    QTextStream stream(&myfile);
    stream << "score\tprecursorMz\tsampleName\tscan\tmatchCount\timatches\tmzs\n";

    for(int i=0; i <matches.size(); i++ ) {
	    SpectralHit& hit = matches[i];
	    NumericTreeWidgetItem *item = new NumericTreeWidgetItem(resultTable,0);

	    QString mzString, intsString;
	    for(int i=0; i < hit.mzList.size(); i++ ) {
		    mzString += tr("%1 [%2], ").arg(mzUtils::ppmround(hit.mzList[i],100000)).arg(round(hit.intensityList[i]));
	    }

	    for(int i=0; i < hit.mzList.size(); i++ ) {
		    intsString += tr("%1,").arg(round(hit.intensityList[i]));
	    }

	    stream << QString::number(hit.score,'f',2)   << "\t"
               << hit.precursorMz << "\t"
               << hit.sampleName << "\t"
               << QString::number(hit.scan->scannum) << "\t"
	    	   << QString::number(hit.matchCount)    << "\t"
	    	   << intsString  << "\t"
	    	   << mzString << "\n";
    }
    myfile.close();
}
