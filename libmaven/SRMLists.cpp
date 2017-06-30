#include "SRMLists.h"

SRMLists::SRMLists(vector<mzSample*>samples, deque<Compound*> compoundsDB){
    this->samples = samples;
    this->compoundsDB = compoundsDB;
}

vector<mzSlice*> SRMLists::getSrmSlices(double amuQ1, double amuQ3, int userPolarity, bool rtMatch, bool associateCompoundNames) {
	//Merged with Maven776 - Kiran
    QMap<QString, Scan*>seenMRMS;
    //+118.001@cid34.00 [57.500-58.500]
    //+ c ESI SRM ms2 102.000@cid19.00 [57.500-58.500]
    //-87.000 [42.500-43.500]
    //- c ESI SRM ms2 159.000 [113.500-114.500]

    QRegExp rx1a("[+/-](\\d+\\.\\d+)");
    QRegExp rx1b("ms2\\s*(\\d+\\.\\d+)");
    QRegExp rx2("(\\d+\\.\\d+)-(\\d+\\.\\d+)");
    int countMatches=0;

    vector<mzSlice*>slices;
    for(int i=0; i < samples.size(); i++ ) {
        mzSample* sample = samples[i];
        for( int j=0; j < sample->scans.size(); j++ ) {
            Scan* scan = sample->getScan(j);
            if (!scan) continue;

            QString filterLine(scan->filterLine.c_str());

            if (filterLine.isEmpty())   continue;

            if (seenMRMS.contains(filterLine)){
                if(scan->intensity[0] <= seenMRMS.value(filterLine)->intensity[0])    continue;
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
                if( rx1a.indexIn(filterLine) != -1 ) {
                    precursorMz = rx1a.capturedTexts()[1].toDouble();
                } else if ( rx1b.indexIn(filterLine) != -1 ) {
                    precursorMz = rx1b.capturedTexts()[1].toDouble();
                }
            }

            if (productMz == 0) {
                if ( rx2.indexIn(filterLine) != -1 ) {
                    float lb = rx2.capturedTexts()[1].toDouble();
                    float ub = rx2.capturedTexts()[2].toDouble();
                    productMz = lb+(ub-lb)/2;
                }
            }

            if (precursorMz != 0 && productMz != 0 ) {
                compound = findSpeciesByPrecursor(precursorMz,productMz,rt,rtMatch,polarity,amuQ1,amuQ3);
            }

            if (compound) {
                compound->srmId = filterLine.toStdString();
                s->compound = compound;
                s->rt = compound->expectedRt;
                countMatches++;
            }
        }                       
	}
	return slices;
}


Compound *SRMLists::findSpeciesByPrecursor(float precursorMz, float productMz, float rt, bool rtMatch, int polarity,double amuQ1, double amuQ3) {
    
    Compound* x=NULL;
    float dist=FLT_MAX;

    for(unsigned int i=0; i < compoundsDB.size(); i++ ) {
            if (compoundsDB[i]->precursorMz == 0 ) continue;
            //cerr << polarity << " " << compoundsDB[i]->charge << endl;
            if ((int) compoundsDB[i]->charge != polarity ) continue;
            float a = abs(compoundsDB[i]->precursorMz - precursorMz);
            if ( a > amuQ1 ) continue; // q1 tollorance
            float b = abs(compoundsDB[i]->productMz - productMz);
            if ( b > amuQ3 ) continue; // q2 tollarance
            float c = abs( compoundsDB[i]->expectedRt - rt);
            if ( rtMatch == 0) c = 0.0; 
            float d = sqrt(a*a+b*b+c*c);
            if ( d < dist) { x = compoundsDB[i]; dist=d;}
    }
    return x;
}