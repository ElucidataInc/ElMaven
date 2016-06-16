#include "comparesampleslogic.h"

CompareSamplesLogic::CompareSamplesLogic() {
}



void CompareSamplesDialog::compareSets() {
	if (!table)
		return;

	vector<mzSample*> sampleSet;
	vector<mzSample*> sset1 = getSampleSet(filelist1);
	vector<mzSample*> sset2 = getSampleSet(filelist2);
	for (int i = 0; i < sset1.size(); i++)
		sampleSet.push_back(sset1[i]);
	for (int i = 0; i < sset2.size(); i++)
		sampleSet.push_back(sset2[i]);
	QList<PeakGroup*> allgroups = table->getGroups();
	rand_scores.clear();

	for (int i = 0; i < allgroups.size(); i++) {
		PeakGroup* group = allgroups[i];
		group->changeFoldRatio = 0;
		group->changePValue = 1;
		//group->groupStatistics();
		vector<float> yvalues = group->getOrderedIntensityVector(sampleSet,
				_qtype);
		StatisticsVector<float> groupA(sset1.size());
		StatisticsVector<float> groupB(sset2.size());

		int missingSet1 = 0;
		int missingSet2 = 0;
		for (int i = 0; i < sset1.size(); i++) {
			groupA[i] = yvalues[i];
			if (groupA[i] == 0)
				missingSet1++;
		}
		for (int i = 0; i < sset2.size(); i++) {
			groupB[i] = yvalues[sset1.size() + i];
			if (groupB[i] == 0)
				missingSet2++;
		}

		//replace missing values
		float _missingValue = missingValue->value();
		for (int i = 0; i < groupA.size(); i++)
			if (groupA[i] < _missingValue)
				groupA[i] = _missingValue;
		for (int i = 0; i < groupB.size(); i++)
			if (groupB[i] < _missingValue)
				groupB[i] = _missingValue;

		//skip empty
		float meanA = abs(groupA.mean());
		float meanB = abs(groupB.mean());
		if (meanA == 0)
			meanA = 1;
		if (meanB == 0)
			meanB = 1;

		group->changeFoldRatio = meanA > meanB ? meanA / meanB : -meanB / meanA;
		group->changePValue = abs(mzUtils::ttest(groupA, groupB));
		shuffle(groupA, groupB);

		//qDebug() << "CompareSamplesDialog: " << i << " " << meanA << " " << meanB;
		emit(setProgressBar("CompareSamples", i + 1, allgroups.size()));
	}

	float alpha = minPValue->value(); //alpha value

	//calculate Pvalues
	std::sort(rand_scores.begin(), rand_scores.end()); //sort random scores,
	for (int i = 0; i < allgroups.size(); i++) {
		PeakGroup* group = allgroups[i];
		if (group->changeFoldRatio == 0)
			continue;
		int rank = countBelow(rand_scores, group->changePValue); //calculate p-value
		//cerr << group->changePValue << " " <<  ((float) rank)/rand_scores.size() << endl;
		group->changePValue = 1 - ((float) rank) / rand_scores.size();

	}

	//correct P-values (FDR)
	int correction = correctionBox->currentIndex();
	int Ngroups = allgroups.size();
	int j = 0;
	sort(allgroups.begin(), allgroups.end(), PeakGroup::compPvalue);
	for (int i = 0; i < Ngroups; i++) {
		PeakGroup* group = allgroups[i];
		if (group->changeFoldRatio == 0)
			continue;
		//cerr << group->changePValue << " " ;
		if (correction == 1) {	 	//Bonferroni
			group->changePValue *= Ngroups;
		}
		if (correction == 2) { 	//HOLMS
			group->changePValue *= Ngroups - j;
		}
		if (correction == 3) { 	//Benjamini
			group->changePValue *= Ngroups / (j + 1);
		}
		//cerr << group->changePValue << endl;
		j++;
	}

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
	rand_scores.clear();
	allgroups.clear();
}
