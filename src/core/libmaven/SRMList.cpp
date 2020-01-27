#include "SRMList.h"
#include "datastructures/mzSlice.h"
#include "Compound.h"
#include "databases.h"
#include "mzSample.h"
#include "Scan.h"

SRMList::SRMList(vector<mzSample*>samples, deque<Compound*> compoundsDB){
    this->samples = samples;
    this->compoundsDB = compoundsDB;
}

vector<mzSlice*> SRMList::getSrmSlices(double amuQ1, double amuQ3, int userPolarity, bool associateCompoundNames) {
    QMap<QString, Scan*>seenMRMS;
    int countMatches=0;

    vector<mzSlice*>slices;
    for(int i=0; i < samples.size(); i++ ) {
        mzSample* sample = samples[i];
        for( int j=0; j < sample->scans.size(); j++ ) {
            Scan* scan = sample->getScan(j);
            if (!scan) continue;

            // skipping empty scans
            if (scan->totalIntensity() == 0) continue;

            QString filterLine(scan->filterLine.c_str());

            if (filterLine.isEmpty()) continue;

            if (seenMRMS.contains(filterLine)){
                if(scan->intensity[0] <= seenMRMS.value(filterLine)->intensity[0]) continue;
            }

            seenMRMS.insert(filterLine, scan);
        }
    }

    for (auto filterLine: seenMRMS.keys()){

        Scan* scan = seenMRMS.value(filterLine);
        mzSlice* s = new mzSlice(0,0,0,0);
        s->srmId = scan->filterLine.c_str();
        slices.push_back(s);

        if (associateCompoundNames) {
            //match compounds
            Compound* compound = NULL;
            float precursorMz = scan->precursorMz;
            float productMz   = scan->productMz;
            float rt = scan->rt;
            int   polarity= scan->getPolarity();
            if (polarity==0) filterLine[0] == '+' ? polarity=1 : polarity =-1;
            if (userPolarity) polarity=userPolarity;  //user specified ionization mode

            if ( precursorMz == 0 ) {
                precursorMz = getPrecursorOfSrm(filterLine.toStdString());
            }

            if (productMz == 0) {
                productMz = getProductOfSrm(filterLine.toStdString());
            }

            if (precursorMz != 0 && productMz != 0 ) {
                compound = findSpeciesByPrecursor(precursorMz,productMz,rt,polarity,amuQ1,amuQ3);
            }
            
            if (annotation[string(filterLine.toStdString())]) {
                compound = annotation[filterLine.toStdString()];
            }

            if (compound) {
                compound->srmId() = filterLine.toStdString();
                s->compound = compound;
                s->rt = compound->expectedRt();
                countMatches++;
            }
        }
    }
    return slices;
}

Compound *SRMList::findSpeciesByPrecursor(float precursorMz, float productMz, float rt, int polarity,double amuQ1, double amuQ3) {
    
    Compound* x=NULL;
    float distMz=FLT_MAX;
    float distRt=FLT_MAX;

    for(unsigned int i=0; i < compoundsDB.size(); i++ ) {
            if (compoundsDB[i]->precursorMz() == 0 ) continue;
            //cerr << polarity << " " << compoundsDB[i]->charge << endl;
            if ((int) compoundsDB[i]->charge() != polarity && compoundsDB[i]->charge() != 0) continue;
            float a = abs(compoundsDB[i]->precursorMz() - precursorMz);
            if ( a > amuQ1 ) continue; // q1 tolerance
            float b = abs(compoundsDB[i]->productMz() - productMz);
            if ( b > amuQ3 ) continue; // q3 tolerance
            float dMz = sqrt(a*a+b*b);
            float dRt = abs(compoundsDB[i]->expectedRt() - rt);
            if (  ( dMz < distMz)  ||   ((dMz == distMz) && (dRt < distRt))  ) { 
                x = compoundsDB[i];
                distMz=dMz; 
                distRt=dRt;
            }
    }
    return x;
}

double SRMList::getPrecursorOfSrm(string srmId)
{

    QRegExp precursorA("Q1\=([0-9]+)");
    QRegExp precursorB("Q1\=([0-9]+\.[0-9]+)");
    QRegExp precursorC("ms2\\s*([0-9]+\.[0-9]+)");

    QString srmID = QString::fromStdString(srmId);

    double precursorMz = 0;

    if (precursorA.indexIn(srmID) != -1)
    {
        precursorMz = precursorA.capturedTexts()[1].toDouble();
    }
    else if (precursorB.indexIn(srmID) != -1)
    {
        precursorMz = precursorB.capturedTexts()[1].toDouble();
    }
    else if (precursorC.indexIn(srmID) != -1)
    {
        precursorMz = precursorC.capturedTexts()[1].toDouble();
    }

    return precursorMz;
}

double SRMList::getProductOfSrm(string srmId)
{
    QString srmID = QString::fromStdString(srmId);

    QRegExp productA("Q3\=([0-9]+)");
    QRegExp productB("Q3\=([0-9]+\.[0-9]+)");
    QRegExp productC("([0-9]+\.[0-9]+)-([0-9]+\.[0-9]+)");

    double productMz = 0;

    if (productA.indexIn(srmID) != -1)
    {
        productMz = productA.capturedTexts()[1].toDouble();
    }
    else if (productB.indexIn(srmID) != -1)
    {
        productMz = productB.capturedTexts()[1].toDouble();
    }
    else if (productC.indexIn(srmID) != -1)
    {
        double lb = productC.capturedTexts()[1].toDouble();
        double ub = productC.capturedTexts()[2].toDouble();
        productMz = lb + (ub - lb) / 2;
    }

    return productMz;
}

deque<Compound*> SRMList::getMatchedCompounds(string srmId, double amuQ1, double amuQ3, int polarity) {

    deque<Compound*> matchedCompounds;

    float precursorMz = getPrecursorOfSrm(srmId);
    float productMz = getProductOfSrm(srmId);

    for(unsigned int i=0; i < compoundsDB.size(); i++ ) {
        if ((int) compoundsDB[i]->charge() != polarity && compoundsDB[i]->charge() != 0) continue;
        if (compoundsDB[i]->precursorMz() == 0 ) continue;
        float a = abs(compoundsDB[i]->precursorMz() - precursorMz);
        if ( a > amuQ1 ) continue; // q1 tolerance
        float b = abs(compoundsDB[i]->productMz() - productMz);
        if ( b > amuQ3 ) continue; // q3 tolerance

        matchedCompounds.push_back(compoundsDB[i]);
    }

    return matchedCompounds;
}
