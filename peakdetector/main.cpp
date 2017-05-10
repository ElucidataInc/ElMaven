#include "PeakDetectorCLI.h"

int main(int argc, char *argv[]) {

    PeakDetectorCLI* peakdetectorCLI = new PeakDetectorCLI();

    double programStartTime = getTime();

	//read command line options
	peakdetectorCLI->processOptions(argc, argv);

	if (!peakdetectorCLI->status) {
		cerr << peakdetectorCLI->textStatus;
		return(0);
	}

	//load classification model
	peakdetectorCLI->loadClassificationModel(peakdetectorCLI->clsfModelFilename);

	//set Maven Parameters
	peakdetectorCLI->peakDetector->setMavenParameters(peakdetectorCLI->mavenParameters);

	//load compounds file
	peakdetectorCLI->loadCompoundsFile();

	//load files
	peakdetectorCLI->loadSamples(peakdetectorCLI->filenames);

	//get retention time resolution
	peakdetectorCLI->mavenParameters->setAverageScanTime();

	//ionization
	peakdetectorCLI->mavenParameters->setIonizationMode();

	//align samples
	if (peakdetectorCLI->mavenParameters->samples.size() > 1 && peakdetectorCLI->mavenParameters->alignSamplesFlag){
		peakdetectorCLI->peakDetector->alignSamples();
	}


	//process compound list
	if (peakdetectorCLI->mavenParameters->compounds.size()) {
		vector<mzSlice*> slices = peakdetectorCLI->peakDetector->processCompounds(
				peakdetectorCLI->mavenParameters->compounds, "compounds");
		peakdetectorCLI->peakDetector->processSlices(slices, "compounds");
		if (peakdetectorCLI->mavenParameters->pullIsotopesFlag) peakdetectorCLI->peakDetector->pullAllIsotopes();
		peakdetectorCLI->writeReport("compounds");
		delete_all(slices);
	}

	//process all mass slices
	if (peakdetectorCLI->mavenParameters->processAllSlices == true) {
		peakdetectorCLI->mavenParameters->matchRtFlag = false;
		peakdetectorCLI->mavenParameters->checkConvergance = true;
		peakdetectorCLI->peakDetector->processMassSlices();
	}

	//cleanup
	delete_all(peakdetectorCLI->mavenParameters->samples);
	peakdetectorCLI->mavenParameters->samples.clear();
	peakdetectorCLI->mavenParameters->allgroups.clear();

    cerr << "\n\nTotal program execution time : " << getTime() - programStartTime << " seconds \n" << endl;
	return(0);
}