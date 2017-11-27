#ifndef MASSCUTOFFTYPE_h
#define MASSCUTOFFTYPE_h

#include<iostream>
#include<assert.h>
using namespace std;

class MassCutoff{
	/**
	 * this class is used for mass cutoff type. previously, mass cutoff type was ppm 
	 * only which has float type. 
	 * 		But now, mass cutoff type can be given in ppm or mDa value based on value of
	 * _massCutoffType string. In future also, this kind of type can be added very easily
	 */
private:
	string _massCutoffType;
	double _massCutoff;
public:
	void setMassCutoffAndType(double massCutoff, string massCutoffType);
	void setMassCutoffType(string massCutoffType){_massCutoffType=massCutoffType;}
	string getMassCutoffType(){return _massCutoffType;}
	void setMassCutoff(double massCutoff){_massCutoff=massCutoff;}
	double getMassCutoff(){return _massCutoff;}
	double massCutoffValue(double mz);
};
#endif