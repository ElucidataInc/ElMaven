#include <omp.h>

#include <boost/signals2.hpp>

#include <QLabel>
#include <QStringList>
#include <QTextStream>

#include "base64.h"
#include "common/analytics.h"
#include "Compound.h"
#include "datastructures/adduct.h"
#include "errorcodes.h"
#include "globals.h"
#include "ligandwidget.h"
#include "mainwindow.h"
#include "MavenException.h"
#include "mavenparameters.h"
#include "mzAligner.h"
#include "mzfileio.h"
#include "mzrolldbconverter.h"
#include "mzSample.h"
#include "mzUtils.h"
#include "projectdatabase.h"
#include "projectdockwidget.h"
#include "projectsaveworker.h"
#include "Scan.h"
#include "tabledockwidget.h"

mzFileIO::mzFileIO(QWidget*) {
    sampleId = 0;
    _mainwindow = NULL;
    _stopped = true;
    _currentProject = nullptr;
    _encounteredMemoryError = false;

    qRegisterMetaType<QList<QString>>("QList<QString>");
    qRegisterMetaType<map<string, variant>>("map<string, variant>");
    connect(this,
            SIGNAL(sampleLoaded()),
            this,
            SLOT(_postSampleLoadOperations()));
    connect(this,
            SIGNAL(appSettingsUpdated()),
            SLOT(_readSamplesFromCurrentSQLiteProject()));

    _sqliteDbLoadInProgress = false;
    _sqliteDbSaveInProgress = false;
}

void mzFileIO::setMainWindow(MainWindow* mw)
{
    _mainwindow=mw;
}

void mzFileIO::loadSamples(QStringList& files) {
	Q_FOREACH(QString file, files){ addFileToQueue(file); }
	if (filelist.size() > 0) start();
    //setFileList(filenames);
    //start();
}

mzSample* mzFileIO::loadSample(const QString& filename){

    //check if file exists
    QFile file(filename);
    QString sampleName = file.fileName();	//only name of the file, without folder location

    if (!file.exists() ) { 	//couldn't find this file.. check local directory
        return nullptr;
    }

    sampleName.replace(QRegExp(".*/"),"");
    sampleName.replace(".mzXML","",Qt::CaseInsensitive);
    sampleName.replace(".mzML","",Qt::CaseInsensitive);
    sampleName.replace(".cdf","",Qt::CaseInsensitive);

    if (sampleName.isEmpty()) return NULL;
    mzSample* sample = NULL;

    sample = new mzSample();
    try {
        sample->loadSample( filename.toStdString() );
    } catch (const std::bad_alloc&) {
        cerr << "MemoryError: " << "ran out of memory" << endl;
        mzUtils::delete_all(sample->scans);
        _encounteredMemoryError = true;
    }
    if (sample->scans.empty()) {
        delete sample;
        sample = nullptr;
    }

    if ( sample && sample->scans.size() > 0 ) {
        if (sample->sampleNumber > 0){
            qDebug() << sampleName;
            QString sampleNumber =
                sample->sampleNumber != -1 ? QString::number(sample->sampleNumber)
                                           : "NA";
            QString sampleNumberInfo = " | Sample Number=" + sampleNumber;
            sampleName = sampleName + sampleNumberInfo;
        }

        sample->sampleName = string( sampleName.toLatin1().data() );
        
        mtxSampleId.lock();
        sample->setSampleId(++sampleId);
        mtxSampleId.unlock();

        return sample;
    }
    return NULL;
}

int mzFileIO::loadMassBankLibrary(QString fileName) {
    qDebug() << "Loading Nist Libary: " << fileName;
    QFile data(fileName);

    if (!data.open(QFile::ReadOnly) ) {
        qDebug() << "Can't open " << fileName; 
        return 0;
    }

    string dbfilename = fileName.toStdString();
    string dbname = mzUtils::cleanFilename(dbfilename);

   QTextStream stream(&data);

   /* sample
ACCESSION: PR100458
RECORD_TITLE: Cyanidin-3-O-(2''-O-beta-glucopyranosyl-beta-glucopyranoside); LC-ESI-QTOF; MS2; CE:Ramp 5-60 V; [M]+
CH$NAME: Cyanidin-3-O-(2''-O-beta-glucopyranosyl-beta-glucopyranoside)
CH$NAME: Cy 3-Soph
CH$NAME: cyanidin-3-sophoroside
CH$COMPOUND_CLASS: Anthocyanidin
CH$FORMULA: C27H31O16
CH$EXACT_MASS: 611.16121
CH$SMILES: c(c(c([o+1]4)c(cc(c(O)5)c(cc(O)c5)4)OC(O2)C(OC(O3)C(C(C(C3CO)O)O)O)C(C(C2CO)O)O)1)c(O)c(O)cc1
CH$IUPAC: InChI=1S/C27H30O16/c28-7-17-19(34)21(36)23(38)26(41-17)43-25-22(37)20(35)18(8-29)42-27(25)40-16-6-11-13(32)4-10(30)5-15(11)39-24(16)9-1-2-12(31)14(33)3-9/h1-6,17-23,25-29,34-38H,7-8H2,(H3-,30,31,32,33)/p+1/t17-,18-,19-,20-,21+,22+,23-,25-,26+,27-/m1/s1
CH$LINK: CAS 38820-68-7
CH$LINK: CHEMSPIDER 9344547
CH$LINK: KEGG C16306
CH$LINK: KNAPSACK C00006658
CH$LINK: PUBCHEM CID:11169452
PK$NUM_PEAK: 4
PK$PEAK: m/z int. rel.int.
  213.0567 47.98 11
  287.0564 4418 999
  288.0616 60.38 14
  611.1612 927.9 210
//
   */

   QRegExp whiteSpace("\\s+");
   QRegExp formulaMatch("(C\\d+H\\d+\\S*)");

   int charge=0;
   QString line;
   QString id,name, comment,formula,title;
   QStringList compound_class;
   QStringList alias;
   double mw=0;
   double precursor=0;
   int peaks=0;
   vector<float>mzs;
   vector<float>intest;

   int compoundCount=0;

    do {
        line = stream.readLine();

        if(line.startsWith("//",Qt::CaseInsensitive) && !name.isEmpty()) {
            if (!name.isEmpty()) { //insert new compound
               Compound* cpd = new Compound( id.toStdString(), name.toStdString(), formula.toStdString(), charge);
               cpd->setPrecursorMz(precursor);
               cpd->setDb(dbname);
               cpd->setFragmentMzValues(mzs);
               cpd->setFragmentIntensities(intest);
               vector<string> category;
               Q_FOREACH (QString cat, compound_class) { category.push_back(cat.toStdString()); }
                           cpd->setCategory(category);
               DB.addCompound(cpd);
               compoundCount++;
            }

            //reset for the next record
           name = comment = formula = title=QString();
		   compound_class = alias = QStringList();
           mw=precursor=0;
           peaks=0;
           mzs.clear();
           intest.clear();
        }

         if(line.startsWith("ACCESSION:",Qt::CaseInsensitive)) {
             id = line.mid(10,line.length()).simplified();
			 //qDebug() << "ID=" << id;
         } else if (line.startsWith("CH$NAME:",Qt::CaseInsensitive)) {
             QString aliasname = line.mid(9,line.length()).simplified();
			 alias << aliasname;
			 if (name.isEmpty() ) name = aliasname;
			 //qDebug() << "NAME=" << name;
         } else if (line.startsWith("CH$COMPOUND_CLASS:",Qt::CaseInsensitive)) {
             QString comp_class = line.mid(19,line.length()).simplified();
			 compound_class << comp_class;
         } else if (line.startsWith("CH$EXACT_MASS:",Qt::CaseInsensitive)) {
             precursor = line.mid(14,line.length()).simplified().toDouble();
			 //qDebug() << "PRECURSOR=" << precursor;
         } else if (line.startsWith("CH$FORMULA:",Qt::CaseInsensitive)) {
            formula = line.mid(12,line.length()).simplified();
			 //qDebug() << "FORMULA=" << formula;
         } else if (line.startsWith("PK$NUM_PEAK:",Qt::CaseInsensitive)) {
             peaks = line.mid(12,line.length()).simplified().toInt();
			 //qDebug() << "NUM_PEAK=" << peaks;
         } else if (line.startsWith("RECORD_TITLE:",Qt::CaseInsensitive)) {
             title = line.mid(13,line.length()).simplified();
         } else if (line.startsWith("PK$PEAK:",Qt::CaseInsensitive)) {
			 continue;
         } else if ( peaks != 0 ) {
			 line = line.simplified();
             QStringList mzintpair = line.split(whiteSpace);
             if( mzintpair.size() >=2 ) {
                 bool ok=false; bool ook=false;
                 float mz = mzintpair.at(0).toDouble(&ok);
                 float ints = mzintpair.at(1).toDouble(&ook);
                 if (ok && ook && mz >= 0 && ints >= 0) {
                     mzs.push_back(mz); intest.push_back(ints);
			 		 //qDebug() << "PEAK=" << mz << ints;
                 }
             }
         }
    } while (!line.isNull());
    return compoundCount;
}

void mzFileIO::run(void)
{
    if (_sqliteDbLoadInProgress) {
        auto samples = _mainwindow->getSamples();
        _currentProject->updateSamples(samples);
        Q_EMIT(sqliteDBSamplesLoaded());

        Q_EMIT(updateStatusString(tr("Loading alignment data…")));
        _currentProject->loadAndPerformAlignment(samples);
        Q_EMIT(sqliteDBAlignmentDone());

        _readPeakTablesFromSQLiteProject(samples);

        // assuming this was the last step in loading of a SQLite project
        Q_EMIT(projectLoaded());

        _sqliteDbLoadInProgress = false;
        quit();
        return;
    }

    try {
        fileImport();
    } catch (std::exception& excp) {
        // ask user to send back the logs
    } catch (...) {
        // ask user to send back the logs
        qDebug() << "uploading samples failed";
    }
    quit();
}

void mzFileIO::fileImport(void) {
    if ( filelist.size() == 0 ) return;
    Q_EMIT (updateProgressBar( "Importing files", 0, filelist.size()));

    _mainwindow->getProjectWidget()->boostSignal.connect(boost::bind(&mzFileIO::qtSlot, this, _1, _2, _3));
    QStringList samples;
    QStringList peaks;
    QStringList projects;
    QStringList compoundsDatabases;
    QStringList unsupportedFileList;
    bool fileNotSupported = false;

    Q_FOREACH (QString filename, filelist) {
        try {
            QFileInfo fileInfo(filename);
            if (!fileInfo.exists())
                throw MavenException(ErrorMsg::FileNotFound);

            if (isSampleFileType(filename)) {
                samples << filename;
            } else if (isProjectFileType(filename)) {
                projects << filename;
            } else if (isPeakListType(filename)) {
                peaks << filename;
            } else if (isCompoundDatabaseType(filename)) {
                compoundsDatabases << filename;
            } else {
                fileNotSupported = true;
                unsupportedFileList << filename;
                filelist.removeOne(filename);
            }
        } catch (MavenException& excp) {
            qDebug() << "Error: " << excp.what();
        }
    }

    if (fileNotSupported) {
        Q_EMIT(unsupportedFileFormat(unsupportedFileList));
        Q_EMIT (updateProgressBar( "Importing errors", 0, 0));
    }

    Q_FOREACH(QString filename, projects ) {
        if (isMzRollProject(filename)) {
            _mainwindow->ligandWidget->loadCompoundDBMzroll(filename);
            _mainwindow->projectDockWidget->loadMzRollProject(filename);
            QRegExp rxtable("*_table\-[0-9]*");
            rxtable.setPatternSyntax(QRegExp::Wildcard);
            QRegExp rxbm("*_bookmarkedPeaks*");
            rxbm.setPatternSyntax(QRegExp::Wildcard);
            if(rxtable.exactMatch(filename)) {
                Q_EMIT(createPeakTableSignal(filename));
            } else {
                _mainwindow->projectDockWidget->setLastOpenedProject(filename);
                auto groups = readGroupsXML(filename);
                for (auto group : groups) {
                    _mainwindow->bookmarkedPeaks->addPeakGroup(group);
                }
                _mainwindow->bookmarkedPeaks->showAllGroups();
            }
        } else if (isEmdbProject(filename) || isMzrollDbProject(filename)) {
            filename = openSQLiteProject(filename);
            auto fileInfo = QFileInfo(filename);

            if (!_currentProject->openConnection()) {
                Q_EMIT(sqliteDBUnrecognizedVersion(fileInfo.fileName()));
            } else {
                Q_EMIT(sqliteDBLoadStarted(fileInfo.fileName()));
                _beginSQLiteProjectLoad();
            }
        }
    }

    Q_FOREACH(QString filename, peaks ) {
        QFileInfo fileInfo(filename);
        TableDockWidget* tableX = _mainwindow->addPeaksTable();
        auto groups = readGroupsXML(filename);
        for (auto group : groups) {
            _mainwindow->bookmarkedPeaks->addPeakGroup(group);
        }
        _mainwindow->bookmarkedPeaks->showAllGroups();
    }

    // check if the user is trying to upload any cdf file. if, yes then disable
    // mulitprocessing. store the value in temp and restore it once done
    int uploadMultiprocessing = _mainwindow->getSettings()->value("uploadMultiprocessing").toInt();
    Q_FOREACH(const QString& filename, samples) {
      if(filename.endsWith(".nc", Qt::CaseInsensitive) || filename.endsWith(".cdf", Qt::CaseInsensitive)) {
          uploadMultiprocessing = 0;
          break;
      }
    }

    int numMS1SamplesLoaded = 0;
    int numMS2SamplesLoaded = 0;
    int numPRMSamplesLoaded = 0;
    QList<QString> samplesFailedToLoad;
    qDebug() << "uploadMultiprocessing: " <<  uploadMultiprocessing << endl;
    if (uploadMultiprocessing) {
        int iter = 0;
        #pragma omp parallel for shared(iter)
        for (int i = 0; i < samples.size(); i++) {

            QString filename = samples.at(i);
            if (_encounteredMemoryError) {
                samplesFailedToLoad.append(filename);
                continue;
            }

            mzSample* sample = loadSample(filename);
            if (sample && sample->scans.size() > 0) {
                emit addNewSample(sample);

                if (sample->ms1ScanCount() && sample->ms2ScanCount() == 0) {
                    ++numMS1SamplesLoaded;
                } else if (sample->ms1ScanCount() == 0
                           && sample->ms2ScanCount()) {
                    ++numMS2SamplesLoaded;
                } else if (sample->ms1ScanCount() && sample->ms2ScanCount()) {
                    ++numPRMSamplesLoaded;
                }
            } else {
                samplesFailedToLoad.append(filename);
            }

            #pragma omp atomic
            iter++;

            Q_EMIT (updateProgressBar( tr("Importing file %1").arg(filename), iter, samples.size()));
        }
    } else {
        int iter = 0;
        for (int i = 0; i < samples.size(); i++) {
            QString filename = samples.at(i);
            mzSample* sample = loadSample(filename);
            if (sample && sample->scans.size() > 0) {
                _mainwindow->addSample(sample);

                if (sample->ms1ScanCount() && sample->ms2ScanCount() == 0) {
                    ++numMS1SamplesLoaded;
                } else if (sample->ms1ScanCount() == 0
                           && sample->ms2ScanCount()) {
                    ++numMS2SamplesLoaded;
                } else if (sample->ms1ScanCount() && sample->ms2ScanCount()) {
                    ++numPRMSamplesLoaded;
                }
            } else {
                samplesFailedToLoad.append(filename);
            }

            iter++;

            Q_EMIT (updateProgressBar( tr("Importing file %1").arg(filename), iter, samples.size()));

        }
    }

    if (numMS1SamplesLoaded)
        _mainwindow->getAnalytics()->hitEvent("Samples Loaded",
                                              "MS1",
                                              "",
                                              numMS1SamplesLoaded);
    if (numMS2SamplesLoaded)
        _mainwindow->getAnalytics()->hitEvent("Samples Loaded",
                                              "MS2",
                                              "",
                                              numMS2SamplesLoaded);
    if (numPRMSamplesLoaded)
        _mainwindow->getAnalytics()->hitEvent("Samples Loaded",
                                              "PRM",
                                              "",
                                              numPRMSamplesLoaded);

    Q_EMIT(updateStatusString("Loading compounds…"));
    map<QString, int> databaseCompoundCounts;
    Q_FOREACH (QString filename, compoundsDatabases) {
        int compoundCount = loadCompoundsFromFile(filename);
        databaseCompoundCounts[filename] = compoundCount;
    }

    Q_EMIT(updateProgressBar("Done importing", samples.size(), samples.size()));
    if (samples.size() > 0)
        Q_EMIT(sampleLoaded());
    if (samplesFailedToLoad.size() > 0)
        Q_EMIT(sampleLoadFailed(samplesFailedToLoad, _encounteredMemoryError));
    if (projects.size() > 0)
        Q_EMIT(projectLoaded());
    if (peaks.size() > 0)
        Q_EMIT(peaklistLoaded());
    for (auto dbEntry : databaseCompoundCounts)
        Q_EMIT(compoundsLoaded(dbEntry.first, dbEntry.second));

    // clear queue and reset last state
    filelist.clear();
    _encounteredMemoryError = false;
}

void mzFileIO::qtSlot(const string& progressText, unsigned int completed_samples, int total_samples)
{
        Q_EMIT(updateProgressBar(QString::fromStdString(progressText), completed_samples, total_samples));

}

int mzFileIO::loadCompoundsFromFile(QString filename)
{
   int compoundCount = 0;
   if (filename.endsWith("msp", Qt::CaseInsensitive)
       || filename.endsWith("sptxt", Qt::CaseInsensitive)) {
       boost::signals2::signal<void (string, int, int)> signal;
       signal.connect([&](string message, int currentSteps, int totalSteps) {
           Q_EMIT(updateProgressBar(QString::fromStdString(message),
                                    currentSteps,
                                    totalSteps));
       });
       compoundCount = DB.loadNISTLibrary(filename.toStdString(), &signal);
   } else if (filename.endsWith("mgf", Qt::CaseInsensitive)) {
       boost::signals2::signal<void (string, int, int)> signal;
       signal.connect([&](string message, int currentSteps, int totalSteps) {
           Q_EMIT(updateProgressBar(QString::fromStdString(message),
                                    currentSteps,
                                    totalSteps));
       });
       compoundCount = DB.loadMascotLibrary(filename.toStdString(), &signal);
   } else if (filename.endsWith("massbank", Qt::CaseInsensitive)) {
       compoundCount = loadMassBankLibrary(filename);
   } else if (filename.contains("csv", Qt::CaseInsensitive)
              || filename.contains("tab", Qt::CaseInsensitive)) {
       compoundCount = DB.loadCompoundCSVFile(filename.toStdString());
   }
   return compoundCount;
}

bool mzFileIO::isKnownFileType(QString filename) {
    if (isSampleFileType(filename))  return true;
    if (isProjectFileType(filename)) return true;
    if (isPeakListType(filename)) return true;
    if (isCompoundDatabaseType(filename)) return true;
    return false;
}

bool mzFileIO::isCompoundDatabaseType(QString filename)
{
    QStringList extList;
    extList << ".csv" << ".tab" << "msp" << "sptxt" << "mgf" << "massbank";
    Q_FOREACH (QString suffix, extList) {
        if (filename.endsWith(suffix, Qt::CaseInsensitive))
            return true;
    }
    return false;
}

bool mzFileIO::isSampleFileType(QString filename) {
    QStringList extList;
    extList << "mzXML" << "cdf" << "nc" << "mzML" << "mzML";
    Q_FOREACH (QString suffix, extList) {
        if (filename.endsWith(suffix,Qt::CaseInsensitive)) return true;
    }
    return false;
}

bool mzFileIO::isProjectFileType(QString filename) {
    return (isMzRollProject(filename)
            || isEmdbProject(filename)
            || isMzrollDbProject(filename));
}

bool mzFileIO::isMzRollProject(QString filename)
{
    if (filename.endsWith("mzroll", Qt::CaseInsensitive))
        return true;
    return false;
}

bool mzFileIO::isEmdbProject(QString filename)
{
    if (filename.endsWith("emDB", Qt::CaseInsensitive))
        return true;
    return false;
}

bool mzFileIO::isMzrollDbProject(QString filename)
{
    if (filename.endsWith("mzrollDB", Qt::CaseInsensitive))
        return true;
    return false;
}

void mzFileIO::insertSettingForSave(const string key, const variant var)
{
    _settingsMap[key] = var;
}

variant mzFileIO::querySavedSetting(const string key) const
{
    if (_currentProject != nullptr) {
        auto settings = _currentProject->loadGlobalSettings();
        if (settings.count(key))
            return settings.at(key);
    }
    return variant(string());
}

bool mzFileIO::sqliteProjectIsOpen()
{
    return _currentProject != nullptr;
}

void mzFileIO::closeSQLiteProject()
{
    if (!_currentProject)
        return;

    delete _currentProject;
    _currentProject = nullptr;
}

void mzFileIO::writeGroups(QList<PeakGroup*> groups, QString tableName)
{
    _sqliteDbSaveInProgress = true;
    vector<PeakGroup*> groupVector;
    set<Compound*> compoundSet;
    if (_currentProject) {
        _currentProject->deleteTableGroups(tableName.toStdString());
        MavenParameters* mp = _mainwindow->mavenParameters;
        for (auto group : groups) {
            // assuming all groups are parent groups.
            groupVector.push_back(group);
            if (group->hasCompoundLink()) {
                auto compound = group->getCompound();
                compound->setCharge(mp->getCharge(compound));
                compoundSet.insert(compound);
            }
        }
        _currentProject->saveGroups(groupVector, tableName.toStdString());
        _currentProject->saveCompounds(compoundSet);
        Q_EMIT(updateStatusString(QString("Saved %1 groups from %2 to project")
                                  .arg(QString::number(groups.size()))
                                  .arg(tableName))
        );
    }
    _sqliteDbSaveInProgress = false;
}

void mzFileIO::updateGroup(PeakGroup* group, QString tableName)
{
    _sqliteDbSaveInProgress = true;
    if (_currentProject) {
        _currentProject->deletePeakGroup(group);
        auto parentGroupId = 0;
        if (group->parent != nullptr && group->parent->isGhost()) {
            parentGroupId = -1;
        } else if (group->parent != nullptr) {
            parentGroupId = group->parent->groupId();
        }
        _currentProject->saveGroupAndPeaks(group,
                                           parentGroupId,
                                           tableName.toStdString());
        if (group->hasCompoundLink()
            && !_currentProject->compoundExists(group->getCompound())) {
            _currentProject->saveCompounds({group->getCompound()});
        }
        Q_EMIT(updateStatusString("Updated group attributes"));
    }
    _sqliteDbSaveInProgress = false;
}

bool mzFileIO::writeSQLiteProject(const QString filename,
                                  const bool saveRawData,
                                  const bool isTempProject,
                                  const bool saveChromatogram)
{
    _sqliteDbSaveInProgress = true;

    if (filename.isEmpty()) {
        _sqliteDbSaveInProgress = false;
        return false;
    }

    qDebug() << "saving SQLite project " << filename << endl;

    std::vector<mzSample*> sampleSet = _mainwindow->getSamples();
    if (sampleSet.size() == 0) {
        _sqliteDbSaveInProgress = false;
        return false;
    }

    auto projectIsAlreadyOpen = false;
    if (_currentProject) {
        auto currentName = QString::fromStdString(_currentProject->projectName());
        auto currentPath = QString::fromStdString(_currentProject->projectPath());
        auto currentFilename = currentPath + QDir::separator() + currentName;
        if (currentFilename == filename)
            projectIsAlreadyOpen = true;
    }
    auto projectFileExists = QFile::exists(filename);

    if (projectIsAlreadyOpen && projectFileExists) {
        qDebug() << "saving in existing project…";
    } else {
        qDebug() << "closing the current project…";
        closeSQLiteProject();

        qDebug() << "creating new project to save…" << saveChromatogram;
        auto version = _mainwindow->appVersion().toStdString();
        _currentProject = new ProjectDatabase(filename.toStdString(),
                                              version,
                                              saveRawData,
                                              saveChromatogram);
    }

    if (_currentProject) {
        _currentProject->deleteAll();  // this is crazy

        auto allTablesList = _mainwindow->getPeakTableList();
        allTablesList.push_back(_mainwindow->bookmarkedPeaks);
        int topLevelGroupCount = 0;
        for (const auto& peakTable : allTablesList)
            topLevelGroupCount += peakTable->topLevelGroupCount();

        _currentProject->saveGlobalSettings(_settingsMap);
        if (!isTempProject) {
            emit updateProgressBar("Saving project…",
                                   1 * topLevelGroupCount,
                                   10 * topLevelGroupCount,
                                   true);
        }

        _currentProject->saveSamples(sampleSet);
        if (!isTempProject) {
            emit updateProgressBar("Saving project…",
                                   2 * topLevelGroupCount,
                                   10 * topLevelGroupCount,
                                   true);
        }

        _currentProject->saveAlignment(sampleSet);
        if (!isTempProject) {
            emit updateProgressBar("Saving project…",
                                   3 * topLevelGroupCount,
                                   10 * topLevelGroupCount,
                                   true);
        }

        int counter = 0;
        vector<PeakGroup*> groupVector;
        set<Compound*> compoundSet;
        for (const auto& peakTable : allTablesList) {
            for (shared_ptr<PeakGroup> group : peakTable->getGroups()) {
                groupVector.push_back(group.get());
                if (group->hasCompoundLink()) {
                    auto compound = group->getCompound();
                    compound->setCharge(group->parameters()->getCharge(compound));
                    compoundSet.insert(compound);
                }
                ++counter;
                if (!isTempProject) {
                    emit updateProgressBar("Saving project…",
                                           3 * topLevelGroupCount
                                               + 4 * counter,
                                           10 * topLevelGroupCount,
                                           true);
                }
            }
            string tableName = peakTable->titlePeakTable
                                        ->text().toStdString();
            _currentProject->saveGroups(groupVector, tableName);
            groupVector.clear();
        }
        if (!isTempProject) {
            emit updateProgressBar("Saving project…",
                                   8 * topLevelGroupCount,
                                   10 * topLevelGroupCount,
                                   true);
        }

        _currentProject->saveCompounds(compoundSet);
        if (!isTempProject) {
            emit updateProgressBar("Saving project…",
                                   9 * topLevelGroupCount,
                                   10 * topLevelGroupCount,
                                   true);
        }

        _currentProject->vacuum();
        if (!isTempProject) {
            emit updateProgressBar("Saving project…",
                                   10 * topLevelGroupCount,
                                   10 * topLevelGroupCount,
                                   true);
        }

        qDebug() << "finished writing to project" << filename;
        if (!isTempProject) {
            Q_EMIT(updateStatusString(
                QString("Project successfully saved to %1").arg(filename)
            ));
        }
        _sqliteDbSaveInProgress = false;
        return true;
    }
    qDebug() << "cannot write to closed project" << filename;
    _sqliteDbSaveInProgress = false;
    return false;
}

bool mzFileIO::writeSQLiteProjectForPolly(QString filename, bool saveRawData, bool saveChromatogram)
{
    vector<mzSample*> sampleSet = _mainwindow->getSamples();
    if (sampleSet.size() == 0)
        return false;

    auto version = _mainwindow->appVersion().toStdString();
    // For polly if the user choses to export emDB with raw data
    // then the emDB must be written with the whole chromatogram 
    // and not just the peakGroup information. Hence, third and 
    // fourth parameters are both set to saveRawData. 
    auto sessionDb = new ProjectDatabase(filename.toStdString(), 
                                        version, 
                                        saveRawData, 
                                        saveChromatogram);

    if (sessionDb) {
        sessionDb->deleteAll();
        sessionDb->saveGlobalSettings(_settingsMap);
        sessionDb->saveSamples(sampleSet);
        sessionDb->saveAlignment(sampleSet);

        vector<PeakGroup*> groupVector;
        set<Compound*> compoundSet;
        int topLevelGroupCount = 0;
        auto allTablesList = _mainwindow->getPeakTableList();
        allTablesList.push_back(_mainwindow->bookmarkedPeaks);
        for (const auto& peakTable : allTablesList) {
            for (shared_ptr<PeakGroup> group : peakTable->getGroups()) {
                topLevelGroupCount++;
                groupVector.push_back(group.get());
                if (group->hasCompoundLink()) {
                    auto compound = group->getCompound();
                    compound->setCharge(group->parameters()->getCharge(compound));
                    compoundSet.insert(compound);
                }
            }
            string tableName = peakTable->titlePeakTable
                                        ->text().toStdString();
            sessionDb->saveGroups(groupVector, tableName);
            groupVector.clear();
        }
        sessionDb->saveCompounds(compoundSet);
        sessionDb->vacuum();
        delete sessionDb;
        qDebug() << "finished writing to project" << filename;
        return true;
    }
    qDebug() << "cannot write to closed project" << filename;
    return false;
}

QString mzFileIO::openSQLiteProject(QString filename)
{
    if (_currentProject)
        closeSQLiteProject();

    // if the user has opened an mzrollDB project, then we copy its data to a
    // new emDB file and then open that project for use in El-MAVEN.
    QString openedFilename = filename;
    if (isMzrollDbProject(filename)) {
        openedFilename = swapFilenameExtension(filename, "emDB");
        MzrollDbConverter::convertLegacyToCurrent(filename.toStdString(),
                                                  openedFilename.toStdString());
    }

    auto version = _mainwindow->appVersion().toStdString();
    _currentProject = new ProjectDatabase(openedFilename.toStdString(),
                                          version);
    return openedFilename;
}

void mzFileIO::_beginSQLiteProjectLoad()
{
    emit updateStatusString("Loading user settings…");
    auto settings = _currentProject->loadGlobalSettings();
    for (const auto& it : settings)
        _settingsMap[it.first] = it.second;

    emit settingsLoaded(_settingsMap);
}

void mzFileIO::_readSamplesFromCurrentSQLiteProject()
{
    while(isRunning());

    if (!_currentProject)
        return;

    auto filename = QString(_currentProject->projectPath().c_str());
    auto samples = _currentProject->getSampleNames(_mainwindow->getSamples());

    // set missing samples before returning found samples to be loaded
    for (auto missingSample : samples.second)
        _missingSamples.append(QString::fromStdString(missingSample));

    QList<QString> foundSamples;
    for (auto foundSample : samples.first)
        foundSamples.append(QString::fromStdString(foundSample));

    if (!foundSamples.empty() && _missingSamples.empty()) {
        _mainwindow->projectDockWidget->setLastOpenedProject(filename);
        for (auto sample : foundSamples)
            addFileToQueue(sample);
        start();
        return;
    }
    if (!_missingSamples.empty()) {
        _mainwindow->projectDockWidget->setLastOpenedProject(filename);
        _promptForMissingSamples(foundSamples);
    }
    if (foundSamples.empty() && _missingSamples.empty()) {
        // emit mock signals for empty database load
        Q_EMIT(sqliteDBSamplesLoaded());
        Q_EMIT(sqliteDBPeakTablesCreated());
        Q_EMIT(sqliteDBAlignmentDone());
        Q_EMIT(sqliteDBPeakTablesPopulated());
    }
}

void mzFileIO::_promptForMissingSamples(QList<QString> foundSamples)
{
    // load samples that were originally missing, can be a method by itself?
    QList<QString> missingSamples = _missingSamples;
    int samplesRemaining = static_cast<int>(missingSamples.size());
    while(samplesRemaining) {
        QString path = QString::fromStdString(_currentProject->projectPath());
        for (auto& originalFilename : _missingSamples) {
            auto filename = originalFilename;
            QFileInfo sampleFile(filename);

            // first check in the current set path
            if (!sampleFile.exists()) {
                auto fname = path + QDir::separator() + sampleFile.fileName();
                sampleFile.setFile(fname);
            }

            // if not in project path, prompt user to locate path
            while (!sampleFile.exists()) {
                QString message = filename + " was not found. Add new folder "
                                  "for locating samples?";

                QMessageBox::StandardButton reply;
                reply =
                    QMessageBox::question(_mainwindow,
                                          "Unable to locate sample",
                                          message,
                                          QMessageBox::Ok | QMessageBox::Cancel,
                                          QMessageBox::Ok);

                if (reply == QMessageBox::Cancel) {
                    return;
                }

                path = QFileDialog::getExistingDirectory(_mainwindow,
                                                         tr("Open Directory"),
                                                         path);
                auto fname = path + QDir::separator() + sampleFile.fileName();
                sampleFile.setFile(fname);
            }

            // once found, add these files to load queue
            if (sampleFile.exists()) {
                missingSamples.erase(remove(missingSamples.begin(),
                                            missingSamples.end(),
                                            originalFilename),
                                     missingSamples.end());
                samplesRemaining = static_cast<int>(missingSamples.size());
                addFileToQueue(sampleFile.filePath());
            }
        }
    }
    _missingSamples.clear();

    for (const auto& sampleFilepath : foundSamples) {
        addFileToQueue(sampleFilepath);
    }

    // retry loading samples that were previously missing
    if (filelist.size()) {
        start();
        return;
    }
}

void mzFileIO::_readPeakTablesFromSQLiteProject(const vector<mzSample*> newSamples)
{
    if (!_currentProject || newSamples.empty())
        return;

    emit updateStatusString("Loading compounds…");
    int totalCharge = _mainwindow->mavenParameters->charge
                      * newSamples.at(0)->getPolarity();
    DB.updateChargesForZeroCharges(totalCharge);
    auto compounds = _currentProject->loadCompounds();
    for (auto compound : compounds)
        DB.addCompound(compound);

    // lambda: if the given peak-group has an adduct with a name, assign it the
    // same adduct, but from globally shared database instead
    auto assignAdduct = [](PeakGroup* group, Database& db) {
        if (group->adduct() != nullptr && !group->adduct()->getName().empty()) {
            string adductName = group->adduct()->getName();
            delete group->adduct();
            group->setAdduct(db.findAdductByName(adductName));
        }
    };

    // set of compound databases that need to be communicated with ligand widget
    vector<QString> dbNames;

    // load all peakgroups
    auto groups = _currentProject->loadGroups(newSamples,
                                              _mainwindow->mavenParameters);
    auto groupCount = 0;
    for (auto& group : groups) {
        // assign a compound from global "DB" object to the group
        if (group->hasCompoundLink() && !group->getCompound()->db().empty()) {
            Compound* compound = DB.findSpeciesByIdAndName(
                group->getCompound()->id(),
                group->getCompound()->name(),
                group->getCompound()->db());
            group->setCompound(compound);
            for (auto& child : group->childIsotopes())
                child->setCompound(compound);
            for (auto& child : group->childAdducts())
                child->setCompound(compound);
            dbNames.push_back(QString::fromStdString(compound->db()));
        }
        assignAdduct(group, DB);
        for (auto& child : group->childIsotopes())
            assignAdduct(child.get(), DB);
        for (auto& child : group->childAdducts())
            assignAdduct(child.get(), DB);

        // assign group to bookmark table if none exists
        if (group->tableName().empty())
            group->setTableName("Bookmark Table");

        // find appropriate tables and populate them
        auto allTablesList = _mainwindow->getPeakTableList();
        allTablesList.push_back(_mainwindow->bookmarkedPeaks);
        TableDockWidget* table = nullptr;
        for (auto t : allTablesList)
            if (t->windowTitle().toStdString() == group->tableName())
                table = t;

        if (table)
            table->addPeakGroup(group);
        Q_EMIT(updateProgressBar(tr("Loading peak tables and groups…"),
                                 ++groupCount,
                                 static_cast<int>(groups.size())));
    }

    // emit last database name to be set in ligand widget
    if (!dbNames.empty())
        Q_EMIT(_mainwindow->ligandWidget->mzrollSetDB(dbNames.back()));

    // table widgets are ready to show groups
    Q_EMIT(sqliteDBPeakTablesPopulated());
}

void mzFileIO::_postSampleLoadOperations()
{
    if (!_currentProject)
        return;

    while(isRunning());

    // create tables for stored peak groups, this can only be done in the
    // main loop
    auto tableNames = _currentProject->getTableNames();
    auto tableSettings = _currentProject->loadTableSettings();

    for (auto name : tableNames) {
        auto tableName = QString::fromStdString(name);
        TableDockWidget* table = nullptr;
        for (auto t : _mainwindow->getPeakTableList())
            if (t->windowTitle() == tableName)
                table = t;
        
        if (!table
            && tableName != _mainwindow->bookmarkedPeaks->windowTitle()) {
            bool hasClassifiedGroups = false;
            auto it = tableSettings.find(name);
            if(it != tableSettings.end())
                hasClassifiedGroups= tableSettings[name];
            _mainwindow->addPeaksTable(tableName, hasClassifiedGroups);
        }
    }

    _sqliteDbLoadInProgress = true;
    start();
}

QString mzFileIO::swapFilenameExtension(QString filename, QString ext)
{
    QString newFilename = filename;
    QFileInfo fileInfo(filename);
    QDir parentDir = QDir(fileInfo.path());
    QString baseName = fileInfo.completeBaseName();
    newFilename = parentDir.filePath(baseName + "." + ext);
    return newFilename;
}

void mzFileIO::markv_0_1_5mzroll(QString fileName)
{
    mzrollv_0_1_5 = true;

    QFile data(fileName);

    if (!data.open(QFile::ReadOnly)) {
        return;
    }

    QXmlStreamReader xml(&data);
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            if (xml.name() == "SamplesUsed") {
                // mark false if <SamplesUsed> which is only in new version
                mzrollv_0_1_5 = false;
                break;
            }
        }
    }

    data.close();

    return;
}

void mzFileIO::cleanString(QString& name)
{
    name.replace('#', '_');
    name = 's' + name;
}

void mzFileIO::readSamplesXML(QXmlStreamReader& xml,
                              PeakGroup* group,
                              float mzrollVersion)
{
    vector<mzSample*> samples = _mainwindow->getSamples();

    if (mzrollVersion == 1) {
        if (xml.name() == "SamplesUsed") {
            xml.readNextStartElement();
            while (xml.name() == "sample") {
                unsigned int id =
                    xml.attributes().value("id").toString().toInt();
                for (int i = 0; i < samples.size(); ++i) {
                    mzSample* sample = samples[i];
                    if (id == sample->getSampleId()) {
                        group->samples.push_back(sample);
                    }
                }
                xml.readNextStartElement();
            }
        }
    } else {
        for (int i = 0; i < samples.size(); ++i) {
            QString name = QString::fromStdString(samples[i]->sampleName);
            cleanString(name);
            if (xml.name() == "PeakGroup" && mzrollv_0_1_5
                && samples[i]->isSelected) {
                /**
                 * if mzroll is from old version, just insert sample in group
                 * from checking whether it is selected or not at time of
                 * exporting. This can give erroneous result for old version if
                 * at time of exporting mzroll user has selected diffrent
                 * samples from samples were used at time of peak finding which
                 * was inherent problem of old version of ElMaven.
                 */
                group->samples.push_back(samples[i]);
            } else if (xml.name() == "SamplesUsed"
                       && xml.attributes().value(name).toString() == "Used") {
                /**
                 * if mzroll file is of new version, it's sample name will
                 * precede by 's' and has value of <Used> or <NotUsed>
                 */
                group->samples.push_back(samples[i]);
            }
        }
    }
}

PeakGroup* mzFileIO::readGroupXML(QXmlStreamReader& xml, PeakGroup* parent)
{
    PeakGroup* group = new PeakGroup(
        make_shared<MavenParameters>(*_mainwindow->mavenParameters),
        PeakGroup::IntegrationType::Programmatic);

    group->setGroupId(xml.attributes().value("groupId").toString().toInt());
    group->clusterId = xml.attributes().value("clusterId").toString().toInt();
    group->groupRank = xml.attributes().value("grouRank").toString().toFloat();
    group->tagIsotope(xml.attributes().value("tagString").toString().toStdString(),
                      xml.attributes().value("expectedMz").toString().toFloat(),
                      0.0f);
    group->setExpectedMz(xml.attributes().value("expectedMz").toString().toFloat());
    group->setUserLabel(xml.attributes().value("label").toString().toInt());
    group->setType((PeakGroup::GroupType)xml.attributes()
                       .value("type")
                       .toString()
                       .toInt());
    group->changeFoldRatio =
        xml.attributes().value("changeFoldRatio").toString().toFloat();
    group->changePValue =
        xml.attributes().value("changePValue").toString().toFloat();

    string compoundId =
        xml.attributes().value("compoundId").toString().toStdString();
    string compoundDB =
        xml.attributes().value("compoundDB").toString().toStdString();
    string compoundName =
        xml.attributes().value("compoundName").toString().toStdString();

    string srmId = xml.attributes().value("srmId").toString().toStdString();
    if (!srmId.empty())
        group->setSrmId(srmId);

    if (!compoundName.empty() && !compoundDB.empty()) {
        vector<Compound*> matches =
            DB.findSpeciesByName(compoundName, compoundDB);
        if (matches.size() > 0)
            group->setCompound(matches[0]);
    } else if (!compoundId.empty()) {
        Compound* c = nullptr;

        if (group->getCompound() && !group->getCompound()->name().empty()) {
            c = DB.findSpeciesByIdAndName(compoundId,
                                          group->getCompound()->name(),
                                          DB.ANYDATABASE);
        } else if (!compoundDB.empty()) {
            vector<Compound*> matches = DB.findSpeciesById(compoundId, compoundDB);
            if (matches.size())
                c = matches[0];
        }
        
        if (c)
            group->setCompound(c);
    }

    if (!group->getCompound()) {
        if (!compoundId.empty())
            group->tagString = compoundId;
        else if (!compoundName.empty())
            group->tagString = compoundName;
    }

    if (parent) {
        parent->addIsotopeChild(*group);
        if (parent->childIsotopeCount() > 0)
            group = parent->childIsotopes()[parent->childIsotopeCount() - 1].get();
    }

    return group;
}

void mzFileIO::readPeakXML(QXmlStreamReader& xml, PeakGroup* parent)
{
    Peak p;
    p.pos = xml.attributes().value("pos").toString().toInt();
    p.minpos = xml.attributes().value("minpos").toString().toInt();
    p.maxpos = xml.attributes().value("maxpos").toString().toInt();
    p.splineminpos = xml.attributes().value("splineminpos").toString().toInt();
    p.splinemaxpos = xml.attributes().value("splinemaxpos").toString().toInt();
    p.rt = xml.attributes().value("rt").toString().toDouble();
    p.rtmin = xml.attributes().value("rtmin").toString().toDouble();
    p.rtmax = xml.attributes().value("rtmax").toString().toDouble();
    p.mzmin = xml.attributes().value("mzmin").toString().toDouble();
    p.mzmax = xml.attributes().value("mzmax").toString().toDouble();
    p.scan = xml.attributes().value("scan").toString().toInt();
    p.minscan = xml.attributes().value("minscan").toString().toInt();
    p.maxscan = xml.attributes().value("maxscan").toString().toInt();
    p.peakArea = xml.attributes().value("peakArea").toString().toDouble();
    p.peakSplineArea =
        xml.attributes().value("peakSplineArea").toString().toDouble();
    p.peakAreaCorrected =
        xml.attributes().value("peakAreaCorrected").toString().toDouble();
    p.peakAreaTop = xml.attributes().value("peakAreaTop").toString().toDouble();
    p.peakAreaTopCorrected =
        xml.attributes().value("peakAreaTopCorrected").toString().toDouble();
    p.peakAreaFractional =
        xml.attributes().value("peakAreaFractional").toString().toDouble();
    p.peakRank = xml.attributes().value("peakRank").toString().toDouble();
    p.peakIntensity =
        xml.attributes().value("peakIntensity").toString().toDouble();
    p.peakBaseLineLevel =
        xml.attributes().value("peakBaseLineLevel").toString().toDouble();
    p.peakMz = xml.attributes().value("peakMz").toString().toDouble();
    p.medianMz = xml.attributes().value("medianMz").toString().toDouble();
    p.baseMz = xml.attributes().value("baseMz").toString().toDouble();
    p.quality = xml.attributes().value("quality").toString().toDouble();
    p.width = xml.attributes().value("width").toString().toInt();
    p.gaussFitSigma =
        xml.attributes().value("gaussFitSigma").toString().toDouble();
    p.gaussFitR2 = xml.attributes().value("gaussFitR2").toString().toDouble();
    p.groupNum = xml.attributes().value("groupNum").toString().toInt();
    p.noNoiseObs = xml.attributes().value("noNoiseObs").toString().toInt();
    p.noNoiseFraction =
        xml.attributes().value("noNoiseFraction").toString().toDouble();
    p.symmetry = xml.attributes().value("symmetry").toString().toDouble();
    p.signalBaselineRatio =
        xml.attributes().value("signalBaselineRatio").toString().toDouble();
    p.groupOverlap =
        xml.attributes().value("groupOverlap").toString().toDouble();
    p.groupOverlapFrac =
        xml.attributes().value("groupOverlapFrac").toString().toDouble();
    p.localMaxFlag = xml.attributes().value("localMaxFlag").toString().toInt();
    p.fromBlankSample =
        xml.attributes().value("fromBlankSample").toString().toInt();
    p.label = xml.attributes().value("label").toString().toInt();
    string sampleName =
        xml.attributes().value("sample").toString().toStdString();
    vector<mzSample*> samples = _mainwindow->getSamples();
    for (int i = 0; i < samples.size(); i++) {
        if (samples[i]->sampleName == sampleName) {
            p.setSample(samples[i]);
            break;
        }
    }

    parent->addPeak(p);
}

vector<PeakGroup*> mzFileIO::readGroupsXML(QString fileName)
{
    markv_0_1_5mzroll(fileName);

    QFile data(fileName);
    vector<PeakGroup*> groups;
    if ( !data.open(QFile::ReadOnly) ) {
        cerr << "File open: " << fileName.toStdString() << " failed" << endl;
        return groups;
    }

    QXmlStreamReader xml(&data);
    PeakGroup* group = nullptr;
    PeakGroup* parent = nullptr;
    QStack<PeakGroup*> stack;

    float mzrollVersion = 0;

    while (!xml.atEnd()) {
        if (xml.isStartElement() && xml.name() == "project") {
            mzrollVersion = xml.attributes().value("mzrollVersion").toFloat();
        }
        xml.readNext();
        if (xml.hasError()) {
            qDebug() << "Error in xml reading: " << xml.errorString();
        }
        if (xml.isStartElement()) {
            if (xml.name() == "PeakGroup") {
                group = readGroupXML(xml, parent);
                if (!group->isIsotope())
                    groups.push_back(group);
            }
            if (xml.name() == "SamplesUsed" && group) {
                readSamplesXML(xml, group, mzrollVersion);
            }
            if (xml.name() == "Peak" && group) {
                readPeakXML(xml, group);
            }
            if (xml.name() == "children" && group) {
                stack.push(group);
                parent = stack.top();
            }
        }

        if (xml.isEndElement()) {
           if (xml.name() == "children") {
                if (stack.size() > 0)
                    parent = stack.pop();
                if (parent && parent->childIsotopeCount()) {
                    for (int i = 0; i < parent->childIsotopes().size(); i++) {
                        parent->childIsotopes()[i]->minQuality =
                            _mainwindow->mavenParameters->minQuality;
                        parent->childIsotopes()[i]->groupStatistics();
                    }
                }
                if (stack.size() == 0)
                    parent = nullptr;
            }
            if (xml.name() == "PeakGroup") {
                if (group) {
                    group->minQuality =
                        _mainwindow->mavenParameters->minQuality;
                    group->groupStatistics();
                }
                group = nullptr;
            }
        }
    }
    for (auto group : groups) {
        if (!group)
            continue;
        group->minQuality = _mainwindow->mavenParameters->minQuality;
        group->groupStatistics();
    }
    return groups;
}

bool mzFileIO::isPeakListType(QString filename) {
    QStringList extList;
    extList << "mzPeaks";
    Q_FOREACH (QString suffix, extList) {
        if (filename.endsWith(suffix,Qt::CaseInsensitive)) return true;
    }
    return false;
}

void mzFileIO::addFileToQueue(QString f)
{
//    if (isKnownFileType(f)) filelist << f;
    filelist << f;
}

void mzFileIO::removeAllFilefromQueue() {
    filelist.clear();
}
