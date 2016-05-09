#ifndef GLOBAL_H
#define GLOBAL_H

//c++
#include <math.h>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

//local
#include "mzUtils.h"
#include "mzSample.h"
#include "mzMassSlicer.h"
#include "database.h"
#include "classifier.h"
#include "classifierNeuralNet.h"
#include "classifierNaiveBayes.h"

//non gui qt classes
#include <QString>

const QString rsrcPath = ":/images";
const QString wikiUrl =  "http://genomics.princeton.edu/~emelamud/images/";
const QString programName = "MAVEN";
const QString programVersion = "2011.6.17";


static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;

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
		mzLinkType
};

extern Database DB; 

struct compReactionCounts
{
		bool operator()(Compound* s1, Compound* s2) const
		{
				return s1->reactions.size() > s2->reactions.size();
		}
};

#endif
