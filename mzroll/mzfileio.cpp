#include "mzfileio.h"
#include <QStringList>
#include <QTextStream>

mzFileIO::mzFileIO(QWidget*) {
    _mainwindow = NULL;
    _stopped = true;
}

void mzFileIO::setMainWindow(MainWindow* mw) {
    _mainwindow=mw;
    connect(this,SIGNAL(updateProgressBar(QString,int,int)), _mainwindow, SLOT(setProgressBar(QString, int,int)));
    connect(this,SIGNAL(finished()),_mainwindow,SLOT(setupSampleColors()));
    connect(this,SIGNAL(finished()),_mainwindow->projectDockWidget,SLOT(updateSampleList()));
    connect(this,SIGNAL(finished()),_mainwindow,SLOT(showSRMList()));
}


void mzFileIO::loadSamples(QStringList& filenames) {
    setFileList(filenames);
    start();
}

mzSample* mzFileIO::loadSample(QString filename){

    //check if file exists
    QFile file(filename);
    QString sampleName = file.fileName();	//only name of the file, without folder location

    if (!file.exists() ) { 	//couldn't fine this file.. check localdirectory
        qDebug() << "Can't find file " << filename; return 0;
    }

    sampleName.replace(QRegExp(".*/"),"");
    sampleName.replace(QRegExp(".mzCSV"),"");
    sampleName.replace(QRegExp(".mzdata*"),"");
    sampleName.replace(QRegExp(".mzXML*"),"");
    sampleName.replace(QRegExp(".xml*"),"");
    sampleName.replace(QRegExp(".cdf*"),"");
    sampleName.replace(QRegExp(".raw*"),"");

    if (sampleName.isEmpty()) return NULL;
    mzSample* sample = NULL;
    QTime timer; timer.start();

    try {
        if (filename.contains("mzdata",Qt::CaseInsensitive)) {
            sample = mzFileIO::parseMzData(filename);
        } else {
            sample = new mzSample();
            sample->loadSample( filename.toLatin1().data() );
            if ( sample->scans.size() == 0 ) { delete(sample); sample=NULL; }
        }
    } catch(...) {
        qDebug() << "loadSample() " << filename << " failed..";
    }

    qDebug() << "loadSample time (msec) = " << timer.elapsed();

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
   QRegExp formulaMatch("(C\\d+H\\d+\\S*)");

   int charge=0;
   QString line;
   QString name, comment,formula;
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

               cpd->mass=mw;
               cpd->precursorMz=mw;
               cpd->db=dbname;
               cpd->fragment_mzs = mzs;
               cpd->fragment_intensity = intest;
               DB.addCompound(cpd);
               compoundCount++;
            }

            //reset for the next record
           name = comment = formula = QString();
           mw=precursor=0;
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
               formula=formulaMatch.capturedTexts().at(0);
               qDebug() << "Formula=" << formula;
           }
         } else if (line.startsWith("Num Peaks:",Qt::CaseInsensitive)) {
           peaks = line.mid(11,line.length()).simplified().toInt();
        } else if ( peaks != 0 ) {
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
                } else if (xml.name() == "spectrumInstrument" && currentScan) {
                    currentScan->mslevel = xml.attributes().value("msLevel").toString().toInt();
                    if (currentScan->mslevel <= 0 ) currentScan->mslevel=1;
                 } else if (xml.name() == "data" && taglist.size() >= 2 && currentScan) {
                     int precision = xml.attributes().value("precision").toString().toInt();


                     if (taglist.at(taglist.size()-2) == "mzArrayBinary") {
                      currentScan->mz=
                               base64::decode_base64(xml.readElementText().toStdString(),precision/8,false);
                     }

                     if (taglist.at(taglist.size()-2) == "intenArrayBinary") {
                        currentScan->intensity =
                                base64::decode_base64(xml.readElementText().toStdString(),precision/8,false);
                     }
                }

        } else if (xml.isEndElement()) {
               if (!taglist.isEmpty()) taglist.pop_back();
               if (xml.name() == "spectrum" && currentScan) {
                   currentSample->addScan(currentScan);
                   emit (updateProgressBar( "FileImport", scannum%100, 110));
               }
        }
    }

    data.close();
    return currentSample;
}

void mzFileIO::run(void) { fileImport(); quit(); }

void mzFileIO::fileImport(void) {
    if ( filelist.size() == 0 ) return;
    emit (updateProgressBar( "Importing files", filelist.size()+0.01, filelist.size()));

    for (int i = 0; i < filelist.size(); i++) {
        QString filename = filelist.at(i);
        emit (updateProgressBar( tr("Importing file %1").arg(filename), i+1, filelist.size()));

        if(_mainwindow) {
            qDebug() << "Loading sample:" << filename;
            mzSample* sample = this->loadSample(filename);
            if (sample) {
                sample->enumerateSRMScans();
                //set min and max values for rt
                sample->calculateMzRtRange();
                //check if this is a blank sample
                sample->fileName = filename.toLower().toStdString();
                if ( filename.contains("blan",Qt::CaseInsensitive)) sample->isBlank = true;
                if (sample->scans.size()>0) _mainwindow->addSample(sample);

            }
        }
    }
   emit (updateProgressBar( "Done importing", filelist.size(), filelist.size()));
}



