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
		QVector<double>mzList;
		QVector<double>intensityList;
        QVector<QString>annotations; //TODO: Sahil, Added whiler merging point
		float productPPM;
		int matchCount;
		float precursorMz;
		float xcorr;
		float mvh;
		int rank;
		bool decoy;
		int charge;
		int massdiff;
 		int scannum;
		QString unmodPeptideSeq; //TODO: Sahil, Added while merging mzfileio
        bool isFocused; //TODO: Sahil, Added while merging mzfileio
        float rt; //TODO: Sahil, Added while merging mzfileio
        int id; //TODO: Sahil, Added while merging mzfileio
		QString compoundId;
		QString fragmentId;
		QMap<QString,int>proteins; //TODO: Sahil, Added while merging mzfileio
        QMap<int,float> mods; //TODO: Sahil, Added while merging mzfileio


	SpectralHit() { 
		scan = NULL; 
		precursorMz=0;
		matchCount=0; 
		productPPM=0; 
		score=0;
		xcorr=0; 
		massdiff=0;
		mvh=0; 
		rank=0; 
		decoy=false;
		charge=0;
		int scannum;
        isFocused=false; //TODO: Sahil, Added while merging mzfileio
        rt=0; //TODO: Sahil, Added while merging mzfileio
        id=0; //TODO: Sahil, Added while merging mzfileio
	}

        double getMaxIntensity() { 
            double maxIntensity=0; 
            foreach(double x, intensityList) if(x>maxIntensity) maxIntensity=x;
            return  maxIntensity;
        }

        double getMinMz() {
            double min = mzList.first();
            foreach(double x, mzList)  if(x<min) min=x; 
            return  min;
        }

        double getMaxMz() {
            double max = mzList.last();
            foreach(double x, mzList)  if(x>max) max=x;
            return max;
        }

        QString getProteinIds() { QStringList ids = proteins.uniqueKeys(); return(ids.join(";")); }
		QStringList getProteins() { return proteins.uniqueKeys(); } 
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
#endif
