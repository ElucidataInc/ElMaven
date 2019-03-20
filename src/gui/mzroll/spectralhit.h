#ifndef SPECTRALHIT
#define SPECTRALHIT

/*
@author: Sahil
*/
//TODO: Sahil, Added while merging mzfileio
class ProteinHit {
    public:
        int id;
        QString accession;
        bool isDecoy;
        int cluster;
        int proteinGroup;
        int length;
        QString description;
        QString sequence;
        QString geneSymbol;

        ProteinHit() {  id=-1; isDecoy=false; cluster=-1; proteinGroup=-1; length=-1;}
};

class SpectralHit {

	public: 
		double score;
		Scan* scan;
		QString sampleName;
		QVector<double> mzList;
		QVector<double> intensityList;
		float productPPM;
		int matchCount;
		float precursorMz;
		float xcorr;
		float mvh;
		int rank;
		bool decoy;
		int charge;
		double massdiff;
 		int scannum;
		QString compoundId;
		QString fragmentId;
        QString unmodPeptideSeq;
        bool isFocused;
        float rt;
        int id;
        QMap<QString,int> proteins;
        QMap<int,float> mods;
        
	SpectralHit() { 
		scan = NULL; 
		precursorMz = 0;
		matchCount = 0; 
		productPPM = 0.0; 
		score = 0;
		xcorr = 0; 
		massdiff = 0;
		mvh = 0; 
		rank = 0; 
		decoy = false;
		charge = 0;
		scannum = 0;
        isFocused = false;
        rt = 0;
        id = 0;
	}

        double getMaxIntensity() { 
            double maxIntensity=0; 
            Q_FOREACH(double x, intensityList) if(x>maxIntensity) maxIntensity=x;
            return  maxIntensity;
        }

        double getMinMz() {
            double min = mzList.first();
            Q_FOREACH(double x, mzList)  if(x<min) min=x; 
            return  min;
        }

        double getMaxMz() {
            double max = mzList.last();
            Q_FOREACH(double x, mzList)  if(x>max) max=x;
            return max;
        }

        QString getProteinIds() { QStringList ids = proteins.uniqueKeys(); return(ids.join(";")); }
		QStringList getProteins() { return proteins.uniqueKeys(); } 

		QString getUnchargedPeptideString() { 
			int slashIndx =  fragmentId.lastIndexOf('/');
			if(slashIndx != 0 ) {
				return fragmentId.left(slashIndx);
			} else {
				return fragmentId;
			}
		}

		/*
        @author: Sahil
        */
        //TODO: Sahil, Added while merging mzfileio
        QString getModPeptideString() {
            QString peptide = this->unmodPeptideSeq;
            QString modPeptideSeq;
            for(int i=0; i < peptide.length(); i++ ) {
                modPeptideSeq += peptide[i];
                QString sign;
                if(mods.count(i))  {
                   if(mods[i]>0) sign="+";
                   modPeptideSeq += ("[" + sign + QString::number(mzUtils::round(mods[i])) + "]");
                }
            }
            modPeptideSeq = modPeptideSeq + "/" + QString::number(this->charge);
            return modPeptideSeq;
        }

	static bool compScan(SpectralHit* a, SpectralHit* b ) { return a->scan < b->scan; }
	static bool compPrecursorMz(SpectralHit* a, SpectralHit* b) { return a->precursorMz < b->precursorMz; }
	static bool compScore(SpectralHit* a, SpectralHit* b ) { return a->score > b->score; }
	static bool compMVH(SpectralHit* a, SpectralHit* b ) { return a->mvh > b->mvh; }

};
Q_DECLARE_METATYPE(SpectralHit*);
Q_DECLARE_METATYPE(ProteinHit*);

#endif
