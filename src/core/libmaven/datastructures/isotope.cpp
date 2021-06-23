#include "constants.h"
#include "isotope.h"
#include "mzUtils.h"

Isotope::Isotope(std::string name, double mass, int c, int n, int s, int h, int o)
{
    this->mass = mass;
    this->name = name;
    C13 = c;
    N15 = n;
    S34 = s;
    H2 = h;
    O18 = o;
    abundance = 0.0;
}

Isotope::Isotope()
{
    name = "";
    mass = 0.0;
    abundance = 0.0;
    N15 = 0;
    C13 = 0;
    S34 = 0;
    H2 = 0;
    O18 = 0;
}

Isotope::Isotope(const Isotope &b)
{
    name = b.name;
    mass = b.mass;
    abundance = b.abundance;
    N15 = b.N15;
    S34 = b.S34;
    C13 = b.C13;
    H2 = b.H2;
    O18 = b.O18;
}

Isotope& Isotope::operator=(const Isotope &b)
{
    name = b.name;
    mass = b.mass;
    abundance = b.abundance;
    N15 = b.N15;
    S34 = b.S34;
    C13 = b.C13;
    H2 = b.H2;
    O18 = b.O18;
    return *this;
}

bool Isotope::operator==(const Isotope &b) const
{
    return (name == b.name
            && mzUtils::almostEqual(mass, b.mass)
            && mzUtils::almostEqual(abundance, b.abundance)
            && C13 == b.C13
            && N15 == b.N15
            && S34 == b.S34
            && H2 == b.H2
            && O18 == b.O18);
}

bool Isotope::operator<(const Isotope &b) const
{
    if (C13 != b.C13)
        return C13 < b.C13;
    if (H2 != b.H2)
        return H2 < b.H2;
    if (N15 != b.N15)
        return N15 < b.N15;
    if (S34 != b.S34)
        return S34 < b.S34;
    if (O18 != b.O18)
        return O18 < b.O18;
    return name < b.name;
}

bool Isotope::isNone() const
{
    return (mass == 0.0 || name == "");
}

bool Isotope::isParent() const
{
    if (name == C12_PARENT_LABEL)
        return true;
    return false;
}
