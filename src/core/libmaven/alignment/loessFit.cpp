#include "loessFit.h"

void LoessFit::updateSampleRts(QJsonObject &sampleRts) {
    for(mzSample* sm: samples) {
        auto it = sampleRts.find(QString(sm->getSampleName().c_str()));
        if (it != sampleRts.end()) {
            QJsonArray rtArr = it.value().toArray();
            for(int index = 0; index != rtArr.size(); index++)
                sm->scans[index]->rt =  (float)rtArr[index].toDouble();
        }
    }
}

void LoessFit::updateGroupsRts(QJsonObject &groupsRts) {
    for (int grpIndex=0; grpIndex<groups.size(); grpIndex++) {
        PeakGroup* grp = groups.at(grpIndex);
        for (int peakIndex=0; peakIndex<grp->getPeaks().size(); peakIndex++) {
            Peak peak = grp->getPeaks().at(peakIndex);
            auto it = groupsRts.find(QString(peak.getSample()->getSampleName().c_str()));
            if(it != groupsRts.end()) {
                QJsonObject grpObj = it.value().toObject();
                if(!grpObj.isEmpty()){

                    // we have to form the group name in such a manner because that's how it was formed when
                    // we formed our groupsJson. see preProcessing() for more
                    QString groupName = QString(grp->getName().c_str()) + QString("_") + QString::number(grpIndex);
                    groupName.replace(" ", "");
                    QJsonValue val = grpObj.find(groupName).value();
                    if(!val.isNull()) {
                        deltaRt[make_pair(grp->getName(), peak.getSample()->getSampleName())] -= val.toDouble();
                        Peak* p = grp->getPeak(peak.getSample());
                        p->rt = val.toDouble();
                    }
                }
            }
        }
    }
}

void LoessFit::updateRts(QJsonObject &parentObj) {
    if(!parentObj.isEmpty()) {

        QJsonObject groupsRts;
        QJsonObject sampleRts;

        // value of groups and samples is expected to be an object.If this is not the case we return
        if(!parentObj["groups"].isObject() && !parentObj["samples"].isObject())
            return;

        groupsRts = parentObj["groups"].toObject();
        sampleRts = parentObj["samples"].toObject();

        updateSampleRts(sampleRts);
        updateGroupsRts(groupsRts);

    }

}

void LoessFit::readDataFromPython() {
        while(pythonProg->bytesAvailable())
                processedDataFromPython += pythonProg->readLine(1024*1024);
        
}

void LoessFit::writeToPythonProcess(QByteArray data){

        if(pythonProg->state()!=QProcess::Running){
                qDebug()<<"Error in pipe- data sent to be written but python process is not running";
                return;
        }
        QTextStream stream(pythonProg);
        int quantumOfData=1024*1024;
        for(int i=0;i<data.size();i+=quantumOfData){
                stream<<data.mid(i,quantumOfData);
        }
        stream.flush();
        pythonProg->closeWriteChannel();
}

void LoessFit::sendDataToPython() {
    // prepare the data we have to send to python
    QJsonObject jObj;
    jObj.insert("groups", groupsJson);
    jObj.insert("rts", rtsJson);


    QJsonDocument jDoc(jObj);
    QByteArray data = jDoc.toJson();

    writeToPythonProcess(data);

}

void LoessFit::runPythonProg() {

    if(pythonProg->state() != QProcess::NotRunning)
        pythonProg->kill();

    pythonProg->start();

    /**
     * wait for python to start otherwise exit
     */
    if(pythonProg->waitForStarted(-1)) {
        qDebug()<<"python program is running...";
        sendDataToPython();
    }
    else{
        qDebug()<<"Python program did not start. Check availability of execcutable";
    }
}


void LoessFit::loessFit() {
    preProcessing();

    // initialize processedDataFromPython with null 
    processedDataFromPython="";
    /**runPythonProg()
     * sends the json of groups and samples rt to the python exe. for more look in sendDataToPython()
     * python exe is going to correct the rts and send it back to us in json format
     */
    runPythonProg();
    // wait for processing of data by python program
    pythonProg->waitForFinished(-1);                        

    // convert the data to json
    QJsonDocument jDoc;
    QJsonObject parentObj;

    // if jDoc is null that means the json returned from python is malformed
    // in such a case our rts wont update with new values
    jDoc = QJsonDocument::fromJson(processedDataFromPython);

    QString errorMessage=QString::number(processedDataFromPython.size());

    if(!jDoc.isNull()){
        parentObj = jDoc.object();
    }
    else{
        if(processedDataFromPython.size()==0){
                errorMessage=errorMessage + " good groups found." +"<br>"+"Relax parameters for better result";
        }
        else{
                errorMessage=errorMessage+"<br>"+"Incomplete data, re-run alignment";
        }
        qDebug()<<errorMessage;

        return;
    }

    if(!parentObj.isEmpty()) {
        updateRts(parentObj);
        qDebug()<<"Alignment complete";
    }
    else {
        errorMessage=errorMessage+"<br>"+"Incomplete data, re-run alignment";
        qDebug()<<errorMessage;
        return;
    }
}

void LoessFit::preProcessing() {
    if (groups.size() == 0) return;
    

    groupsJson = QJsonObject();
    rtsJson = QJsonObject();

    for (unsigned int ii=0; ii<groups.size();ii++) {
        PeakGroup* grp = groups.at(ii);
        QJsonArray jArr;

        QJsonObject expectedRtObj;
        QString expectedRtKey="expectedRt";
        if(alignWrtExpectedRt && grp->compound!=NULL){
            expectedRtObj.insert(expectedRtKey,grp->compound->expectedRt);
        }
        else{
            expectedRtObj.insert(expectedRtKey,-1);
        }
        jArr.push_back(QJsonValue(expectedRtObj));

        for (unsigned int jj=0; jj<grp->getPeaks().size(); jj++) {
            Peak peak = grp->getPeaks().at(jj);
            deltaRt[make_pair(grp->getName(), peak.getSample()->getSampleName())] = peak.rt;

            QJsonObject obj;
            obj.insert(QString(peak.getSample()->getSampleName().c_str()), peak.rt);
            jArr.push_back(QJsonValue(obj));

        }
        // group name and group number makes the key unique, hence this is important
        QString key = QString(grp->getName().c_str()) + QString("_") +  QString::number(ii);
        key.replace(" ", "");
        groupsJson.insert(key, QJsonValue(jArr));
    }
    
    samples.clear();
    set<mzSample*> samplesSet;
    for (unsigned int i=0; i < groups.size();  i++ ) {
        for ( unsigned int j=0; j < groups[i]->peakCount(); j++ ) {
            Peak& p = groups[i]->peaks[j];
            mzSample* sample = p.getSample();
            if (sample) samplesSet.insert(sample);
        }
    }

    samples.resize(samplesSet.size());
    copy(samplesSet.begin(), samplesSet.end(),samples.begin());        
	
    for(unsigned int i=0; i < samples.size(); i++ ) {
        samples[i]->saveOriginalRetentionTimes();
        QJsonArray jArr;
        for(unsigned int ii=0; ii < samples[i]->scans.size(); ii++ ) {
            jArr.push_back(samples[i]->scans[ii]->rt);
        }
        rtsJson.insert(QString(samples[i]->getSampleName().c_str()), jArr);
    }
    return;
}
