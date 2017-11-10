#ifndef MASSCUTOFFTYPE_h
#define MASSCUTOFFTYPE_h

#include<iostream>
using namespace std;

class MassCutoff{
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