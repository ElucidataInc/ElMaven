#include "datastructures/adduct.h"
#include "mzMassCalculator.h"

Adduct::Adduct()
{
    this->name = "";
    this->nmol = 0;
    this->charge = 0;
    this->mass = 0;
    this->mz = 0;
}

Adduct::Adduct(string name, int nmol, int charge, float mass)
{
    this->name = name;
    this->nmol = nmol;
    this->charge = charge;
    this->mass = mass;
    this->mz = 0;
}

Adduct::Adduct(const Adduct& a)
{
    this->name = a.name;
    this->nmol = a.nmol;
    this->charge = a.charge;
    this->mass =  a.mass;
    this->mz = a.mz;
}

Adduct::~Adduct()
{

}

string Adduct::getName()
{
    return this->name;
}

int Adduct::getCharge()
{
    return this->charge;
}

int Adduct::getNmol()
{
    return this->nmol;
}

float Adduct::getMass()
{
    return this->mass;
}