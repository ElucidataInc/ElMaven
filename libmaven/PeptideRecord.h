#ifndef PEPTIDE_RECORD
#define PEPTIDE_RECORD

#include "Peptide.hpp"

class PeptideRecord { 
	public:
		string sampleName;
		int scanNum;
		double precursorMz;
		int charge;
		double mvh;
		string peptide;
		string protein;
        string unmodPeptide;

        string unmodify() {
            Peptide pep(peptide,charge);
            peptide = pep.stripped;
            return peptide;
        }
};

#endif
