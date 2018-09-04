#include "PeakDetectorCLI.h"

int main(int argc, char *argv[]) {

	// setting nodepath and jspath for PollyCLI library..
	QStringList jsPathlist = QString(argv[0]).split(QDir::separator());
	QStringList jsPathlist_bin;
	for (int i = 0; i < jsPathlist.size()-1; ++i)
         jsPathlist_bin << jsPathlist.at(i);
	QString jsPath = jsPathlist_bin.join(QDir::separator())+QDir::separator()+"index.js";
	QString nodePath;
	#ifdef Q_OS_WIN
      nodePath = jsPathlist_bin.join(QDir::separator()) + QDir::separator() + "node.exe";
    #endif

    #ifdef Q_OS_LINUX
      nodePath = jsPathlist_bin.join(QDir::separator()) + QDir::separator() + "node";
    #endif

    #ifdef Q_OS_MAC
      QString binDir = jsPathlist_bin.join(QDir::separator()) + QDir::separator() + ".." + QDir::separator() + ".." + QDir::separator() + ".." + QDir::separator();
      if(!QStandardPaths::findExecutable("node", QStringList() << binDir + "node_bin" + QDir::separator() ).isEmpty())
        nodePath = binDir + "node_bin" + QDir::separator() + "node";

      jsPath = binDir  + "index.js";
    #endif
	// Polly CLI part over..

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
        peakdetectorCLI->peakDetector->alignSamples((int)peakdetectorCLI->alignType);
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
		peakdetectorCLI->writeReport("compounds",jsPath,nodePath);
	}
	else if (!(peakdetectorCLI->pollyArgs.isEmpty())){
		qDebug()<<"no peaks found..Please try again with different parameters";
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
