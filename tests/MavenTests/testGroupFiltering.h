#ifndef TESTGROUPFILTERING_H
#define TESTGROUPFILTERING_H

#include <iostream>
#include <QtTest>

#include "utilities.h"
#include "PeakGroup.h"
#include "groupFiltering.h"
#include "mavenparameters.h"

using namespace std;

class TestGroupFiltering : public QObject
{
  Q_OBJECT

  public:
    TestGroupFiltering();

  private Q_SLOT:
    void testquantileFilters();


};

#endif //TESTGROUPFILTERING_H