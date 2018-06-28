#include "mzAligner.h"
#include "mzMassSlicer.h"
#include "mzSample.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <QJsonArray>
#include <QJsonValue>


vector<double> Aligner::groupMeanRt() {
    //find retention time deviation
    vector<double> groupRt(groups.size());
    for (unsigned int i=0; i < groups.size(); i++ ) groupRt[i]=groups[i]->medianRt();
    return(groupRt);
}

// polyfit


void PolyFit::saveFit() {
	cerr << "saveFit()" << endl;
	fit.clear();
	fit.resize(samples.size());
	for(unsigned int i=0; i < samples.size(); i++ ) {
		fit[i].resize(samples[i]->scans.size());
		for(unsigned int ii=0; ii < samples[i]->scans.size(); ii++ ) {
			fit[i][ii]=samples[i]->scans[ii]->rt;
		}
	}
}

double PolyFit::checkFit() { 
	vector<double> groupRt  = groupMeanRt();

	double sumR2=0;
	for(unsigned int i=0; i < groups.size(); i++ ) {
		for(unsigned int j=0; j < groups[i]->peakCount(); j++ ) {
			sumR2 += POW2(groupRt[i]-groups[i]->peaks[j].rt);
		}
	}
	cerr << "groups=" << groups.size() << " checkFit() " << sumR2 << endl;
	return sumR2;
}

void PolyFit::polyFitAlgo() {
    if (groups.size() == 0) return;


	for (unsigned int ii=0; ii<groups.size();ii++) {
		PeakGroup* grp = groups.at(ii);
		for (unsigned int jj=0; jj<grp->getPeaks().size(); jj++) {
			Peak peak = grp->getPeaks().at(jj);
			deltaRt[make_pair(grp->getName(), peak.getSample()->getSampleName())] = peak.rt;
		}
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
    }

    saveFit();
	double R2_before = checkFit();

    cerr << "Max Itterations: " << maxIterations << endl;
    for(int iter=0; iter < maxIterations; iter++) {
        cerr << iter << endl;

        /** polyfit algo starts **/ 
        
        polyFit(polynomialDegree);


        /** polyfit algo ends **/

        double R2_after = checkFit();
        cerr << "Itteration:" << iter << " R2_before" << R2_before << " R2_after=" << R2_after << endl;

        if (R2_after > R2_before) {
            cerr << "done...restoring previous fit.." << endl;
            restoreFit();
            break;
        } else {
            saveFit();
        }
        R2_before = R2_after;
    }

}

void PolyFit::restoreFit() {
	cerr << "restoreFit() " << endl;
	for(unsigned int i=0; i < samples.size(); i++ ) {
		for(unsigned int ii=0; ii < samples[i]->scans.size(); ii++ ) {
			samples[i]->scans[ii]->rt = fit[i][ii];
		}
	}
}

void PolyFit::polyFit(int poly_align_degree) {

	if (groups.size() < 2 ) return;
	cerr << "Align: " << groups.size() << endl;

	vector<double> GroupsMeansRt  = groupMeanRt();

	for (unsigned int s=0; s < samples.size(); s++ ) {
        mzSample* sample = samples[s];
        if (sample == NULL) continue;

        StatisticsVector<float>subj;
        StatisticsVector<float>ref;
        int n=0;

        map<int,int>duplicates;
        for(unsigned int j=0; j < groups.size(); j++ ) {
            Peak* p = groups[j]->getPeak(sample);
            if (!p) continue;
            if (!p || p->rt <= 0 || GroupsMeansRt[j] <=0 ) continue;

            int intTime = (int) p->rt*100;
            duplicates[intTime]++;
            if ( duplicates[intTime] > 5 ) continue;

            ref.push_back(GroupsMeansRt[j]);
            subj.push_back(p->rt);
            n++; 
        }
        if ( n < 10 ) continue;

        PolyAligner polyAligner(subj,ref);
        AlignmentStats* stats = polyAligner.optimalPolynomial(1,poly_align_degree,10);

        sampleDegree[sample] = stats->poly_align_degree;
        sampleCoefficient[sample] = stats->getCoeffients();

        if (stats->transformImproved()) {

            bool failedTransformation=false;
            for(unsigned int ii=0; ii < sample->scans.size(); ii++ ) {
                double newrt =  stats->predict(sample->scans[ii]->rt);
                if (std::isnan(newrt) || std::isinf(newrt))  failedTransformation = true;
                break;
            }

            if (!failedTransformation) {
                for(unsigned int ii=0; ii < sample->scans.size(); ii++ ) {
                    sample->scans[ii]->rt = stats->predict(sample->scans[ii]->rt);
                }

                for(unsigned int ii=0; ii < groups.size(); ii++ ) {
                    Peak* p = groups[ii]->getPeak(sample);
                    if (p)  p->rt = stats->predict(p->rt);
                }
            }
        } else 	{
            cerr << "APPLYTING TRANSFORM FAILED! " << endl;
        }
    }
}

PolyFit::PolyAligner::PolyAligner(StatisticsVector<float>& subj, StatisticsVector<float>& ref) {

	if( subj.size() != ref.size()) { 
		cerr << "alignVector failed.. vectors are different size\n";
		exit(-1);
	}

	//copy values that are presents in both vectors
	for(unsigned int indx=0; indx < subj.size(); indx++ ) {
		if (subj[indx]> 0 and ref[indx] > 0)  { 
			subjVector.push_back(subj[indx]);
			refVector.push_back(ref[indx]);
		}
	}
	mtRand = new MTRand(time(NULL));
	calculateOutliers(3);
} 

double PolyFit::PolyAligner::calculateR2(AlignmentStats* model) { 
	double R2=0;
	int N = subjVector.size();
	if (N == 0) return DBL_MAX;

	for(int i=0; i < N; i++ ) {
         double newrt = model->predict(subjVector[i]);
		 R2 += POW2(refVector[i] - newrt);
	}
	return sqrt(R2/N);
}

double PolyFit::PolyAligner::countInliners(AlignmentStats* model, float zValueCutoff) { 

	int N = subjVector.size();
	if (N == 0) return 0;

	StatisticsVector<float>residuals;
	for(int i=0; i < N; i++ ) {
		 if(outlierVector[i] == true ) {
         	double newrt = model->predict(subjVector[i]);
		 	residuals.push_back(POW2(refVector[i] - newrt));
		 }
	}

	float meanResidual=residuals.mean();
	float stdResidual=sqrt(residuals.variance(meanResidual));

	int inlinerCount=0; 
	double R2=0;
	for(unsigned int i=0; i < residuals.size(); i++ ) {
		if (abs((residuals[i] - meanResidual)/stdResidual) < zValueCutoff ) {
			R2 += residuals[i];
			inlinerCount++;
		}
	}
	if (inlinerCount > 0 ) {
		return sqrt(R2/inlinerCount);
	} else {
		return DBL_MAX;
	}
}

void PolyFit::PolyAligner::randomOutliers(double keepFrac) {

	int N = subjVector.size();
	if(!outlierVector.size()) outlierVector = vector<bool>(N,false);

	for(int i=0; i < N; i++ ) {
		 double r = mtRand->rand(); //random number from 0 to 1
		 if (r < keepFrac ) {
			 outlierVector[i]=false;
		 } else {
			 outlierVector[i]=true;
		 }
	}
}

PolyFit::AlignmentStats* PolyFit::PolyAligner::align(int degree) {

	AlignmentStats* stats = new AlignmentStats();
	stats->poly_align_degree = degree; 
	if (stats->poly_align_degree >= 100) stats->poly_align_degree=99;
	
	int knownInBoth=subjVector.size();
	double* x  =   new double[subjVector.size()];
	double* ref =  new double[subjVector.size()];

	//copy values that are presents in both vectors to c arrays
	for(unsigned int indx=0; indx < subjVector.size(); indx++ ) {
		x[indx]  =subjVector[indx];
		ref[indx]=refVector[indx];
	}

	stats->N = knownInBoth;

	//remove outliers?
	if(outlierVector.size() > 0 ) {
		int j=0;
		stats->N=0;
		for(unsigned int i=0; i< outlierVector.size();i++) { 
			if(outlierVector[i] == false) { 
				x[j]= x[i];
				ref[j]=ref[i];
				stats->N++;
				j++;
			}
		}
		//cerr << "Removed " << knownInBoth - stats->N << endl;
	}

	if(stats->poly_align_degree > stats->N/3 ) { stats->poly_align_degree = stats->N/3; }

	double* result = new double[(stats->poly_align_degree+1)];
	double* w = new double[(stats->poly_align_degree+1)*(stats->poly_align_degree+1)];

	
	if(stats->N > 0) {
		stats->R_before=0; 
		stats->R_after=0;

		sort_xy(x, ref, stats->N, 1, 0);
        leasqu(stats->N, x, ref, stats->poly_align_degree, w, (stats->poly_align_degree+1), result);	//polynomial fit

		stats->transformedFailed=0;
		for(int ii=0; ii < stats->N; ii++)  { 
            double newrt = leasev(result, stats->poly_align_degree, x[ii]);
            if (newrt != newrt || std::isinf(newrt)) {
				cerr << "Transform failed: " << ii << "\t" << x[ii] << "-> " << newrt << endl;
				stats->transformedFailed++;
			} else { 
				stats->R_before += POW2(ref[ii] - x[ii]);
				stats->R_after  += POW2(ref[ii] - newrt);
			}
		}

		if(stats->transformedFailed == 0) {
			stats->poly_transform_result = vector<double>(stats->poly_align_degree+1,0);
			for(int ii=0; ii <= stats->poly_align_degree; ii++ ) stats->poly_transform_result[ii]=result[ii]; 
		}
	}

    delete[] result;
	delete[] w;
	delete[] x;
	delete[] ref;
	return stats;
}

void PolyFit::PolyAligner::calculateOutliers(int initDegree) { 
	if (subjVector.size() <= 2) return;

	AlignmentStats* stats = this->align(initDegree); 
	int N = subjVector.size();

	StatisticsVector<float> distVector(N,0);
	for(int i=0; i < N; i++ ) {
		if(refVector[i]>0) {
			distVector[i] = POW2(refVector[i] - stats->predict(subjVector[i]));
		}
	}

	double meanDelta = distVector.mean();
	double stddev = sqrt(distVector.variance(meanDelta));

	outlierVector = vector<bool>(N,false);
	for(int i=0; i < N; i++ ) {
		distVector[i] = abs((distVector[i] - meanDelta)/stddev);
		if (distVector[i] > 1.5) {
			outlierVector[i] = true;
		} else {
			outlierVector[i] = false;
		}
    }

	/*(
	cerr << "OUTLIERS: " << endl;
	for(int i=0; i < N; i++ ) {
		cerr << outlierVector[i] << " r=" << refVector[i] << " pred=" << stats->predict(subjVector[i]) << " dist=" << distVector[i] << endl;
	}
	*/

	delete stats;
}


PolyFit::AlignmentStats* PolyFit::PolyAligner::optimalPolynomial(int fromDegree, int toDegree, int sampleSize=0) { 

	float R_best = FLT_MAX;
    AlignmentStats* bestModel = NULL;

	for(int deg=fromDegree; deg<toDegree; deg++) { 
		for(int attempt=0; attempt <= sampleSize; attempt++ ) { 

			AlignmentStats* thisModel = this->align(deg); 
			double R_thismodel = this->calculateR2(thisModel);
			//double R_thismodel = this->countInliners(thisModel,2.0);

			randomOutliers(0.35); //new random subset of data

			if (R_thismodel < R_best ) { 
                //cerr << "optimalPolynomial() \t attempt=" << attempt << " R2=" << R_thismodel << endl;
				if (bestModel) delete(bestModel);
				bestModel = thisModel;
				R_best = R_thismodel; 
				continue;
			}  else {
				delete(thisModel);
			}

		}
	}

	if ( bestModel ) return bestModel;
	else return this->align(1);
}

void PolyFit::PolyAligner::test() { 

	vector<float> x(100);
	vector<float> y(100);

	for(float i=0; i < 100; i++ ) { 
		float xi = i/100;
		x[i] = xi;
		y[i] = 3+(2*xi)-0.5*(xi*xi)+0.25*(xi*xi*xi)-0.1*(xi*xi*xi*xi)+0.01*(xi*xi*xi*xi*xi)-0.001*(xi*xi*xi*xi*xi);
	}
	
	this->subjVector = x;
	this->refVector  = y;
	this->calculateOutliers(2);
	this->optimalPolynomial(0,10);

	//for(float i=0; i < 100; i++ ) { 
	//	cerr << x[i] << " " << y[i] << " " << stats->predict(x[i]) << endl;
	//}

	//stats->summary(); 

	exit(-1);
}


// loess fit

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

// obiwarp

void ObiWarp::alignSampleRts(mzSample* sample, vector<float> &mzPoints, bool setAsReference){

    vector<float> rtPoints(sample->scans.size());
    vector<vector<float> > mxn(sample->scans.size());
    for(int j = 0; j < sample->scans.size(); ++j){
        rtPoints[j] = sample->scans[j]->originalRt;
        mxn[j] = vector<float> (mzPoints.size());
    }

    for(int j=0;j<sample->scans.size();++j){
        for(int k=0;k<sample->scans[j]->mz.size();++k){
            if( sample->scans[j]->mz[k] < mzPoints.front() || sample->scans[j]->mz[k] > mzPoints.back())
                continue;
            int index = upper_bound( mzPoints.begin(), mzPoints.end(), sample->scans[j]->mz[k] ) - mzPoints.begin() -1;

            mxn[j][index] =  max(mxn[j][index] , sample->scans[j]->intensity[k]);

        }
    }
    
    if(setAsReference)
        setReferenceData(rtPoints, mzPoints, mxn);
    else{
        rtPoints = align(rtPoints, mzPoints, mxn);
        for(int j = 0; j < sample->scans.size(); ++j)
            sample->scans[j]->rt = rtPoints[j];
    }
}

void ObiWarp::setReferenceData(vector<float> &rtPoints, vector<float> &mzPoints, vector<vector<float> >& intMat){
    _tm_vals = rtPoints.size();
    tmPoint = new float[_tm_vals];
    for(int i=0; i < _tm_vals ; ++i)
        tmPoint[i] = rtPoints[i];
    _tm.take(_tm_vals, tmPoint);

    _mz_vals = mzPoints.size();
    mzPoint = new float[_mz_vals];
    for(int i = 0; i < _mz_vals; ++i)
        mzPoint[i] = mzPoints[i];
    _mz.take(_mz_vals, mzPoint);

    assert(_tm_vals == intMat.size());
    MatF mat(_tm_vals, _mz_vals);
    for(int i=0; i < _tm_vals; ++i){
        assert(_mz_vals == intMat[i].size());
        for(int j = 0; j < _mz_vals; ++j)
            mat(i,j) = intMat[i][j];
    }
    _mat.take(mat);

}

vector<float> ObiWarp::align(vector<float> &rtPoints, vector<float> &mzPoints, vector<vector<float> >& intMat){
    
    VecF tm;
    int tm_vals = rtPoints.size();
    float* tmPoint = new float[tm_vals];
    for(int i = 0; i < tm_vals; ++i)
        tmPoint[i] = rtPoints[i];
    tm.take(tm_vals, tmPoint);

    VecF mz;
    int mz_vals = mzPoints.size();
    float* mzPoint = new float[mz_vals];
    for(int i = 0; i < mz_vals; ++i)
        mzPoint[i] = mzPoints[i];
    mz.take(mz_vals, mzPoint);

    assert(tm_vals = intMat.size());
    MatF mat(tm_vals, mz_vals);
    for(int i = 0; i < tm_vals; ++i){
        assert(mz_vals == intMat[i].size());
        for(int j = 0; j < mz_vals; ++j)
            mat(i,j)=intMat[i][j];
    }

    MatF smat;

    char *score_ptr = &params->score[0];
    dyn.score(_mat, mat, smat, score_ptr);

    if (!params->nostdnrm) {
        if (!smat.all_equal()) { 
            smat.std_normal();
        }
    }

    int gp_length = smat.rows() + smat.cols();

    VecF gp_array;
    dyn.linear_less_before(params->gap_extend,params->gap_init,gp_length,gp_array);

    int minimize = 0;
    dyn.find_path(smat, gp_array, minimize, params->factor_diag, params->factor_gap, params->local, params->init_penalty);

    VecI mOut;
    VecI nOut;
    dyn.warp_map(mOut, nOut, params->response, minimize);

    VecF nOutF;
    VecF mOutF;
    tm_axis_vals(mOut, mOutF, _tm,_tm_vals);
    tm_axis_vals(nOut, nOutF, tm,tm_vals); //
    warp_tm(nOutF, mOutF, tm);
    
    vector<float> alignedRts;
    float* rts = tm.pointer();
    for(int i = 0; i < tm_vals; ++i)
        alignedRts.push_back(rts[i]);
    
    // delete[] tmPoint;
    // delete[] mzPoint;

    return alignedRts;
}

void ObiWarp::tm_axis_vals(VecI &tmCoords, VecF &tmVals,VecF &_tm ,int _tm_vals){
    VecF tmp(tmCoords.length());
    for (int i = 0; i < tmCoords.length(); ++i) {
        if (tmCoords[i] < _tm_vals) {
            tmp[i] = _tm[tmCoords[i]];
        }
        else {
            printf("asking for time value at index: %d (length: %d)\n", tmCoords[i], _tm_vals);
            exit(1);
        }
    }
    tmVals.take(tmp);
}

void ObiWarp::warp_tm(VecF &selfTimes, VecF &equivTimes, VecF &_tm){
    VecF out;
    VecF::chfe(selfTimes, equivTimes, _tm, out, 1);  // run with sort option
    _tm.take(out);
}

void ObiWarp::obiWarp () {
    std::cerr<<"Aligning Sample Retention times..."<<std::endl;

    if(referenceSampleIndex < 0) {
        /**
         * currently reference sample is choosen randomly,
         * TODO: give user options to choose reference sample and pass index of
         * that sample as referenceSampleIndex
         */
        srand(time(NULL));
        referenceSampleIndex = rand()%samples.size();
    }
    assert(referenceSampleIndex < samples.size());

    // ObiWarp* obiWarp = new ObiWarp(obiParams);

    float binSize = params->binSize;
    float minMzRange = 1e9;
    float maxMzRange = 0;
    
    mzSample* referenceSample = samples[referenceSampleIndex];
    for(int j=0;j<referenceSample->scans.size();++j){
        for(int k=0;k<referenceSample->scans[j]->mz.size();++k){
            minMzRange = min ( minMzRange, referenceSample->scans[j]->mz[k] );
            maxMzRange = max ( maxMzRange, referenceSample->scans[j]->mz[k] );
        }
    }

    maxMzRange += 10;
    minMzRange -= 10;
    if(minMzRange < 0.f)
        minMzRange = 0.f;
    minMzRange = floor(minMzRange);
    maxMzRange = ceil(maxMzRange);

    vector<float> mzPoints;
    for(float bin = minMzRange; bin <= maxMzRange; bin += binSize)
        mzPoints.push_back(bin);

    alignSampleRts(referenceSample, mzPoints, true);

    for(int i=0 ; i < samples.size();++i){
        cerr<<"Alignment: "<<(i+1)<<"/"<<samples.size()<<" processing..."<<endl;
        if(i == referenceSampleIndex)
            continue;
        alignSampleRts(samples[i], mzPoints, false);
    }
    
    cerr<<"Alignment complete"<<endl;    
    
}