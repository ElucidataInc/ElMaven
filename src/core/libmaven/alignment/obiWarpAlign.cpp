#include "mzAligner.h"
#include "obiWarpAlign.h"
#include "mzMassSlicer.h"
#include "mzSample.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <QJsonArray>
#include <QJsonValue>


void ObiWarpAlign::alignSampleRts(mzSample* sample, vector<float> &mzPoints, ObiWarp& obiWarp ,bool setAsReference){

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
        obiWarp.setReferenceData(rtPoints, mzPoints, mxn);
    else{
        rtPoints = obiWarp.align(rtPoints, mzPoints, mxn);
        for(int j = 0; j < sample->scans.size(); ++j)
            sample->scans[j]->rt = rtPoints[j];
    }
}

void ObiWarpAlign::obiWarpAlign () {
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

    ObiWarp* obiWarp = new ObiWarp(params);

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

    alignSampleRts(referenceSample, mzPoints, *obiWarp, true);

    for(int i=0 ; i < samples.size();++i){
        cerr<<"Alignment: "<<(i+1)<<"/"<<samples.size()<<" processing..."<<endl;
        if(i == referenceSampleIndex)
            continue;
        alignSampleRts(samples[i], mzPoints,*obiWarp ,false);
    }
    
    delete obiWarp;
    cerr<<"Alignment complete"<<endl;    
    
}