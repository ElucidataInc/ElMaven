#include "comparesamplesdialog.h"

CompareSamplesDialog::CompareSamplesDialog(QWidget *parent) : 
	QDialog(parent) { 
		setupUi(this); 
        table = NULL;
		setModal(false);
        _qtype = PeakGroup::AreaTop;

		connect(compareButton, SIGNAL(clicked(bool)), SLOT(compareSamples()));
		connect(resetButton, SIGNAL(clicked(bool)), SLOT(resetSamples()));
		connect(cancelButton, SIGNAL(clicked(bool)), SLOT(cancel()));
		connect(filelist1, SIGNAL(itemSelectionChanged()), SLOT(updateSampleList()));
		connect(filelist2, SIGNAL(itemSelectionChanged()), SLOT(updateSampleList()));
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

CompareSamplesDialog::~CompareSamplesDialog() {}

void CompareSamplesDialog::cancel() {
    QDialog::close();
}

void CompareSamplesDialog::setTableWidget(TableDockWidget* w ) {
	if(!w) return;
    table = w;
	samples.clear();
	QList<PeakGroup*>allgroups =  table->getGroups();
	foreach (PeakGroup* group, allgroups) {
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

	foreach(mzSample* sample, samples ) {
         QString setname(sample->getSetName().c_str());
         QString sampleName(sample->sampleName.c_str());
         //if( setname.contains(setName1) ) filelist1->addItem(sampleName);
         //if( setname.contains(setName2) ) filelist2->addItem(sampleName);
	}
	*/
}


void CompareSamplesDialog::showEvent(QShowEvent *) { 
	if(!table || table->groupCount() == 0 ) return;

    filelist1->clear();
    filelist2->clear();

    QSet<QString>setnames;
	QRegExp splitStr("[\\t*|\\s*|,|;]");

	foreach(mzSample* sample, samples ) {
		QString qname( sample->getSetName().c_str());
		qname=qname.simplified();
		QList<QString> names = qname.split(splitStr);
		setnames.insert(qname);
		foreach(QString name, names) { name=name.simplified(); if(!name.isEmpty()) setnames.insert(name); }
	}

    foreach(QString name, setnames ) {
         filelist1->addItem(name);
         filelist2->addItem(name);
    }
	
	if (setnames.size() >= 2) {
		//filelist1->setCurrentIndex(0);
		//filelist2->setCurrentIndex(1);
	} else {
		QMessageBox msgBox;
 		msgBox.setText("Sets were not defined. Please map samples to sets in  the Samples Widget");
 		msgBox.exec(); return;
	}
}

vector<mzSample*> CompareSamplesDialog::getSampleSet(QListWidget *set) {
		vector<mzSample*>subset;

		foreach(mzSample* sample, samples ) {
			QString sampleName( sample->getSetName().c_str());

			for(int j=0; j < set->count(); j++ ) {
				if (set->item(j)->isSelected() == false) continue;

				QString setName=set->item(j)->text();
				if ( sampleName.contains(setName) )  { 
					qDebug() << setName << " sample:" << sampleName;
					subset.push_back(sample);
					break;
				}
			}
		}
		return subset;
}


void CompareSamplesDialog::compareSamples() {
	if(!table) return;

    vector<mzSample*> sset1 = getSampleSet(filelist1);
    vector<mzSample*> sset2 = getSampleSet(filelist2); 
    if ( sset1.size() == 0 || sset2.size() == 0 ) return;
	vector<mzSample*> sset3;
	for(int i=0; i < sset1.size(); i++ ) sset3.push_back(sset1[i]);
	for(int i=0; i < sset2.size(); i++ ) sset3.push_back(sset2[i]);
    compareSets();


}

void CompareSamplesDialog::shuffle(StatisticsVector<float>& groupA, StatisticsVector<float>& groupB ) {
    int n1 = groupA.size();
    int n2 = groupB.size();
    int n3 = n1+n2;
    if ( n1 == 0 || n2 == 0) return;
    MTRand mtrand;

    //combine two sets
    //cerr << "N3=" << n3 << endl;
    StatisticsVector<float>groupC(n1+n2);
    for(int i=0; i < n1; i++ ) groupC[i] = groupA[i];
    for(int i=0; i < n2; i++ ) groupC[n1+i] = groupB[i];
    //for(int i=0; i < n3; i++ ) cerr << groupC[i] << " "; cerr << endl;

    //float realT = ttest(groupA,ratio>100groupB);

    for (int s=0; s < 10; s++ ) {
        //shuffle combined set
        for(int i=0; i < n3; i++ ) {
            int r = mtrand.randInt(n3-1);
            assert(r<=n3-1);
            if ( i == r ) continue;
            float tmp = groupC[i]; groupC[i] = groupC[r]; groupC[r]=tmp; //swap
        }

        //split random vector
        StatisticsVector<float>tmpA(n1);
        StatisticsVector<float>tmpB(n2);
        for(int i=0; i < n1; i++ ) tmpA[i] = groupC[i];
        for(int i=0; i < n2; i++ ) tmpB[i] = groupC[n1+i];

        //for(int i=0; i < n1; i++ ) cerr << tmpA[i] << " "; cerr << endl;
        //for(int i=0; i < n2; i++ ) cerr << tmpB[i] << " "; cerr << endl;

        //ttest
        float randT = abs(mzUtils::ttest(tmpA,tmpB));
        //cerr << "RealT=" << realT << "  RandT=" << randT << endl;
        rand_scores.push_back(randT);
    }
/*
	std::sort(rand_scores.begin(), rand_scores.end());
	int rank = countBelow(rand_scores,  ttest(groupA,groupB));
	cerr << "Q=" << rank << " p-value=" << 1.0-(float) rank / rand_scores.size() << endl;
	*/
	
}

void CompareSamplesDialog::compareSets() {
	if (!table) return;

	vector<mzSample*> sampleSet;
    vector<mzSample*> sset1 = getSampleSet(filelist1);
    vector<mzSample*> sset2 = getSampleSet(filelist2);
	for(int i=0; i < sset1.size(); i++ ) sampleSet.push_back(sset1[i]);
	for(int i=0; i < sset2.size(); i++ ) sampleSet.push_back(sset2[i]);
	QList<PeakGroup*>allgroups =  table->getGroups();
	rand_scores.clear();


	for(int i=0; i < allgroups.size(); i++ ) {
		PeakGroup* group = allgroups[i];
		group->changeFoldRatio=0; 
		group->changePValue=1;
		//group->groupStatistics();
        vector<float> yvalues = group->getOrderedIntensityVector(sampleSet,_qtype);
		StatisticsVector<float>groupA(sset1.size());
		StatisticsVector<float>groupB(sset2.size());

		int missingSet1=0;
		int missingSet2=0;
		for(int i=0; i < sset1.size(); i++ ) { groupA[i]=yvalues[i]; if (groupA[i] == 0) missingSet1++; }
		for(int i=0; i < sset2.size(); i++ ) { groupB[i]=yvalues[ sset1.size()+i ]; if (groupB[i] ==0 ) missingSet2++; }

       	//replace missing values
    	float _missingValue  =   missingValue->value();
		for(int i=0; i < groupA.size(); i++ ) if(groupA[i]<_missingValue) groupA[i]=_missingValue;
		for(int i=0; i < groupB.size(); i++ ) if(groupB[i]<_missingValue) groupB[i]=_missingValue;

		//skip empty
		float meanA = abs(groupA.mean());
        float meanB = abs(groupB.mean());
		if (meanA==0) meanA=1;
		if (meanB==0) meanB=1;
		
		group->changeFoldRatio = meanA>meanB ? meanA/meanB : -meanB/meanA;
		group->changePValue= abs(mzUtils::ttest(groupA,groupB));
		shuffle(groupA,groupB);

		//qDebug() << "CompareSamplesDialog: " << i << " " << meanA << " " << meanB;
		emit(setProgressBar("CompareSamples", i+1, allgroups.size()));
    }

    float alpha = minPValue->value(); //alpha value

	//calculate Pvalues
	std::sort(rand_scores.begin(), rand_scores.end()); //sort random scores, 
	for(int i=0; i < allgroups.size(); i++ ) {
			PeakGroup* group = allgroups[i];
			if (group->changeFoldRatio == 0 ) continue;
			int rank = countBelow(rand_scores, group->changePValue); 	//calculate p-value
            //cerr << group->changePValue << " " <<  ((float) rank)/rand_scores.size() << endl;
			group->changePValue=1-((float) rank)/rand_scores.size();

	}

	//correct P-values (FDR)
	int correction = correctionBox->currentIndex();
	int Ngroups=allgroups.size();
	int j=0;
	sort(allgroups.begin(), allgroups.end(),PeakGroup::compPvalue);
	for(int i=0; i < Ngroups; i++ ) {
		PeakGroup* group = allgroups[i];
		if (group->changeFoldRatio == 0 ) continue;
		//cerr << group->changePValue << " " ;
		if (correction == 1 ) {	 	//Bonferroni
			group->changePValue *= Ngroups;
		} if (correction == 2 ) { 	//HOLMS
			group->changePValue *= Ngroups-j;
		} if (correction == 3 ) { 	//Benjamini
			group->changePValue *= Ngroups/(j+1);
		}
		//cerr << group->changePValue << endl;
		j++;
	}

	//if (table) { table->updateTable();}
    if (parentWidget()) ((ScatterPlot*) parentWidget())->replot();


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
	rand_scores.clear();
    allgroups.clear();
}
