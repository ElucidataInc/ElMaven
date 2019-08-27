#include "datastructures/adduct.h"
#include "mzMassCalculator.h"

Adduct::Adduct()
{
    this->_name = "";
    this->_nmol = 0;
    this->_charge = 0;
    this->_mass = 0;
    this->_mz = 0;
}

Adduct::Adduct(string name, int nmol, int charge, float mass)
{
    this->_name = name;
    this->_nmol = nmol;
    this->_charge = charge;
    this->_mass = mass;
    this->_mz = 0;
}

Adduct::Adduct(const Adduct& a)
{
    this->_name = a._name;
    this->_nmol = a._nmol;
    this->_charge = a._charge;
    this->_mass =  a._mass;
    this->_mz = a._mz;
}

string Adduct::getName()
{
    return this->_name;
}

int Adduct::getCharge()
{
    return this->_charge;
}

int Adduct::getNmol()
{
    return this->_nmol;
}

float Adduct::getMass()
{
    return this->_mass;
}

float Adduct::computeParentMass(float mz)
{
    return (mz * abs(_charge) - _mass) / _nmol;
}

float Adduct::computeAdductMass(float parentMz)
{
    return (parentMz * _nmol + _mass) / abs(_charge);
}
