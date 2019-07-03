#ifndef TESTGROUPFILTERING_H
#define TESTGROUPFILTERING_H

#include <iostream>
#include <QtTest>

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
