#ifndef ADDUCT_H
#define ADDUCT_H

#include "standardincludes.h"

using namespace std;

class Adduct {
    private:
        string name;
        int nmol;
        int charge;
        float mass;
        float mz;
    
    public:
        Adduct();
        Adduct(string name, int nmol, int charge, float mass);
        Adduct(const Adduct& a);
        ~Adduct();

        string getName();
        int getCharge();
        int getNmol();
        float getMass();
};

#endif