#include "datastructures/adduct.h"
#include "mzMassCalculator.h"

Adduct::Adduct(string name, int nmol, int charge, float mass)
{
    this->name = name;
    this->nmol = nmol;
    this->charge = charge;
    this->mass = mass;
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