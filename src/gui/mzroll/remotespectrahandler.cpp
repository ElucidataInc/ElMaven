#include <QDoubleSpinBox>

#include "Compound.h"
#include "eicwidget.h"
#include "globals.h"
#include "Peptide.hpp"
#include "projectdockwidget.h"
#include "remotespectrahandler.h"
#include "mainwindow.h"
#include "mzSample.h"
#include "spectrawidget.h"
#include "Scan.h"

bool RemoteSpectraHandler::handleRequest(Pillow::HttpConnection *connection)
{
        //const QByteArray someHeaderToken("Some-Header");

		n += connection->requestContent().size();
		n += connection->requestFragment().size();
        //n += connection->requestHeaderValue(someHeaderToken).size();
		n += connection->requestMethod().size();
		n += connection->requestPath().size();
		n += connection->requestQueryString().size();
		n += connection->requestUri().size();
		n += connection->requestUriDecoded().size();
		n += connection->requestFragmentDecoded().size();
		n += connection->requestPathDecoded().size();
		n += connection->requestQueryStringDecoded().size();

        QString path = connection->requestPathDecoded();

        QByteArray responce;
        int status = doCommand(path,
                               connection->requestQueryString(),
                               connection->requestContent(),
                               responce);

        connection->writeResponse(status, Pillow::HttpHeaderCollection(), responce);
        return true;
}

int RemoteSpectraHandler::doCommand(QString path, QString queryString, QString content, QByteArray& responce) {

    QStringList command = path.split('/'); command.pop_front();
    int fieldCount = command.length();
    qDebug() << "Processing Remote Command: " << command;

    if (path.startsWith("/help") or path == "/") { //show help
        responce +="MAVEN REMOTE HTTP INTERFACE\n";
        responce +="AVAILABLE COMMANDS:\n";
        responce +="/setSampleColor/SAMPLENAME/COLOR\n";
        responce +="/setSampleName/SAMPLENAME/NEWNAME\n";
        responce +="/setSampleSetName/SAMPLENAME/SETNAME\n";
        responce +="/setNormalizationConstant/SAMPLENAME/VALUE\n";
        responce +="/hideSample/SAMPLENAME\n";
        responce +="/showSample/SAMPLENAME\n";
        responce +="/setMZ/VALUE\n";
        responce +="/setPPM/VALUE\n";
        responce +="/setMzSlice/MZMIN/MZMAX/RTMIN/RTMAX\n";
        responce +="/getEICView/\n";
        return 400;
    }

    else if (path.startsWith("/setSampleColor/") and fieldCount == 3){
        mzSample* sample =_mw->getSampleByName(command[1]);
        QColor color(command[2]);
        if (sample and color.isValid()) {
            _mw->projectDockWidget->setSampleColor(sample,color);
            _mw->projectDockWidget->updateSampleList();
            responce =  "OK: color changed";  return 200;
        }
    }

    else if (path.startsWith("/setSampleName/") and fieldCount == 3){
        mzSample* sample =_mw->getSampleByName(command[1]);
        QString newName(command[2]); newName = newName.trimmed();
        if (sample and !newName.isEmpty()) {
            sample->sampleName = newName.toStdString();
            _mw->projectDockWidget->updateSampleList();
            responce =  "OK: sample name changed";  return 200;
        }
    }

    else if (path.startsWith("/setSampleSetName/") and fieldCount == 3){
        mzSample* sample =_mw->getSampleByName(command[1]);
        QString newName(command[2]); newName = newName.trimmed();
        if (sample and !newName.isEmpty()) {
            sample->_setName = newName.toStdString();
            _mw->projectDockWidget->updateSampleList();
            responce =  "OK: sample set name changed";  return 200;
        }
    }

    else if (path.startsWith("/setNormalizationConstant/") and fieldCount == 3){
        mzSample* sample =_mw->getSampleByName(command[1]);
        bool ok=false; double newValue = command[2].toDouble(&ok);
        if (sample and ok) {
            sample->_normalizationConstant = newValue;
            _mw->projectDockWidget->updateSampleList();
            responce =  "OK: sample normalization constant changed";  return 200;
        }
    }

    else if (path.startsWith("/hideSample/") and fieldCount == 2){
        mzSample* sample =_mw->getSampleByName(command[1]);
        if (sample) {
            sample->isSelected = false;
            _mw->projectDockWidget->updateSampleList();
            responce =  "OK: sample unsellected";  return 200;
        }
    }

    else if (path.startsWith("/showSample/") and fieldCount == 2){
        mzSample* sample =_mw->getSampleByName(command[1]);
        if (sample) {
            sample->isSelected = true;
            _mw->projectDockWidget->updateSampleList();
            responce =  "OK: sample selected";  return 200;
        }
    }



    else if (path.startsWith("/setMZ/") and fieldCount == 2) {
        bool ok=false; double mz = command[1].toDouble(&ok);
        if(ok) {
            _mw->setMzValue(mz);
            responce =  "OK: mz forcus changed";  return 200;
        }
    }

    else if (path.startsWith("/setPPM/") and fieldCount == 2) {
         bool ok=false; double ppm = command[1].toDouble(&ok);
         if (ok) {
            _mw->massCutoffWindowBox->setValue(ppm);
            _mw->setUserMassCutoff(ppm);
            responce =  "OK: ppm changed";  return 200;
         }
    }

    else if (path.startsWith("/setMzSlice/") and fieldCount == 5) {
         bool ok1=false; double mzmin = command[1].toDouble(&ok1);
         bool ok2=false; double mzmax = command[2].toDouble(&ok2);
         bool ok3=false; double rtmin = command[3].toDouble(&ok3);
         bool ok4=false; double rtmax = command[4].toDouble(&ok4);
         if (ok1 and ok2 and ok3 and ok4) {
             mzSlice slice(mzmin,mzmax,rtmin,rtmax);
            _mw->getEicWidget()->setMzSlice(slice);
            _mw->getEicWidget()->replotForced();
            responce =  "OK: slice changed";  return 200;
         }
    }

    else if (path.startsWith("/setScan/") and fieldCount == 3){
        mzSample* sample =_mw->getSampleByName(command[1]);
        bool ok=false; int scanIndx = command[2].toInt(&ok);
        if (sample and scanIndx < sample->scans.size()) {
            Scan* scan = sample->getScan(scanIndx);
            if (scan) {
                _mw->spectraWidget->setScan(scan);
                 responce =  "OK: scan set";  return 200;
            }
        }
    }

    else if (path.startsWith("/setPeptideSequence/") and fieldCount == 3) {
        QString peptideSeq(command[1]);
        int charge =  command[2].toInt();
        Peptide peptide(peptideSeq.toStdString(),charge);
        if (peptide.isGood()) {
            _mw->setPeptideSequence(command[1] + "/" + command[2]);
            responce =  "OK: peptide changed";  return 200;
         }
    }

    else if (path.startsWith("/getEICView")) {
            responce += _mw->getEicWidget()->eicToTextBuffer();
            return 200;
    }

    responce += ("ERROR: Command=" + path + " has failed");
    return 400;
}
