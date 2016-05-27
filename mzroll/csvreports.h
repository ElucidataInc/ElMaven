#ifndef CSVREPORT_H
#define CSVREPORT_H

#include "stable.h"
#include "mzUtils.h"
#include "mzSample.h"

using namespace std;
using namespace mzUtils;

class mzSample;
class EIC;
class PeakGroup;

class CSVReports {

	public:
		CSVReports(){};
        CSVReports(vector<mzSample*>& insamples);
		~CSVReports();
        void openGroupReport(string filename);
        void openPeakReport(string filename);
		void addGroup(PeakGroup* group);
		void closeFiles();
		void setSamples(vector<mzSample*>& insamples) { samples = insamples; }
		void setUserQuantType(PeakGroup::QType t) { qtype=t; }
        void setTabDelimited()   { SEP="\t"; }
        void setCommaDelimited() { SEP=","; }

	private:
		void writeGroupInfo(PeakGroup* group);
		void writePeakInfo(PeakGroup* group);
						
		int groupId;	//sequential group numbering
        string SEP;

		vector<mzSample*>samples;
		ofstream groupReport;
		ofstream peakReport;
		PeakGroup::QType qtype;
		

};

#endif
