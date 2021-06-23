#ifndef ISOTOPE_H
#define ISOTOPE_H

#include <string>

class Isotope
{
public:
    std::string name;
    double mass;
    double abundance;
    int N15;
    int C13;
    int S34;
    int H2;
    int O18;

    Isotope(std::string name,
            double mass,
            int c = 0,
            int n = 0,
            int s = 0,
            int h = 0,
            int o = 0);

    Isotope();

    Isotope(const Isotope &b);

    Isotope& operator=(const Isotope &b);

    bool operator==(const Isotope &b) const;

    bool operator<(const Isotope &b) const;

    bool isNone() const;

    bool isParent() const;
};

#endif // ISOTOPE_H
