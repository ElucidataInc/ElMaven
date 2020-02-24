#ifndef GLOBAL_H
#define GLOBAL_H

//c++
#include <math.h>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

//non gui qt classes
#include <QString>

class mzSample;
class mzSlice;
class Peak;
class PeakGroup;
class Compound;
class Scan;
class Pathway;

const QString rsrcPath = ":/images";
const QString programName = "El-MAVEN";

static const double Pi = 3.14159265358979323846264338327950288419717;

struct UserNote {
    int  noteid;
    float mz;
    float mzmin;
    float mzmax;
    float rt;
    float intensity;
    QString compound_name;
    QString compound_id;
    QString srm_id;
    QString title;
    QString description;
    QString method;
    QString author;
    QString creationTime;
    QString guid;
    QString link;
    bool remoteNote;
    QString remoteUrl;
};


Q_DECLARE_METATYPE(mzSample*)
Q_DECLARE_METATYPE(Peak*)
Q_DECLARE_METATYPE(Compound*)
Q_DECLARE_METATYPE(Scan*)
Q_DECLARE_METATYPE(PeakGroup*)
Q_DECLARE_METATYPE(mzSlice*)
Q_DECLARE_METATYPE(Pathway*)
Q_DECLARE_METATYPE(UserNote*)
Q_DECLARE_METATYPE(mzSlice)

enum itemType {
    SampleType=4999,
    PeakGroupType,
    CompoundType,
    ScanType,
    EICType,
    PeakType,
    PathwayType,
    mzSliceType,
    mzLinkType,
    AdductType
};

#endif
