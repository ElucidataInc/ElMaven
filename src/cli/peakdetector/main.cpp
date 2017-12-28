#include "PeakDetectorCLI.h"

int main(int argc, char *argv[]) {

    PeakDetectorCLI* peakdetectorCLI = new PeakDetectorCLI();

     #ifndef __APPLE__
     double programStartTime = getTime();
     #endif
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
	if (peakdetectorCLI->mavenParameters->processAllSlices == false) peakdetectorCLI->loadCompoundsFile();

	//load files
	peakdetectorCLI->loadSamples(peakdetectorCLI->filenames);

	//get retention time resolution
	peakdetectorCLI->mavenParameters->setAverageScanTime();

	//ionization
	peakdetectorCLI->mavenParameters->setIonizationMode(MavenParameters::AutoDetect);

	//align samples
	if (peakdetectorCLI->mavenParameters->samples.size() > 1 && peakdetectorCLI->mavenParameters->alignSamplesFlag){
		peakdetectorCLI->peakDetector->alignSamples();
	}


	//process compound list
	if (peakdetectorCLI->mavenParameters->compounds.size() && !peakdetectorCLI->mavenParameters->processAllSlices) {
		vector<mzSlice*> slices = peakdetectorCLI->peakDetector->processCompounds(
				peakdetectorCLI->mavenParameters->compounds, "compounds");
		peakdetectorCLI->peakDetector->processSlices(slices, "compounds");
		if (peakdetectorCLI->mavenParameters->pullIsotopesFlag) {
			peakdetectorCLI->peakDetector->pullAllIsotopes();
		}
		delete_all(slices);
	}

	//process all mass slices
	if (peakdetectorCLI->mavenParameters->processAllSlices == true) {
		peakdetectorCLI->mavenParameters->matchRtFlag = false;
		peakdetectorCLI->mavenParameters->checkConvergance = true;
		peakdetectorCLI->peakDetector->processMassSlices();
	}

	//write report
	if (peakdetectorCLI->mavenParameters->allgroups.size() > 0) {
		peakdetectorCLI->writeReport("compounds");
	}

	//cleanup
	delete_all(peakdetectorCLI->mavenParameters->samples);
	peakdetectorCLI->mavenParameters->samples.clear();
	peakdetectorCLI->mavenParameters->allgroups.clear();

     #ifndef __APPLE__
     cout << "\n\nTotal program execution time : " << getTime() - programStartTime << " seconds \n" << endl;
     #endif
	return(0);
}
