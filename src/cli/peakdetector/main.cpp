#include <QCoreApplication>

#include "common/analytics.h"
#include "common/logger.h"
#include "mavenparameters.h"
#include "peakdetectorcli.h"

int main(int argc, char *argv[]) {

    QCoreApplication app(argc, argv);
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

    QString parentFolder = "ElMaven";
    QString logFile = QString::fromStdString(Logger::constant_time()
                                             + "_peakdetector_cli.log");
    QString fpath = QStandardPaths::writableLocation(
                        QStandardPaths::GenericConfigLocation)
                    + QDir::separator()
                    + parentFolder
                    + QDir::separator()
                    + logFile;
    Logger log(fpath.toStdString(), true);
    Analytics analytics;
    analytics.sessionStart();
    PeakDetectorCLI* peakdetectorCLI = new PeakDetectorCLI(&log,
                                                           &analytics,
                                                           argc,
                                                           argv);

#ifndef __APPLE__
    double programStartTime = getTime();
#endif
    //read command line options
    peakdetectorCLI->processOptions(argc, argv);

    if (!peakdetectorCLI->status) {
        cerr << peakdetectorCLI->textStatus;
        analytics.sessionEnd();
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
    if (peakdetectorCLI->mavenParameters->samples.size() > 1 && peakdetectorCLI->mavenParameters->alignSamplesFlag) {
        peakdetectorCLI->alignSamples((int)peakdetectorCLI->alignMode);
	}


	//process compound list
    if (peakdetectorCLI->mavenParameters->compounds.size()
        && !peakdetectorCLI->mavenParameters->processAllSlices) {
        peakdetectorCLI->peakDetector->processCompounds(
            peakdetectorCLI->mavenParameters->compounds);
	}

	//process all mass slices
	if (peakdetectorCLI->mavenParameters->processAllSlices == true) {
		peakdetectorCLI->mavenParameters->matchRtFlag = false;
        peakdetectorCLI->peakDetector->processFeatures();
	}

	//write report
	if (peakdetectorCLI->mavenParameters->allgroups.size() > 0) {
		peakdetectorCLI->writeReport("compounds",jsPath,nodePath);
        peakdetectorCLI->exportPeakReport("peakReport");
        peakdetectorCLI->exportSampleReport("sampleReport");
        if (peakdetectorCLI->saveAnalysisAsProject())
            peakdetectorCLI->saveEmdb();
    } else if (!(peakdetectorCLI->pollyArgs.isEmpty())){
        log.info() << "No peaks found. Please try again with different "
                      "parameters."
                   << flush;
    }

    //cleanup
    delete_all(peakdetectorCLI->mavenParameters->samples);
    peakdetectorCLI->mavenParameters->samples.clear();
    // clearing `allgroups` crashes on Windows - also undebuggable
    // peakdetectorCLI->mavenParameters->allgroups.clear();
    delete peakdetectorCLI;

#ifndef __APPLE__
    cout << "\n\nTotal program execution time : " << getTime() - programStartTime << " seconds \n" << endl;
#endif
    analytics.sessionEnd();
    return(0);
}
