#include "mzfileio.h"
#include <QStringList>
#include <QTextStream>

#ifndef __APPLE__
#include <omp.h>
#endif

#include <MavenException.h>

mzFileIO::mzFileIO(QWidget*) {
    _mainwindow = NULL;
    _stopped = true;
    process = NULL;
}

void mzFileIO::setMainWindow(MainWindow* mw) {

    _mainwindow=mw;
    //connect(this,SIGNAL(finished()),_mainwindow,SLOT(setupSampleColors()));
    //connect(this,SIGNAL(finished()),_mainwindow->projectDockWidget,SLOT(updateSampleList()));
   
	//if ( mw->srmDockWidget->isVisible()) connect(this,SIGNAL(finished()),_mainwindow,SLOT(showSRMList()));
    
	//process = new QProcess();
 	//connect(process, SIGNAL(finished(int)), this, SLOT(readProcessOutput(int)));
    //connect(process, SIGNAL(readyReadStandardOutput()), this, SLOT(readThermoRawFileImport()));

}

void mzFileIO::loadSamples(QStringList& files) {
	Q_FOREACH(QString file, files){ addFileToQueue(file); }
	if (filelist.size() > 0) start();
    //setFileList(filenames);
    //start();
}

mzSample* mzFileIO::loadSample(QString filename){

    //check if file exists
    QFile file(filename);
    QString sampleName = file.fileName();	//only name of the file, without folder location

    if (!file.exists() ) { 	//couldn't find this file.. check local directory
        return nullptr;
    }

    sampleName.replace(QRegExp(".*/"),"");
    sampleName.replace(".mzCSV","",Qt::CaseInsensitive);
    sampleName.replace(".mzdata","",Qt::CaseInsensitive);
    sampleName.replace(".mzXML","",Qt::CaseInsensitive);
    sampleName.replace(".mzML","",Qt::CaseInsensitive);
    sampleName.replace(".mz5","",Qt::CaseInsensitive);
    sampleName.replace(".pepXML","",Qt::CaseInsensitive);
    sampleName.replace(".xml","",Qt::CaseInsensitive);
    sampleName.replace(".cdf","",Qt::CaseInsensitive);
    sampleName.replace(".raw","",Qt::CaseInsensitive);

    if (sampleName.isEmpty()) return NULL;
    mzSample* sample = NULL;

    if (filename.contains("mzdata",Qt::CaseInsensitive)) {
        // mzFileIO::loadPepXML(filename);
        sample = mzFileIO::parseMzData(filename);
    } else  if (filename.endsWith("raw",Qt::CaseInsensitive)) {
        mzFileIO::ThermoRawFileImport(filename);
    } else {
        sample = new mzSample();
        sample->loadSample( filename.toLatin1().data() );
        if ( sample->scans.size() == 0 ) { delete(sample); sample=NULL; }
    }



    if ( sample && sample->scans.size() > 0 ) {
        sample->sampleName = string( sampleName.toLatin1().data() );
        sample->enumerateSRMScans();

        //set min and max values for rt
        sample->calculateMzRtRange();

        //set file path
        sample->fileName = filename.toStdString();

        if (filename.contains("blan",Qt::CaseInsensitive)) sample->isBlank = true;
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
               cpd->precursorMz=precursor;
               cpd->db=dbname;
               cpd->fragment_mzs = mzs;
               cpd->fragment_intensity = intest;
			   Q_FOREACH (QString cat, compound_class) { cpd->category.push_back(cat.toStdString()); }
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
//TODO: Shouldnot be here
int mzFileIO::loadNISTLibrary(QString fileName) {
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
   Name: DGDG 8:0; [M-H]-; DGDG(2:0/6:0)
   MW: 555.22888
   PRECURSORMZ: 555.22888
   Comment: Parent=555.22888 Mz_exact=555.22888 ; DGDG 8:0; [M-H]-; DGDG(2:0/6:0); C23H40O15
   Num Peaks: 2
   115.07586 999 "sn2 FA"
   59.01330 999 "sn1 FA"
   */

   QRegExp whiteSpace("\\s+");
   QRegExp formulaMatch("Formula\\=(C\\d+H\\d+\\S*)");
   QRegExp retentionTimeMatch("AvgRt\\=(\\S+)");

   int charge=0;
   QString line;
   QString name, comment,formula;
   double retentionTime;
   double mw=0;
   double precursor=0;
   int peaks=0;
   vector<float>mzs;
   vector<float>intest;

   int compoundCount=0;

    do {
        line = stream.readLine();

        if(line.startsWith("Name:",Qt::CaseInsensitive) && !name.isEmpty()) {
            if (!name.isEmpty()) { //insert new compound

                Compound* cpd = new Compound(
                           name.toStdString(),
                           name.toStdString(),
                           formula.toStdString(),
                           charge);
			   if (precursor and mw) { cpd->mass=precursor; cpd->precursorMz=precursor; }
			   else if (mw) { cpd->mass=mw; cpd->precursorMz=precursor; }
               //cpd->mass=mw;
               //cpd->precursorMz=mw;
               cpd->db=dbname;
               cpd->fragment_mzs = mzs;
               cpd->fragment_intensity = intest;
			   cpd->expectedRt=retentionTime;
               DB.addCompound(cpd);
               compoundCount++;
            }

            //reset for the next record
           name = comment = formula = QString();
           mw=precursor=0;
		   retentionTime=0;
           peaks=0;
           mzs.clear();
           intest.clear();
        }

         if(line.startsWith("Name:",Qt::CaseInsensitive)) {
             name = line.mid(5,line.length()).simplified();
         } else if (line.startsWith("MW:",Qt::CaseInsensitive)) {
             mw = line.mid(4,line.length()).simplified().toDouble();
         } else if (line.startsWith("PRECURSORMZ:",Qt::CaseInsensitive)) {
             precursor = line.mid(13,line.length()).simplified().toDouble();
         } else if (line.startsWith("Comment:",Qt::CaseInsensitive)) {
             comment = line.mid(8,line.length()).simplified();
             if (comment.contains(formulaMatch)){
                 formula=formulaMatch.capturedTexts().at(1);
                 qDebug() << "Formula=" << formula;
             }
			if (comment.contains(retentionTimeMatch)){
                 retentionTime=retentionTimeMatch.capturedTexts().at(1).simplified().toDouble();
                 //qDebug() << "retentionTime=" << retentionTimeString;
             }
         } else if (line.startsWith("Num Peaks:",Qt::CaseInsensitive) || line.startsWith("NumPeaks:",Qt::CaseInsensitive)) {
            //  peaks = line.mid(11,line.length()).simplified().toInt();
             peaks = 1;
         } else if ( peaks ) {
             QStringList mzintpair = line.split(whiteSpace);
             if( mzintpair.size() >=2 ) {
                 bool ok=false; bool ook=false;
                 float mz = mzintpair.at(0).toDouble(&ok);
                 float ints = mzintpair.at(1).toDouble(&ook);
                 if (ok && ook && mz >= 0 && ints >= 0) {
                     mzs.push_back(mz);
                     intest.push_back(ints);
                 }
             }
         }

    } while (!line.isNull());

    return compoundCount;
}

//TODO: Should not be here
int mzFileIO::loadPepXML(QString fileName) {

    qDebug() << "Loading pepXML sample: " << fileName;
    QFile data(fileName);
    string dbname = mzUtils::cleanFilename(fileName.toStdString());

    if (!data.open(QFile::ReadOnly) ) {
        qDebug() << "Can't open " << fileName; return 0;
    }

    QXmlStreamReader xml(&data);
    xml.setNamespaceProcessing(false);
    QList<QStringRef> taglist;
/*
    <spectrum_query spectrum="BSA_run_120909192952.2.2.2" spectrumNativeID="controllerType=0 controllerNumber=1 scan=2" start_scan="2" end_scan="2" precursor_neutral_mass="887.14544706624" assumed_charge="2" index="2">
      <search_result num_target_comparisons="0" num_decoy_comparisons="0">
        <search_hit hit_rank="1" peptide="SCHTGLGR" peptide_prev_aa="K" peptide_next_aa="S" protein="sp|P02787|TRFE_HUMAN" num_tot_proteins="1" calc_neutral_pep_mass="886.94606" massdiff="-0.19938706624" num_tol_term="2" num_missed_cleavages="0" num_matched_ions="5" tot_num_ions="42">
          <modification_info>
            <mod_aminoacid_mass position="2" mass="160.0306444778"/>
          </modification_info>
          <search_score name="number of matched peaks" value="5"/>
          <search_score name="number of unmatched peaks" value="37"/>
*/
 
    int hitCount=0;
    int charge; 
    float precursorMz;
    int  scannum;

    Compound* cpd = NULL;
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
                taglist << xml.name();
                if (xml.name() == "spectrum_query") {
                    scannum = xml.attributes().value("start_scan").toString().toInt();
                    charge = xml.attributes().value("charge").toString().toInt();
                    precursorMz = xml.attributes().value("precursor_neutral_mass").toString().toInt();
		    precursorMz = (precursorMz-charge)/charge;	

                } else if (xml.name() == "search_hit") {
		    hitCount++;
                    int hit_rank = xml.attributes().value("hit_rank").toString().toInt();
                    QString peptide = xml.attributes().value("peptide").toString();
                    QString protein = xml.attributes().value("protein").toString();
		    QString formula = "";

		    cpd = new Compound(
				    protein.toStdString() + "_" + peptide.toStdString(),
				    peptide.toStdString(),
				    formula.toStdString(),
				    charge);

		    cpd->mass=precursorMz;
		    cpd->precursorMz=precursorMz;
		    cpd->db=dbname;
		    //cpd->fragment_mzs = mzs;
		    //cpd->fragment_intensity = intest;
		    DB.addCompound(cpd);

                } else if (xml.name() == "mod_aminoacid_mass" ) {
                    int pos =          xml.attributes().value("position").toString().toInt();
		    double massshift = xml.attributes().value("mass").toString().toDouble();
                } else if (xml.name() == "search_score" ) {
                    QString name = xml.attributes().value("name").toString();
                    QString value = xml.attributes().value("value").toString();
                }
        } else if (xml.isEndElement()) {
               if (!taglist.isEmpty()) taglist.pop_back();
               if (xml.name() == "search_hit") {

               }
        }
    }

    data.close();
    return hitCount;
}
//TODO: should not be here
mzSample* mzFileIO::parseMzData(QString fileName) {

    qDebug() << "Loading mzData sample: " << fileName;
    QFile data(fileName);

    if (!data.open(QFile::ReadOnly) ) {
        qDebug() << "Can't open " << fileName; return NULL;
    }

    QXmlStreamReader xml(&data);
    xml.setNamespaceProcessing(false);
    QList<QStringRef> taglist;

    int scannum=0;

    mzSample* currentSample=NULL;
    Scan* currentScan=NULL;

    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
                taglist << xml.name();
                if (xml.name() == "spectrum") {
                    scannum++;
                    if (!currentSample) currentSample = new mzSample();
                    currentScan = new Scan(currentSample,scannum,1,0,0,0);
                } else if (xml.name() == "cvParam" && currentScan) {
                    QString _name = xml.attributes().value("name").toString();
                    QString _value = xml.attributes().value("value").toString();
                   //qDebug() << _name << "->" << _value;

                    if(_name.contains("TimeInMinutes",Qt::CaseInsensitive))  currentScan->rt = _value.toFloat();
                    else if(_name.contains("time in seconds",Qt::CaseInsensitive))  currentScan->rt = _value.toFloat();
                    else if(_name.contains("Polarity",Qt::CaseInsensitive)) {
                        if ( _value[0] == 'p' || _value[0] == 'P') {
                            currentScan->setPolarity(+1);
                        } else {
                            currentScan->setPolarity(-1);
                        }
                    }

                    if (_name.contains("MassToChargeRatio",Qt::CaseInsensitive)) {
                        currentScan->precursorMz = _value.toFloat();
                    }

                    if (_name.contains("CollisionEnergy",Qt::CaseInsensitive)) {
                        currentScan->collisionEnergy = _value.toFloat();
                    }

                } else if (xml.name() == "spectrumInstrument" && currentScan) {
                    currentScan->mslevel = xml.attributes().value("msLevel").toString().toInt();
                    if (currentScan->mslevel <= 0 ) currentScan->mslevel=1;
                 } else if (xml.name() == "data" && taglist.size() >= 2 && currentScan) {
                     int precision = xml.attributes().value("precision").toString().toInt();


                     if (taglist.at(taglist.size()-2) == "mzArrayBinary") {
                      currentScan->mz=
                               base64::decode_base64(xml.readElementText().toStdString(),precision/8,false,false);
                     }

                     if (taglist.at(taglist.size()-2) == "intenArrayBinary") {
                        currentScan->intensity =
                                base64::decode_base64(xml.readElementText().toStdString(),precision/8,false,false);
                     }
                }

        } else if (xml.isEndElement()) {
               if (!taglist.isEmpty()) taglist.pop_back();
               if (xml.name() == "spectrum" && currentScan) {
                   currentSample->addScan(currentScan);
                   Q_EMIT (updateProgressBar( "FileImport", scannum%100, 110));
               }
        }
    }

    data.close();
    return currentSample;
}

void mzFileIO::run(void) {

    try {
        fileImport();
    }
    catch (std::exception& excp) {

        // ask user to send back the logs
        LOGD << excp.what();
    }
    catch (...) {
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
    QStringList spectralhits;

    Q_FOREACH(QString filename, filelist ) {
        try {
            QFileInfo fileInfo(filename);
            if (!fileInfo.exists())
                throw MavenException(ErrorCodes::FileNotFound, filename.toStdString());

            if (isSampleFileType(filename)) {
                samples << filename;
            } else if (isProjectFileType(filename)) {
                projects << filename;
            } else if (isPeakListType(filename)) {
                peaks << filename;
            } else if (isSpectralHitType(filename)) {
                spectralhits << filename;
            }
            else
                throw (MavenException(ErrorCodes::UnsupportedFormat, filename.toStdString()));
        }

        catch (MavenException& excp) {
            qDebug() << "Error: " << excp.what();
        }
    }

    Q_FOREACH(QString filename, projects ) {
        _mainwindow->ligandWidget->loadCompoundDBMzroll(filename);
        _mainwindow->projectDockWidget->loadProject(filename);
        QRegExp rxtable("*_table\-[0-9]*");
        rxtable.setPatternSyntax(QRegExp::Wildcard);
        QRegExp rxbm("*_bookmarkedPeaks*");
        rxbm.setPatternSyntax(QRegExp::Wildcard);
        if(rxtable.exactMatch(filename)) {
            Q_EMIT(createPeakTableSignal(filename));
        } else if (rxbm.exactMatch(filename)) {
            _mainwindow->bookmarkedPeaks->loadPeakTable(filename);
        } else {
            _mainwindow->bookmarkedPeaks->loadPeakTable(filename);
        }
    }

    Q_FOREACH(QString filename, peaks ) {
        QFileInfo fileInfo(filename);
        TableDockWidget* tableX = _mainwindow->addPeaksTable("Group Set " + fileInfo.fileName());
        tableX->loadPeakTable(filename);
    }

    if (_mainwindow->getSettings()->value("uploadMultiprocessing").toInt()) {
        int iter = 0;
        #ifndef __APPLE__
        #pragma omp parallel for shared(iter)
        #endif
        for (int i = 0; i < samples.size(); i++) {
            QString filename = samples.at(i);
            mzSample* sample = loadSample(filename);
            if (sample) {
                    sample->enumerateSRMScans();
                    sample->calculateMzRtRange();    //set min and max values for rt
                    sample->fileName = filename.toStdString();

                    if ( filename.contains("blan",Qt::CaseInsensitive))
                            sample->isBlank = true;   //check if this is a blank sample

                    if (sample->scans.size()>0)
                        emit addNewSample(sample);

            }
            #ifndef __APPLE__
            #pragma omp atomic
            #endif
            iter++;

            Q_EMIT (updateProgressBar( tr("Importing file %1").arg(filename), iter, samples.size()));

        }
    } else {
        int iter = 0;
        for (int i = 0; i < samples.size(); i++) {
            QString filename = samples.at(i);
            mzSample* sample = loadSample(filename);
            if (sample) {
                    sample->enumerateSRMScans();
                    sample->calculateMzRtRange();    //set min and max values for rt
                    sample->fileName = filename.toStdString();

                    if ( filename.contains("blan",Qt::CaseInsensitive))
                            sample->isBlank = true;   //check if this is a blank sample

                    if (sample->scans.size()>0)
                            _mainwindow->addSample(sample);

            }
            iter++;

            Q_EMIT (updateProgressBar( tr("Importing file %1").arg(filename), iter, samples.size()));

        }
    }

    Q_FOREACH(QString filename, spectralhits ) {
        if (filename.contains("pepXML",Qt::CaseInsensitive)) {
            _mainwindow->spectralHitsDockWidget->loadPepXML(filename);
        }
        else if (filename.contains("pep.xml",Qt::CaseInsensitive)) {
             _mainwindow->spectralHitsDockWidget->loadPepXML(filename);
        }
        else if (filename.contains("idpDB",Qt::CaseInsensitive)) {
             _mainwindow->spectralHitsDockWidget->loadIdPickerDB(filename);
        }
   }

    //done..
    Q_EMIT (updateProgressBar( "Done importing", samples.size(), samples.size()));
    if (samples.size() > 0)       Q_EMIT(sampleLoaded());
    if (spectralhits.size() >0)   Q_EMIT(spectraLoaded());
    if (projects.size() >0)       Q_EMIT(projectLoaded());
    if (peaks.size() > 0)    	  Q_EMIT(peaklistLoaded());
    filelist.clear(); //empty queue
}

void mzFileIO::qtSlot(const string& progressText, unsigned int completed_samples, int total_samples)
{
        Q_EMIT(updateProgressBar(QString::fromStdString(progressText), completed_samples, total_samples));

}


bool mzFileIO::isKnownFileType(QString filename) {
    if (isSampleFileType(filename))  return true;
    if (isProjectFileType(filename)) return true;
    if (isSpectralHitType(filename)) return true;
    if (isPeakListType(filename)) return true;
    return false;
}

bool mzFileIO::isSampleFileType(QString filename) {
    QStringList extList;
    extList << "mzXML" << "cdf" << "nc" << "mzML" << "mzData" << "mzML";
    Q_FOREACH (QString suffix, extList) {
        if (filename.endsWith(suffix,Qt::CaseInsensitive)) return true;
    }
    return false;
}

bool mzFileIO::isProjectFileType(QString filename) {
    if (filename.endsWith("mzroll",Qt::CaseInsensitive)) return true;
    return false;
}

bool mzFileIO::isSpectralHitType(QString filename) {
    QStringList extList;
    extList << "pep.xml" << "pepXML" << "idpDB";
    Q_FOREACH (QString suffix, extList) {
        if (filename.endsWith(suffix,Qt::CaseInsensitive)) return true;
    }
    return false;
}

bool mzFileIO::isPeakListType(QString filename) {
    QStringList extList;
    extList << "mzPeaks";
    Q_FOREACH (QString suffix, extList) {
        if (filename.endsWith(suffix,Qt::CaseInsensitive)) return true;
    }
    return false;
}

void mzFileIO::readThermoRawFileImport() {
    if(process) {
        QByteArray data = process->readAllStandardOutput();
        qDebug() << "Captured:" << data;
    }
}

void mzFileIO::addFileToQueue(QString f)
{
//    if (isKnownFileType(f)) filelist << f;
    filelist << f;
}

void mzFileIO::removeAllFilefromQueue() {
    filelist.clear();
}

int mzFileIO::ThermoRawFileImport(QString fileName) {

    if (process->pid()){
           process->terminate();
           qDebug()  <<  "Killing process..\n";
           return -1;
    }

   QString rawExtractExe = _mainwindow->getSettings()->value("RawExtractProgram").toString();
    if(!QFile::exists(rawExtractExe)) {
        qDebug() << "Can't find " + rawExtractExe;
        return -1;
    }

    //start process
    QStringList arguments; arguments  << fileName;
    qDebug() << "Running:" << rawExtractExe << arguments;

    process->start(rawExtractExe, arguments);
}
